#include "opencplc.h" // Import funkcji sterownika
#include "modbus-master.h" // Import funkcji dla Modbus RTU Master
#include "modbus-slave.h" // Import funkcji dla Modbus RTU Slave

/**
 * @name  Projekt: rs485/modbus
 * @brief Komunikacja Modbus RTU Master/Slave na jednym urządzeniu.
 *        Interfejs RS1 pracuje jako Master, RS2 jako Slave.
 *        Master odczytuje trzy (wszystkie) rejestry ze Slave'a, chyba że użytkownik wciśnie przycisk.
 *        Wówczas Master nadpisuje jeden z rejestrów, na co Slave reaguje.
 * @short RS1A --> RS2A
 *        RS1B --> RS2B
 */

#define MODBUS_ADDR 0x07 // Adres urządzenia Modbus Slave
#define MODBUS_REG_COUNT 3 // Ilość rejestrów Modbus
#define MODBUS_TIMEOUT 1000 // Timeout dla wszystkich funkcji komunikacyjnych w protokole Modbus wynosi 1 sekundę.

// Stworzenie mapy rejestrów Modbus'a
typedef enum {
  MODBUS_Reg_DigitalInputs,
  MODBUS_Reg_HexConfig,
  MODBUS_Reg_DecConfig,
} MODBUS_Reg_e;

//------------------------------------------------------------------------------------------------- Slave

// Deklaracja pamięci dla rejestrów Modbusa Slave
static uint16_t slave_memory[MODBUS_REG_COUNT];
// Deklaracja tablic pomocniczych dla Modbusa Slave
static bool slave_write[MODBUS_REG_COUNT];
static bool slave_update[MODBUS_REG_COUNT];

void SlaveInit(void)
{
  // Ustawienie wartości początkowej rejestru 'DigitalInputs'
  slave_memory[MODBUS_Reg_DigitalInputs] = 0b1100101011110000;
  // Ustawienie wartości początkowej rejestru 'HexConfig'
  slave_memory[MODBUS_Reg_HexConfig] = 0x69CF;
  // Umożliwienie nadpisania/aktualizacji wartości rejestru 'HexConfig'
  slave_write[MODBUS_Reg_HexConfig] = true;
  // Ustawienie wartości początkowej rejestru 'DecConfig'
  slave_memory[MODBUS_Reg_DecConfig] = 2137;
  // Umożliwienie nadpisania/aktualizacji wartości rejestru 'DecConfig'
  slave_write[MODBUS_Reg_DecConfig] = true;
}

// Deklaracja struktury Modbusa Slave
MODBUS_Slave_t ModbusSlave = {
  .uart = &RS2, // Wybór interfejsu 'RS2'
  .address = MODBUS_ADDR,
  .regmap = slave_memory,
  .regmap_size = MODBUS_REG_COUNT,
  .write_mask = slave_write,
  .update_flag = slave_update
};

void SlaveLoop(void)
{
  SlaveInit(); // Inicjacja mapy pamięci
  while(1) {
    // Engine Modbus Slave zwracający status komunikacji
    MODBUS_Status_e status = MODBUS_Loop(&ModbusSlave);
    if(MODBUS_STATUS_ERROR(status)) {
      // Szczegóły błędu są raportowane do debuggera wewnątrz funkcji
      // TODO: Dodać sygnalizację błędu
    }
    else if(status == MODBUS_Status_FrameForMe) {
      // Komunikacja przebiega prawidłowo
      // Sprawdzanie, czy jakikolwiek rejestr został nadpisany/zaktualizowany
      LOG_Info("SLAVE data-transfer:ok");
      if(MODBUS_IsUpdate(&ModbusSlave)) {
        // Poszukiwania nadpisanych/zaktualizowanych rejestrów
        for(MODBUS_Reg_e reg = 0; reg < MODBUS_REG_COUNT; reg++) {
          if(slave_update[reg]) {
            switch(reg) {
              case MODBUS_Reg_HexConfig: // aktualizacja 'HexConfig'
                // TODO: HexConfig Job
                LOG_Warning("SLAVE update HexConfig: %04x", slave_memory[reg]);
                break;
              case MODBUS_Reg_DecConfig: // aktualizacja 'DecConfig'
                // TODO: DecConfig Job
                LOG_Warning("SLAVE update DecConfig: %d", slave_memory[reg]);
                break;
              default:
                break;
            }
            // Reset flagi aktualizacji 'update_flag'
            slave_update[reg] = false;
          }
        }
      }
    }
    let();
  }
}

