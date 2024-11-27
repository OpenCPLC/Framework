#include "pwmi.h"
#include "dbg.h"

static void PWMI_Interrupt(PWMI_t *pwmi)
{
  if(pwmi->reg->SR & TIM_SR_TIF) PWMI_Run(pwmi);
}

static const uint16_t PWMI_POW2[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 9192 };

static float PWMI_GetTimeoutMaxMs(PWMI_t *pwmi)
{
  uint32_t auto_reload;
  if(pwmi->reg == TIM2) auto_reload = 0xFFFFFFFF;
  else auto_reload = 0xFFFF;
  return ((float)pwmi->prescaler + 1) * PWMI_POW2[pwmi->input_prescaler] * auto_reload * 1000 / SystemCoreClock;
}

static void PWMI_Begin(PWMI_t *pwmi)
{
  RCC_EnableTIM(pwmi->reg);
  pwmi->reg->PSC = pwmi->prescaler;
  pwmi->reg->ARR = 0xFFFF;
  if(pwmi->channel[TIM_CH3] && pwmi->trig3) {
    pwmi->trig3->port = TIM_CHx_MAP[pwmi->channel[TIM_CH3]].port;
    pwmi->trig3->pin = TIM_CHx_MAP[pwmi->channel[TIM_CH3]].pin;
    pwmi->trig3->rise = true;
    pwmi->trig3->fall = false;
    pwmi->trig3->interrupt_level = pwmi->interrupt_level;
    pwmi->trig3->rise_function = (void (*)(void *))&PWMI_Run;
    pwmi->trig3->rise_struct = pwmi;
    EXTI_Init(pwmi->trig3);
  }
  if(pwmi->channel[TIM_CH4] && pwmi->trig4) {
    pwmi->trig4->port = TIM_CHx_MAP[pwmi->channel[TIM_CH4]].port;
    pwmi->trig4->pin = TIM_CHx_MAP[pwmi->channel[TIM_CH4]].pin;
    pwmi->trig4->rise = true;
    pwmi->trig4->fall = false;
    pwmi->trig4->interrupt_level = pwmi->interrupt_level;
    pwmi->trig3->rise_function = (void (*)(void *))&PWMI_Run;
    pwmi->trig3->rise_struct = pwmi;
    EXTI_Init(pwmi->trig4);
  }
  for(uint8_t i = TIM_CH1; i < 4; i++)
    if(pwmi->channel[i]) GPIO_AlternateInit(&TIM_CHx_MAP[pwmi->channel[i]], true);
  INT_EnableTIM(pwmi->reg, pwmi->interrupt_level, (void (*)(void *))&PWMI_Interrupt, pwmi);
  if(!pwmi->oversampling) pwmi->oversampling = 1;
  if(!pwmi->timeout_ms) pwmi->timeout_ms = PWMI_GetTimeoutMaxMs(pwmi);
}

static void PWMI_Reset(PWMI_t *pwmi)
{
  pwmi->reg->CR1 |= TIM_CR1_CEN;
  pwmi->reg->DIER |= TIM_DIER_TIE;
  pwmi->count = 0;
  pwmi->inc = 0;
  for(uint8_t i = 0; i < 4; i++) {
    pwmi->reload[i] = 0;
    pwmi->value[i] = 0;
  }
}

