#ifndef AIN_H_
#define AIN_H_

#include <stdint.h>
#include <stdbool.h>
#include "exmath.h"
#include "vrts.h"
#include "adc.h"
#include "log.h"
#include "main.h"

// Średni czas próbkowania (uśredniania pomiarów) ADC w milisekundach
#ifndef AIN_AVERAGE_TIME_MS
  #define AIN_AVERAGE_TIME_MS 200
#endif

// Poziom logowania dla błędów (over-value/under-value)
#ifndef AIN_LOG_LEVEL
  #define AIN_LOG_LEVEL LOG_LEVEL_ERR
#endif

// Sprawdza, czy wystąpił błąd (wartość ujemna wskazuje na błąd)
#define AIN_Value_Error(value) (value < 0)
#define AIN_Value_OK(value) (value >= 0)

typedef enum {
  AIN_Error_OverValue = -1, // Zbyt wysoka wartość prądu/napięcia
  AIN_Error_UnderValue = -2 // Zbyt niska wartość prądu/napięcia
} AIN_Error_e;

typedef enum {
  AIN_Threshold_Voltage_V = 1,
  AIN_Threshold_Current_mA = 2,
  AIN_Threshold_Percent = 5,
} AIN_Threshold_e;

//-------------------------------------------------------------------------------------------------

typedef struct {
  const char *name; // Nazwa wejścia analogowego
  uint16_t *data; // Wskaźnik do danych wejścia
  uint16_t count; // Licznik próbek lub liczba odczytów
  bool mode_4_20mA; // 'true' dla pracy zakresie 4-20mA (lub 2-10V)
  float value; // Podręczna wartość wyjścia bez konwersji
  float threshold_up;
  float threshold_down;
  uint64_t tick;
} AIN_t;

// Sortuje dane z bufora ADC do tablic kanałów
void AIN_Sort(uint16_t *buff, uint16_t channels, uint16_t samples, uint16_t data[channels][samples]);
void AIN_Threshold(AIN_t *ain, float down, float up, AIN_Threshold_e scale); // Ustawia progi błędu
float AIN_Raw(AIN_t *ain);; // Zwraca surową wartość z ADC
float AIN_Voltage_V(AIN_t *ain); // Zwraca napięcie [V] na wejściu analogowym
float AIN_Current_mA(AIN_t *ain); // Zwraca prąd [mA] na wejściu analogowym
float AIN_Percent(AIN_t *ain); // Zwraca wartość w procentach [0-100%] na wejściu analogowym
float AIN_Normalized(AIN_t *ain); // Zwraca wartość znormalizowaną [0-1] na wejściu analogowym
float POT_Voltage_V(AIN_t *ain); // Zwraca napięcie [V] na potencjometrze
float POT_Percent(AIN_t *ain); // Zwraca wartość w procentach [0-100%] na potencjometrze
float POT_Normalized(AIN_t *ain); // Zwraca wartość znormalizowaną [0-1] na potencjometrze

//-------------------------------------------------------------------------------------------------
#endif


