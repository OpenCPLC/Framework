// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

// Wartość domyślna zmiennej `value`, używana, gdy nie ma jej w pamięci EEPROM
#define DEFAULT_VALUE 1
// Zmienna robocza, której wartość będzie przechowywana w pamięci EEPROM
uint32_t value;

void loop(void)
{
  if(EEPROM_Load(&value)) {
    // Obsługa, gdy zmienna nigdy nie była zapisana w pamięci EEPROM
    // Może być wykorzystana do wczytania wartości domyślnej
    value = DEFAULT_VALUE;
    DBG_String("EEPROM default value:");
  }
  else {
    // Wartość z pamięci EEPROM została znaleziona i przekopiowana do zmiennej `value`
    DBG_String("EEPROM load value:");
  }
  DBG_uDec(value);
  DBG_Enter();

  while(1) {
    // Pojawienie się sygnału na wejściu cyfrowym DI1
    if(DIN_Rise(&DI1)) {
      value++; // Zwiększenie wartości zmiennej `value` o 1
      DBG_String("EEPROM inc value:");
      DBG_uDec(value);
      DBG_Enter();
    }
    // Pojawienie się sygnału na wejściu cyfrowym DI2
    if(DIN_Rise(&DI2)) {
      EEPROM_Save(&value); // Zapisz aktualną wartość zmiennej `value` w pamięci EEPROM
      DBG_String("EEPROM save");
      DBG_Enter();
    }
    // Pojawienie się sygnału na wejściu cyfrowym DI3
    if(DIN_Rise(&DI3)) {
      DBG_String("PWR Reset");
      DBG_Enter();
      DBG_BeforeReset(); // Poczekaj, aż wszystkie dane z DBG zostaną wysłane
      PWR_Reset(); // Zresetuj sterownik - program rozpocznie się od nowa
    }
    if(DIN_Rise(&DI4)) {
      EEPROM_Remove(); // Reset wszystkich zmiennych z pamięci EEPROM
      DBG_String("EEPROM remove");
      DBG_Enter();
      value = DEFAULT_VALUE; // Ustawienie wartości na domyślną
    }
    let();
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
