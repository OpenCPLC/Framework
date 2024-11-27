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
 * @param size Size of the stack in 32-bit words (minimum: 80)
 * @return True if the thread was successfully added, false if the thread limit is reached
 */
bool vrts_thread(void (*handler)(void), uint32_t *stack, uint16_t size)
{
  if(vrts.count >= VRTS_THREAD_LIMIT - 1) return false;
  VRTS_Task_t *thread = &vrts.threads[vrts.count];
  thread->handler = handler;
  thread->stack = (uint32_t)(stack + size - 16);
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
void vrts_init(void)
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
void vrts_lock(void)
{
  vrts.enabled = false;
}

/** 
 * @brief Enables thread switching if VRTS is initialized
 * @return True if thread switching was enabled, false otherwise
 */
bool vrts_unlock(void)
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
  vrts.i++;
  if(vrts.i >= vrts.count)
    vrts.i = 0;
  vrts_next_thread = &vrts.threads[vrts.i];
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
uint8_t vrts_active_thread(void)
{
  #if(VRTS_SWITCHING)
    return vrts.i;
  #else
    return 0;
  #endif
}

/**
 * @brief Returns the adjusted system tick with an offset.
 * @param offset_ms Milliseconds to add to the current tick.
 * @return Adjusted tick.
 *  @note Interoperates with 'tick_over', 'tick_away', 'delay_until', 'sleep_until' functions.
 */
uint64_t tick_keep(uint32_t offset_ms)
{
  return ticker + ((offset_ms + (tick_ms - 1)) / tick_ms);
}

/**
 * @brief Checks if an event time has passed.
 * @param tick Pointer to the event time tick.
 * @return true if passed, resets tick; false otherwise.
 */
bool tick_over(uint64_t *tick)
{
  if(!*tick || *tick > ticker) return false;
  *tick = 0;
  return true;
}

/**
 * @brief Checks if an event time is still to come.
 * @param tick Pointer to the future event time tick.
 * @return true if future, false if passed and resets tick.
 */
bool tick_away(uint64_t *tick)
{
  if(!*tick) return false;
  if(*tick > ticker) return true;
  *tick = 0;
  return false;
}

/** 
 * @brief Delays for the specified milliseconds.
 * Uses 'let()' to allow thread switching while waiting.
 * @param ms Milliseconds to delay
 */
void delay(uint32_t ms)
{
  uint64_t end = tick_keep(ms);
  while(end > ticker) let();
}

/** 
 * @brief Sleeps for the specified milliseconds.
 * Uses '__WFI()', so no thread switching occurs.
 * @param ms Milliseconds to sleep
 */
void sleep(uint32_t ms)
{
  uint64_t end = tick_keep(ms);
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
  uint64_t end = tick_keep(ms);
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
 * @brief Sleeps until the specified tick is reached
 * Uses '__WFI()', so no thread switching occurs
 * @param tick Pointer to the target tick count
 */
void sleep_until(uint64_t *tick)
{
  if(!*tick) return;
  while(*tick > ticker) __WFI();
  *tick = 0;
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
 * The accuracy of timing functions (e.g., 'sleep' and 'delay') will match this interval
 * @param systick_ms Interval duration in milliseconds
 * @return True if initialization is successful, false otherwise
 */
bool systick_init(uint32_t systick_ms)
{
  tick_ms = systick_ms;
  uint32_t overflow = (uint32_t)((float)tick_ms * SystemCoreClock / 1000);
  if(SysTick_Config(overflow)) return false;
  NVIC_SetPriority(SysTick_IRQn, 3);
  return true;
}

/** @brief SysTick interrupt handler to increment the global ticker */
void SysTick_Handler(void)
{
  ticker++;
}
