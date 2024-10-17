#ifndef DOUT_H_
#define DOUT_H_

/**
 * @file dout.h
 * @brief Obsługa wyjść cyfrowych tranzystorowych i triakowych OpenCPLC
 */

#include <stdint.h>
#include <stdbool.h>
#include "eeprom.h"
#include "pwm.h"
#include "vrts.h"
#include "exstring.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef DOUT_RELAY_DELAY
  #define DOUT_RELAY_DELAY 200
#endif

typedef enum {
  DOUT_Hash_Dout = 2090192161,
  DOUT_Hash_Set = 193505681,
  DOUT_Hash_On = 5863682,
  DOUT_Hash_Enable = 4218778540,
  DOUT_Hash_Rst = 193505054,
  DOUT_Hash_Reset = 273105544,
  DOUT_Hash_Off = 193501344,
  DOUT_Hash_Disable = 314893497,
  DOUT_Hash_Tgl = 193506828,
  DOUT_Hash_Toggle = 512249127,
  DOUT_Hash_Sw = 5863823,
  DOUT_Hash_Switch = 482686839,
  DOUT_Hash_Pulse = 271301518,
  DOUT_Hash_Impulse = 2630979716,
  DOUT_Hash_Burst = 254705173,
  DOUT_Hash_Duty = 2090198667,
  DOUT_Hash_Fill = 2090257196
} DOUT_Hash_e;

typedef struct {
  bool relay;            // Czy wyjście jest przekaźnikowe RO
  char *name;            // Nazwa wyświetlana podczas zapytań `bash`
  GPIO_t gpio;           // Wskaźnik na wyjście GPIO_t. Należy skonfigurować pola `port` i `pin`.
  PWM_t *pwm;            // Wskaźnik na kontroler PWM_t.
  TIM_Channel_e channel; // Kanał kontrolera PWM_t sterujący 
  EEPROM_t *eeprom;      // Wskaźnik na pamięć EEPROM_t do przechowywania wartości nieulotnych.
  uint32_t save;         // Określa, czy zachować stan wyjścia po resecie.
  uint32_t value;        // Wypełnienie [%] sygnału na wyjściu. Zaleca się ustawić funkcją `DOUT_Set`.
  uint32_t cycles;       // Całkowita liczba przełączeń przekażnika.
  bool pulse;            // Flaga informująca, czy przekażnik wykonuje impuls
  uint64_t _stun;
  uint16_t _pulse;
} DOUT_t;

float PWM_Frequency(PWM_t *pwm, float frequency);
float DOUT_Duty(DOUT_t *dout, float duty);
void DOUT_Set(DOUT_t *dout);
void DOUT_Rst(DOUT_t *dout);
void DOUT_Tgl(DOUT_t *dout);
void DOUT_Preset(DOUT_t *dout, bool value);
bool DOUT_Pulse(DOUT_t *dout, uint16_t time_ms);
uint32_t DOUT_State(DOUT_t *dout);
bool DOUT_IsPulse(DOUT_t *relay);
float DOUT_GetDuty(DOUT_t *dout);
void DOUT_Init(DOUT_t *dout);
void DOUT_Loop(DOUT_t *dout);
void DOUT_Settings(DOUT_t *dout, bool save);

void DOUT_Print(DOUT_t *dout);
void DOUT_BashInit(DOUT_t *douts[]);
bool DOUT_Bash(char **argv, uint16_t argc);

//-------------------------------------------------------------------------------------------------
#endif