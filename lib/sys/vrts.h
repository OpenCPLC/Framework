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

// Struct to represent a thread in VRTS
typedef struct {
  volatile uint32_t stack;
  void (*handler)(void);
} VRTS_Task_t;


void let(void); // Yield control to the next thread
void delay(uint32_t ms); // Delay execution by ms
void sleep(uint32_t ms); // Sleep for ms without thread switching
bool timeout(uint32_t ms, bool (*Free)(void *), void *subject); // Timeout with condition check
uint64_t gettick(uint32_t offset_ms); // Get system tick with offset
void delay_until(uint64_t *tick); // Delay until specific tick
void sleep_until(uint64_t *tick); // Sleep until specific tick
bool waitfor(uint64_t *tick); // Check if tick is reached
int32_t watch(uint64_t tick); // Measure time since a specific tick

bool vrts_thread(void (*handler)(void), uint32_t *stack, uint16_t size);
#define stack(name, size) static uint32_t name[8 * ((size + 7) / 8)] __attribute__((aligned(8)))
#define thread(fnc, stack_name) vrts_thread(&fnc, (uint32_t *)stack_name, sizeof(stack_name) / sizeof(uint32_t));

void vrts_init(void); // Initialize VRTS system
void vrts_lock(void); // Disable thread switching
bool vrts_unlock(void); // Enable thread switching if initialized
uint8_t vrts_active_thread(void); // Get index of the active thread
bool systick_init(uint32_t systick_ms); // Initialize SysTick with a specified interval

#endif
