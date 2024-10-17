#include "exbit.h"

//-------------------------------------------------------------------------------------------------

uint32_t setbit(uint32_t reg, uint32_t bit)
{
  return reg |= (1 << bit);
}

uint32_t rstbit(uint32_t reg, uint32_t bit)
{
  return reg &= ~(1 << bit);
}

uint32_t tglbit(uint32_t reg, uint32_t bit)
{
  return reg ^= (1 << bit);
}

bool getbit(uint32_t reg, uint32_t bit)
{
  return (reg >> bit) & 1;
}

//-------------------------------------------------------------------------------------------------
