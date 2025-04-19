#include "opencplc.h" // Import funkcji sterownika

/**
 * @name  Projekt: ain/basic
 * @brief Odczytywane są wartości napięcia z wejścia AI1 oraz prądu z wejścia AI2
 */

// Pobieranie wartości wejść analogowych AI
void loop(void)
{
  // Ustawienie trybu pracy AI2 na 4-20mA
  AI2.mode_4_20mA = true;
  while(1) {
    // Odczyt napięcia z wejścia analogowego AI1
    float V = AIN_Voltage_V(&AI1);
    if(AIN_Value_Error(V)) {
      // Obsługa błędu dla napięcia poza spodziewanym zakresem
      // LOG z poziomem AIN_LOG_LEVEL będzie generowany automatycznie
    }
    else {
      // Logowanie odczytanego napięcia
      LOG_Info("Analog input AI1 voltage: %fV", V);
    }
    // Odczyt prądu z wejścia analogowego AI2
    float mA = AIN_Current_mA(&AI2);
    if(AIN_Value_OK(mA)) {
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