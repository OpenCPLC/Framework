#include "opencplc.h" // Import funkcji sterownika

/**
 * @name  Projekt: ain/scale
 * @brief Odczytywanie sygnału z wejścia prądowego 4-20mA
 *        oraz przeliczanie go na wartość temperatury [°C]
 */

//----------------------------------------------------------------------------- math-style

// Definicja zakresu temperatury
#define TEMPERATURE_MIN_4mA  -40.0 // Minimalna temperatura [°C] przy 4mA
#define TEMPERATURE_MAX_20mA 125.0 // Maksymalna temperatura [°C] przy 20mA
// Obliczenia parametrów a, b dla funkcji liniowej,
// aby wyznaczyć temperaturę na podstawie prądu
#define PARAM_A ((TEMPERATURE_MAX_20mA - TEMPERATURE_MIN_4mA) / (20.0 - 4.0))
#define PARAM_B (TEMPERATURE_MIN_4mA - (PARAM_A * 4))

// Mapowanie AI1 na nazwę temperature_ai z użyciem wskaźnika
static AIN_t *temperature_ai = &AI1;

// Funkcja zwracająca wartość temperatury na podstawie aktualnego prądu
static float GetTemperature(void)
{
  float current_mA = AIN_Current_mA(temperature_ai); // Pobierz aktualny prąd
  if(AIN_Value_Error(current_mA)) { // Sprawdzenie błędu pomiaru
    LOG_Error("Temperature measurement fault"); // Logowanie błędu
    return NaN; // Zwróć wartość błędu
  }
  // Obliczenie temperatury z funkcji liniowej
  float temperature = (PARAM_A * current_mA) + PARAM_B;
  // Informacja o zmierzonej temperaturze
  LOG_Info("Temperature: %f°C", temperature);
  return temperature;
}

// Funkcja skalująca prąd na temperaturę "math-style"
static void ScaleMath(void)
{
  temperature_ai->mode_4_20mA = true; // Ustawienie trybu 4-20mA dla AI
  while(1) {
    float temperature = GetTemperature(); // Pobierz wartość temperatury
    // TODO: Użycie wartości temperatury
    delay(1000); // Pomiar wykonywany co ok. 1s
  }
}

//----------------------------------------------------------------------------- plc-style

// Definicja zakresu temperatury
#define TEMPERATURE_MIN  -60.0 // Minimalna temperatura [°C]
#define TEMPERATURE_MAX 100.0 // Maksymalna temperatura
// Definicja zakresu prądu wejściowego
#define ERROR_VAL (float)0xFFFF // Wartość zwracana w przypadku błędu

// Mapowanie AI1 na nazwę AI_Temperature z użyciem definicji
#define AI_Temperature AI1

// Funkcja skalująca prąd na temperaturę "plc-style"
static void ScalePLC(void)
{
  float temperature;
  AI_Temperature.mode_4_20mA = true; // Ustawienie trybu 4-20mA dla AI
  while(1) {
    // Pobierz znormalizowaną wartość prądu [0-1]
    float normalized_current = AIN_Normalized(&AI_Temperature);
    if(AIN_Value_OK(normalized_current)) { // Sprawdzenie, czy pomiar jest prawidłowy
      // Przeskalowanie do temperatury
      temperature = normalized_current * (TEMPERATURE_MAX - TEMPERATURE_MIN) + TEMPERATURE_MIN;
      LOG_Info("Temperature: %f°C", temperature); // Logowanie zmierzonej temperatury
    }
    else {
      temperature = ERROR_VAL; // Wartość w przypadku błędu pomiaru
      LOG_Error("Temperature measurement fault"); // Logowanie błędu pomiaru
    }
    // TODO: Użycie wartości temperatury
    delay(1000); // Pomiar wykonywany co ok. 1s
  }
}

//-----------------------------------------------------------------------------

void loop(void)
{
  // Wywołaj tę funkcję, jeśli chcesz użyć skalowania z "math-style"
  ScaleMath();
  // Jeśli preferujesz skalowanie z "plc-style", zakomentuj powyższą linię i odkomentuj poniższą:
  // ScalePLC();
}

stack(stack_plc, 256); // Stos pamięci dla wątku PLC
stack(stack_dbg, 256); // Stos pamięci dla wątku debug'era (logs + bash)
stack(stack_loop, 1024); // Stos pamięci dla funkcji loop

int main(void)
{
  thread(PLC_Thread, stack_plc); // Dodanie wątku sterownika
  thread(DBG_Loop, stack_dbg); // Dodanie wątku debug'era (logs + bash)
  thread(loop, stack_loop); // Dodanie funkcji loop jako wątek
  vrts_init(); // Włączenie systemy przełączania wątków VRTS
  while(1); // W to miejsce program nigdy nie powinien dojść
}