uint8_t PWMI_Run(PWMI_t *pwmi)
{
  pwmi->watchdog = gettick(pwmi->timeout_ms);
  if(pwmi->count == pwmi->oversampling) pwmi->inc++;
  else {
    switch(pwmi->inc) {
      case (TIM_CH1 + 1): pwmi->reload[TIM_CH1] += pwmi->reg->CCR1; pwmi->value[TIM_CH1] += pwmi->reg->CCR2; break;
      case (TIM_CH2 + 1): pwmi->reload[TIM_CH2] += pwmi->reg->CCR2; pwmi->value[TIM_CH2] += pwmi->reg->CCR1; break;
      case (TIM_CH3 + 1): pwmi->reload[TIM_CH3] += pwmi->reg->CCR3; pwmi->value[TIM_CH3] += pwmi->reg->CCR4; break;
      case (TIM_CH4 + 1): pwmi->reload[TIM_CH4] += pwmi->reg->CCR4; pwmi->value[TIM_CH4] += pwmi->reg->CCR3; break;
    }
  }
  pwmi->reg->CNT = 0;
  if(pwmi->count) pwmi->count--;
  else {
    while(!pwmi->channel[pwmi->inc] && pwmi->inc < 4) pwmi->inc += 1;
    switch(pwmi->inc) {
      case TIM_CH1:
        pwmi->reg->CCER = 0;
        pwmi->reg->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
        pwmi->reg->CCMR2 = 0;
        pwmi->reg->CCER = TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC2E;
        pwmi->reg->SMCR = TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;
        pwmi->count = pwmi->oversampling;
        break;
      case TIM_CH2:
        pwmi->reg->CCER = 0;
        pwmi->reg->CCMR1 = TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC2S_0;
        pwmi->reg->CCMR2 = 0;
        pwmi->reg->CCER = TIM_CCER_CC1P | TIM_CCER_CC1E | TIM_CCER_CC2E;
        pwmi->reg->SMCR = TIM_SMCR_TS_2 | TIM_SMCR_TS_1 | TIM_SMCR_SMS_2;
        pwmi->count = pwmi->oversampling;
        break;
      case TIM_CH3:
        pwmi->reg->CCER = 0;
        pwmi->reg->CCMR1 = 0;
        pwmi->reg->CCMR2 = TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_1;
        pwmi->reg->CCER = TIM_CCER_CC4P | TIM_CCER_CC3E | TIM_CCER_CC4E;
        pwmi->count = pwmi->oversampling;
        if(pwmi->trig3) EXTI_On(pwmi->trig3);
        break;
      case TIM_CH4:
        if(pwmi->trig3) EXTI_Off(pwmi->trig3);
        pwmi->reg->CCER = 0;
        pwmi->reg->CCMR1 = 0;
        pwmi->reg->CCMR2 = TIM_CCMR2_CC3S_1 | TIM_CCMR2_CC4S_0;
        pwmi->reg->CCER = TIM_CCER_CC3P | TIM_CCER_CC3E | TIM_CCER_CC4E;
        pwmi->count = pwmi->oversampling;
        if(pwmi->trig4) EXTI_On(pwmi->trig4);
        break;
      default:
        pwmi->reg->DIER &= ~TIM_DIER_TIE;
        pwmi->reg->CR1 &= ~TIM_CR1_CEN;
        if(pwmi->trig3) EXTI_Off(pwmi->trig3);
        if(pwmi->trig4) EXTI_Off(pwmi->trig4);
        pwmi->inc = 0;
        return 0;
    }
  }
  pwmi->reg->SR &= ~TIM_SR_TIF;
  return pwmi->count;
}

static void PWMI_Skip(PWMI_t *pwmi)
{
  uint8_t inc = pwmi->inc;
  if(pwmi->count != pwmi->oversampling) inc--;
  while(PWMI_Run(pwmi));
  PWMI_Run(pwmi);
  pwmi->reload[inc] = 0;
  pwmi->value[inc] = 0;
}

static bool PWMI_IsInterrupt(PWMI_t *pwmi)
{
  if(pwmi->reg->DIER & TIM_DIER_TIE) {
    return true;
  }
  else return false;
}

static float PWMI_GetFrequency(PWMI_t *pwmi, TIM_Channel_t channel)
{
  float frequency = 0;
  if(pwmi->reload[channel])
    frequency = (float)SystemCoreClock * pwmi->oversampling/ (pwmi->prescaler + 1) / PWMI_POW2[pwmi->input_prescaler] / pwmi->reload[channel];
  return frequency;
}

static float PWMI_GetFill(PWMI_t *pwmi, TIM_Channel_t channel)
{
  float fill = 0;
  if(pwmi->reload[channel])
    fill = (float)pwmi->value[channel] / pwmi->reload[channel];
  return fill;
}

//---------------------------------------------------------------------

bool pwmi_begin;

void PWMI_Init(PWMI_t *pwmi)
{
  //if(PWMI_IsInterrupt(pwmi)) return ERR;
  PWMI_Begin(pwmi);
  PWMI_Reset(pwmi);
  PWMI_Run(pwmi);
  pwmi_begin = true;
}

bool PWMI_Loop(PWMI_t *pwmi)
{
  if(!pwmi_begin) return false;
  if(waitfor(&pwmi->watchdog)) PWMI_Skip(pwmi);
  if(!PWMI_IsInterrupt(pwmi)) {
    for(uint8_t i = 0; i < 4; i++) {
      pwmi->frequency[i] = PWMI_GetFrequency(pwmi, i);
      pwmi->fill[i] = PWMI_GetFill(pwmi, i);
    }
    PWMI_Reset(pwmi);
    PWMI_Run(pwmi);
    return true;
  }
  return false;
}

void PWMI_Print(PWMI_t *pwmi)
{
  for(uint8_t i = 0; i < 4; i++) {
    if(!pwmi->channel[i]) continue;
    DBG_String("PWMI CH"); DBG_uDec(i + 1); DBG_Char(' ');
    DBG_Float(pwmi->frequency[i], 2);  DBG_String("Hz ");
    DBG_Float(pwmi->fill[i], 2);  DBG_Char('%');
    DBG_Enter();
  }
}

//-------------------------------------------------------------------------------------------------
