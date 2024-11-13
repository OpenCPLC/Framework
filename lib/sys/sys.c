#include "sys.h"

void SYS_Clock_Init(void)
{
  #if(SYS_CLOCK_FREQ == 16000000)
    RCC_16MHz();
  #elif(SYS_CLOCK_FREQ == 48000000)
    RCC_48MHz();
  #elif(SYS_CLOCK_FREQ == 64000000)
    RCC_64MHz();
  #elif(SYS_CLOCK_FREQ == 18432000)
    RCC_HSE(18432000);
  #elif(SYS_CLOCK_FREQ == 59904000)
    RCC_PLL(18432000, 2, 13, 2);
  #else
    #error "System clock frequency SYS_CLOCK_FREQ isn't supported"
  #endif
}

void panic(const char *message)
{
  LOG_Panic(message);
  DBG_BeforeReset();
  PWR_Reset();
}
