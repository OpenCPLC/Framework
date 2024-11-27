#include "new.h"

static struct {
  #if(VRTS_SWITCHING)
    NEW_t *stacks[VRTS_THREAD_LIMIT]; // Stacks, one for each thread for multi-threading mode
  #else
    NEW_t *stacks[1]; // Single stack for single-threaded mode
  #endif
  uint16_t total; // Total allocated memory in bytes
  void *aloc_vars[ALOC_LIMIT]; // List of variables to be allocated using aloc
  uint16_t aloc_sizes[ALOC_LIMIT]; // List storing the size of each allocated space via aloc
} state;

//-------------------------------------------------------------------------------------------------

/**
 * @brief Permanently reserves memory.
 * Uses memory allocated specifically for the garbage collector.
 * @param size Size of the memory block in bytes.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void *new_static(size_t size)
{
  state.total += size;
  return malloc(size);
}

/**
 * @brief Sets up garbage-collector dynamic memory allocation for a thread
 * (works in single-threaded mode as well).
 * Creates a stack for variables and reserves space for them.
 * The function will be called automatically with a default limit if 
 * 'new' is used without prior initialization.
 * @param limit The maximum number of variables the thread can allocate.
 * @retval NEW_t* Pointer to the stack structure for the active thread.
 */
NEW_t *new_init(uint16_t limit)
{
  uint8_t active_thread = vrts_active_thread();
  NEW_t *stack = state.stacks[active_thread];
  if(stack) return stack;
  stack = new_static(sizeof(NEW_t));
  uint16_t var_space = sizeof(void *) * limit;
  state.total += var_space;
  stack->var = (void **)new_static(var_space);
  stack->count = 0;
  stack->limit = limit;
  stack->size = 0;
  state.stacks[active_thread] = stack;
  return stack;
}

/**
 * @brief Handles critical memory errors.
 * Disables interrupts and enters an infinite loop to halt execution.
 * @param error_code The error code indicating the type of memory error.
 */
static void MEM_ErrorHandler(int8_t error_code)
{
  // TODO: Implement critical error handling
  __disable_irq(); // Disable interrupts
  volatile uint32_t i = 0;
  while(1) i++; // Infinite loop to halt system
}

/**
 * @brief Reserves memory in the garbage-collector for the active thread.
 * Allocates a memory block of the specified size, adding it to the
 * current thread's stack.
 * @param size Size of the memory block in bytes.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void *new(size_t size)
{
  void *pointer = NULL;
  if(!size) return pointer;
  uint8_t active_thread = vrts_active_thread();
  NEW_t *stack = state.stacks[active_thread];
  if(!stack) {
    stack = new_init(NEW_DEFAULT_LIMIT);
  }
  if(stack->count >= stack->limit) MEM_ErrorHandler(NEW_ERROR_LIMIT);
  if((state.total + size) >= NEW_TOTAL) MEM_ErrorHandler(NEW_ERROR_SIZE);
  pointer = malloc(size);
  stack->var[stack->count] = pointer;
  stack->count++;
  stack->size += size;
  state.total += size;
  return pointer;
}

/**
 * @brief Clears memory allocated by 'new' for the active thread.
 * Frees each variable in the active thread's stack and resets the allocation count.
 */
void clear()
{
  uint8_t active_thread = vrts_active_thread();
  NEW_t *stack = state.stacks[active_thread];
  if(!stack) return;
  if(stack->count) {
    for(int i = 0; i < stack->count; i++)
      free(stack->var[i]);
    stack->count = 0;
    state.total -= stack->size;
    stack->size = 0;
  }
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Allocates a memory block and tracks it.
 * Checks limits, updates allocation list, and returns a pointer.
 * @param size Size in bytes.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void *aloc(size_t size)
{
  uint16_t i = 0;
  void *pointer = 0;
  if(!size) return pointer;
  if((state.total + size) >= NEW_TOTAL) MEM_ErrorHandler(NEW_ERROR_SIZE);
  while(state.aloc_vars[i]) {
    i++;
    if(i >= ALOC_LIMIT) MEM_ErrorHandler(ALOC_ERROR_SIZE);
  }
  pointer = malloc(size);
  state.aloc_vars[i] = pointer;
  state.aloc_sizes[i] = size;
  state.total += size;
  return pointer;
}

/**
 * @brief Deallocates memory for a specified pointer.
 * Frees memory, updates usage, and clears tracking info.
 * @param pointer Double pointer to the memory to free.
 */
void dloc(void **pointer)
{
  if(!pointer) return;
  uint16_t i = 0;
  while(i < ALOC_LIMIT) {
    if(*pointer == state.aloc_vars[i]) {
      free(*pointer);
      *pointer = NULL;
      state.total -= state.aloc_sizes[i];
      state.aloc_sizes[i] = 0;
      state.aloc_vars[i] = NULL;
      return;
    }
    i++;
  }
  MEM_ErrorHandler(DLOC_NOT_FOUND);
  return;
}

//-------------------------------------------------------------------------------------------------
