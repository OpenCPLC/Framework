#ifndef OPENCPLC_H_
#define OPENCPLC_H_

#define OPENCPLC_VERSION "develop"

#if defined(OPENCPLC_UNO)
  #include "opencplc-uno.h"
#elif defined(OPENCPLC_DIO)
  #include "opencplcdio.h"
#elif defined(OPENCPLC_ECO)
  #include "opencplc-eco.h"
#endif

#endif