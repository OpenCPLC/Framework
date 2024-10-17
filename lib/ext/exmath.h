#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

//-------------------------------------------------------------------------------------------------

void swap(int32_t *a, int32_t *b);
void swap_double(double *a, double *b);

void keep_range(int32_t *a, int32_t min, int32_t max);
void keep_range_double(double *a, double min, double max);

void find_min_max_uint16(uint16_t *array, uint16_t length, uint16_t *min, uint16_t *max);

void array_unshift(int32_t value, int32_t *array, uint16_t length);
void array_shift(int32_t *array, uint16_t length);
int32_t array_average(int32_t *array, uint16_t length);
int32_t array_sum(int32_t *array, uint16_t length);

void array_unshift_int16(int16_t value, int16_t *array, uint16_t length);
void array_shift_int16(int16_t *array, uint16_t length);
int16_t array_average_int16(int16_t *array, uint16_t length);
int32_t array_sum16(int16_t *array, uint16_t length);

uint8_t array_sum8(uint8_t *array, uint16_t length);
uint8_t push_front_uint8(uint8_t value, uint8_t *array, uint16_t length);
uint8_t push_back_uint8(uint8_t value, uint8_t *array, uint16_t length);

uint32_t ieee754_pack(float nbr);
float ieee754_unpack(uint32_t value);

double power(double a, double b);
float std_deviation(uint16_t *data, uint16_t count);

//-------------------------------------------------------------------------------------------------

#endif
