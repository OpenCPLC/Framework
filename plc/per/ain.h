#ifndef AIN_H_
#define AIN_H_

/**
 * @file ain.h
 * @brief Obsługa wejść analogowych OpenCPLC
 */

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  AIN_Type_Volts,  // Wejście napięciowe   0-10V
  AIN_Type_mAmps,  // Wejście prądowe      0-20mA
  AIN_Type_Percent // Wartość w procentach 0-100%
} AIN_Type_t;

typedef struct {
  ADC_t *adc;      // Wskaźnik na kontroler przetwornika ADC.
  uint8_t channel; // Indeks do tabeli output, pod którym znajduje się wartość powiązana z strukturą 
  AIN_Type_t type; // Rodzaj wejścia: napięciowe/prądowe
  #if(ADC_RECORD)
    bool record;   // TODO: Ustawienie flagi pozwoli na pomiar wartości szybkozmiennych na wejściu
  #endif
} AIN_t;

uint16_t AIN_Raw(AIN_t *ain);
float AIN_Volts(AIN_t *ain);
float AIN_mAmps(AIN_t *ain);
float AIN_Percent(AIN_t *ain);
float AIN_Value(AIN_t *ain);

//-------------------------------------------------------------------------------------------------
#endif