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

#define ANSI_RED     "\x1B[31m"
#define ANSI_GREEN   "\x1B[32m"
#define ANSI_BLUE    "\x1B[94m"
#define ANSI_YELLOW  "\x1B[33m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN    "\x1B[36m"
#define ANSI_GREY    "\033[90m"
#define ANSI_CREAM   "\x1B[38;5;216m"
#define ANSI_TEAL    "\x1B[38;2;32;178;170m"
#define ANSI_END     "\x1B[0m"
#define ANSI_OK      ANSI_GREEN"OK"ANSI_END

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
