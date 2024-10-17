#ifndef STREAM_H_
#define STREAM_H_

#include "exstring.h"
#include "crc.h"
#include "main.h"

#ifndef STREAM_ADDRESS
  #define STREAM_ADDRESS 0
#endif

#ifndef STREAM_CRC
  #define STREAM_CRC 0
#endif

#ifndef STREAM_DBG
  #define STREAM_DBG 0
#endif

//-------------------------------------------------------------------------------------------------

typedef enum {
  STREAM_Mode_String = 0,
  STREAM_Mode_Data = 1
} STREAM_Mode_e;

typedef enum {
  STREAM_Modify_Free = 0,
  STREAM_Modify_Lowercase = 1,
  STREAM_Modify_Uppercase = 2
} STREAM_Modify_e;

typedef struct {
  const char *name;
  STREAM_Mode_e mode;
  STREAM_Modify_e modify;
  char *(*Read)(void);
  uint16_t (*Size)(void);
  uint16_t counter;
  #if(STREAM_ADDRESS)
    void (*Readdress)(uint8_t);
    uint8_t address;
  #endif
  #if(STREAM_CRC)
    CRC_t *crc;
  #endif
} STREAM_t;

//-------------------------------------------------------------------------------------------------

uint16_t STREAM_Read(STREAM_t *stream, char *** argv);

//-------------------------------------------------------------------------------------------------
#endif
