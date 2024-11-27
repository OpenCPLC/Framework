#include "pwm.h"

void PWM_SetPrescaler(PWM_t *pwm, uint32_t prescaler)
{
  if(!prescaler) prescaler = 1;
  pwm->prescaler = prescaler;  
  pwm->reg->PSC = pwm->prescaler - 1;
}

void PWM_SetAutoreload(PWM_t *pwm, uint32_t auto_reload)
{
  if(!auto_reload) return;
  pwm->auto_reload = auto_reload;
  pwm->reg->ARR = pwm->auto_reload;
}

void PWM_SetValue(PWM_t *pwm, TIM_Channel_t channel, uint32_t value)
{
  switch(channel) {
    case TIM_CH1: case TIM_CH1N: pwm->value[TIM_CH1] = value; pwm->reg->CCR1 = value; break;
    case TIM_CH2: case TIM_CH2N: pwm->value[TIM_CH2] = value; pwm->reg->CCR2 = value; break;
    case TIM_CH3: case TIM_CH3N: pwm->value[TIM_CH3] = value; pwm->reg->CCR3 = value; break;
    case TIM_CH4: case TIM_CH4N: pwm->value[TIM_CH4] = value; pwm->reg->CCR4 = value; break;
  }
}

void PWM_SetDeadtime(PWM_t *pwm, uint32_t deadtime)
{
  if(deadtime >= 1024) deadtime = 1024;
  uint8_t dtg, temp;
  if(deadtime < 128) {
    dtg = deadtime;
    pwm->deadtime = deadtime;
  }
  else if(deadtime < 256) {
    temp = ((deadtime - 128) / 2);
    dtg = (0b10 << 6) | temp;
    pwm->deadtime = (uint16_t)temp * 2 + 128;
  }
  else if(deadtime < 512) {
    temp = ((deadtime - 256) / 8);
    dtg = (0b110 << 5) | temp;
    pwm->deadtime = (uint16_t)temp * 8 + 256;
  }
  else {
    temp = ((deadtime - 512) / 16);
    dtg = (0b111 << 5) | temp;
    pwm->deadtime = (uint16_t)temp * 16 + 512;
  }
  pwm->reg->BDTR = (pwm->reg->BDTR & ~TIM_BDTR_DTG_Msk) | dtg;
}

static void PWM_ChannelInit(PWM_t *pwm, TIM_Channel_t channel)
{
  bool invert_pos = pwm->invert[channel];
  bool invert_neg = pwm->invert[channel + 4];
  RCC_EnableTIM(pwm->reg);
  pwm->reg->CCER |= ((invert_neg << 3) | TIM_CCER_CC1NE | (invert_pos << 1) | TIM_CCER_CC1E) << (4 * channel);
  PWM_SetValue(pwm, channel, pwm->value[channel]);
  switch(channel) {
    case TIM_CH1: pwm->reg->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; break;
    case TIM_CH2: pwm->reg->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; break;
    case TIM_CH3: pwm->reg->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; break;
    case TIM_CH4: pwm->reg->CCMR2 |= TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1; break;
    default: break;
  }
}

void PWM_Init(PWM_t *pwm)
{
  bool init;
  pwm->reg->EGR &= ~TIM_EGR_UG;
  pwm->reg->CR1 &= ~TIM_CR1_CEN;
  pwm->reg->CCER = 0;
  pwm->reg->CCMR1 = 0;
  pwm->reg->CCMR2 = 0;
  for(uint8_t i = 0; i < 4; i++) {
    init = false;
    if(pwm->channel[i]) { GPIO_AlternateInit(&TIM_CHx_MAP[pwm->channel[i]], false); init = true; }
    if(pwm->channel[i + 4]) { GPIO_AlternateInit(&TIM_CHx_MAP[pwm->channel[i + 4]], false); init = true; }
    if(init) PWM_ChannelInit(pwm, i);
  }
  PWM_SetPrescaler(pwm, pwm->prescaler);
  pwm->reg->ARR = pwm->auto_reload;
  pwm->reg->CR1 = TIM_CR1_ARPE | (pwm->center_aligned << 6) | (pwm->center_aligned << 5);
  PWM_SetDeadtime(pwm, pwm->deadtime);
  pwm->reg->DIER &= ~TIM_DIER_UIE;
  pwm->reg->BDTR |= TIM_BDTR_MOE;
  pwm->reg->EGR |= TIM_EGR_UG;
  pwm->reg->CR1 |= TIM_CR1_CEN;
}
