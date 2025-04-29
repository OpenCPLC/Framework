#ifndef PWMI_H_
#define PWMI_H_

#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "exmath.h"
#include "vrts.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  PWMI_CapturePrescaler_1 = 0,
  PWMI_CapturePrescaler_2 = 1,
  PWMI_CapturePrescaler_4 = 2,
  PWMI_CapturePrescaler_8 = 3
} PWMI_CapturePrescaler_t;

#ifndef PWMI_OVERSAMPLING_AUTO
  #define PWMI_OVERSAMPLING_AUTO 0xFFFF
#endif

#define PWMI_CONFIG_NOT_READY 0xFFFF

typedef struct {
  TIM_TypeDef *reg;
  uint32_t prescaler;
  uint32_t timeout_ms;
  PWMI_CapturePrescaler_t capture_prescaler;
  INT_Prioryty_t int_prioryty;
  TIM_Filter_t filter;
  TIM_CHx_t channel[4];
  uint32_t reload[4];
  uint32_t value[4];
  float frequency[4];
  float duty[4];
  #if(PWMI_OVERSAMPLING_AUTO)
    uint16_t oversampling[4];
  #else
    uint16_t oversampling;
  #endif
  uint16_t count;
  uint8_t inc;
  uint64_t timeout_tick;
  EXTI_t *trig3;
  EXTI_t *trig4;
  bool start;
} PWMI_t;

uint16_t PWMI_Run(PWMI_t *pwmi);
void PWMI_Init(PWMI_t *pwmi);
bool PWMI_Loop(PWMI_t *pwmi);

//-------------------------------------------------------------------------------------------------
#endif
