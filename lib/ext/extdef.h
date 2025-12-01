#ifndef EXDEF_H_
#define EXDEF_H_

#include <math.h>
#include <limits.h>

//-------------------------------------------------------------------------------------------------

typedef enum {
  OK = 0,    // Normal execution
  FREE = 0,  // Resource free
  IDLE = 0,  // Nothing active
  ERR = 1,   // Error occurred
  BUSY = 2,  // Resource busy
} status_t;

#define NaN         NAN
#define Inf         INFINITY
#define isNaN(nbr)  isnan(nbr)
#define isInf(nbr)  isinf(nbr)

/**
 * @brief Set bit at position `bit` in register `reg`.
 * @param reg Register or variable to modify.
 * @param bit Bit index to set (0 = LSB).
 */
#define bit_set(reg, bit)  ((reg) |= (1U << (bit)))

/**
 * @brief Clear bit at position `bit` in register `reg`.
 * @param reg Register or variable to modify.
 * @param bit Bit index to clear (0 = LSB).
 */
#define bit_rst(reg, bit)  ((reg) &= ~(1U << (bit)))

/**
 * @brief Toggle bit at position `bit` in register `reg`.
 * @param reg Register or variable to modify.
 * @param bit Bit index to toggle (0 = LSB).
 */
#define bit_tgl(reg, bit)  ((reg) ^= (1U << (bit)))

/**
 * @brief Read bit at position `bit` in register `reg`.
 * @param reg Register or value to read.
 * @param bit Bit index to check (0 = LSB).
 * @return `1` if bit is set, `0` if cleared.
 */
#define bit_get(reg, bit)  (((reg) >> (bit)) & 1U)

//------------------------------------------------------------------------------------------------- ANSI

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
  #define ANSI_RED      "\x1B[38;5;167m"  /* #D75F5F */
  #define ANSI_GREEN    "\x1B[38;5;71m"   /* #5FD7A7 */
  #define ANSI_YELLOW   "\x1B[38;5;221m"  /* #FFD75F */
  #define ANSI_BLUE     "\x1B[38;5;69m"   /* #5F87D7 */
  #define ANSI_VIOLET   "\x1B[38;5;99m"   /* #875FD7 */
  #define ANSI_MAGENTA  "\x1B[38;5;135m"  /* #AF87D7 */
  #define ANSI_PINK     "\x1B[38;5;181m"  /* #D787AF */
  #define ANSI_CYAN     "\x1B[38;5;44m"   /* #00D7D7 */
  #define ANSI_GREY     "\x1B[38;5;245m"  /* #8A8A8A */
  #define ANSI_DGREY    "\x1B[38;5;240m"  /* #585858 */
  #define ANSI_GOLD     "\x1B[38;5;184m"  /* #D7D700 */
  #define ANSI_ORANGE   "\x1B[38;5;173m"  /* #D7875F */
  #define ANSI_CREAM    "\x1B[38;5;230m"  /* #FFFFD7 */
  #define ANSI_TEAL     "\x1B[38;5;75m"   /* #5FAFFF */
  #define ANSI_TURQUS   "\x1B[38;5;79m"   /* #5FD7AF */
  #define ANSI_LIME     "\x1B[38;5;112m"  /* #87D75F */
  #define ANSI_WHITE    "\x1B[38;5;15m"   /* #FFFFFF */
#endif

#define ANSI_END  "\x1B[0m"
#define ANSI_OK   " " ANSI_GREEN "OK" ANSI_END

//-------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------

#define _args6(a,b,c,d,e,f,name,...) name

#define _min2(a,b) ((a) < (b) ? (a) : (b))
#define _min3(a,b,c) _min2(_min2(a,b), (c))
#define _min4(a,b,c,d) _min2(_min3(a,b,c), (d))
#define _min5(a,b,c,d,e) _min2(_min4(a,b,c,d), (e))
#define _min6(a,b,c,d,e,f) _min2(_min5(a,b,c,d,e), (f))
/**
 * @brief Return minimum of provided values.
 * @note Supports 2–6 arguments.
 * @return Smallest of given arguments.
 */
#define minv(...) (_args6(__VA_ARGS__, _min6, _min5, _min4, _min3, _min2)(__VA_ARGS__))

#define _max2(a,b) ((a) > (b) ? (a) : (b))
#define _max3(a,b,c) _max2(_max2(a,b), (c))
#define _max4(a,b,c,d) _max2(_max3(a,b,c), (d))
#define _max5(a,b,c,d,e) _max2(_max4(a,b,c,d), (e))
#define _max6(a,b,c,d,e,f) _max2(_max5(a,b,c,d,e), (f))
/**
 * @brief Return maximum of provided values.
 * @note Supports 2–6 arguments.
 * @return Greatest of given arguments.
 */
#define maxv(...) (_args6(__VA_ARGS__, _max6, _max5, _max4, _max3, _max2)(__VA_ARGS__))

/**
 * @brief Return sign of number.
 * @param value Input value.
 * @return `-1` if negative, `0` if zero, `1` if positive.
 */
#define has_sign(value) ({ \
  __typeof__(value) _v = (value); \
  (_v > 0) - (_v < 0); \
})

/**
 * @brief Safe ABS for integer and floating-point types.
 *   Signed integers: |x| with saturation (MIN → MAX to avoid UB).
 *   Unsigned integers: returns x unchanged.
 *   Floating point: uses fabsf/fabs/fabsl.
 */
