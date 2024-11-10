#include "sys.h"

void panic(const char *message)
{
  LOG_Panic(message);
  DBG_BeforeReset();
  PWR_Reset();
}
