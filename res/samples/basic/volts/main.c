#include "opencplc.h" // Import funkcji sterownika

/**
 * @name  Projekt: basic/volts
 * @brief Odczytywane są wartości potencjometru oraz napięcie zasilania
 */

// Pobieranie nastaw z potencjometrów POT oraz pamiar napięcia zasilania
void loop(void)
{
  while(1) {
    // Odczyt wartości potencjometru
    float value = POT_Percent(&POT1);
    LOG_Info("Potentiometer value: %f%%", value);
    // Odczyt napięcia zasilania
    float supply = VCC_Voltage_V();
    LOG_Info("Power supply: %fV", supply);
    // Odczekaj 1s
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