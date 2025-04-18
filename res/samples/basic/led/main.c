#include "opencplc.h" // Import funkcji sterownika

/**
 * @name  Projekt: basic/led
 * @brief Dioda zmienia kolory w sekwencji: Red, Green, Blue.
 *        Na początku działa w trybie normalnym (ciągłe świecenie),
 *        następnie przechodzi w tryb migania.
 *        Tryby przełączają się naprzemiennie co cykl.
 */

// Wątek aplikacji
void loop(void)
{
  // Zmienna pomocnicza przechowująca stan, określająca czy dioda ma migać
  // dla wartości `true`, czy świecić ciągłym światłem dla wartości `false`.
  bool blink = false;
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