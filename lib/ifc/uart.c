#include "uart.h"

//------------------------------------------------------------------------------------------------- Interrupt

static void UART_InterruptDMA(UART_t *uart)
{
  if(uart->dma.reg->ISR & DMA_ISR_TCIF(uart->dma.pos)) {
    uart->dma.reg->IFCR |= DMA_ISR_TCIF(uart->dma.pos);
    uart->reg->CR1 |= USART_CR1_TCIE;
    uart->busy_tx = false;
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
    uart->busy_tc = false;
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
  [LPUART1_TX_PC1] = { .port = GPIOC, .pin = 1, .alternate = 1 }
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
  [LPUART1_RX_PC0] = { .port = GPIOC, .pin = 0, .alternate = 1 }
};

//------------------------------------------------------------------------------------------------- Init

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
    case (uint32_t)LPUART2: uart->dma.mux->CCR |= DMAMUX_REQ_LPUART2_TX; break;
  }
  GPIO_AlternateInit(&UART_TX_MAP[uart->tx_pin], false);
  GPIO_AlternateInit(&UART_RX_MAP[uart->rx_pin], false);
  uart->dma.cha->CPAR = (uint32_t)&(uart->reg->TDR);
  uart->dma.cha->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
  if((uint32_t)uart->reg == (uint32_t)LPUART1) {
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
  INT_EnableDMA(uart->dma_nbr, uart->int_prioryty, (void (*)(void*))&UART_InterruptDMA, uart);
  INT_EnableUART(uart->reg, uart->int_prioryty, (void (*)(void*))&UART_InterruptEV, uart);
  if(uart->tim) {
    uart->tim->prescaler = 100;
    uart->tim->auto_reload = (float)SystemCoreClock * uart->timeout / (uart->baud) / 100;
    uart->tim->function = (void (*)(void*))BUFF_Break;
    uart->tim->function_struct = (void*)uart->buff;
    uart->tim->int_prioryty = uart->int_prioryty;
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

bool UART_During(UART_t *uart)
{
  return uart->busy_tc;
}

bool UART_Idle(UART_t *uart)
{
  return !(uart->busy_tc);
}

bool UART_IsBusy(UART_t *uart)
{
  return uart->busy_tx;
}

bool UART_IsFree(UART_t *uart)
{
  return !(uart->busy_tx);
}

//------------------------------------------------------------------------------------------------- Send

state_t UART_Send(UART_t *uart, uint8_t *data, uint16_t length)
{
  if(!uart->busy_tx) {
    if(uart->gpio_direction) GPIO_Set(uart->gpio_direction);
    uart->dma.cha->CCR &= ~DMA_CCR_EN;
    uart->dma.cha->CMAR = (uint32_t)data;
    uart->dma.cha->CNDTR = length;
    if(uart->prefix) uart->reg->TDR = uart->prefix; // send address for stream
    uart->dma.cha->CCR |= DMA_CCR_EN;
    uart->busy_tx = true;
    uart->busy_tc = true;
    return FREE;
  }
  else return BUSY;
}

//------------------------------------------------------------------------------------------------- Read

uint16_t UART_ReadSize(UART_t *uart)
{
  return BUFF_Size(uart->buff);
}

uint16_t UART_ReadArray(UART_t *uart, uint8_t *array)
{
  return BUFF_Array(uart->buff, array);
}

char *UART_ReadString(UART_t *uart)
{
  return BUFF_String(uart->buff);
}

bool UART_ReadSkip(UART_t *uart)
{
  return BUFF_Skip(uart->buff);
}

void UART_ReadClear(UART_t *uart)
{
  return BUFF_Clear(uart->buff);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Oblicza czas trwania transmisji dla określonej długości ramki UART.
 * @param uart Wskaźnik do struktury UART_t reprezentującej ustawienia interfejsu UART.
 * @param length Długość ramki w ilości bajtów.
 * @return uint16_t Czas trwania transmisji w milisekundach.
 */
uint16_t UART_CalcTime(UART_t *uart, uint16_t length)
{
  uint32_t bits = 10; // start_bit + space
  if(uart->parity) bits++;
  switch(uart->stop_bits) {
    case UART_StopBits_0_5: case UART_StopBits_1_0: bits += 1; 
    case UART_StopBits_1_5: case UART_StopBits_2_0: bits += 2;
  } 
  return 1000 * ((bits * length) + uart->timeout) / uart->baud;
}
