#ifndef RING_H_
#define RING_H_

#include <stdint.h>

/**
 * @brief Helper macro for static ring16_t declaration.
 * Example: `ring16_new(myRing, 16);`
 * Declares: `uint16_t myRing_storage[16]; ring16_t myRing = { myRing_storage, 16, 0, 0, 0 };`
 * @param Name Name of the ring16_t variable.
 * @param Limit Capacity of the ring buffer (number of elements).
 */
#define ring16_init(Name, Limit) \
  uint16_t Name##_storage[Limit]; \
  ring16_t Name = { \
    .memory = Name##_storage, \
    .limit  = (Limit), \
    .head   = 0, \
    .tail   = 0, \
    .count  = 0 \
  }

typedef struct {
  uint16_t *memory;
  uint16_t limit;
  uint16_t head;
  uint16_t tail;
  uint16_t count;
} ring16_t;

void ring16_push(ring16_t *ring, uint16_t value);
uint8_t ring16_pop(ring16_t *ring, uint16_t *out_value);
uint8_t ring16_peek(const ring16_t *ring, uint16_t *out_value);
uint8_t ring16_peek_last(const ring16_t *ring, uint16_t *out_value);
uint16_t ring16_copy_last(ring16_t *ring, uint16_t n, uint16_t *out_array);
void ring16_clear(ring16_t *ring);

#endif