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
} INT_e;

//-------------------------------------------------------------------------------------------------

void INT_EnableTIM(TIM_TypeDef *tim_typedef, uint8_t priority, void (*handler)(void *), void *object);
void INT_EnableUART(USART_TypeDef *uart_typedef, uint8_t priority, void (*handler)(void *), void *object);
void INT_EnableI2C(I2C_TypeDef *i2c_typedef, uint8_t priority, void (*handler)(void *), void *object);
void INT_EnableADC(uint8_t priority, void (*handler)(void *), void *object);
void INT_EnableDMA(uint8_t dma_nbr, uint8_t priority, void (*handler)(void *), void *object);
void INT_EnableEXTI(uint8_t exti_nbr, uint8_t priority, void (*handler)(void *), void *object);

//-------------------------------------------------------------------------------------------------

#endif
