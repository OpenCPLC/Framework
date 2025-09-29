#ifndef SYS_H_
#define SYS_H_

#include "tim.h"
#include "log.h"
#include "main.h"

#ifndef SYS_CLOCK_FREQ
  #define SYS_CLOCK_FREQ 16000000
#endif

#ifndef SYS_PANIC_RESET
  #define SYS_PANIC_RESET 0
#endif

void sys_clock_init(void);
void sys_memory_guard(void);
void panic(const char *message);
void panic_hook(void (*handler)(void));
void sleep_us_init(TIM_t *tim);
void sleep_us(uint32_t us);

#endif