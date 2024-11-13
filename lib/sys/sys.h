#ifndef SYS_H_
#define SYS_H_

#include "dbg.h"
#include "pwr.h"
#include "main.h"

#ifndef SYS_CLOCK_FREQ
  #define SYS_CLOCK_FREQ 16000000
#endif

void SYS_Clock_Init(void);
void panic(const char *message);

#endif