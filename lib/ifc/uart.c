#include "uart.h"

//------------------------------------------------------------------------------------------------- Interrupt

static void UART_InterruptDMA(UART_t *uart)
{
  if(uart->dma.reg->ISR & DMA_ISR_TCIF(uart->dma.pos)) {
    uart->dma.reg->IFCR |= DMA_ISR_TCIF(uart->dma.pos);
    uart->reg->CR1 |= USART_CR1_TCIE;
    uart->tx_flag = false;
  }
}

static void UART_InterruptEV(UART_t *uart)
{
  if(uart->reg->ISR & USART_ISR_RXNE_RXFNE) {
    uint8_t value = (uint8_t)uart->reg->RDR;
    BUFF_Push(uart->buff, value);
    if(uart->tim) {
      TIM_ResetValue(uart->tim);
      TIM_Enable(uart->tim);
    }
  }
  if((uart->reg->CR1 & USART_CR1_TCIE) && (uart->reg->ISR & USART_ISR_TC)) {
    uart->reg->CR1 &= ~USART_CR1_TCIE;
    uart->reg->ICR |= USART_ICR_TCCF;
    uart->tc_flag = false;
    if(uart->gpio_direction) GPIO_Rst(uart->gpio_direction);
  }
  if(uart->reg->ISR & USART_ISR_RTOF) {
    uart->reg->ICR |= USART_ICR_RTOCF;
    BUFF_Break(uart->buff);
  }
}

//------------------------------------------------------------------------------------------------- Map

const GPIO_Map_t UART_TX_MAP[] = {
  [UART1_TX_PA9] = { .port = GPIOA, .pin = 9, .alternate = 1 },
  [UART1_TX_PB6] = { .port = GPIOB, .pin = 6, .alternate = 0 },
  [UART1_TX_PC4] = { .port = GPIOC, .pin = 4, .alternate = 1 },
  [UART2_TX_PA2] = { .port = GPIOA, .pin = 2, .alternate = 1 },
  [UART2_TX_PA14] = { .port = GPIOA, .pin = 14, .alternate = 1 },
  [UART2_TX_PD5] = { .port = GPIOD, .pin = 5, .alternate = 0 },
  [UART3_TX_PB2] = { .port = GPIOB, .pin = 2, .alternate = 4 },
  [UART3_TX_PB8] = { .port = GPIOB, .pin = 8, .alternate = 4 },
  [UART3_TX_PB10] = { .port = GPIOB, .pin = 10, .alternate = 4 },
  [UART3_TX_PC4] = { .port = GPIOC, .pin = 4, .alternate = 0 },
  [UART3_TX_PC10] = { .port = GPIOC, .pin = 10, .alternate = 0 },
  [UART3_TX_PD8] = { .port = GPIOD, .pin = 8, .alternate = 0 },
  [UART4_TX_PA0] = { .port = GPIOA, .pin = 0, .alternate = 4 },
  [UART4_TX_PC10] = { .port = GPIOC, .pin = 10, .alternate = 1 },
  [LPUART1_TX_PA2] = { .port = GPIOA, .pin = 2, .alternate = 6 },
  [LPUART1_TX_PB11] = { .port = GPIOB, .pin = 11, .alternate = 1 },
  [LPUART1_TX_PC1] = { .port = GPIOC, .pin = 1, .alternate = 1 },
  #ifdef STM32G0C1xx
    [LPUART2_TX_PA2] = { .port = GPIOA, .pin = 2, .alternate = 8 },
    [LPUART2_TX_PC2] = { .port = GPIOC, .pin = 2, .alternate = 8 },
    [LPUART2_TX_PD5] = { .port = GPIOD, .pin = 5, .alternate = 7 }
  #endif
};

