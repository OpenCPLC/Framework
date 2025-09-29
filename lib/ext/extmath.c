#include "extmath.h"

//------------------------------------------------------------------------------------------------- ieee754

/**
 * @brief Packs `float` value into raw IEEE 754 `uint32_t` representation.
 * @param nbr Input float value.
 * @return Packed 32-bit representation of `nbr`.
 */
uint32_t ieee754_pack(float nbr)
{
  uint32_t *p_value;
  p_value = (uint32_t *)&nbr;
  return *p_value;
}

/**
 * @brief Unpacks raw IEEE 754 `uint32_t` value into `float`.
 * @param value 32-bit binary representation of a float.
 * @return Unpacked float value.
 */
float ieee754_unpack(uint32_t value)
{
  float *p_nbr;
  p_nbr = (float *)&value;
  return *p_nbr;
}

//------------------------------------------------------------------------------------------------- float

/**
 * @brief Returns maximum of multiple `float` values, ignoring `NaN`.
 *   If all inputs are `NaN`, returns `NaN`.
 * @param count Number of float arguments (promoted to `double`).
 * @param ... List of float values.
 * @return Maximum valid value or `NaN` if none are valid.
 */
float max_f32_NaN(uint16_t count, ...)
{
  va_list args;
  va_start(args, count);
  float max_value = NaN;
  for(uint16_t i = 0; i < count; i++) {
    float v = (float)va_arg(args, double);
    if(!isNaN(v)) {
      if(isNaN(max_value) || v > max_value) max_value = v;
    }
  }
  va_end(args);
  return max_value;
}

/**
 * @brief Returns minimum of multiple `float` values, ignoring `NaN`.
 *   If all inputs are `NaN`, returns `NaN`.
 * @param count Number of float arguments (promoted to `double`).
 * @param ... List of float values.
 * @return Minimum valid value or `NaN` if none are valid.
 */
float min_f32_NaN(uint16_t count, ...)
{
  va_list args;
  va_start(args, count);
  float min_value = NaN;
  for(uint16_t i = 0; i < count; i++) {
    float v = (float)va_arg(args, double);
    if(!isNaN(v)) {
      if(isNaN(min_value) || v < min_value) min_value = v;
    }
  }
  va_end(args);
  return min_value;
}

//------------------------------------------------------------------------------------------------- sort

/**
 * @brief Sorts `uint16_t` array in ascending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_asc_u16(uint16_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    uint16_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `int16_t` array in ascending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_asc_i16(int16_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    int16_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `uint16_t` array in descending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_desc_u16(uint16_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    uint16_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `int16_t` array in descending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_desc_i16(int16_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    int16_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `uint32_t` array in ascending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_asc_u32(uint32_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    uint32_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `int32_t` array in ascending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_asc_i32(int32_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    int32_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] > key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `uint32_t` array in descending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_desc_u32(uint32_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    uint32_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Sorts `int32_t` array in descending order (in-place).
 * @param array Pointer to array to sort.
 * @param len Number of elements in `array`.
 */
