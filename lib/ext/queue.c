#include "queue.h"

/**
 * @brief Helper to initialize `QUEUE_t` structure.
 * This function does the same as manual struct init.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param buffer Pointer to external buffer memory. Must fit capacity `* elem_size` bytes.
 * @param capacity Max number of elements in queue.
 * @param elem_size Size of single element in bytes.
 */
inline void QUEUE_Init(QUEUE_t *queue, void *buffer, uint16_t capacity, uint16_t elem_size, bool unique)
{
  queue->buffer = buffer;
  queue->capacity = capacity;
  queue->elem_size = elem_size;
  queue->unique = unique;
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0;
}

/**
 * @brief Push element to queue if not full, skip if duplicate when unique enabled.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param element Pointer to element to add.
 * @return `true` if element was added, `false` if queue is full or duplicate.
 */
bool QUEUE_Push(QUEUE_t *queue, const void *element)
{
  if(queue->count == queue->capacity) return false;
  if(queue->unique) {
    for(uint16_t i = 0; i < queue->count; i++) {
      uint16_t idx = (queue->head + i) % queue->capacity;
      const void *item = (const char*)queue->buffer + idx * queue->elem_size;
      if(memcmp(item, element, queue->elem_size) == 0) return false;
    }
  }
  void *dest = (char*)queue->buffer + queue->tail * queue->elem_size;
  memcpy(dest, element, queue->elem_size);
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count++;
  return true;
}

/**
 * @brief Pop first element from queue (FIFO).
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param element Pointer to memory where element will be copied.
 * @return `true` if element copied, `false` if queue empty.
 */
bool QUEUE_Pop(QUEUE_t *queue, void *element)
{
  if(queue->count == 0) return false;
  void *src = (char*)queue->buffer + queue->head * queue->elem_size;
  memcpy(element, src, queue->elem_size);
  queue->head = (queue->head + 1) % queue->capacity;
  queue->count--;
  return true;
}

/**
 * @brief Peek first element from queue (FIFO) without removing it.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param element Pointer to memory where element will be copied.
 * @return `true` if first element exists, `false` if `queue` is empty.
 */
bool QUEUE_Peek(const QUEUE_t *queue, void *element)
{
  if(queue->count == 0) return false;
  const void *src = (const char*)queue->buffer + queue->head * queue->elem_size;
  memcpy(element, src, queue->elem_size);
  return true;
}

/**
 * @brief Check if queue is empty.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @return `true` if empty, `false` otherwise.
 */
bool QUEUE_IsEmpty(const QUEUE_t *queue)
{
  return (queue->count == 0);
}

/**
 * @brief Check if queue is full.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @return `true` if full, `false` otherwise.
 */
bool QUEUE_IsFull(const QUEUE_t *queue)
{
  return (queue->count == queue->capacity);
}

/**
 * @brief Get number of elements in queue.
 * @param queue Pointer to `QUEUE_t` control structure.
 * @return Current number of elements.
 */
uint16_t QUEUE_Count(const QUEUE_t *queue)
{
  return queue->count;
}
