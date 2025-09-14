#include "exmath.h"

//-------------------------------------------------------------------------------------------------

uint32_t ieee754_pack(float nbr)
{
  uint32_t *p_value;
  p_value = (uint32_t *)&nbr;
  return (*p_value);
}

float ieee754_unpack(uint32_t value)
{
  float *p_nbr;
  p_nbr = (float *)&(value);
  return (*p_nbr);
}

//-------------------------------------------------------------------------------------------------

float std_deviation(uint16_t *data, uint16_t count)
{
  if(count <= 1) return 0;
  float mean = 0.0;
  for(uint16_t i = 0; i < count; i++)
    mean += data[i];
  mean /= count;
  float sum_squared_diff = 0;
  for (uint16_t i = 0; i < count; i++) {
    float diff = data[i] - mean;
    sum_squared_diff += diff * diff;
  }
  float variance = sum_squared_diff / (count - 1);
  return sqrt(variance);
}

//-------------------------------------------------------------------------------------------------

void sort_asc_uint16(uint16_t *array, uint16_t length)
{
  for(uint16_t i = 1; i < length; i++) {
    uint16_t key = array[i];
    int j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

void sort_desc_uint16(uint16_t *array, uint16_t length)
{
  for(uint16_t i = 1; i < length; i++) {
    uint16_t key = array[i];
    int j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

float average_uint16(uint16_t *array, uint16_t length) 
{
  if(length == 0) return 0;
  float sum = 0;
  for(uint16_t i = 0; i < length; i++) {
    sum += array[i];
  }
  return sum / length;
}

void sort_asc_uint32(uint32_t *array, uint16_t length)
{
  for(uint16_t i = 1; i < length; i++) {
    uint32_t key = array[i];
    int j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

void sort_desc_uint32(uint32_t *array, uint16_t length)
{
  for(uint16_t i = 1; i < length; i++) {
    uint32_t key = array[i];
    int j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

float average_uint32(uint32_t *array, uint16_t length) 
{
  if(length == 0) return 0;
  float sum = 0;
  for(uint16_t i = 0; i < length; i++) {
    sum += array[i];
  }
  return sum / length;
}

/**
 * @brief Checks if a value exists in array `uint32_t` or `int32_t` by pointer casting.
 * @param array Pointer to the array.
 * @param len Number of elements.
 * @param value Value to search for.
 * @return `true` if found, otherwise `false`.
 */
bool contains_uint32(const uint32_t *array, uint16_t len, uint32_t value)
{
  while (len--) {
    if (*array++ == value) return true;
  }
  return false;
}

//-------------------------------------------------------------------------------------------------

float distance(float ax, float ay, float bx, float by)
{
  return sqrtf((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

/**
 * @brief Generates a log-lin scale between `start` and `end`
 *   (if start > end, the scale is generated in reverse order)
 * @param start Start value `> 0`
 * @param end End value `> 0`
 * @param n Number of points `≥ 2`
 * @param transition Blend factor between log and linear scale:
 *   if 'transition == 0' then: Full logarithmic scale
 *   if 'transition == 1' then: Full linear scale
 * @param scale_array Pointer to output array.
 * @return `true` if scale was generated, `false` on invalid input.
 */
bool scale_fill(float start, float end, int n, float transition, float *scale_array)
{
  if(start <= 0 || end <= 0 || n < 2 || transition < 0 || transition > 1 || !scale_array) {
    return false;
  }
  float tmp;
  bool reverse = false;
  if(start > end) {
    tmp = start;
    start = end;
    end = tmp;
    reverse = true;
  }
  float log_start = log10(start);
  float log_end = log10(end);
  for(int i = 0; i < n; i++) {
    float t = (float)i / (n - 1);
    float log_value = pow(10, log_start + t * (log_end - log_start));
    float lin_value = start + t * (end - start);
    scale_array[i] = (1 - transition) * log_value + transition * lin_value;
  }
  if(reverse) {
    for(int i = 0; i < n / 2; i++) {
      tmp = scale_array[i];
      scale_array[i] = scale_array[n - 1 - i];
      scale_array[n - 1 - i] = tmp;
    }
  }
  return true;
}

//-------------------------------------------------------------------------------------------------

float ema_filter_float(float input, float prev, float alpha)
{
  return prev + alpha * (input - prev);
}

float step_limiter_float(float input, float prev, float max_delta)
{
  float diff = input - prev;
  if(diff > max_delta) prev += max_delta;
  else if(diff < -max_delta) prev -= max_delta;
  else prev = input;
  return prev;
}

/**
 * @brief Simple exponential moving average (EMA) filter. Works with `int16_t` values (-32768..32767).
 * @param input New input sample value.
 * @param prev Previous sample value.
 * @param alpha_shift Filter strength (higher shift = stronger smoothing). Example: 3 means alpha = 1/8.
 * @return Smoothed output value (-32768..32767).
 */
int16_t ema_filter_int16(int16_t input, int16_t prev, uint8_t alpha_shift)
{
  if(alpha_shift > 15) alpha_shift = 15;
  int32_t diff = (int32_t)input - (int32_t)prev;
  int32_t step = diff >> alpha_shift;
  if(step == 0 && diff != 0) {
    if(abs(diff) > 4) step = (diff > 0) ? 1 : -1;
    else prev = input;
  }
  prev += (int16_t)step;
  return (int16_t)prev;
}

/**
 * @brief Simple exponential moving average (EMA) filter. Works with `uint16_t` values (0..65535).
 * @param input New input sample value.
 * @param prev Previous sample value.
 * @param alpha_shift Filter strength (higher shift = stronger smoothing). Example: 3 means alpha = 1/8.
 * @return Smoothed output value (0..65535).
 */
uint16_t ema_filter_uint16(uint16_t input, uint16_t prev, uint8_t alpha_shift)
{
  if(alpha_shift > 15) alpha_shift = 15;
  int32_t diff = (int32_t)input - (int32_t)prev;
  int32_t step = diff >> alpha_shift;
  if(step == 0 && diff != 0) {
    if(abs(diff) > 4) step = (diff > 0) ? 1 : -1;
    else prev = input;
  }
  int32_t new_value = (int32_t)prev + step;
  if(new_value < 0) new_value = 0;
  if(new_value > 0xFFFF) new_value = 0xFFFF;
  return (uint16_t)new_value;
}

/**
 * @brief Step limiter to control maximum change between values for `int16_t` (-32768..32767).
 * @param input New input sample `int16_t` value.
 * @param prev Previous sample `int16_t` value.
 * @param max_delta Maximum allowed change per step.
 * @return Limited output value as `int16_t` (-32768..32767).
 */
int16_t step_limiter_int16(int16_t input, int16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)input - (int32_t)prev;
  if(diff > (int32_t)max_delta) prev += max_delta;
  else if(diff < -(int32_t)max_delta) prev -= max_delta;
  else prev = input;
  return (int16_t)prev;
}

/**
 * @brief Step limiter to control maximum change between values for `uint16_t` (0..65535).
 * @param input New input sample `uint16_t` value.
 * @param prev Previous sample `uint16_t` value.
 * @param max_delta Maximum allowed change per step.
 * @return Limited output value as `uint16_t` (0..65535).
 */
uint16_t step_limiter_uint16(uint16_t input, uint16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)input - (int32_t)prev; // pełne różnice na uint16
  if(diff > (int32_t)max_delta) prev += max_delta;
  else if(diff < -(int32_t)max_delta) prev -= max_delta;
  else prev = input;
  return prev;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Find maximum value among multiple floats, ignoring NaN.
 *   Accepts a variable number of float arguments and returns the maximum.
 *   If all values are NaN, the result is NaN.
 * @param count Number of float arguments passed.
 * @param ...   List of float arguments (promoted to double in varargs).
 * @return Maximum valid float value, or NaN if none are valid.
 */
float max_float_NaN(int count, ...)
{
  va_list args;
  va_start(args, count);
  float max_value = NaN;
  for(int i = 0; i < count; i++) {
    float v = (float)va_arg(args, double);
    if(!isNaN(v)) {
      if(isNaN(max_value) || v > max_value) max_value = v;
    }
  }
  va_end(args);
  return max_value;
}

/**
 * @brief Find minimum value among multiple floats, ignoring NaN.
 *   Accepts a variable number of float arguments and returns the minimum.
 *   If all values are NaN, the result is NaN.
 * @param count Number of float arguments passed.
 * @param ...   List of float arguments (promoted to double in varargs).
 * @return Minimum valid float value, or NaN if none are valid.
 */
float min_float_NaN(int count, ...)
{
  va_list args;
  va_start(args, count);
  float min_value = NaN;
  for(int i = 0; i < count; i++) {
    float v = (float)va_arg(args, double);
    if(!isNaN(v)) {
      if(isNaN(min_value) || v < min_value) min_value = v;
    }
  }
  va_end(args);
  return min_value;
}