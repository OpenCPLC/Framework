// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"
// Import funkcji dla Modbus RTU Master
#include "modbus-master.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

#define MODBUS_ADDR 0x07 // Adres urządzenia Modbus Slave
#define MODBUS_REG_COUNT 3 // Ilość rejestrów Modbus
#define MODBUS_TIMEOUT 1000 // Timeout dla wszystkich funkcji komunikacyjnych w protokole Modbus wynosi 1 sekundę.

// Stworzenie mapy rejestrów Modbus'a
typedef enum {
  MODBUS_Reg_DigitalInputs,
  MODBUS_Reg_HexConfig,
  MODBUS_Reg_DecConfig,
} MODBUS_Reg_e;

// Deklaracja pamięci podręcznej dla rejestrów odczytanych z urządzenia Slave
uint16_t modbus_memory[MODBUS_REG_COUNT];
// Mapowanie interfejsu `RS1` na protokół modbus_master
UART_t *modbus_master = &RS1;

void loop(void)
{
  while(1) {
    // Jeżeli zostanie naciśnięty przycisk, wykonaj operację wpisywania
    if(DIN_Fall(&BTN)) {
      // Wpisz do rejestru `HexConfig` wartość `0x1234`
      if(MODBUS_PresetRegister(modbus_master, MODBUS_ADDR, MODBUS_Reg_HexConfig, 0x1234, MODBUS_TIMEOUT)) {
        // Obsługa błędów komunikacji lub braku odpowiedzi dla operacji wpisywania
        DBG_String("MODBUS write:no-respond");
        DBG_Enter();
        LED_OneShoot(RGB_Yellow, 200);
        // Dodatkowe opóźnienie umożliwia urządzeniom slave przygotowanie się na kolejne ramki komunikacyjne
        delay(500);
      }
      else {
        // Obsługa poprawnej komunikacji dla operacji wpisywania
        DBG_String("MODBUS write:ok");
        DBG_Enter();
        LED_OneShoot(RGB_Cyan, 200);
      }
    }
    // Jeśli nie zostanie odnotowane zdarzenie wciśnięcia przycisku, wykonaj operację odczytu
    else {
      // Odczytaj 3 rejestry, rozpoczynając od rejestru `DigitalInputs`, a wartości odczytane umieść w tablicy `modbus_memory`
      if(MODBUS_ReadHoldingRegisters(modbus_master, MODBUS_ADDR, MODBUS_Reg_DigitalInputs, 3, (uint16_t *)&modbus_memory, MODBUS_TIMEOUT)) {
        // Obsługa błędów komunikacji lub braku odpowiedzi dla operacji odczytu
        DBG_String("MODBUS read:no-respond");
        DBG_Enter();
        LED_OneShoot(RGB_Red, 200);
        // Dodatkowe opóźnienie umożliwia urządzeniom slave przygotowanie się na kolejne ramki komunikacyjne
        delay(500);
      }
      else {
        // Wyświetlanie odczytanych wartości rejestrów w przypadku poprawnej komunikacji dla operacji odczytu
        DBG_String("MODBUS 0b");
        DBG_Bin16(modbus_memory[MODBUS_Reg_DigitalInputs], "");
        DBG_String(" 0x");
        DBG_Hex16(modbus_memory[MODBUS_Reg_HexConfig]);
        DBG_Char(' ');
        DBG_Dec(modbus_memory[MODBUS_Reg_DecConfig]);
        DBG_Enter();
        LED_OneShoot(RGB_Green, 200);
      }
    }
    // Zwolnij rdzeń i odczekaj 500 ms przed kolejną komunikacją
    delay(500);
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
