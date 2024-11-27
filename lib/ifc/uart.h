#ifndef UART_H_
#define UART_H_

#include "int.h"
#include "tim.h"
#include "buff.h"
#include "main.h"

//---------------------------------------------------------------------------------------------------------------------

#define UART_115200 baud = 115200, .parity = UART_Parity_None, .stop_bits = UART_StopBits_1_0
#define UART_57600 baud = 57600, .parity = UART_Parity_None, .stop_bits = UART_StopBits_1_0
#define UART_19200 baud = 19200, .parity = UART_Parity_None, .stop_bits = UART_StopBits_1_0
#define UART_9600 baud = 9600, .parity = UART_Parity_None, .stop_bits = UART_StopBits_1_0

//---------------------------------------------------------------------------------------------------------------------

typedef enum {
  UART1_TX_PA9, UART1_TX_PB6, UART1_TX_PC4,
  UART2_TX_PA2, UART2_TX_PA14, UART2_TX_PD5,
  UART3_TX_PB2, UART3_TX_PB8, UART3_TX_PB10, UART3_TX_PC4, UART3_TX_PC10, UART3_TX_PD8,
  UART4_TX_PA0, UART4_TX_PC10,
  LPUART1_TX_PA2, LPUART1_TX_PB11, LPUART1_TX_PC1
} UART_TX_t;

typedef enum {
  UART1_RX_PA10, UART1_RX_PB7, UART1_RX_PC5,
  UART2_RX_PA3, UART2_RX_PA15, UART2_RX_PD6,
  UART3_RX_PB0, UART3_RX_PB9, UART3_RX_PB11, UART3_RX_PC5, UART3_RX_PC11, UART3_RX_PD9,
  UART4_RX_PA1, UART4_RX_PC11,
  LPUART1_RX_PA3, LPUART1_RX_PB10, LPUART1_RX_PC0,
} UART_RX_t;

typedef enum {
  UART_Parity_None = 0,
  UART_Parity_Odd = 1,
  UART_Parity_Even = 2
} UART_Parity_t;

typedef enum {
  UART_StopBits_0_5 = 0,
  UART_StopBits_1_0 = 1,
  UART_StopBits_2_0 = 2,
  UART_StopBits_1_5 = 3
} UART_StopBits_t;

//---------------------------------------------------------------------------------------------------------------------

typedef struct {
  USART_TypeDef *reg;
  UART_TX_t tx_pin;
  UART_RX_t rx_pin;
  DMA_Channel_t dma_channel;
  INT_Prioryty_t int_prioryty;
  uint32_t baud;
  UART_Parity_t parity;
  UART_StopBits_t stop_bits;
  uint16_t timeout;
  GPIO_t *gpio_direction;
  TIM_t *tim;
  BUFF_t *buff;
  DMA_Channel_TypeDef *_tx_dma;
  DMAMUX_Channel_TypeDef *_tx_dmamux;
  uint8_t prefix;
  bool _busy_tx;
  bool _busy_tc;
} UART_t;

//--------------------------------------------------------------------------------------------------------------------------------

void UART_Init(UART_t *uart);
void UART_ReInit(UART_t *uart);
void UART_SetTimeout(UART_t *uart, uint16_t timeout);
bool UART_During(UART_t *uart);
bool UART_Idle(UART_t *uart);
bool UART_IsBusy(UART_t *uart);
bool UART_IsFree(UART_t *uart);

state_t UART_Send(UART_t *uart, uint8_t *array, uint16_t length);
uint16_t UART_ReadSize(UART_t *uart);
uint16_t UART_ReadArray(UART_t *uart, uint8_t *array);
char *UART_ReadString(UART_t *uart);
bool UART_ReadSkip(UART_t *uart);
void UART_ReadClear(UART_t *uart);

uint16_t UART_CalcTime(UART_t *uart, uint16_t length);

//--------------------------------------------------------------------------------------------------------------------------------
#endif
