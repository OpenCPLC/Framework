// Import podstawowych funkcji sterownika.
#include "opencplc.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla wątku Debugera (bash + dbg + log)
static uint32_t stack_dbg[256];
// Stos pamięci dla czujnika RTD (PT100 / PT1000)
static uint32_t stack_rtd[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[1024];

void loop(void)
{
  while(1) {
    // Pobieranie wartości temperatury
    float temp = RTD_Temperature_C(&RTD1);
    // Wyświetlanie wartości temperatury
    LOG_Info("Temperature: %f°C", temp);
    // Oczekiwanie 2 sekundy
    delay(1000);
  }
}

int main(void)
{
  // Konfiguracja czujnika RTD do pracy z termometrem PT100
  // Wartości konfiguracyjne muszą być ustawione przed uruchomieniem systemu VRTS
  RTD1.nominal_ohms = RTD_Type_PT100; // Czujnik PT100 o nominalnej rezystancji 100Ω
  RTD1.wire = RTD_Wire_3; // Czujnik 3-przewodowy
  RTD1.reject = RTD_Reject_50Hz; // Odfiltrowywanie zakłóceń z sieci 50Hz
  // RTD1.oversampling = false; // Wykonaj pojedynczy pomiar
  RTD1.oversampling = 7; // Wykonaj 7 pomiarów i uśrednij ich wartość
  RTD1.interval_ms = 1000; // Okres wykonywania pomiarów (zbyt częste pomiary mogą powodować samonagrzewanie)
  // Dodanie wątku sterownika
  thread(&PLC_Thread, stack_plc, sizeof(stack_plc) / sizeof(uint32_t));
  // Dodanie wątku debuger'a (bash + dbg + log)
  thread(&DBG_Loop, stack_dbg, sizeof(stack_dbg) / sizeof(uint32_t));
  // Dodanie wątku czujnika RTD (PT100 / PT1000)
  thread(&RTD_Thread, stack_rtd, sizeof(stack_rtd) / sizeof(uint32_t));
  // Dodanie funkcji loop jako wątek
  thread(&loop, stack_loop, sizeof(stack_loop) / sizeof(uint32_t));
  // Włączenie systemy przełączania wątków VRTS
  VRTS_Init();
  // W to miejsce program nigdy nie powinien dojść
  while(1);
}