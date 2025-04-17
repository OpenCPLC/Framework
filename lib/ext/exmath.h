#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//-------------------------------------------------------------------------------------------------

#define NaN NAN
#define isNaN(nbr) isnan(nbr)

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

//-------------------------------------------------------------------------------------------------

#endif
