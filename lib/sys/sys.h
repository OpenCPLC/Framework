#ifndef SYS_H_
#define SYS_H_

#include "log.h"
#include "pwr.h"
#include "main.h"

#ifndef SYS_CLOCK_FREQ
  #define SYS_CLOCK_FREQ 16000000
#endif

#ifndef SYS_PANIC_RESET
  #define SYS_PANIC_RESET 0
#endif

#ifndef SYS_PANIC_HANDLER
  #define SYS_PANIC_HANDLER 0
#endif

void sys_clock_init(void);
void panic(const char *message);
void sleep_us_init(TIM_t *tim);
void sleep_us(uint32_t us);
void sys_memory_guard(void);

#endif