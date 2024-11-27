#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

//-------------------------------------------------------------------------------------------------

#define NaN NAN
#define isNaN(nbr) isnan(nbr)

uint32_t ieee754_pack(float nbr);
float ieee754_unpack(uint32_t value);

double power(double a, double b);
float std_deviation(uint16_t *data, uint16_t count);

void sort_inc_uint16(uint16_t *array, uint16_t length);
void sort_dec_uint16(uint16_t *array, uint16_t length);
float average_uint16(uint16_t *array, uint16_t length);

//-------------------------------------------------------------------------------------------------

#endif
