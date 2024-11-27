#include "sys.h"

void system_clock_init(void)
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
    #error "System clock frequency SYS_CLOCK_FREQ isn't supported!"
  #endif
}

void panic(const char *message)
{
  LOG_Panic(message);
  DBG_Wait4Uart();
  PWR_Reset();
}

static TIM_t *SystemSleepTimer;

void sleep_us_init(TIM_t *tim)
{
  SystemSleepTimer = tim;
  DELAY_Init(tim, TIM_BaseTime_1us);
}

void sleep_us(uint32_t us)
{
  DELAY_Wait(SystemSleepTimer, us);
}
