#ifndef OPENCPLC_H_
#define OPENCPLC_H_

#define OPENCPLC_VERSION "1.0.0-rc.1"

#if defined(OPENCPLC_UNO)
  #include "opencplc-uno.h"
#elif defined(OPENCPLC_DIO)
  #include "opencplcdio.h"
#elif defined(OPENCPLC_ECO)
  #include "opencplc-eco.h"
#endif

#endif