const GPIO_Map_t UART_RX_MAP[] = {
  [UART1_RX_PA10] = { .port = GPIOA, .pin = 10, .alternate = 1 },
  [UART1_RX_PB7] = { .port = GPIOB, .pin = 7, .alternate = 0 },
  [UART1_RX_PC5] = { .port = GPIOC, .pin = 5, .alternate = 1 },
  [UART2_RX_PA3] = { .port = GPIOA, .pin = 3, .alternate = 1 },
  [UART2_RX_PA15] = { .port = GPIOA, .pin = 15, .alternate = 1 },
  [UART2_RX_PD6] = { .port = GPIOD, .pin = 6, .alternate = 0 },
  [UART3_RX_PB0] = { .port = GPIOB, .pin = 0, .alternate = 4 },
  [UART3_RX_PB9] = { .port = GPIOB, .pin = 9, .alternate = 4 },
  [UART3_RX_PB11] = { .port = GPIOB, .pin = 11, .alternate = 4 },
  [UART3_RX_PC5] = { .port = GPIOC, .pin = 5, .alternate = 0 },
  [UART3_RX_PC11] = { .port = GPIOC, .pin = 11, .alternate = 0 },
  [UART3_RX_PD9] = { .port = GPIOD, .pin = 9, .alternate = 0 },
  [UART4_RX_PA1] = { .port = GPIOA, .pin = 1, .alternate = 4 },
  [UART4_RX_PC11] = { .port = GPIOC, .pin = 11, .alternate = 1 },
  [LPUART1_RX_PA3] = { .port = GPIOA, .pin = 3, .alternate = 6 },
  [LPUART1_RX_PB10] = { .port = GPIOB, .pin = 10, .alternate = 1 },
  [LPUART1_RX_PC0] = { .port = GPIOC, .pin = 0, .alternate = 1 },
  #ifdef STM32G0C1xx
    [LPUART2_RX_PA3] = { .port = GPIOA, .pin = 3, .alternate = 8 },
    [LPUART2_RX_PC3] = { .port = GPIOC, .pin = 3, .alternate = 8 },
    [LPUART2_RX_PD6] = { .port = GPIOD, .pin = 6, .alternate = 7 }
  #endif
};

//------------------------------------------------------------------------------------------------- Init

/**
 * @brief Initialize `UART_t` instance with DMA, GPIO and optional timeout.
 * Configures buffer, DMA request, GPIO pins, UART registers,
 * stop bits, parity, baudrate and IRQ handlers.
 * If `tim` is provided, uses timer for timeout; otherwise uses hardware RTO.
 * @param uart Pointer to `UART_t` control structure.
 */
