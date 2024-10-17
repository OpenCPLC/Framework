#ifndef VAR_H_
#define VAR_H_

#include <stdint.h>
#include <stdbool.h>

//-------------------------------------------------------------------------------------------------

typedef struct {
  float value;
  int8_t precision;
  const char *unit;
  bool isset;
} var_t;

void VAR_Print(var_t *var);

//-------------------------------------------------------------------------------------------------

#endif