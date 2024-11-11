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

//-------------------------------------------------------------------------------------------------

void sort_inc_uint16(uint16_t *array, uint16_t length)
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

void sort_dec_uint16(uint16_t *array, uint16_t length)
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

//-------------------------------------------------------------------------------------------------
