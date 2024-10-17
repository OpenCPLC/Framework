// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

void loop(void)
{
  // Zmienna pomocnicza przechowująca stan, określająca czy dioda ma migać
  // dla wartości `true`, czy świecić ciągłym światłem dla wartości `false`.
  bool blink = true;
  while(1) {
    if(blink) {
      // Włączenie migania z częstotliwością zmiany stanu diody wynoszącą 200ms
      LED_Blink_ON(200); 
    }
    else {
      // Wyłączenie migania diody (dioda będzie świecić ciągłym światłem)
      LED_Blink_OFF();
    }
    // Ustawienie diody informacyjnej, na kolor czerwony
    LED_Set(RGB_Red);
    delay(3000); // Odczekaj 3s
    // Ustawienie diody informacyjnej, na kolor zielony
    LED_Set(RGB_Green);
    delay(3000); // Odczekaj 3s
    // Ustawienie diody informacyjnej, na kolor niebieski
    LED_Set(RGB_Blue);
    delay(3000); // Odczekaj 3s  
    // Zmiana trybu z migania na świecenie i odwrotnie
    blink = !blink;
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
