#include "sys.h"

void sys_clock_init(void)
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
  #if(SYS_PANIC_HANDLER)
    SYS_PANIC_HANDLER()
  #endif
  LOG_Panic(message);
  __disable_irq(); // Disable interrupts
  #if(SYS_PANIC_RESET)
    PWR_Reset();
  #endif
  volatile uint32_t i = 0;
  while(1) i++; // Infinite loop to halt system
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

volatile static uint32_t memory_guard = 0xA5A5DEAD;

void sys_memory_guard(void)
{
  while(1) {
    if(memory_guard != 0xA5A5DEAD) {
      panic("Memory corruption " LOG_LIB("SYS"));
    }
    delay(500);
  }
}