void UART_Init(UART_t *uart)
{
  if(uart->gpio_direction) {
    uart->gpio_direction->mode = GPIO_Mode_Output;
    GPIO_Init(uart->gpio_direction);
  }
  BUFF_Init(uart->buff);
  DMA_SetRegisters(uart->dma_nbr, &uart->dma);
  RCC_EnableDMA(uart->dma.reg);
  RCC_EnableUART(uart->reg);
  uart->dma.mux->CCR &= 0xFFFFFFC0;
  switch((uint32_t)uart->reg) {
    case (uint32_t)USART1: uart->dma.mux->CCR |= DMAMUX_REQ_USART1_TX; break;
    case (uint32_t)USART2: uart->dma.mux->CCR |= DMAMUX_REQ_USART2_TX; break;
    case (uint32_t)USART3: uart->dma.mux->CCR |= DMAMUX_REQ_USART3_TX; break;
    case (uint32_t)USART4: uart->dma.mux->CCR |= DMAMUX_REQ_USART4_TX; break;
    case (uint32_t)LPUART1: uart->dma.mux->CCR |= DMAMUX_REQ_LPUART1_TX; break;
    #ifdef STM32G0C1xx
      case (uint32_t)LPUART2: uart->dma.mux->CCR |= DMAMUX_REQ_LPUART2_TX; break;
    #endif
  }
  GPIO_InitAlternate(&UART_TX_MAP[uart->tx_pin], false);
  GPIO_InitAlternate(&UART_RX_MAP[uart->rx_pin], false);
  uart->dma.cha->CPAR = (uint32_t)&(uart->reg->TDR);
  uart->dma.cha->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
  if((uint32_t)uart->reg == (uint32_t)LPUART1
  #ifdef STM32G0C1xx
    || (uint32_t)uart->reg == (uint32_t)LPUART2
  #endif
  ) {
    uart->reg->BRR = (uint64_t)256 * SystemCoreClock / uart->baud;
  }
  else uart->reg->BRR = SystemCoreClock / uart->baud;
  uart->reg->CR3 |= USART_CR3_DMAT | USART_CR3_OVRDIS;
  switch(uart->stop_bits) {
    case 0: uart->reg->CR2 |= USART_CR2_STOP_0; break;
    case 1: break;
    case 2: uart->reg->CR2 |= USART_CR2_STOP_1; break;
    case 3: uart->reg->CR2 |= USART_CR2_STOP_0 | USART_CR2_STOP_1; break;
  }
  switch(uart->parity) {
    case 0: break;
    case 1: uart->reg->CR1 |= USART_CR1_PCE | USART_CR1_PS; break;
    case 2: uart->reg->CR1 |= USART_CR1_PCE; break;
  }
  uart->reg->CR1 |= USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
  uart->reg->ICR |= USART_ICR_TCCF;
  uart->reg->RQR |= USART_RQR_RXFRQ;
  IRQ_EnableDMA(uart->dma_nbr, uart->irq_priority, (void (*)(void*))&UART_InterruptDMA, uart);
  IRQ_EnableUART(uart->reg, uart->irq_priority, (void (*)(void*))&UART_InterruptEV, uart);
  if(uart->tim) {
    uart->tim->prescaler = 100;
    uart->tim->auto_reload = (float)SystemCoreClock * uart->timeout / (uart->baud) / 100;
    uart->tim->function = (void (*)(void*))BUFF_Break;
    uart->tim->function_struct = (void*)uart->buff;
    uart->tim->irq_priority = uart->irq_priority;
    uart->tim->one_pulse_mode = true;
    if(uart->timeout) {
      uart->tim->enable = true;
      uart->tim->enable_interrupt = true;
    }
    TIM_Init(uart->tim);
  }
  else if(uart->timeout) {
    uart->reg->RTOR = uart->timeout;
    uart->reg->CR1 |= USART_CR1_RTOIE;
    uart->reg->CR2 |= USART_CR2_RTOEN;
  }
  uart->init_flag = true;
}

void UART_ReInit(UART_t *uart)
{
  uart->reg->CR1 &= ~USART_CR1_UE;
  RCC_DisableUART(uart->reg);
  UART_Init(uart);
}

void UART_SetTimeout(UART_t *uart, uint16_t timeout)
{
  if(uart->tim) {
    if(uart->timeout) {
      TIM_SetAutoreload(uart->tim, (float)SystemCoreClock * uart->timeout / (uart->baud) / 100);
      TIM_Disable(uart->tim);
      TIM_InterruptEnable(uart->tim);
      TIM_Enable(uart->tim);
    }
    else {
      TIM_InterruptDisable(uart->tim);
      TIM_ResetValue(uart->tim);
    }
  }
  else {
    if(uart->timeout) {
      uart->reg->RTOR = uart->timeout;
      uart->reg->CR1 |= USART_CR1_RTOIE;
      uart->reg->CR2 |= USART_CR2_RTOEN;
    }
    else {
      uart->reg->CR2 &= ~USART_CR2_RTOEN;
      uart->reg->CR1 &= ~USART_CR1_RTOIE;
    }
  }
}

//------------------------------------------------------------------------------------------------- Flags

/**
 * @brief Check if last send is completed.
 * @param uart Pointer to `UART_t` control structure.
 * @return `true` if send completed, `false` if still transmitting.
 */
bool UART_SendCompleted(UART_t *uart)
{
  return !uart->tc_flag;
}

/**
 * @brief Check if UART is currently sending.
 * @param uart Pointer to `UART_t` control structure.
 * @return `true` if sending, `false` if send completed.
 */
