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

typedef struct {
  const char *name;  // Nazwa wyświetlana podczas zapytań `bash`
  GPIF_t gpif;       // Wskaźnik na wejście GPIF_t. Należy skonfigurować pola `gpio.port` i `gpio.pin`.
  EEPROM_t *eeprom;  // Wskaźnik na pamięć EEPROM_t do przechowywania wartości nieulotnych.
  bool fast_counter; // Aktywacja trybu szybkiego licznika
  float *frequency;  // Wskaźnik zwracający wartość częstotliwości dla trybu szybkiego licznika.
  float *fill;       // Wskaźnik zwracający wartość wypełnienia dla trybu szybkiego licznika.
} DIN_t;

bool DIN_State(DIN_t *din);
bool DIN_RawState(DIN_t *din);
bool DIN_Toggling(DIN_t *din);
bool DIN_Rise(DIN_t *din);
bool DIN_Fall(DIN_t *din);
bool DIN_Edge(DIN_t *din);
float DIN_Freq(DIN_t *din);
float DIN_Fill(DIN_t *din);
bool DIN_Init(DIN_t *din);
bool DIN_Loop(DIN_t *din);
void DIN_Settings(DIN_t *din, uint16_t ton_ms, uint16_t toff_ms, uint16_t toggle_ms);

//-------------------------------------------------------------------------------------------------
#endif