//------------------------------------------------------------------------------------------------- Master

// Deklaracja pamięci podręcznej dla rejestrów odczytanych z urządzenia Slave
static uint16_t master_memory[MODBUS_REG_COUNT];
// Mapowanie interfejsu 'RS1' na protokół ModbusMaster
UART_t *ModbusMaster = &RS1;

void MasterLoop(void)
{
  while(1) {
    // Jeżeli zostanie naciśnięty przycisk, wykonaj operację wpisywania
    if(DIN_Fall(&BTN)) {
      // Wpisz do rejestru 'HexConfig' wartość '0x1234'
      if(MODBUS_PresetRegister(ModbusMaster, MODBUS_ADDR, MODBUS_Reg_HexConfig, 0x1234, MODBUS_TIMEOUT)) {
        // Obsługa błędów komunikacji lub braku odpowiedzi dla operacji wpisywania
        LOG_Error("MASTER write:no-respond");
        LED_OneShoot(RGB_Yellow, 200);
        // Dodatkowe opóźnienie umożliwia urządzeniom slave przygotowanie się na kolejne ramki komunikacyjne
        delay(500);
      }
      else {
        // Obsługa poprawnej komunikacji dla operacji wpisywania
        LOG_Info("MASTER write:ok");
        LED_OneShoot(RGB_Blue, 200);
      }
    }
    // Jeśli nie zostanie odnotowane zdarzenie wciśnięcia przycisku, wykonaj operację odczytu
    else {
      // Odczytaj 3 rejestry, rozpoczynając od rejestru 'DigitalInputs', a wartości odczytane umieść w tablicy 'master_memory'
      if(MODBUS_ReadHoldingRegisters(ModbusMaster, MODBUS_ADDR, MODBUS_Reg_DigitalInputs, 3, (uint16_t *)&master_memory, MODBUS_TIMEOUT)) {
        // Obsługa błędów komunikacji lub braku odpowiedzi dla operacji odczytu
        LOG_Error("MASTER read:no-respond");
        LED_OneShoot(RGB_Red, 200);
        // Dodatkowe opóźnienie umożliwia urządzeniom slave przygotowanie się na kolejne ramki komunikacyjne
        delay(500);
      }
      else {
        // Wyświetlanie odczytanych wartości rejestrów w przypadku poprawnej komunikacji dla operacji odczytu
        LOG_Info("MASTER read:ok 0b%16b 0x%04x %d",
          master_memory[MODBUS_Reg_DigitalInputs],
          master_memory[MODBUS_Reg_HexConfig],
          master_memory[MODBUS_Reg_DecConfig]
        );
        LED_OneShoot(RGB_Green, 200);
      }
    }
    // Zwolnij rdzeń i odczekaj 1s przed kolejną komunikacją
    delay(1000);
  }
}

//------------------------------------------------------------------------------------------------- Threads

stack(stack_plc, 256); // Stos pamięci dla wątku PLC
stack(stack_dbg, 256); // Stos pamięci dla wątku debug'era (logs + bash)
stack(stack_slave, 1024); // Stos pamięci dla funkcji loop
stack(stack_master, 1024); // Stos pamięci dla funkcji loop

int main(void)
{
  thread(PLC_Thread, stack_plc); // Dodanie wątku sterownika
  thread(DBG_Loop, stack_dbg); // Dodanie wątku debug'era (logs + bash)
  thread(SlaveLoop, stack_slave); // Dodanie funkcji loop dla Modbus RTU Slave
  thread(MasterLoop, stack_master); // Dodanie funkcji loop dla Modbus RTU Master
  vrts_init(); // Włączenie systemy przełączania wątków VRTS
  while(1); // W to miejsce program nigdy nie powinien dojść
}