#define absv(value) ({ \
  __auto_type _v = (value); \
  _Generic((_v), \
    signed char: ({ \
      signed char _t = _v; \
      (signed char)(_t == SCHAR_MIN ? SCHAR_MAX : (_t < 0 ? -_t : _t)); \
    }), \
    short: ({ \
      short _t = _v; \
      (short)(_t == SHRT_MIN ? SHRT_MAX : (_t < 0 ? -_t : _t)); \
    }), \
    int: ({ \
      int _t = _v; \
      (int)(_t == INT_MIN ? INT_MAX : (_t < 0 ? -_t : _t)); \
    }), \
    long: ({ \
      long _t = _v; \
      (long)(_t == LONG_MIN ? LONG_MAX : (_t < 0 ? -_t : _t)); \
    }), \
    long long: ({ \
      long long _t = _v; \
      (long long)(_t == LLONG_MIN ? LLONG_MAX : (_t < 0 ? -_t : _t)); \
    }), \
    unsigned char: ({ \
      unsigned char _t = _v; \
      unsigned char _half = (unsigned char)(UCHAR_MAX >> 1); \
      _t <= _half ? _t : (unsigned char)((unsigned char)0 - _t); \
    }), \
    unsigned short: ({ \
      unsigned short _t = _v; \
      unsigned short _half = (unsigned short)(USHRT_MAX >> 1); \
      _t <= _half ? _t : (unsigned short)((unsigned short)0 - _t); \
    }), \
    unsigned int: ({ \
      unsigned int _t = _v; \
      unsigned int _half = (unsigned int)(UINT_MAX >> 1); \
      _t <= _half ? _t : (unsigned int)(0u - _t); \
    }), \
    unsigned long: ({ \
      unsigned long _t = _v; \
      unsigned long _half = (unsigned long)(ULONG_MAX >> 1); \
      _t <= _half ? _t : (unsigned long)(0ul - _t); \
    }), \
    unsigned long long: ({ \
      unsigned long long _t = _v; \
      unsigned long long _half = (unsigned long long)(ULLONG_MAX >> 1); \
      _t <= _half ? _t : (unsigned long long)(0ull - _t); \
    }), \
    float: fabsf(_v), \
    double: fabs(_v), \
    long double: fabsl(_v), \
    default: (_v < 0 ? -_v : _v) \
  ); \
})

/**
 * @brief Clamp value to `[min, max]` range.
 * @param value Value to clamp.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return Clamped value in range `[min, max]`.
 */
#define clamp(value, min, max) ({ \
  __typeof__(value) _v   = (value); \
  __typeof__(value) _min = (min); \
  __typeof__(value) _max = (max); \
  _v < _min ? _min : (_v > _max ? _max : _v); \
})

/**
 * @brief Check if value is inside `[min, max]` range (inclusive).
 * @param value Value to check.
 * @param min Minimum allowed value.
 * @param max Maximum allowed value.
 * @return `true` if value ∈ [min, max], else `false`.
 */
#define in_range(value, min, max) ({ \
  __typeof__(value) _v  = (value); \
  __typeof__(value) _min = (min); \
  __typeof__(value) _max = (max); \
  (_v >= _min) && (_v <= _max); \
})

/**
 * @brief Swap two variables of the same type.
 * @param a First variable.
 * @param b Second variable.
 */
#define swap(a, b) do { \
  __typeof__(a) _tmp = (a); \
  (a) = (b); \
  (b) = _tmp; \
} while(0)

/**
 * @brief Get number of elements in static array.
 * @param x Static array (not a pointer).
 * @return Element count (e.g. 4 for `int x[4]`).
 */
#define array_len(x) (sizeof(x) / sizeof((x)[0]))

//-------------------------------------------------------------------------------------------------

// TODO: inne mijesce na to
#define TIM_ARR_INIT(freq_Hz, clock_Hz, center_aligned) (clock_Hz / freq_Hz / (center_aligned + 1))

//------------------------------------------------------------------------------------------------- TRY

/**
 * @brief Executes function and stores result in `err`. Breaks loop if result is non-zero.
 * @param fn Function or expression returning an error code.
 */
#define try_break(fn) \
  { err = (fn); if(err) break; }

/**
 * @brief Executes function and stores result in `err`. On error, adds `val` via bitwise OR and breaks loop.
 * @param fn Function or expression returning an error code.
 * @param val Additional error flag to OR with the result.
 */
#define try_break_or(fn, val) \
  { err = (fn); if(err) { err |= (val); break; } }

/**
 * @brief Executes function and stores result in `err`. On error, overwrites `err` with `val` and breaks loop.
 * @param fn Function or expression returning an error code.
 * @param val Error code to set if function fails.
 */
#define try_break_set(fn, val) \
  { err = (fn); if(err) { err = (val); break; } }

/**
 * @brief Executes function and stores result in `err`. On error, overwrites `err`, logs message, and breaks loop.
 * @param fn Function or expression returning an error code.
 * @param val Error code to set if function fails.
 * @param ... Log message (passed to LOG_ERR).
 */
#define try_break_set_log(fn, val, ...) \
  { err = (fn); if(err) { err = (val); LOG_ERR(__VA_ARGS__); break; } }

//-------------------------------------------------------------------------------------------------

#endif
