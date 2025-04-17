// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

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
    // Wyłącz diodę
    LED_Rst();
    delay(1000);  // Odczekaj 1s
  }
}

int main(void)
{
  // Inicjacja sterownika
  PLC_Init(); 
  // Dodanie wątku sterownika
  thread(&PLC_Thread, stack_plc, sizeof(stack_plc));
  // Dodanie funkcji loop jako wątek
  thread(&loop, stack_loop, sizeof(stack_loop));
  // Włączenie systemy przełączania wątków VRTS
  VRTS_Init();
  // W to miejsce program nigdy nie powinien dojść
  while(1);
}
