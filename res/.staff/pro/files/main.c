// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla wątku Debugera (bash + dbg + log)
stack(stack_dbg, 256);
// Stos pamięci dla funkcji loop
stack(stack_loop, 1024);

void loop(void)
{
  while(1) {
    // Ustawienie diody informacyjnej, aby świeciła na czerwoną
    LED_Set(RGB_Red);
    delay(1000); // Odczekaj 1s
    // Ustawienie diody informacyjnej, aby świeciła na zieloną
    LED_Set(RGB_Green);
    delay(1000); // Odczekaj 1s
    // Ustawienie diody informacyjnej, aby świeciła na niebieską
    LED_Set(RGB_Blue);
    delay(1000); // Odczekaj 1s
    // Wyłączenie diody informacyjnej
    LED_Rst();
    delay(1000); // Odczekaj 1s
  }
}

int main(void)
{
  // Dodanie wątku sterownika
  thread(PLC_Thread, stack_plc);
  // Dodanie wątku debuger'a (bash + dbg + log)
  thread(DBG_Loop, stack_dbg);
  // Dodanie funkcji loop jako wątek
  thread(loop, stack_loop);
  // Włączenie systemy przełączania wątków VRTS
  vrts_init();
  // W to miejsce program nigdy nie powinien dojść
  while(1);
}