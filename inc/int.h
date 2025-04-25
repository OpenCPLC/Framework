#ifndef INT_H_
#define INT_H_

#include <stdint.h>
#include "stm32g0xx.h"
#include "startup.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  INT_NonMaskableInt = -14,
  INT_HardFault = -13,
  INT_SVCall = -5,
  INT_PendSV = -2,
  INT_SysTick = -1,
  INT_Watchdog = 0,
  INT_PVD = 1,
  INT_RTC = 2,
  INT_FLASH = 3,
  INT_RCC = 4,
  INT_EXTI01 = 5,
  INT_EXTI23 = 6,
  INT_EXTI4F = 7,
  INT_USB = 8,
  INT_DMA1_CH1 = 9,
  INT_DMA1_CH23 = 10,
  INT_DMA1_CH47_DMA2 = 11, 
  INT_ADC = 12,
  INT_TIM1 = 13, 
  INT_TIM1_CC = 14,
  INT_TIM2 = 15,
  INT_TIM3_TIM4 = 16,  
  INT_TIM6_DAC_LPTIM1 = 17,  
  INT_TIM7_LPTIM2 = 18,
  INT_TIM14 = 19,   
  INT_TIM15 = 20,   
  INT_TIM16 = 21, 
  INT_TIM17 = 22,
  INT_I2C1 = 23,   
  INT_I2C23 = 24,
  INT_SPI1 = 25,
  INT_SPI23 = 26,
  INT_UART1 = 27,  
  INT_UART2_LPUART2 = 28,
  INT_UART3456_LPUART1 = 29,
  INT_CEC = 30,
  INT_RNG = 31
} INT_t;

typedef enum {
  INT_Prioryty_VeryHigh = 0,
  INT_Prioryty_High = 1,
  INT_Prioryty_Medium = 2,
  INT_Prioryty_Low = 3
} INT_Prioryty_t;

typedef enum {
  DMA_Nbr_1 = 1,
  DMA_Nbr_2 = 2,
  DMA_Nbr_3 = 3,
  DMA_Nbr_4 = 4,
  DMA_Nbr_5 = 5,
  DMA_Nbr_6 = 6,
  DMA_Nbr_7 = 7,
  #ifdef STM32G0C1xx
    DMA_Nbr_8 = 8,
    DMA_Nbr_9 = 9,
    DMA_Nbr_10 = 10,
    DMA_Nbr_11 = 11,
    DMA_Nbr_12 = 12
  #endif
} DMA_Nbr_t;

typedef struct {
  DMA_TypeDef *REG;
  DMA_Channel_TypeDef *CHA;
  DMAMUX_Channel_TypeDef *MUX;
  uint8_t pos;
} DMA_t;

