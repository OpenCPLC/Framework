#include "exmath.h"

//-----------------------------------------------------------------------------

void swap(int32_t *a, int32_t *b)
{
  int32_t c = *a;
  *a = *b;
  *b = c;
}

void swap_double(double *a, double *b)
{
  double c = *a;
  *a = *b;
  *b = c;
}

void keep_range(int32_t *a, int32_t min, int32_t max)
{
  if(*a < min) *a = min;
  else if (*a > max) *a = max;
}

void keep_range_double(double *a, double min, double max)
{
  if(*a < min) *a = min;
  else if (*a > max) *a = max;
}

void sort_inc(int32_t *array, uint16_t length)
{
  for(uint16_t i = 1; i < length; i++)
  {
    for(uint16_t j = 0; j < length - i; j++)
    {
      if(array[j] > array[j + 1]) swap(&array[j], &array[j + 1]);
    }
  }
}

//-----------------------------------------------------------------------------

void find_min_max_uint16(uint16_t *array, uint16_t length, uint16_t *min, uint16_t *max)
{
  *min = 0xFFFF;
  *max = 0;

  for(uint16_t i = 0; i < length; i++)
  {
    if(array[i] < *min) *min = array[i];
    if(array[i] > *max) *max = array[i];
  }
}

//-----------------------------------------------------------------------------

void array_unshift(int32_t value, int32_t *array, uint16_t length)
{
  for(uint16_t i = length - 1; i > 0; i--) array[i] = array[i - 1];
  array[0] = value;
}

void array_shift(int32_t *array, uint16_t length)
{
  uint16_t i;
  for(i = 0; i < length - 1; i++) array[i] = array[i + 1];
  array[i] = 0;
}

int32_t array_average(int32_t *array, uint16_t length)
{
  double value = 0;
  for(uint16_t i = 0; i < length; i++) value += (double)array[i];
  return (int32_t)(value / length);
}

int32_t array_sum(int32_t *array, uint16_t length)
{
  int32_t sum = 0;
  for(uint16_t i = 0; i < length; i++) sum += array[i];
  return sum;
}

void array_unshift16(int16_t value, int16_t *array, uint16_t length)
{
  for(uint16_t i = length - 1; i > 0; i--) array[i] = array[i - 1];
  array[0] = value;
}

void array_shift16(int16_t *array, uint16_t length)
{
  uint16_t i;
  for(i = 0; i < length - 1; i++) array[i] = array[i + 1];
  array[i] = 0;
}

int16_t array_average16(int16_t *array, uint16_t length)
{
  double value = 0;
  for(uint16_t i = 0; i < length; i++) value += (double)array[i];
  return (int16_t)(value / length);
}

int32_t array_sum16(int16_t *array, uint16_t length)
{
  int32_t sum = 0;
  for(uint16_t i = 0; i < length; i++) sum += (int32_t)array[i];
  return sum;
}

uint8_t array_sum8(uint8_t *array, uint16_t length)
{
  uint32_t sum = 0;
  for(uint16_t i = 0; i < length; i++) sum += (uint32_t)array[i];
  return sum;
}

uint8_t push_front_uint8(uint8_t value, uint8_t *array, uint16_t length)
{
  uint8_t drop = array[length - 1];
  for(uint16_t i = length - 1; i > 0; i--) array[i] = array[i - 1];
  array[0] = value;
  return drop;
}

uint8_t push_back_uint8(uint8_t value, uint8_t *array, uint16_t length)
{
  uint8_t drop = array[0];
  for(uint16_t i = 0; i < length - 1; i++) array[i] = array[i + 1];
  array[length - 1] = value;
  return drop;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

double power(double a, double b)
{
  int e = (int)b;
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)((b - e) * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  double r = 1.0;
  while(e) {
    if(e & 1) {
      r *= a;
    }
    a *= a;
    e >>= 1;
  }
  return r * u.d;
}

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


//-----------------------------------------------------------------------------


