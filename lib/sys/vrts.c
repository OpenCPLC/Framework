#include "vrts.h"

static volatile uint64_t ticker; // global ticker
static uint32_t tick_ms; // time in ms for a single ticker tick

volatile VRTS_Task_t *vrts_now_thread; // Current thread
volatile VRTS_Task_t *vrts_next_thread; // Next thread

#if(VRTS_SWITCHING)

// Structure to manage threads in the VRTS system 
struct {
  VRTS_Task_t threads[VRTS_THREAD_LIMIT];
  uint32_t i; // Active thread
  uint32_t count; // Thread count
  bool enabled; // Switching VRTS enabled flag
  bool init; // VRTS initialization flag
} vrts;

/**
 * @brief Handles end of thread execution
 */
static void VRTS_TaskFinished(void)
{
  volatile uint32_t i = 0;
  while(1) i++;
}

/**
 * @brief Adds a new thread to the VRTS system
 * @param handler Function pointer to the thread's main function
 * @param stack Pointer to the memory allocated for the thread's stack
 * @param size Size of the stack in 32-bit words
 * @return True if the thread was successfully added, false if the thread limit is reached
 */
bool thread(void (*handler)(void), uint32_t *stack, uint16_t size)
{
  if(vrts.count >= VRTS_THREAD_LIMIT - 1) return false;
  VRTS_Task_t *thread = &vrts.threads[vrts.count];
  thread->handler = handler;
  thread->stack = (uint32_t)(stack + size - 16);
  thread->status = VRTS_Status_Idle;
  stack[size - 1] = 0x01000000; // XPSR: Default value
  stack[size - 2] = (uint32_t)handler; // PC: Point to the handler function
  // LR: Point to a function to be called when the handler returns
  stack[size - 3] = (uint32_t)&VRTS_TaskFinished;
  // Next: R12, R3, R2, R1, R0, R7, R6, R5, R4, R11, R10, R9, R8
  vrts.count++;
  return true;
}

/**
 * @brief Initializes the VRTS system and starts the first thread
 */
void VRTS_Init(void)
{
  NVIC_SetPriority(PendSV_IRQn, 3);
  vrts_now_thread = &vrts.threads[vrts.i];
  __set_PSP(vrts_now_thread->stack + 64); // Set PSP to the top of thread's stack
  __set_CONTROL(0x02); // Switch to PSP, privileged mode
  __ISB(); // Exec. ISB after changing CONTORL (recommended)
  vrts.enabled = true;
  vrts.init = true;
  vrts_now_thread->handler();
}

/** @brief Disables thread switching by setting the enabled flag to false */
void VRTS_Lock(void)
{
  vrts.enabled = false;
}

/** 
 * @brief Enables thread switching if VRTS is initialized
 * @return True if thread switching was enabled, false otherwise
 */
bool VRTS_Unlock(void)
{
  if(!vrts.init) return false;
  vrts.enabled = true;
  return true;
}

/**
 * @brief Yields control to the next thread in the schedule
 */
void let(void)
{
  if(!vrts.enabled) return;
  vrts_now_thread = &vrts.threads[vrts.i];
  vrts_now_thread->status = VRTS_Status_Idle;
  vrts.i++;
  if(vrts.i >= vrts.count)
    vrts.i = 0;
  vrts_next_thread = &vrts.threads[vrts.i];
  vrts_next_thread->status = VRTS_Status_Active;
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

#else
void let(void)
{
  __WFI();
}
#endif

/** 
 * @brief Gets the index of the currently active thread 
 * @return Index of the current thread
 */
uint8_t VRTS_ActiveThread(void)
{
  #if(VRTS_SWITCHING)
    return vrts.i;
  #else
    return 0;
  #endif
}

/**
 * @brief Returns the current system tick with an added offset
 * @param offset_ms Offset in milliseconds to add to the tick
 * @return The system tick plus the specified offset
 */
uint64_t gettick(uint32_t offset_ms)
{
  return ticker + ((offset_ms + (tick_ms - 1)) / tick_ms);
}

/** 
 * @brief Delays for the specified milliseconds.
 * Uses 'let()' to allow thread switching while waiting.
 * @param ms Milliseconds to delay
 */
void delay(uint32_t ms)
{
  uint64_t end = gettick(ms);
  while(end > ticker) let();
}

/** 
 * @brief Sleeps for the specified milliseconds.
 * Uses '__WFI()', so no thread switching occurs.
 * @param ms Milliseconds to sleep
 */
void sleep(uint32_t ms)
{
  uint64_t end = gettick(ms);
  while(end > ticker) __WFI();
}

/**
 * @brief Checks a condition repeatedly until timeout or condition met
 * @param ms Timeout duration in milliseconds
 * @param Free Function pointer that checks the condition
 * @param subject Pointer to data for condition checking
 * @return True if timed out, false if condition met
 */
bool timeout(uint32_t ms, bool (*Free)(void *), void *subject)
{
  uint64_t end = gettick(ms);
  while(end > ticker) {
    if(Free(subject)) {
      return false;
    }
    let();
  }
  return true;
}

/** 
 * @brief Delays until the specified tick is reached.
 * Uses 'let()' to allow thread switching while waiting.
 * @param tick Pointer to the target tick count
 */
void delay_until(uint64_t *tick)
{
  if(!*tick) return;
  while(*tick > ticker) let();
  *tick = 0;
}

/** 
 * @brief Sleeps until the specified tick is reached.
 * Uses '__WFI()', so no thread switching occurs.
 * @param tick Pointer to the target tick count
 */
void sleep_until(uint64_t *tick)
{
  if(!*tick) return;
  while(*tick > ticker) __WFI();
  *tick = 0;
}

/**
 * @brief Checks if the specified tick has been reached
 * @param tick Pointer to the target tick count
 * @return True if tick reached, false otherwise
 */
bool waitfor(uint64_t *tick)
{
  if(!*tick || *tick > ticker) return false;
  *tick = 0;
  return true;
}

/**
 * @brief Measures the time elapsed since a specified tick
 * @param tick Reference tick to measure from
 * @return Elapsed time in milliseconds
 */
int32_t watch(uint64_t tick)
{
  return (int32_t)(((int64_t)ticker - tick) * tick_ms) ;
}

/**
 * @brief Initializes SysTick with a specified interval
 * @param systick_ms Interval duration in milliseconds
 * @return True if initialization successful, false otherwise
 */
bool SYSTICK_Init(uint32_t systick_ms)
{
  tick_ms = systick_ms;
  uint32_t overflow = (uint32_t)((float)tick_ms * SystemCoreClock / 1000);
  if(SysTick_Config(overflow) != 0) return false;
  NVIC_SetPriority(SysTick_IRQn, 3);
  return true;
}

/** @brief SysTick interrupt handler to increment the global ticker */
void SysTick_Handler(void)
{
  ticker++;
}
