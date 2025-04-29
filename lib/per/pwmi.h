#ifndef PWMI_H_
#define PWMI_H_

#include "gpio.h"
#include "tim.h"
#include "exmath.h"
#include "vrts.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  PWMI_CapturePrescaler_1 = 0,
  PWMI_CapturePrescaler_2 = 1,
  PWMI_CapturePrescaler_4 = 2,
  PWMI_CapturePrescaler_8 = 3
} PWMI_CapturePrescaler_t;

// Setting to 0 forces using a fixed oversampling count for measurements.
// When a value is set, samples are counted until the threshold is reached.
// The number of samples is stored and used for final calculations.
// This ensures a roughly constant measurement time, regardless of the input frequency.
#ifndef PWMI_OVERSAMPLING_AUTO
  #define PWMI_OVERSAMPLING_AUTO 0xFFFF
#endif

// Using TIM2 requires buffer variables of type uint64_t,
// because TIM2 is a 32-bit counter and needs more memory space.
#ifndef PWMI_USED_TIM2
  #define PWMI_USED_TIM2 0
#endif

typedef struct {
  TIM_TypeDef *reg;
  uint32_t prescaler;
  uint32_t timeout_ms;
  PWMI_CapturePrescaler_t capture_prescaler;
  INT_Prioryty_t int_prioryty;
  TIM_Filter_t filter;
  TIM_CHx_t channel[4];
  #if(PWMI_USED_TIM2)
    uint64_t reload[4];
    uint64_t value[4];
  #else
    uint32_t reload[4];
    uint32_t value[4];
  #endif
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

void PWMI_Init(PWMI_t *pwmi);
bool PWMI_Loop(PWMI_t *pwmi);

//-------------------------------------------------------------------------------------------------
#endif
