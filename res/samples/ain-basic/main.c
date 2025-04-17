// Import podstawowych funkcji sterownika.
#include "opencplc.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla wątku Debuger'a (bash + dbg + log)
static uint32_t stack_dbg[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[1024];

// Pobieranie wartości wejść analogowych AI
void loop(void)
{
  // Ustawienie trybu pracy AI2 na 4-20mA
  AI2.mode_4_20mA = true;
  while(1) {
    // Odczyt napięcia z wejścia analogowego AI1
    float V = AIN_Voltage_V(&AI1);
    if(AIN_Error(V)) {
      // Obsługa błędu dla napięcia poza spodziewanym zakresem
      // LOG z poziomem AIN_LOG_LEVEL będzie generowany automatycznie
    }
    else {
      // Logowanie odczytanego napięcia
      LOG_Info("Analog input AI1 voltage: %fV", V);
    }
    // Odczyt prądu z wejścia analogowego AI2
    float mA = AIN_Current_mA(&AI2);
    if(AIN_OK(mA)) {
      // Logowanie odczytanego prądu
      LOG_Info("Analog input AI2 current: %fmA", mA);
    }
    else {
      // Obsługa błędu dla prądu poza spodziewanym zakresem
      // LOG z poziomem AIN_LOG_LEVEL będzie generowany automatycznie
    }
    // Pomiary wykonywane co ok. 1s
    delay(1000);
  }
}

int main(void)
{
  // Dodanie wątku sterownika
  thread(&PLC_Thread, stack_plc, sizeof(stack_plc) / sizeof(uint32_t));
  // Dodanie wątku debuger'a (bash + dbg + log)
  thread(&DBG_Loop, stack_dbg, sizeof(stack_dbg) / sizeof(uint32_t));
  // Dodanie funkcji loop jako wątek
  thread(&loop, stack_loop, sizeof(stack_loop) / sizeof(uint32_t));
  // Włączenie systemy przełączania wątków VRTS
  VRTS_Init();
  // W to miejsce program nigdy nie powinien dojść
  while(1);
}
