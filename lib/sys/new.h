/**
 * @file  new.h
 * @brief Library for safer dynamic memory allocation.
 *        Defines limits for dynamic allocation, returning an error if exceeded.
 *        The library works with the VRTS multi-threading system: https://github.com/Xaeian/VRTS.
 *        Provides 'new' for memory allocation and 'clear' to free all allocated variables 
 *        in the current thread within a garbage collector system.
 *        Additionally, functions 'aloc' and 'dloc' allow allocating and freeing memory 
 *        using the same memory pool.
 * @date  2024-11-09
 */

#ifndef NEW_H_
#define NEW_H_

#include <stdlib.h>
#include <stdint.h>
#include "main.h"
#include "vrts.h"

//-------------------------------------------------------------------------------------------------

// Total allocated memory limit
#ifndef NEW_TOTAL
  #define NEW_TOTAL 8000 
#endif

// Variables limit allocated by the 'new' function
#ifndef NEW_DEFAULT_LIMIT
  #define NEW_DEFAULT_LIMIT 32
#endif

// Variables limit allocated by the 'aloc' function
#ifndef ALOC_LIMIT
  #define ALOC_LIMIT 16 
#endif

#define NEW_OK 0
#define NEW_ERROR_SIZE -1
#define NEW_ERROR_LIMIT -2
#define ALOC_ERROR_SIZE -3
#define DLOC_NOT_FOUND -3

//-------------------------------------------------------------------------------------------------

// Memory stack for garbage collector, one per thread
typedef struct {
  void **var; // Pointers to dynamically allocated variables
  uint16_t count; // Current number of variables
  uint16_t limit; // Maximum number of variables allowed
  uint16_t size; // Total memory allocated by this stack
} NEW_t;

NEW_t *new_init(uint16_t limit); // Initialize memory with a limit on variable count
void *new_static(size_t size); // Allocate static memory block
void *new(size_t size); // Allocate dynamic memory for current thread
void clear(); // Clear all allocated memory for current thread
void *aloc(size_t size); // Allocate memory with tracking
void dloc(void **pointer); // Deallocate memory and update tracking

//-------------------------------------------------------------------------------------------------

#endif
