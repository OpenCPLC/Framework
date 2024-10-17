#ifndef OPENCPLC_DIN_H_
#define OPENCPLC_DIN_H_

#include "dout.h"
#include "din.h"
#include "ain.h"
#include "dbg.h"
#include "bash.h"
#include "rgb.h"
#include "vrts.h"
#include "main.h"

#define PLC_RS485

#ifndef PLC_BOOTLOADER
  #define PLC_BOOTLOADER 0
#endif
#ifndef PLC_BASETIME
  #define PLC_BASETIME 10
#endif

// Wyjścia cyfrowe tranzystorowe (TO)
extern DOUT_t TO1;
extern DOUT_t TO2;
extern DOUT_t TO3;
extern DOUT_t TO4;
extern DOUT_t TO5;
extern DOUT_t TO6;
extern DOUT_t TO7;
extern DOUT_t TO8;
extern DOUT_t TO9;
extern DOUT_t TO10;
extern DOUT_t TO11;
extern DOUT_t TO12;
void TO149C_Frequency(float frequency);
void TO2_Frequency(float frequency);
void TO3_Frequency(float frequency);
void TO6_Frequency(float frequency);
void TO7A_Frequency(float frequency);
void TO_Frequency(float frequency);

// Wejścia cyfrowe (DI)
extern DIN_t DI1;
extern DIN_t DI2;
extern DIN_t DI3;
extern DIN_t DI4;
extern DIN_t DI5;
extern DIN_t DI6;
extern DIN_t DI7;
extern DIN_t DI8;
extern DIN_t DI9;
extern DIN_t DI10;
extern DIN_t DI11;
extern DIN_t DI12;

// Wejścia analogowe (AI)
extern AIN_t AI1;
extern AIN_t AI2;
extern AIN_t AI3;
extern AIN_t AI4;
float VCC_Value(void);

// RS485
#ifndef RS_BUFFER_SIZE
  #define RS_BUFFER_SIZE 1000
#endif
extern UART_t RS;

// Dioda RGB
extern RGB_t RGB;

// Functions
void PLC_Init(void);
void PLC_Loop(void);
void PLC_Thread(void);

//-------------------------------------------------------------------------------------------------
#endif