bool UART_IsSending(UART_t *uart)
{
  return uart->tc_flag;
}

/**
 * @brief Check if UART is busy.
 * @param uart Pointer to `UART_t` control structure.
 * @return `true` if busy, `false` if free.
 */
bool UART_IsBusy(UART_t *uart)
{
  return uart->tx_flag;
}

/**
 * @brief Check if UART is free.
 * @param uart Pointer to `UART_t` control structure.
 * @return `true` if free, `false` if busy.
 */
bool UART_IsFree(UART_t *uart)
{
  return !uart->tx_flag;
}

//------------------------------------------------------------------------------------------------- Send

/**
 * @brief Start UART transmit using DMA.
 * @param uart Pointer to `UART_t` control structure.
 * @param data Pointer to data buffer to send.
 * @param len Number of bytes to send.
 * @return `OK` if transfer started, `ERR` if not initialized, `BUSY` if already transmitting.
 * Function configures DMA channel, sets memory address and transfer size,
 * optionally sends prefix byte (address) if `uart->prefix` is set,
 * and starts DMA transfer. Flag `tx_flag` is set until transfer complete.
 */
status_t UART_Send(UART_t *uart, uint8_t *data, uint16_t len)
{
  if(!uart->init_flag) return ERR;
  if(uart->tx_flag) return BUSY;
  if(uart->gpio_direction) GPIO_Set(uart->gpio_direction);
  uart->dma.cha->CCR &= ~DMA_CCR_EN;
  uart->dma.cha->CMAR = (uint32_t)data;
  uart->dma.cha->CNDTR = len;
  if(uart->prefix) uart->reg->TDR = uart->prefix; // send address for stream
  uart->dma.cha->CCR |= DMA_CCR_EN;
  uart->tx_flag = true;
  uart->tc_flag = true;
  return OK;
}

//------------------------------------------------------------------------------------------------- Read

/**
 * @brief Get number of bytes in uart buffer.
 * @param uart Pointer to `UART_t` control structure.
 * @return Number of bytes available in buffer.
 */
uint16_t UART_Size(UART_t *uart)
{
  return BUFF_Size(uart->buff);
}

/**
 * @brief Read data from uart buffer.
 * @param uart Pointer to `UART_t` control structure.
 * @param array Pointer to destination array.
 * @return Number of bytes copied to `array`.
 */
uint16_t UART_Read(UART_t *uart, uint8_t *array)
{
  return BUFF_Read(uart->buff, array);
}

/**
 * @brief Read current message from uart buffer as allocated string.
 * Memory is allocated dynamically and must be freed by caller.
 * @param uart Pointer to `UART_t` control structure.
 * @return Pointer to allocated null-terminated string, or NULL if empty or alloc failed.
 */
char *UART_ReadString(UART_t *uart)
{
  return BUFF_ReadString(uart->buff);
}

/**
 * @brief Skip current message in uart buffer.
 * @param uart Pointer to `UART_t` control structure.
 * @return `true` if message skipped, `false` if buffer empty.
 */
bool UART_Skip(UART_t *uart)
{
  return BUFF_Skip(uart->buff);
}

/**
 * @brief Clear all messages in uart buffer.
 * @param uart Pointer to `UART_t` control structure.
 */
void UART_Clear(UART_t *uart)
{
  BUFF_Clear(uart->buff);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Calculates transmission time for given UART frame length.
 * @param uart UART_t structure with UART settings.
 * @param len Frame length in bytes.
 * @return uint16_t Transmission time in milliseconds.
 */
uint32_t UART_CalcTime_ms(UART_t *uart, uint16_t len)
{
  uint32_t bits = 10; // start_bit + space
  if(uart->parity) bits++;
  switch(uart->stop_bits) {
    case UART_StopBits_0_5: case UART_StopBits_1_0: bits += 1; break;
    case UART_StopBits_1_5: case UART_StopBits_2_0: bits += 2; break;
  } 
  return 1000 * ((bits * len) + uart->timeout) / uart->baud;
}
