#ifndef PWMI_H_
#define PWMI_H_

#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "exmath.h"
#include "vrts.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  PWMI_Prescaler_1 = 0,
  PWMI_Prescaler_2 = 1,
  PWMI_Prescaler_4 = 2,
  PWMI_Prescaler_8 = 3
} PWMI_Prescaler_e;

typedef struct {
  TIM_TypeDef *reg;
  uint32_t prescaler;
  uint32_t timeout_ms;
  PWMI_Prescaler_e input_prescaler;
  uint8_t int_prioryty;
  TIM_Filter_t filter;
  TIM_CHx_t channel[4];
  uint32_t reload[4];
  uint32_t value[4];
  float frequency[4];
  float fill[4];
  uint8_t oversampling;
  uint8_t count;
  uint8_t inc;
  uint64_t watchdog;
  EXTI_t *trig3;
  EXTI_t *trig4;
} PWMI_t;

uint8_t PWMI_Run(PWMI_t *pwmi);
void PWMI_Init(PWMI_t *pwmi);
bool PWMI_Loop(PWMI_t *pwmi);
void PWMI_Print(PWMI_t *pwmi);

//-------------------------------------------------------------------------------------------------
#endif
