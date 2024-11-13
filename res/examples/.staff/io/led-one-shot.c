// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

void loop(void)
{
  while(1) {
    // Wykonaj pojedyncze mignięcie diodą czerwoną trwające 100ms
    LED_OneShoot(RGB_Red, 100);
    delay(2000); // Odczekaj 2s
    // Wykonaj pojedyncze mignięcie diodą zieloną trwające 300ms
    LED_OneShoot(RGB_Green, 300);
    delay(2000); // Odczekaj 2s
    // Wykonaj pojedyncze mignięcie diodą niebieską trwające 600ms
    LED_OneShoot(RGB_Blue, 600);
    delay(2000); // Odczekaj 2s
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
