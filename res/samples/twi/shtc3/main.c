#include "opencplc.h" // Import funkcji sterownika
#include "shtc3.h" // Interfejs do obsługi czujnika SHTC3

/**
 * @name  Projekt: twi/shtc3
 * @brief Odczytywane są wartości potencjometru oraz napięcie zasilania
 */

void loop(void)
{
  while(1) {
    float temp = SHTC3_Temperature_C(); // Pobierz wartość temperatury
    LOG_Info("SHTC3 temperature: %f°C", temp);
    float humi = SHTC3_Humidity_RH(); // Pobierz wartość wilgotności
    LOG_Info("SHTC3 humidity: %f%% RH", humi);
    delay(1000); // Zaczekaj 1000ms
  }
}

// Definicja funkcji wątku o nazwie SHTC3_Thread.
// Makro 'thread_fnc' tworzy funkcję, bez inicjalizacji '__NOP',
// która w swojej pętli wykonuje 'SHTC3_Loop()' oraz 'let()'.
// Inicjalizacja I2C jest wykonywana w wątku PLC.
thread_fnc(SHTC3_Thread, __NOP, SHTC3_Loop)

stack(stack_plc, 256); // Stos pamięci dla wątku PLC
stack(stack_dbg, 256); // Stos pamięci dla wątku debug'era (logs + bash)
stack(stack_shtc, 128); // Stos pamięci dla czujnika SHTC3
stack(stack_loop, 1024); // Stos pamięci dla funkcji loop

int main(void)
{
  thread(PLC_Thread, stack_plc); // Dodanie wątku sterownika
  thread(DBG_Loop, stack_dbg); // Dodanie wątku debug'era (logs + bash)
  thread(SHTC3_Thread, stack_shtc); // Dodanie funkcji obsługującej czujnik SHTC3
  thread(loop, stack_loop); // Dodanie funkcji loop jako wątek
  vrts_init(); // Włączenie systemy przełączania wątków VRTS
  while(1); // W to miejsce program nigdy nie powinien dojść
}