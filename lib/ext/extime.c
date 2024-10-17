#include "extime.h"

void sleep_core_us(uint32_t us)
{
  #if SYS_CLOCK_FREQ == 16000000
    if (us <= 1) return;
    if (us == 2) return;
    us = (us << 2) + us + (us >> 2) + (us >> 4) + (us >> 6);
    us -= 13;
  #endif
  __asm__ __volatile__ (
    ".syntax unified\n\t"
    "1: subs %0, %0, #1\n\t"
    "bne 1b" : "+l" (us)
  );
}

void DELAY_Init(TIM_t *tim, BaseTime_e base_time)
{
  tim->_reserve = base_time;
  TIM_SetPrescaler(tim, (SystemCoreClock / base_time) - 1);
  TIM_Init(tim);
}

void DELAY_Wait(TIM_t *tim, uint32_t value)
{
  tim->reg->ARR = value;
  tim->reg->CR1 = TIM_CR1_CEN;
  while(!tim->reg->SR);
  tim->reg->SR = 0;
}

TIM_t *sleep_us_tim;

void sleep_us_init(TIM_t *tim)
{
  sleep_us_tim = tim;
  DELAY_Init(tim, BaseTime_1us);
}

void sleep_us(uint32_t us)
{
  DELAY_Wait(sleep_us_tim, us);
}