#ifndef EXTIME_H_
#define EXTIME_H_

#include <stdint.h>
#include "tim.h"
#include "main.h"

typedef enum {
  BaseTime_1us = 1000000,
  BaseTime_10us = 100000,
  BaseTime_100us = 10000,
  BaseTime_1ms = 1000,
  BaseTime_10ms = 100 // TIM2 only!
} BaseTime_e;


void sleep_core_us(uint32_t us);
void sleep_us_init(TIM_t *tim);
void sleep_us(uint32_t us);

void DELAY_Init(TIM_t *tim, BaseTime_e base_time);
void DELAY_Wait(TIM_t *tim, uint32_t value);

// TODO: Watch

#endif