#ifndef SYS_H_
#define SYS_H_

#include "log.h"
#include "pwr.h"
#include "main.h"

#ifndef SYS_CLOCK_FREQ
  #define SYS_CLOCK_FREQ 16000000
#endif

void system_clock_init(void);
void panic(const char *message);
void sleep_us_init(TIM_t *tim);
void sleep_us(uint32_t us);

#endif