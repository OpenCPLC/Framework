#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "exdef.h"

//-------------------------------------------------------------------------------------------------

#define NaN              NAN
#define isNaN(nbr)       isnan(nbr)

uint32_t ieee754_pack(float nbr);
float ieee754_unpack(uint32_t value);
float std_deviation(uint16_t *data, uint16_t count);
void sort_asc_uint16(uint16_t *array, uint16_t length);
void sort_desc_uint16(uint16_t *array, uint16_t length);
float average_uint16(uint16_t *array, uint16_t length);
void sort_asc_uint32(uint32_t *array, uint16_t length);
void sort_desc_uint32(uint32_t *array, uint16_t length);
float average_uint32(uint32_t *array, uint16_t length);
bool contains_uint32(const uint32_t *array, uint16_t len, uint32_t value);
float distance(float ax, float ay, float bx, float by);
bool scale_fill(float start, float end, int n, float transition, float *scale_array);

int16_t ema_filter_int16(int16_t raw, int16_t prev, uint8_t alpha_shift);
#define ema_filter_uint16(raw, prev, alpha_shift) (uint16_t)(ema_filter_int16((int16_t)(raw), (int16_t)(prev), (alpha_shift)))
int16_t step_limiter_int16(int16_t raw, int16_t prev, uint16_t max_delta);
uint16_t step_limiter_uint16(uint16_t raw, uint16_t prev, uint16_t max_delta);

//-------------------------------------------------------------------------------------------------

#endif
