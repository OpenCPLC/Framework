#ifndef TIM_H_
#define TIM_H_

#include "gpio.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  TIMx_CHx_Free,
  TIM1_CH1_PA8, TIM1_CH1_PC8,
  TIM1_CH1N_PA7, TIM1_CH1N_PB13, TIM1_CH1N_PD2,
  TIM1_CH2_PA9, TIM1_CH2_PB3, TIM1_CH2_PC9,
  TIM1_CH2N_PB0, TIM1_CH2N_PB14, TIM1_CH2N_PD3,
  TIM1_CH3_PA10, TIM1_CH3_PB6, TIM1_CH3_PC10,
  TIM1_CH3N_PB1, TIM1_CH3N_PB15, TIM1_CH3N_PD4,
  TIM1_CH4_PA11, TIM1_CH4_PC11,
  TIM2_CH1_PA0, TIM2_CH2_PA1, TIM2_CH3_PA2, TIM2_CH4_PA3,
  TIM2_CH1_PA5, TIM2_CH1_PA15,
  TIM2_CH2_PB3, TIM2_CH3_PB10, TIM2_CH4_PB11,
  TIM2_CH1_PC4, TIM2_CH2_PC5, TIM2_CH3_PC6, TIM2_CH4_PC7,
  TIM3_CH1_PA6, TIM3_CH2_PA7, TIM3_CH3_PB0, TIM3_CH4_PB1,
  TIM3_CH1_PB4, TIM3_CH2_PB5,
  TIM3_CH1_PC6, TIM3_CH2_PC7, TIM3_CH3_PC8, TIM3_CH4_PC9,
  TIM15_CH1N_PA1, TIM15_CH1_PA2, TIM15_CH2_PA3,
  TIM15_CH1N_PB13, TIM15_CH1_PB14, TIM15_CH1N_PB15, TIM15_CH2_PB15,
  TIM15_CH1_PC1, TIM15_CH2_PC2,
  TIM16_CH1_PA6, TIM16_CH1N_PB6, TIM16_CH1_PB8, TIM16_CH1_PD0,
  TIM17_CH1_PA7, TIM17_CH1N_PB7, TIM17_CH1_PB9, TIM17_CH1_PD1,
  #ifdef STM32G0C1xx
    TIM14_CH1_PA4, TIM14_CH1_PA7, TIM14_CH1_PB1, TIM14_CH1_PC12
  #endif
} TIM_CHx_t;

extern const GPIO_Map_t TIM_CHx_MAP[];

typedef enum {
  TIM_CH1 = 0,
  TIM_CH2 = 1,
  TIM_CH3 = 2,
  TIM_CH4 = 3,
  TIM_CH1N = 4,
  TIM_CH2N = 5,
  TIM_CH3N = 6,
  TIM_CH4N = 7
} TIM_Channel_t;

typedef enum {
  TIM_Filter_NoFilter = 0x0,
  TIM_Filter_FCLK_N2 = 0x1,
  TIM_Filter_FCLK_N4 = 0x2,
  TIM_Filter_FCLK_N8 = 0x3,
  TIM_Filter_FDTS_2xN6 = 0x4,
  TIM_Filter_FDTS_2xN8 = 0x5,
  TIM_Filter_FDTS_4xN6 = 0x6,
  TIM_Filter_FDTS_4xN8 = 0x7,
  TIM_Filter_FDTS_8xN6 = 0x8,
  TIM_Filter_FDTS_8xN8 = 0x9,
  TIM_Filter_FDTS_16xN5 = 0xA,
  TIM_Filter_FDTS_16xN6 = 0xB,
  TIM_Filter_FDTS_16xN8 = 0xC,
  TIM_Filter_FDTS_32xN5 = 0xD,
  TIM_Filter_FDTS_32xN6 = 0xE,
  TIM_Filter_FDTS_32xN8 = 0xF
} TIM_Filter_t;

typedef enum {
  TIM_MasterMode_Reset = 0,
  TIM_MasterMode_Enable = 1,
  TIM_MasterMode_Update = 2,
  TIM_MasterMode_ComparePulse = 3,
  TIM_MasterMode_OC1 = 4,
  TIM_MasterMode_OC2 = 5,
  TIM_MasterMode_OC3 = 6,
  TIM_MasterMode_OC4 = 7,
} TIM_MasterMode_t;

//-------------------------------------------------------------------------------------------------

typedef struct {
	TIM_TypeDef *reg;
	INT_Prioryty_t int_prioryty;
	bool one_pulse_mode;
	uint32_t prescaler;
	uint32_t auto_reload;
	void (*function)(void *);
	void *function_struct;
	volatile uint16_t _inc;
	volatile bool enable;
	volatile bool enable_interrupt;
  uint32_t _reserve;
} TIM_t;

//-------------------------------------------------------------------------------------------------

void TIM_Enable(TIM_t *tim);
void TIM_Disable(TIM_t *tim);
void TIM_InterruptEnable(TIM_t *tim);
void TIM_InterruptDisable(TIM_t *tim);
void TIM_ResetValue(TIM_t *tim);
uint32_t TIM_GetValue(TIM_t *tim);
bool TIM_IsDisable(TIM_t *tim);
bool TIM_IsEnable(TIM_t *tim);
void TIM_SetPrescaler(TIM_t *tim, uint32_t prescaler);
void TIM_SetAutoreload(TIM_t *tim, uint32_t auto_reload);
void TIM_MaxAutoreload(TIM_t *tim);
uint16_t TIM_Event(TIM_t *tim);
void TIM_Init(TIM_t *tim);
void TIM_MasterMode(TIM_t *tim, TIM_MasterMode_t mode);

//-------------------------------------------------------------------------------------------------

typedef enum {
  TIM_BaseTime_1us = 1000000,
  TIM_BaseTime_10us = 100000,
  TIM_BaseTime_100us = 10000,
  TIM_BaseTime_1ms = 1000,
  TIM_BaseTime_10ms = 100 // TIM2 only!
} TIM_BaseTime_t;

void DELAY_Init(TIM_t *tim, TIM_BaseTime_t base_time);
void DELAY_Wait(TIM_t *tim, uint32_t value);

#endif
