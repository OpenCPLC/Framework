#ifndef DIN_H_
#define DIN_H_

/**
 * @file din.h
 * @brief Obsługa wejść cyfrowych OpenCPLC
 */

#include <stdint.h>
#include <stdbool.h>
#include "eeprom.h"
#include "gpio.h"
#include "pwmi.h"
#include "vrts.h"

//-------------------------------------------------------------------------------------------------

#define DIN_DEFAULT 0xFFFF

#ifndef DIN_Signal
  #define DIN_Signal DIN_Rise
#endif

typedef struct {
  const char *name;      // Nazwa wyświetlana podczas zapytań `bash`
  GPIF_t gpif;           // Wskaźnik na wejście GPIF_t. Należy skonfigurować pola `gpio.port` i `gpio.pin`.
  EEPROM_t *eeprom;      // Wskaźnik na pamięć EEPROM_t do przechowywania wartości nieulotnych.
  bool fast_counter;     // Aktywacja trybu szybkiego licznika
  PWMI_t *pwmi;          // Wskaźnik na kontroler PWMI_t.
  TIM_Channel_t channel; // Kanał kontrolera PWMI_t sterujący
} DIN_t;

bool DIN_State(DIN_t *din);
bool DIN_RawState(DIN_t *din);
bool DIN_Toggling(DIN_t *din);
bool DIN_Rise(DIN_t *din);
bool DIN_Fall(DIN_t *din);
bool DIN_Edge(DIN_t *din);
bool DIN_Init(DIN_t *din);
bool DIN_Loop(DIN_t *din);
void DIN_Settings(DIN_t *din, uint16_t ton_ms, uint16_t toff_ms, uint16_t toggle_ms);
float DIN_Duty_Percent(DIN_t *din);
float DIN_Frequency_Hz(DIN_t *din);

//-------------------------------------------------------------------------------------------------
#endif
