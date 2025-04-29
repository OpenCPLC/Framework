#ifndef BUFF_H_
#define BUFF_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "new.h"

#ifndef BUFF_MESSAGE_LIMIT
  #define BUFF_MESSAGE_LIMIT 32
#endif

//-------------------------------------------------------------------------------------------------

typedef struct {
  int32_t (*Echo)(uint8_t);
  int32_t (*Enter)(void);
  int32_t (*Kill)(void);
  int32_t (*Execute)(void);
  void (*Stop)(void);
  void (*Run)(bool);
  volatile uint8_t esc;
  int16_t cursor;
} BUFF_Console_t;

typedef struct {
  uint8_t *mem;
  uint16_t size;
  uint8_t *end;
  uint8_t *tail;
  volatile uint8_t *head;
  BUFF_Console_t *console;
  volatile uint16_t msg_counter;
  uint16_t msg_size[BUFF_MESSAGE_LIMIT];
  uint16_t msg_head;
  uint16_t msg_tail;
  bool msg_history[BUFF_MESSAGE_LIMIT];
  volatile bool break_allow;
} BUFF_t;

void BUFF_Init(BUFF_t *buff);
void BUFF_Push(BUFF_t *buff, uint8_t value);
bool BUFF_Break(BUFF_t *buff);
uint16_t BUFF_Size(BUFF_t *buff);
uint16_t BUFF_Array(BUFF_t *buff, uint8_t *array);
char *BUFF_String(BUFF_t *buff);
bool BUFF_Skip(BUFF_t *buff);
void BUFF_Clear(BUFF_t *buff);

//-------------------------------------------------------------------------------------------------
#endif
