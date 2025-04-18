// Import podstawowych funkcji sterownika.
#include "opencplc.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla wątku Debuger'a (bash + dbg + log)
static uint32_t stack_dbg[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[1024];

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
