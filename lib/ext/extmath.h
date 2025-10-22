#ifndef EXMATH_H_
#define EXMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include "extdef.h"

//-------------------------------------------------------------------------------------------------

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

uint32_t ieee754_pack(float nbr);
float ieee754_unpack(uint32_t value);

float max_f32_NaN(uint16_t count, ...);
float min_f32_NaN(uint16_t count, ...);

void sort_asc_u16(uint16_t *array, uint16_t len);
void sort_asc_i16(int16_t *array, uint16_t len);
void sort_desc_u16(uint16_t *array, uint16_t len);
void sort_desc_i16(int16_t *array, uint16_t len);
void sort_asc_u32(uint32_t *array, uint16_t len);
void sort_asc_i32(int32_t *array, uint16_t len);
void sort_desc_u32(uint32_t *array, uint16_t len);
void sort_desc_i32(int32_t *array, uint16_t len);

float avg_u16(const uint16_t *array, uint16_t len);
float avg_i16(const int16_t *array, uint16_t len);
float avg_u32(const uint32_t *array, uint16_t len);
float avg_i32(const int32_t *array, uint16_t len);

bool stats_u16(const uint16_t *data, uint16_t count, uint16_t *min, uint16_t *max, uint32_t *sum, float *avg);
bool stats_i16(const int16_t *data, uint16_t count, int16_t *min, int16_t *max, int32_t *sum, float *avg);
bool stats_u32(const uint32_t *data, uint16_t count, uint32_t *min, uint32_t *max, uint64_t *sum, float *avg);
bool stats_i32(const int32_t *data, uint16_t count, int32_t *min, int32_t *max, int64_t *sum, float *avg);

float stddev_u16(const uint16_t *data, uint16_t count, float *avg);
float stddev_i16(const int16_t *data, uint16_t count, float *avg);
float stddev_u32(const uint32_t *data, uint16_t count, float *avg);
float stddev_i32(const int32_t *data, uint16_t count, float *avg);

bool contains_u8(const uint8_t *array, uint16_t len, uint8_t value);
bool contains_u16(const uint16_t *array, uint16_t len, uint16_t value);
bool contains_u32(const uint32_t *array, uint16_t len, uint32_t value);

int16_t ema_filter_i16(int16_t input, int16_t prev, uint8_t alpha_shift);
uint16_t ema_filter_u16(uint16_t input, uint16_t prev, uint8_t alpha_shift);
float ema_filter_f32(float input, float prev, float alpha);
int16_t step_limiter_i16(int16_t input, int16_t prev, uint16_t max_delta);
uint16_t step_limiter_u16(uint16_t input, uint16_t prev, uint16_t max_delta);
float step_limiter_f32(float input, float prev, float max_delta);

//-------------------------------------------------------------------------------------------------

#define ext_min2(a,b) ((a) < (b) ? (a) : (b))
#define ext_min3(a,b,c) ext_min2(ext_min2(a,b), (c))
#define ext_min4(a,b,c,d) ext_min2(ext_min3(a,b,c), (d))
#define ext_min5(a,b,c,d,e) ext_min2(ext_min4(a,b,c,d), (e))
#define GET_EXT_MIN(_1,_2,_3,_4,_5,NAME,...) NAME
/**
 * @brief Return minimum of provided values.
 * @note Supports 2–5 arguments.
 * @return Smallest of given arguments.
 */
#define ext_min(...) GET_EXT_MIN(__VA_ARGS__, ext_min5, ext_min4, ext_min3, ext_min2)(__VA_ARGS__)


#define ext_max2(a,b) ((a) > (b) ? (a) : (b))
#define ext_max3(a,b,c) ext_max2(ext_max2(a,b), (c))
#define ext_max4(a,b,c,d) ext_max2(ext_max3(a,b,c), (d))
#define ext_max5(a,b,c,d,e) ext_max2(ext_max4(a,b,c,d), (e))
#define GET_EXT_MAX(_1,_2,_3,_4,_5,NAME,...) NAME
/**
 * @brief Return maximum of provided values.
 * @note Supports 2–5 arguments.
 * @return Greatest of given arguments.
 */
#define ext_max(...) GET_EXT_MAX(__VA_ARGS__, ext_max5, ext_max4, ext_max3, ext_max2)(__VA_ARGS__)

/**
 * @brief Return absolute value of number.
 * @param a Input value (signed).
 * @return Positive equivalent of `a`.
 */
#define ext_abs(a) (((a) < 0) ? -(a) : (a))

/**
 * @brief Return sign of number.
 * @param a Input value.
 * @return `-1` if negative, `0` if zero, `1` if positive.
 */
#define ext_sign(a) (((a) > 0) - ((a) < 0))

/**
 * @brief Clamp value to `[min, max]` range.
 * @param value Value to clamp.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return Clamped value in range `[min, max]`.
 */
#define ext_clamp(value, min, max) \
  (((value) < (min)) ? (min) : ((value) > (max)) ? (max) : (value))

/**
 * @brief Check if value is inside `[min, max]` range (inclusive).
 * @param value Value to check.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return `true` if value ∈ [min, max], else `false`.
 */
#define in_range(value, min, max) \
  (((value) >= (min)) && ((value) <= (max)))

/**
 * @brief Swap two variables of the same type.
 * @param a First variable.
 * @param b Second variable.
 */
#define ext_swap(a, b) do { \
  __typeof__(a) _tmp = (a); \
  (a) = (b); \
  (b) = _tmp; \
} while(0)

/**
 * @brief Get number of elements in static array.
 * @param x Static array (not a pointer).
 * @return Element count (e.g. 4 for `int x[4]`).
 */
#define ext_array_len(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @brief Scale/map `x` from one range to another.
 * @param x Input value.
 * @param in_min Lower bound of input range.
 * @param in_max Upper bound of input range.
 * @param out_min Lower bound of output range.
 * @param out_max Upper bound of output range.
 * @return Scaled value.
 */
#define ext_scale(x, in_min, in_max, out_min, out_max) \
  (((x) - (in_min)) * ((out_max) - (out_min)) / \
   ((in_max) - (in_min)) + (out_min))

//-------------------------------------------------------------------------------------------------
#endif
