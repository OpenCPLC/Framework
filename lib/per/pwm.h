#ifndef PWM_H_
#define PWM_H_

#include "gpio.h"
#include "tim.h"

//-------------------------------------------------------------------------------------------------

typedef struct {
  TIM_TypeDef *reg;
  uint32_t prescaler;
  uint32_t auto_reload;
  TIM_CHx_t channel[8];
  bool invert[8];
  uint32_t value[4];
  bool center_aligned;
  uint16_t deadtime; // 0-1024 [tick]
} PWM_t;

void PWM_SetPrescaler(PWM_t *pwm, uint32_t prescaler);
void PWM_SetAutoreload(PWM_t *pwm, uint32_t auto_reload);
void PWM_SetValue(PWM_t *pwm, TIM_Channel_t channel, uint32_t value);
void PWM_SetDeadtime(PWM_t *pwm, uint32_t deadtime);
void PWM_Init(PWM_t *pwm);

//-------------------------------------------------------------------------------------------------
#endif