typedef enum {
  DMAMUX_REQ_GEN0 = 1,
  DMAMUX_REQ_GEN1 = 2,
  DMAMUX_REQ_GEN2 = 3,
  DMAMUX_REQ_GEN3 = 4,
  DMAMUX_REQ_ADC = 5,
  DMAMUX_REQ_AES_IN = 6,
  DMAMUX_REQ_AES_OUT = 7,
  DMAMUX_REQ_DAC_CH1 = 8,
  DMAMUX_REQ_DAC_CH2 = 9,
  DMAMUX_REQ_I2C1_RX = 10,
  DMAMUX_REQ_I2C1_TX = 11,
  DMAMUX_REQ_I2C2_RX = 12,
  DMAMUX_REQ_I2C2_TX = 13,
  DMAMUX_REQ_LPUART_RX = 14,
  DMAMUX_REQ_LPUART_TX = 15,
  DMAMUX_REQ_SPI1_RX = 16,
  DMAMUX_REQ_SPI1_TX = 17,
  DMAMUX_REQ_SPI2_RX = 18,
  DMAMUX_REQ_SPI2_TX = 19,
  DMAMUX_REQ_TIM1_CH1 = 20,
  DMAMUX_REQ_TIM1_CH2 = 21,
  DMAMUX_REQ_TIM1_CH3 = 22,
  DMAMUX_REQ_TIM1_CH4 = 23,
  DMAMUX_REQ_TIM1_TRIG_COM = 24,
  DMAMUX_REQ_TIM1_UP = 25,
  DMAMUX_REQ_TIM2_CH1 = 26,
  DMAMUX_REQ_TIM2_CH2 = 27,
  DMAMUX_REQ_TIM2_CH3 = 28,
  DMAMUX_REQ_TIM2_CH4 = 29,
  DMAMUX_REQ_TIM2_TRIG = 30,
  DMAMUX_REQ_TIM2_UP = 31,
  DMAMUX_REQ_TIM3_CH1 = 32,
  DMAMUX_REQ_TIM3_CH2 = 33,
  DMAMUX_REQ_TIM3_CH3 = 34,
  DMAMUX_REQ_TIM3_CH4 = 35,
  DMAMUX_REQ_TIM3_TRIG = 36,
  DMAMUX_REQ_TIM3_UP = 37,
  DMAMUX_REQ_TIM6_UP = 38,
  DMAMUX_REQ_TIM7_UP = 39,
  DMAMUX_REQ_TIM15_CH1 = 40,
  DMAMUX_REQ_TIM15_CH2 = 41,
  DMAMUX_REQ_TIM15_TRIG_COM = 42,
  DMAMUX_REQ_TIM15_UP = 43,
  DMAMUX_REQ_TIM16_CH1 = 44,
  DMAMUX_REQ_TIM16_COM = 45,
  DMAMUX_REQ_TIM16_UP = 46,
  DMAMUX_REQ_TIM17_CH1 = 47,
  DMAMUX_REQ_TIM17_COM = 48,
  DMAMUX_REQ_TIM17_UP = 49,
  DMAMUX_REQ_USART1_RX = 50,
  DMAMUX_REQ_USART1_TX = 51,
  DMAMUX_REQ_USART2_RX = 52,
  DMAMUX_REQ_USART2_TX = 53,
  DMAMUX_REQ_USART3_RX = 54,
  DMAMUX_REQ_USART3_TX = 55,
  DMAMUX_REQ_USART4_RX = 56,
  DMAMUX_REQ_USART4_TX = 57,
  DMAMUX_REQ_UCPD1_RX = 58,
  DMAMUX_REQ_UCPD1_TX = 59,
  DMAMUX_REQ_UCPD2_RX = 60,
  DMAMUX_REQ_UCPD2_TX = 61,
  DMAMUX_REQ_I2C3_RX = 62,
  DMAMUX_REQ_I2C3_TX = 63,
  DMAMUX_REQ_LPUART2_RX = 64,
  DMAMUX_REQ_LPUART2_TX = 65,
  DMAMUX_REQ_SPI3_RX = 66,
  DMAMUX_REQ_SPI3_TX = 67,
  DMAMUX_REQ_TIM4_CH1 = 68,
  DMAMUX_REQ_TIM4_CH2 = 69,
  DMAMUX_REQ_TIM4_CH3 = 70,
  DMAMUX_REQ_TIM4_CH4 = 71,
  DMAMUX_REQ_TIM4_TRIG = 72,
  DMAMUX_REQ_TIM4_UP = 73,
  DMAMUX_REQ_USART5_RX = 74,
  DMAMUX_REQ_USART5_TX = 75,
  DMAMUX_REQ_USART6_RX = 76,
  DMAMUX_REQ_USART6_TX = 77
} DMAMUX_REQ_t;

//-------------------------------------------------------------------------------------------------

void INT_EnableTIM(TIM_TypeDef *tim_typedef, INT_Prioryty_t priority, void (*handler)(void *), void *object);
void INT_EnableUART(USART_TypeDef *uart_typedef, INT_Prioryty_t priority, void (*handler)(void *), void *object);
void INT_EnableI2C(I2C_TypeDef *i2c_typedef, INT_Prioryty_t priority, void (*handler)(void *), void *object);
void INT_EnableADC(INT_Prioryty_t priority, void (*handler)(void *), void *object);
void INT_EnableDMA(DMA_Nbr_t dma_nbr, INT_Prioryty_t priority, void (*handler)(void *), void *object);
void INT_EnableEXTI(uint8_t exti_nbr, INT_Prioryty_t priority, void (*handler)(void *), void *object);
void DMA_SetRegisters(DMA_Nbr_t dma_nbr, DMA_t *dma);

//-------------------------------------------------------------------------------------------------

#endif
