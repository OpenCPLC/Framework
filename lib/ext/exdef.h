#ifndef EXDEF_H_
#define EXDEF_H_

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

typedef enum {
  OK = 0,
  FREE = 0,
  IDLE = 0,
  ERR = 1,
  BUSY = 2,
} state_t;

#ifndef ANSI_BASIC
  #define ANSI_BASIC 0
#endif

#if(ANSI_BASIC)
  #define ANSI_RED      "\x1B[31m"
  #define ANSI_GREEN    "\x1B[32m"
  #define ANSI_YELLOW   "\x1B[33m"
  #define ANSI_BLUE     "\x1B[94m"
  #define ANSI_VIOLET   "\x1B[35m"
  #define ANSI_MAGENTA  "\x1B[35m"
  #define ANSI_PINK     "\x1B[95m"
  #define ANSI_CYAN     "\x1B[36m"
  #define ANSI_GREY     "\x1B[90m"
  #define ANSI_DGREY    "\x1B[90m"
  #define ANSI_GOLD     "\x1B[33m"
  #define ANSI_ORANGE   "\x1B[91m"
  #define ANSI_CREAM    "\x1B[97m"
  #define ANSI_TEAL     "\x1B[36m"
  #define ANSI_TURQUS   "\x1B[96m"
  #define ANSI_LIME     "\x1B[92m"
  #define ANSI_WHITE    "\x1B[97m"
#else
  #define ANSI_RED      "\x1B[38;5;167m"  // #D75F5F
  #define ANSI_GREEN    "\x1B[38;5;71m"   // #5FD7A7
  #define ANSI_YELLOW   "\x1B[38;5;221m"  // #FFD75F
  #define ANSI_BLUE     "\x1B[38;5;69m"   // #5F87D7
  #define ANSI_VIOLET   "\x1B[38;5;99m"   // #875FD7
  #define ANSI_MAGENTA  "\x1B[38;5;135m"  // #AF87D7
  #define ANSI_PINK     "\x1B[38;5;212m"  // #FF8787
  #define ANSI_CYAN     "\x1B[38;5;44m"   // #00D7D7
  #define ANSI_GREY     "\x1B[38;5;245m"  // #8A8A8A
  #define ANSI_DGREY    "\x1B[38;5;240m"  // #585858
  #define ANSI_GOLD     "\x1B[38;5;184m"  // #D7D700   
  #define ANSI_ORANGE   "\x1B[38;5;208m"  // #FF8700
  #define ANSI_CREAM    "\x1B[38;5;230m"  // #FFFFD7
  #define ANSI_TEAL     "\x1B[38;5;75m"   // #5FAFFF
  #define ANSI_TURQUS   "\x1B[38;5;79m"   // #5FD7AF)
  #define ANSI_LIME     "\x1B[38;5;112m"  // #87D75F
  #define ANSI_WHITE    "\x1B[38;5;15m"   // #FFFFFF
#endif

#define ANSI_END  "\x1B[0m"
#define ANSI_OK    ANSI_GREEN "OK" ANSI_END

typedef enum {
  HASH_Number_0 = 177621,
  HASH_Number_1 = 177622,
  HASH_Number_2 = 177623,
  HASH_Number_3 = 177624,
  HASH_Number_4 = 177625,
  HASH_Number_5 = 177626,
  HASH_Number_6 = 177627,
  HASH_Number_7 = 177628,
  HASH_Number_8 = 177629,
  HASH_Number_9 = 177630
} HASH_Number_t;

#endif
