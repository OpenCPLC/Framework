#include "ring.h"

void ring16_push(ring16_t *ring, uint16_t value)
{
  ring->memory[ring->head] = value;
  if(ring->count == ring->limit) ring->tail = (ring->tail + 1) % ring->limit;
  else ring->count++;
  ring->head = (ring->head + 1) % ring->limit;
}

uint8_t ring16_pop(ring16_t *ring, uint16_t *out_value)
{
  if(ring->count == 0) return 0;
  if(out_value) *out_value = ring->memory[ring->tail];
  ring->tail = (ring->tail + 1) % ring->limit;
  ring->count--;
  return 1;
}

uint8_t ring16_peek(const ring16_t *ring, uint16_t *out_value)
{
  if(ring->count == 0) return 0;
  if(out_value) *out_value = ring->memory[ring->tail];
  return 1;
}

uint8_t ring16_peek_last(const ring16_t *ring, uint16_t *out_value)
{
  if(ring->count == 0) return 0;
  uint16_t idx = (ring->head == 0 ? ring->limit - 1 : ring->head - 1);
  if(out_value) *out_value = ring->memory[idx];
  return 1;
}

uint16_t ring16_copy_last(ring16_t *ring, uint16_t n, uint16_t *out_array)
{
  if(ring->count == 0) return 0;
  if(n > ring->count) n = ring->count;
  uint32_t tmp = (uint32_t)ring->tail + (uint32_t)(ring->count - n);
  uint16_t start_index = (uint16_t)(tmp % ring->limit);
  for(uint16_t i = 0; i < n; ++i) {
    out_array[i] = ring->memory[(start_index + i) % ring->limit];
  }
  return n;
}

void ring16_clear(ring16_t *ring)
{
  ring->head = 0;
  ring->tail = 0;
  ring->count = 0;
}
