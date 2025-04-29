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

/**
 * @brief Simple exponential moving average (EMA) filter.
 *   Works with both `int16_t` and `uint16_t` (using casting for unsigned types).
 * @param raw New input sample value.
 * @param prev Previous sample value.
 * @param alpha_shift Filter strength (higher shift = stronger smoothing). Example: 3 means alpha = 1/8.
 * @return Smoothed output value.
 */
int16_t ema_filter_int16(int16_t raw, int16_t prev, uint8_t alpha_shift)
{
  int32_t diff = (int32_t)raw - (int32_t)prev;
  // Add a small fraction (1 / 2^alpha_shift) of the difference to the previous output:
  int32_t step = diff >> alpha_shift;
  if(step == 0 && diff != 0) step = (diff > 0) ? 1 : -1;
  prev += (int16_t)step;
  return (int16_t)prev;
}

/**
 * @brief Step limiter to control maximum change between values for `int16_t`
 * @param raw New input sample `int16_t` value.
 * @param prev Previous sample `int16_t` value.
 * @param max_delta Maximum allowed change per step.
 * @return Limited output value as `int16_t`.
 */
int16_t step_limiter_int16(int16_t raw, int16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)raw - (int32_t)prev;
  if(diff > (int32_t)max_delta) prev += max_delta;
  else if(diff < -(int32_t)max_delta) prev -= max_delta;
  else prev = raw;
  return (int16_t)prev;
}

/**
 * @brief Step limiter to control maximum change between values for `uint16_t`
 * @param raw New input sample `uint16_t` value.
 * @param prev Previous sample `uint16_t` value.
 * @param max_delta Maximum allowed change per step.
 * @return Limited output value as `uint16_t`.
 */
uint16_t step_limiter_uint16(uint16_t raw, uint16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)raw - (int32_t)prev; // pełne różnice na uint16
  if(diff > (int32_t)max_delta) prev += max_delta;
  else if(diff < -(int32_t)max_delta) prev -= max_delta;
  else prev = raw;
  return prev;
}


//-------------------------------------------------------------------------------------------------
