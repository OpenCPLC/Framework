#ifndef OPENCPLC_H_
#define OPENCPLC_H_

#if defined(OPENCPLC_UNO)
  #include "opencplc-uno.h"
#elif defined(OPENCPLC_DIO)
  #include "opencplcdio.h"
#elif defined(OPENCPLC_ECO)
  #include "opencplc-eco.h"
#endif

#endif