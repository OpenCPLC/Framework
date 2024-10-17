#include "var.h"
#include "dbg.h"

//-------------------------------------------------------------------------------------------------

void VAR_Print(var_t *var)
{
  if(var->isset) {
    DBG_Float(var->value, var->precision);
    DBG_String((char *)var->unit);
  }
  else {
    if(var->unit) {
      DBG_String("[-]");
      DBG_String((char *)var->unit);
    }
    else DBG_String("null");
  }
}

//-------------------------------------------------------------------------------------------------
