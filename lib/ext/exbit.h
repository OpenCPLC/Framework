#ifndef EXBIT_H_
#define EXBIT_H_

#include <stdint.h>
#include <stdbool.h>

//-------------------------------------------------------------------------------------------------

uint32_t setbit(uint32_t reg, uint32_t bit);
uint32_t rstbit(uint32_t reg, uint32_t bit);
uint32_t tglbit(uint32_t reg, uint32_t bit);
bool getbit(uint32_t reg, uint32_t bit);

//-------------------------------------------------------------------------------------------------
#endif
