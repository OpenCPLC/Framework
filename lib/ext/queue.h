#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------

/**
 * @brief Queue control structure (ring buffer with optional unique filter).
 * @param buffer Pointer to buffer memory. Size = `capacity * elem_size` bytes. [user]
 * @param elem_size Element size in bytes. [user]
 * @param capacity Max number of elements (not bytes). [user]
 * @param unique When `true`, duplicates are rejected. [user]
 * @param head Index of oldest element. [internal]
 * @param tail Index of next free slot. [internal]
 * @param count Current number of elements. [internal]
 */
typedef struct {
  void *buffer;
  uint16_t elem_size;
  uint16_t capacity;
  bool unique;
  uint16_t head;
  uint16_t tail;
  uint16_t count;
} QUEUE_t;

//-------------------------------------------------------------------------------------------------

void QUEUE_Init(QUEUE_t *queue, void *buffer, uint16_t capacity, uint16_t elem_size, bool unique);
bool QUEUE_Push(QUEUE_t *queue, const void *element);
bool QUEUE_Pop(QUEUE_t *queue, void *element);
bool QUEUE_Peek(const QUEUE_t *queue, void *element);
bool QUEUE_IsEmpty(const QUEUE_t *queue);
bool QUEUE_IsFull(const QUEUE_t *queue);
uint16_t QUEUE_Count(const QUEUE_t *queue);

//-------------------------------------------------------------------------------------------------
#endif