void sort_desc_i32(int32_t *array, uint16_t len)
{
  for(uint16_t i = 1; i < len; i++) {
    int32_t key = array[i];
    int16_t j = i - 1;
    while(j >= 0 && array[j] < key) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

//------------------------------------------------------------------------------------------------- avg

/**
 * @brief Calculates average of `uint16_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @return Average value as `float`. Returns `0` if `len` is `0`.
 */
float avg_u16(const uint16_t *array, uint16_t len)
{
  if(len == 0) return 0.0f;
  uint32_t sum = 0;
  for(uint16_t i = 0; i < len; i++) {
    sum += array[i];
  }
  return (float)sum / len;
}

/**
 * @brief Calculates average of `int16_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @return Average value as `float`. Returns `0` if `len` is `0`.
 */
float avg_i16(const int16_t *array, uint16_t len)
{
  if(len == 0) return 0.0f;
  int32_t sum = 0;
  for(uint16_t i = 0; i < len; i++) {
    sum += array[i];
  }
  return (float)sum / len;
}

/**
 * @brief Calculates average of `uint32_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @return Average value as `float`. Returns `0` if `len` is `0`.
 */
float avg_u32(const uint32_t *array, uint16_t len)
{
  if(len == 0) return 0.0f;
  uint64_t sum = 0;
  for(uint16_t i = 0; i < len; i++) {
    sum += array[i];
  }
  return (float)sum / len;
}

/**
 * @brief Calculates average of `int32_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @return Average value as `float`. Returns `0` if `len` is `0`.
 */
float avg_i32(const int32_t *array, uint16_t len)
{
  if(len == 0) return 0.0f;
  int64_t sum = 0;
  for(uint16_t i = 0; i < len; i++) {
    sum += array[i];
  }
  return (float)sum / len;
}

//------------------------------------------------------------------------------------------------- stats: min, max, sun, avg

/**
 * @brief Calculates min, max, and average of `uint16_t` buffer.
 * @param data Input buffer of `uint16_t` values.
 * @param count Number of elements in `data`.
 * @param min Pointer to store minimum value (cannot be `NULL`).
 * @param max Pointer to store maximum value (cannot be `NULL`).
 * @param avg Optional pointer to store average as `float` (can be `NULL`).
 * @retval `true` on success; `false` if `count` is 0 or pointers are invalid.
 */
bool stats_u16(const uint16_t *data, uint16_t count, uint16_t *min, uint16_t *max, uint32_t *sum, float *avg)
{
  if(count == 0 || !min || !max) return false;
  *min = 0xFFFF;
  *max = 0;
  uint32_t local_sum = 0;
  for(uint16_t i = 0; i < count; ++i) {
    uint16_t v = data[i];
    if(v < *min) *min = v;
    if(v > *max) *max = v;
    local_sum += v;
  }
  if(avg) *avg = (float)local_sum / count;
  if(sum) *sum = local_sum;
  return true;
}

/**
 * @brief Calculates min, max, and average of `int16_t` buffer.
 * @param data Input buffer of `int16_t` values.
 * @param count Number of elements in `data`.
 * @param min Pointer to store minimum value (cannot be `NULL`).
 * @param max Pointer to store maximum value (cannot be `NULL`).
 * @param avg Optional pointer to store average as `float` (can be `NULL`).
 * @retval `true` on success; `false` if `count` is 0 or pointers are invalid.
 */
bool stats_i16(const int16_t *data, uint16_t count, int16_t *min, int16_t *max, int32_t *sum, float *avg)
{
  if(count == 0 || !min || !max) return false;
  *min = INT16_MAX;
  *max = INT16_MIN;
  int32_t local_sum = 0;
  for(uint16_t i = 0; i < count; ++i) {
    int16_t v = data[i];
    if(v < *min) *min = v;
    if(v > *max) *max = v;
    local_sum += v;
  }
  if(avg) *avg = (float)local_sum / count;
  if(sum) *sum = local_sum;
  return true;
}

/**
 * @brief Calculates min, max, and average of `uint32_t` buffer.
 * @param data Input buffer of `uint32_t` values.
 * @param count Number of elements in `data`.
 * @param min Pointer to store minimum value (cannot be `NULL`).
 * @param max Pointer to store maximum value (cannot be `NULL`).
 * @param avg Optional pointer to store average as `float` (can be `NULL`).
 * @retval `true` on success; `false` if `count` is 0 or pointers are invalid.
 */
bool stats_u32(const uint32_t *data, uint16_t count, uint32_t *min, uint32_t *max, uint64_t *sum, float *avg)
{
  if(count == 0 || !min || !max) return false;
  *min = 0xFFFFFFFF;
  *max = 0;
  uint64_t local_sum = 0;
  for(uint16_t i = 0; i < count; ++i) {
    uint32_t v = data[i];
    if(v < *min) *min = v;
    if(v > *max) *max = v;
    local_sum += v;
  }
  if(avg) *avg = (float)local_sum / count;
  if(sum) *sum = local_sum;
  return true;
}

/**
 * @brief Calculates min, max, and average of `int32_t` buffer.
 * @param data Input buffer of `int32_t` values.
 * @param count Number of elements in `data`.
 * @param min Pointer to store minimum value (cannot be `NULL`).
 * @param max Pointer to store maximum value (cannot be `NULL`).
 * @param avg Optional pointer to store average as `float` (can be `NULL`).
 * @retval `true` on success; `false` if `count` is 0 or pointers are invalid.
 */
bool stats_i32(const int32_t *data, uint16_t count, int32_t *min, int32_t *max, int64_t *sum, float *avg)
{
  if(count == 0 || !min || !max) return false;
  *min = INT32_MAX;
  *max = INT32_MIN;
  int64_t local_sum = 0;
  for(uint16_t i = 0; i < count; ++i) {
    int32_t v = data[i];
    if(v < *min) *min = v;
    if(v > *max) *max = v;
    local_sum += v;
  }
  if(avg) *avg = (float)local_sum / count;
  if(sum) *sum = local_sum;
  return true;
}

//------------------------------------------------------------------------------------------------- standard-deviation

/**
 * @brief Calculates sample standard deviation of `uint16_t` data.
 * @param data Pointer to input array.
 * @param count Number of elements in `data`. Must be ≥ 2.
 * @param avg Optional pointer to store average (`float`).
 * @return Standard deviation as `float`. Returns `0` if `count <= 1`.
 */
float stddev_u16(const uint16_t *data, uint16_t count, float *avg)
{
  if(count <= 1) return 0.0f;
  float mean = avg_u16(data, count);
  if(avg) *avg = mean;

  float sum_sq = 0.0f;
  for(uint16_t i = 0; i < count; i++) {
    float diff = (float)data[i] - mean;
    sum_sq += diff * diff;
  }

  return sqrtf(sum_sq / (count - 1));
}

/**
 * @brief Calculates sample standard deviation of `int16_t` data.
 * @param data Pointer to input array.
 * @param count Number of elements in `data`. Must be ≥ 2.
 * @param avg Optional pointer to store average (`float`).
 * @return Standard deviation as `float`. Returns `0` if `count <= 1`.
 */
float stddev_i16(const int16_t *data, uint16_t count, float *avg)
{
  if(count <= 1) return 0.0f;
  float mean = avg_i16(data, count);
  if(avg) *avg = mean;

  float sum_sq = 0.0f;
  for(uint16_t i = 0; i < count; i++) {
    float diff = (float)data[i] - mean;
    sum_sq += diff * diff;
  }

  return sqrtf(sum_sq / (count - 1));
}

/**
 * @brief Calculates sample standard deviation of `uint32_t` data.
 * @param data Pointer to input array.
 * @param count Number of elements in `data`. Must be ≥ 2.
 * @param avg Optional pointer to store average (`float`).
 * @return Standard deviation as `float`. Returns `0` if `count <= 1`.
 */
float stddev_u32(const uint32_t *data, uint16_t count, float *avg)
{
  if(count <= 1) return 0.0f;
  float mean = avg_u32(data, count);
  if(avg) *avg = mean;

  float sum_sq = 0.0f;
  for(uint16_t i = 0; i < count; i++) {
    float diff = (float)data[i] - mean;
    sum_sq += diff * diff;
  }

  return sqrtf(sum_sq / (count - 1));
}

/**
 * @brief Calculates sample standard deviation of `int32_t` data.
 * @param data Pointer to input array.
 * @param count Number of elements in `data`. Must be ≥ 2.
 * @param avg Optional pointer to store average (`float`).
 * @return Standard deviation as `float`. Returns `0` if `count <= 1`.
 */
float stddev_i32(const int32_t *data, uint16_t count, float *avg)
{
  if(count <= 1) return 0.0f;
  float mean = avg_i32(data, count);
  if(avg) *avg = mean;

  float sum_sq = 0.0f;
  for(uint16_t i = 0; i < count; i++) {
    float diff = (float)data[i] - mean;
    sum_sq += diff * diff;
  }

  return sqrtf(sum_sq / (count - 1));
}

//------------------------------------------------------------------------------------------------- contains

/**
 * @brief Checks if `value` exists in `uint8_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @param value Value to search for.
 * @return `true` if found, `false` otherwise.
 */
bool contains_u8(const uint8_t *array, uint16_t len, uint8_t value)
{
  while(len--) {
    if(*array++ == value) return true;
  }
  return false;
}

/**
 * @brief Checks if `value` exists in `uint16_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @param value Value to search for.
 * @return `true` if found, `false` otherwise.
 */
bool contains_u16(const uint16_t *array, uint16_t len, uint16_t value)
{
  while(len--) {
    if(*array++ == value) return true;
  }
  return false;
}

/**
 * @brief Checks if `value` exists in `uint32_t` array.
 * @param array Pointer to input array.
 * @param len Number of elements in `array`.
 * @param value Value to search for.
 * @return `true` if found, `false` otherwise.
 */
bool contains_u32(const uint32_t *array, uint16_t len, uint32_t value)
{
  while(len--) {
    if(*array++ == value) return true;
  }
  return false;
}

//------------------------------------------------------------------------------------------------- filters

/**
 * @brief Exponential moving average filter for `int16_t` (-32768..32767).
 * @param input New input sample.
 * @param prev Previous sample.
 * @param alpha_shift Filter strength (higher = smoother), e.g. 3 → alpha = 1/8.
 * @return Smoothed value.
 */
int16_t ema_filter_i16(int16_t input, int16_t prev, uint8_t alpha_shift)
{
  if(alpha_shift > 15) alpha_shift = 15;
  int32_t diff = (int32_t)input - (int32_t)prev;
  int32_t step = diff >> alpha_shift;
  if(step == 0 && diff != 0) {
    if(abs(diff) > 4) step = (diff > 0) ? 1 : -1;
    else prev = input;
  }
  return (int16_t)((int32_t)prev + step);
}

/**
 * @brief Exponential moving average filter for `uint16_t` (0..65535).
 * @param input New input sample.
 * @param prev Previous sample.
 * @param alpha_shift Filter strength (higher = smoother), e.g. 3 → alpha = 1/8.
 * @return Smoothed value.
 */
uint16_t ema_filter_u16(uint16_t input, uint16_t prev, uint8_t alpha_shift)
{
  if(alpha_shift > 15) alpha_shift = 15;
  int32_t diff = (int32_t)input - (int32_t)prev;
  int32_t step = diff >> alpha_shift;
  if(step == 0 && diff != 0) {
    if(abs(diff) > 4) step = (diff > 0) ? 1 : -1;
    else return input;
  }
  int32_t result = (int32_t)prev + step;
  if(result < 0) result = 0;
  if(result > 0xFFFF) result = 0xFFFF;
  return (uint16_t)result;
}

/**
 * @brief Exponential moving average filter for `float` values.
 * @param input New input sample.
 * @param prev Previous filtered value.
 * @param alpha Filter factor (0.0f to 1.0f). Lower = smoother.
 * @return Smoothed output value.
 */
float ema_filter_f32(float input, float prev, float alpha)
{
  return prev + alpha * (input - prev);
}

/**
 * @brief Limits max change between samples for `int16_t` (-32768..32767).
 * @param input New sample.
 * @param prev Previous sample.
 * @param max_delta Maximum allowed step.
 * @return Limited output.
 */
int16_t step_limiter_i16(int16_t input, int16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)input - (int32_t)prev;
  if(diff > (int32_t)max_delta) return prev + max_delta;
  if(diff < -(int32_t)max_delta) return prev - max_delta;
  return input;
}

/**
 * @brief Limits max change between samples for `uint16_t` (0..65535).
 * @param input New sample.
 * @param prev Previous sample.
 * @param max_delta Maximum allowed step.
 * @return Limited output.
 */
uint16_t step_limiter_u16(uint16_t input, uint16_t prev, uint16_t max_delta)
{
  int32_t diff = (int32_t)input - (int32_t)prev;
  if(diff > (int32_t)max_delta) return prev + max_delta;
  if(diff < -(int32_t)max_delta) return prev - max_delta;
  return input;
}

/**
 * @brief Limits maximum step change between `float` values.
 * @param input New input sample.
 * @param prev Previous value.
 * @param max_delta Maximum allowed change.
 * @return Limited output value.
 */
float step_limiter_f32(float input, float prev, float max_delta)
{
  float diff = input - prev;
  if(diff > max_delta) prev += max_delta;
  else if(diff < -max_delta) prev -= max_delta;
  else prev = input;
  return prev;
}

//-------------------------------------------------------------------------------------------------
