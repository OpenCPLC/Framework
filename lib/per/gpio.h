#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stm32g0xx.h"
#include "int.h"
#include "pwr.h"
#include "vrts.h"
#include "main.h"

#ifndef GPIO_INCLUDE_WAKEUP
  #define GPIO_INCLUDE_WAKEUP 0
#endif

//------------------------------------------------------------------------------------------------- GPIO

typedef enum {
  GPIO_Mode_Input = 0x00,
  GPIO_Mode_Output = 0x01,
  GPIO_Mode_Alternate = 0x02,
  GPIO_Mode_Analog = 0x03
} GPIO_Mode_e;

typedef enum {
  GPIO_Pull_Floating = 0x00,
  GPIO_Pull_Up = 0x01,
  GPIO_Pull_Down = 0x02,
} GPIO_Pull_e;

typedef enum {
  GPIO_OutType_PushPull = 0x00,
  GPIO_OutType_OpenDrain = 0x01
} GPIO_OutType_e;

typedef enum {
  GPIO_Speed_VeryLow = 0x00,
  GPIO_Speed_Low = 0x01,
  GPIO_Speed_High = 0x02,
  GPIO_Speed_VeryHigh = 0x03
} GPIO_Speed_e;

#if(GPIO_INCLUDE_WAKEUP)
typedef enum {
  GPIO_WakeupPull_Floating = 0x00,
  GPIO_WakeupPull_Up = 0x01,
  GPIO_WakeupPull_Down = 0x02
} GPIO_WakeupPull_e;
#endif

#define GPIO_DEFAULT { NULL, 0, 0, 0, 0, 0, 0, 0, 0 }
#define GPIO_ALTERNATE { NULL, 0, 0, GPIO_Mode_Alternate, 0, 0, GPIO_Speed_VeryHigh, 0, 0 }

typedef struct {
  GPIO_TypeDef *port;
  uint8_t pin;
  bool reverse;
  GPIO_Mode_e mode;
  GPIO_Pull_e pull;
  GPIO_OutType_e out_type;
  GPIO_Speed_e speed;
  #if(GPIO_INCLUDE_WAKEUP)
    GPIO_WakeupPull_e wakeup_pull;
  #endif
  uint8_t alternate;
  bool set;
} GPIO_t;

#pragma pack(1)
typedef struct {
  GPIO_TypeDef *port;
  uint8_t pin;
  uint8_t alternate;
} GPIO_Map_t;
#pragma pack()

void GPIO_Init(GPIO_t *gpio);
void GPIO_InitList(GPIO_t *gpio, ...);
void GPIO_AlternateInit(const GPIO_Map_t *gpio_map, bool open_drain_pull_up);
void GPIO_SupplyInit(GPIO_t *gpio);
void GPIO_Set(GPIO_t *gpio);
void GPIO_Rst(GPIO_t *gpio);
void GPIO_Tgl(GPIO_t *gpio);
bool GPIO_In(GPIO_t *gpio);
bool GPIO_NotIn(GPIO_t *gpio);
void GPIO_ModeInput(GPIO_t *gpio);
void GPIO_ModeOutput(GPIO_t *gpio);

//------------------------------------------------------------------------------------------------- GPIF

#define GPIF_TON_DELAULT 100
#define GPIF_TOFF_DELAULT 100
#define GPIF_TOGGLE_DELAULT 500
#define GPIF_TRESET 5000

typedef struct {
  GPIO_t gpio;
  bool value;
  bool rise;
  bool fall;
  uint32_t ton_ms;
  uint32_t toff_ms;
  uint32_t toggle_ms;
  uint64_t _tio_tick;
  uint64_t _res_rise_tick;
  uint64_t _res_fall_tick;
  uint64_t _toggle_tick;
} GPIF_t;

bool GPIF_Input(GPIF_t *gpif);
bool GPIF_Toggling(GPIF_t *gpif);
bool GPIF_Rise(GPIF_t *gpif);
bool GPIF_Fall(GPIF_t *gpif);
bool GPIF_Edge(GPIF_t *gpif);
void GPIF_Init(GPIF_t *gpif);
bool GPIF_Loop(GPIF_t *gpif);

//------------------------------------------------------------------------------------------------- EXTI

typedef struct {
  GPIO_TypeDef *port;
  uint8_t pin;
  GPIO_Mode_e mode;
  GPIO_Pull_e pull;
  bool rise;
  bool fall;
  INT_Prioryty_t int_prioryty;
  bool one_shot;
  bool enable;
  void (*rise_function)(void*);
  void *rise_struct;
  uint16_t rise_cnt;
  void (*fall_function)(void*);
  void *fall_struct;
  uint16_t fall_cnt;
} EXTI_t;

void EXTI_Init(EXTI_t *exti);
void EXTI_On(EXTI_t *exti);
void EXTI_Off(EXTI_t *exti);
uint16_t EXTI_In(EXTI_t *exti);
uint16_t EXTI_Rise(EXTI_t *exti);
uint16_t EXTI_Fall(EXTI_t *exti);

//-------------------------------------------------------------------------------------------------
#endif
