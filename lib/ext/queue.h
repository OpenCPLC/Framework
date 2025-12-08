#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------

/**
 * @brief Queue control structure (ring buffer with optional unique filter).
 * @param[in] buffer Pointer to buffer memory. Size = `capacity * struct_size` bytes.
 * @param[in] struct_size Element size in bytes.
 * @param[in] capacity Max number of elements (not bytes).
 * @param[in] unique When `true`, duplicate elements are rejected.
 * @param[in] invert If `true`, pop/peek from tail instead of head.
 * @param[in] Equal Optional equality predicate; returns `true` if `a` and `b` represent the same element.
 *   If `NULL`, uniqueness falls back to byte-wise comparison of the entire element.
 * @param[in] Compare Optional comparison function; returns `<0` if `a<b`, `0` if `a==b`, `>0` if `a>b`.
 *   If `NULL`, the queue behaves as a regular FIFO/LIFO (no sorting).
 * @param head Index of oldest element. [internal]
 * @param tail Index of next free slot. [internal]
 * @param count Current number of elements. [internal]
 */
typedef struct {
  void *buffer;
  uint16_t struct_size;
  uint16_t capacity;
  bool unique;
  bool invert;
  bool (*Equal)(const void *a, const void *b);
  int32_t (*Compare)(const void *a, const void *b);
  uint16_t head;
  uint16_t tail;
  uint16_t count;
} QUEUE_t;

//-------------------------------------------------------------------------------------------------

bool QUEUE_Push(QUEUE_t *queue, const void *element);
bool QUEUE_Pop(QUEUE_t *queue, void *element);
bool QUEUE_Peek(const QUEUE_t *queue, void *element);
bool QUEUE_IsEmpty(const QUEUE_t *queue);
bool QUEUE_IsFull(const QUEUE_t *queue);
uint16_t QUEUE_Count(const QUEUE_t *queue);
void QUEUE_Clear(QUEUE_t *queue);

//-------------------------------------------------------------------------------------------------
#endif