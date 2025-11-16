#include "queue.h"

static void QUEUE_Swap(void *a, void *b, size_t n) 
{
  unsigned char *pa = (unsigned char*)a, *pb = (unsigned char*)b;
  for(size_t i = 0; i < n; i++) { unsigned char t = pa[i]; pa[i] = pb[i]; pb[i] = t; }
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
      const void *item = (char *)queue->buffer + (uint32_t)idx * queue->struct_size;
      bool same = queue->Equal ? queue->Equal(item, element) : memcmp(item, element, queue->struct_size) == 0;
      if(same) return false;
    }
  }
  void *dest = (char *)queue->buffer + (uint32_t)queue->tail * queue->struct_size;
  memcpy(dest, element, queue->struct_size);
  uint16_t cur = queue->tail;
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count++;
  if(queue->Compare) {
    while(cur != queue->head) {
      uint16_t prev = (uint16_t)((cur + queue->capacity - 1) % queue->capacity);
      void *a = (char *)queue->buffer + (uint32_t)prev * queue->struct_size;
      void *b = (char *)queue->buffer + (uint32_t)cur * queue->struct_size;
      if(queue->Compare(a, b) >= 0) break;
      QUEUE_Swap(a, b, queue->struct_size);
      cur = prev;
    }
  }
  return true;
}

/**
 * @brief Pop one element.
 *   Without `Compare`: FIFO (or LIFO if `invert` is set).
 *   With Compare: returns the largest (or the smallest if invert==true).
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param element Pointer to memory where element will be copied.
 * @return `true` if element copied, `false` if queue empty.
 */
bool QUEUE_Pop(QUEUE_t *queue, void *element)
{
  if(queue->count == 0) return false;
  if(queue->invert) {
    uint16_t idx = (uint16_t)((queue->tail + queue->capacity - 1) % queue->capacity);
    void *src = (char *)queue->buffer + (size_t)idx * queue->struct_size;
    memcpy(element, src, queue->struct_size);
    queue->tail = idx;
  }
  else {
    void *src = (char *)queue->buffer + (size_t)queue->head * queue->struct_size;
    memcpy(element, src, queue->struct_size);
    queue->head = (uint16_t)((queue->head + 1) % queue->capacity);
  }
  queue->count--;
  return true;
}

/**
 * @brief Peek element from queue without removing it.
 *   Without `Compare`: FIFO (or LIFO if `invert` is set).
 *   With Compare: returns the largest (or the smallest if invert==true).
 * @param queue Pointer to `QUEUE_t` control structure.
 * @param element Pointer to memory where element will be copied.
 * @return `true` if first element exists, `false` if `queue` is empty.
 */
bool QUEUE_Peek(const QUEUE_t *queue, void *element)
{
  if(queue->count == 0) return false;
  if(queue->invert) {
    uint16_t idx = (uint16_t)((queue->tail + queue->capacity - 1) % queue->capacity);
    const void *src = (const char *)queue->buffer + (size_t)idx * queue->struct_size;
    memcpy(element, src, queue->struct_size);
  }
  else {
    const void *src = (const char *)queue->buffer + (size_t)queue->head * queue->struct_size;
    memcpy(element, src, queue->struct_size);
  }
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

/**
 * @brief Clear queue to an empty state (buffer contents are left intact).
 * @param[in,out] queue Pointer to `QUEUE_t` control structure.
 */
void QUEUE_Clear(QUEUE_t *queue)
{
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0;
}