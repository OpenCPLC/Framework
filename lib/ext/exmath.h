#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "exdef.h"

//-------------------------------------------------------------------------------------------------

#define NaN         NAN
#define isNaN(nbr)  isnan(nbr)

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

float ema_filter_float(float input, float prev, float alpha);
float step_limiter_float(float input, float prev, float max_delta);
int16_t ema_filter_int16(int16_t input, int16_t prev, uint8_t alpha_shift);
uint16_t ema_filter_uint16(uint16_t input, uint16_t prev, uint8_t alpha_shift);
int16_t step_limiter_int16(int16_t input, int16_t prev, uint16_t max_delta);
uint16_t step_limiter_uint16(uint16_t input, uint16_t prev, uint16_t max_delta);

float max_float_NaN(int count, ...);
float min_float_NaN(int count, ...);

//-------------------------------------------------------------------------------------------------

#endif
