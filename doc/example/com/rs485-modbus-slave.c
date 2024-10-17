// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"
// Import funkcji dla Modbus RTU Slave
#include "modbus-slave.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];

#define MODBUS_ADDR 0x07 // Adres urządzenia Modbus Slave
#define MODBUS_REG_COUNT 3 // Ilość rejestrów Modbus

// Stworzenie mapy rejestrów Modbus'a
typedef enum {
  MODBUS_Reg_DigitalInputs,
  MODBUS_Reg_HexConfig,
  MODBUS_Reg_DecConfig,
} MODBUS_Reg_e;

// Deklaracja pamięci dla rejestrów Modbusa Slave
uint16_t modbus_memory[MODBUS_REG_COUNT];
// Deklaracja tablic pomocniczych dla Modbusa Slave
bool modbus_write[MODBUS_REG_COUNT];
bool modbus_update[MODBUS_REG_COUNT];

void init(void)
{
  // Ustawienie wartości początkowej rejestru `DigitalInputs`
  modbus_memory[MODBUS_Reg_DigitalInputs] = 0b1100101011110000;
  // Ustawienie wartości początkowej rejestru `HexConfig`
  modbus_memory[MODBUS_Reg_HexConfig] = 0x69CF;
  // Umożliwienie nadpisania/aktualizacji wartości rejestru `HexConfig`
  modbus_write[MODBUS_Reg_HexConfig] = true;
  // Ustawienie wartości początkowej rejestru `DecConfig`
  modbus_memory[MODBUS_Reg_DecConfig] = 2137;
  // Umożliwienie nadpisania/aktualizacji wartości rejestru `DecConfig`
  modbus_write[MODBUS_Reg_DecConfig] = true;
}

// Deklaracja struktury Modbusa Slave
MODBUS_Slave_t modbus_slave = {
  .uart = &RS2, // Wybór interfejsu `RS2`
  .address = MODBUS_ADDR,
  .regmap = modbus_memory,
  .regmap_size = MODBUS_REG_COUNT,
  .write_mask = modbus_write,
  .update_flag = modbus_update
};

void loop(void)
{
  init(); // Inicjacja mapy pamięci
  while(1) {
    // Engine Modbus Slave zwracający status komunikacji
    MODBUS_Status_e status = MODBUS_Loop(&modbus_slave);
    if(MODBUS_STATUS_ERROR(status)) {
      // Mignięcie czerwoną diodą w przypadku błędu komunikacji
      LED_OneShoot(RGB_Red, 200);
    }
    else if(status == MODBUS_Status_FrameForMe) {
      // Mignięcie zieloną diodą w przypadku poprawnej komunikacji
      LED_OneShoot(RGB_Green, 200);
      // Sprawdzanie, czy jakikolwiek rejestr został nadpisany/zaktualizowany
      if(MODBUS_IsUpdate(&modbus_slave)) {
        // Poszukiwania nadpisanych/zaktualizowanych rejestrów
        for(MODBUS_Reg_e reg = 0; reg < MODBUS_REG_COUNT; reg++) {
          if(modbus_slave.update_flag[reg]) {
            switch(reg) {
              case MODBUS_Reg_HexConfig: // aktualizacja `HexConfig`
                // TODO: HexConfig Job
                DBG_String("UPDATE HexConfig:");
                DBG_Hex16(modbus_memory[reg]);
                DBG_Enter();
                break;
              case MODBUS_Reg_DecConfig: // aktualizacja `DecConfig`
                // TODO: DecConfig Job
                DBG_String("UPDATE HexConfig:");
                DBG_Dec(modbus_memory[reg]);
                DBG_Enter();
                break;
              default:
                break;
            }
            // Reset flagi aktualizacji `update_flag`
            modbus_slave.update_flag[reg] = false;
          } 
        }
      }
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
