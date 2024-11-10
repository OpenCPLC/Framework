/**
 * @file  vrts.h 
 * @brief Voluntary Release Threads System (VRTS) Liblary: https://github.com/Xaeian/VRTS.
 *        Lightweight, cooperative multitasking library for multi-threaded embedded systems.
 *        Provides simple thread management without complex synchronization by using 
 *        voluntary thread release. Key features include timing functions (delay, sleep, timeout).
 *        Ideal for low-latency, resource-efficient applications on ARM Cortex-M microcontrollers.
 * @date  2024-11-09
 */

#ifndef VRTS_H_
#define VRTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32g0xx.h"
#include "main.h"

// Max number of threads if not defined
#ifndef VRTS_THREAD_LIMIT
  #define VRTS_THREAD_LIMIT 12
#endif

// Enable thread switching by default
#ifndef VRTS_SWITCHING
  #define VRTS_SWITCHING 1 
#endif

#define WAIT_ (bool (*)(void *)) // Type cast for timeout function

#define seconds(ms)  (1000 * ms) // Convert seconds to milliseconds
#define minutes(min) (60 * 1000 * min) // Convert minutes to milliseconds

// Enum for thread status
typedef enum {
  VRTS_Status_Idle = 1,
  VRTS_Status_Active = 2,
} VRTS_Status_e;

// Struct to represent a thread in VRTS
typedef struct {
  volatile uint32_t stack;
  void (*handler)(void);
  volatile VRTS_Status_e status;
} VRTS_Task_t;

bool thread(void (*handler)(void), uint32_t *stack, uint16_t size);
void let(void); // Yield control to the next thread
void delay(uint32_t ms); // Delay execution by ms
void sleep(uint32_t ms); // Sleep for ms without thread switching
bool timeout(uint32_t ms, bool (*Free)(void *), void *subject); // Timeout with condition check
uint64_t gettick(uint32_t offset_ms); // Get system tick with offset
void delay_until(uint64_t *tick); // Delay until specific tick
void sleep_until(uint64_t *tick); // Sleep until specific tick
bool waitfor(uint64_t *tick); // Check if tick is reached
int32_t watch(uint64_t tick); // Measure time since a specific tick

void VRTS_Init(void); // Initialize VRTS system
void VRTS_Lock(void); // Disable thread switching
bool VRTS_Unlock(void); // Enable thread switching if initialized
uint8_t VRTS_ActiveThread(void); // Get index of the active thread
bool SYSTICK_Init(uint32_t systick_ms); // Initialize SysTick with a specified interval

#endif
