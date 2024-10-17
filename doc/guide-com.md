# 🔗 Communication [➥](../readme.md)

## Komunikacja `RS485`

W sterowniku **Uno** dostępne są dwa interfejsy **RS485**: `RS1` oraz `RS2`. Wsparcie obejmuje protokoły **Modbus RTU** oraz **BACnet** w trybach master i slave.

### 🧔🏻 Modbus Master

W przykładzie ustanawiamy komunikację z urządzeniem slave o adresie `0x07` przy użyciu protokołu **Modbus RTU**.

```c
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
```

Podczas normalnej pracy funkcją `0x03` odczytywane są pierwsze **3** rejestry z urządzenia i wyświetlane poprzez interfejs `DBG`. Gdy zostanie naciśnięty przycisk, zamiast operacji odczytu zostanie wykonana operacja nadpisania rejestru funkcją 0x06. Przerwy między komunikacjami wynoszą **500ms**, chyba że wystąpi błąd, wtedy są zwiększane do **1s**.

```c
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
```

🚀 Kompletny przykład: [Komunikacja RS485 Modbuse Master](./example/com/rs485-modbus-master.c)

### 👨🏿‍🦲 Modbus Slave

W tej implementacji pamięć jest wspólna dla wszystkich funkcji Modbus. Zatem funkcje _"Read Input Registers"_ `0x04` oraz _"Read Holding Registers"_ `0x03` odwołują się do tego samego bloku pamięci. Podobnie funkcje _"Read Outputs"_ `0x02` oraz _"Read Bits"_ `0x01`. Funkcje bitowe odwołują się do tego samego magazynu pamięci, z tą różnicą, że pamięć indeksowana jest bitowe/binarne, a nie `uint16_t`.

W przykładowej konfiguracji **Modbus RTU** w trybie **Slave** należy stworzyć strukturę `MODBUS_Slave_t` oraz ustawić w niej:

- `uart` - wskaźnik na wykorzystywany interfejs **UART**,
- `address` - adres urządzenia Modbus,
- `regmap` - tablicę stanowiącą pamięć Modbus z domyślnymi/początkowymi wartościami,
- `regmap_size` - wielkość tablicy/pamięci `regmap`,
- `write_mask` - tablicę z wartościami `true` dla rejestrów, które mogą zostać nadpisane _(`RW`)_,
- `update_flag` - deklaracja pustej tablicy o długości `regmap_size`, w której będą automatycznie wskazywane rejestry, które zostały zaktualizowane/nadpisane.

Tablice regmap, write_mask i update_flag muszą mieć taką samą długość wynoszącą regmap_size i są powiązane ze sobą indeksem tablicy. Zatem o tym, czy wartość `regmap[index]` będzie można nadpisać, decyduje maska `write_mask[index]`. Gdy wartość zostanie nadpisana, wartość `update_flag[index]` zostanie ustawiona na true. Dodatkowo warto stworzyć sobie zmienną wyliczeniową `enum` z nazwami rejestrów powiązanymi z ich numerami.

W przykładzie urządzeniu slave został nadany adres `0x07`. Urządzenie udostępnia **3** rejestry: `DigitalInputs`, `HexConfig` i `DecConfig`. Rejestry `HexConfig` oraz `DecConfig` mogą zostać nadpisane.

```c
// Import funkcji dla Modbus RTU Slave
#include "modbus-slave.h"

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
```

Dla komunikacji, która oczekuje na przychodzące ramki, jaką jest **Modbus Slave**, należy przeznaczyć osobny wątek. W tym wątku należy umieścić funkcję `MODBUS_Loop`, która obsługuje całość komunikacji na przekazanej strukturze `MODBUS_Slave_t`.

Jeśli w wyniku komunikacji jakakolwiek wartość została nadpisana, funkcja `MODBUS_IsUpdate` zwróci `true`. Wówczas należy przejrzeć tablicę `update_flag`. Jeśli natrafimy na komórkę o wartości true, oznacza to, że rejestr odpowiadający indeksowi z tablicy `update_flag` został nadpisany i możemy obsłużyć taką operację. W przypadku wartości konfiguracyjnej może być konieczna ponowna inicjalizacji lub zmiany stanu powiązanych peryferiów, a czasem będziemy chcieli tę wartość zapisać w pamięci nieulotnej. Po obsłużeniu aktualizacji zminnej `update_flag` należy ponownie ustawić ją na `false`.

```c
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
```

🚀 Kompletny przykład: [Komunikacja RS485 Modbuse Slave](./example/com/rs485-modbus-slave.c)

## 🔀 Komunikacja `I2C`

```c
#include "opencplc-uno.h"

static uint32_t stack_plc[256];

static uint32_t stack_loop[256];

#define ADDR 0x18 // LIS3DH device address

void loop(void)
{
  uint8_t buffer[6];
  buffer[0] = 0x77;
  I2C_Write(ADDR, 0x20, buffer, 1);
  while(1) {
    if(I2C_Read(ADDR, 0x28 | 0x80, buffer, 6)) {
      int16_t x = ((int16_t)buffer[1] << 8) | buffer[0];
      int16_t y = ((int16_t)buffer[3] << 8) | buffer[2];
      int16_t z = ((int16_t)buffer[5] << 8) | buffer[4];
      DBG_String("LIS3DH x:"); DBG_Dec(x);
      DBG_String(" y:"); DBG_Dec(y);
      DBG_String(" z:"); DBG_Dec(z);
      DBG_Enter();
      LED_Set(RGB_Green);
    }
    else {
      LED_Set(RGB_Red);
      delay(100);
    }
    delay(100);
  }
}

int main(void)
{
  PLC_Init();
  thread(&PLC_Thread, stack_plc, sizeof(stack_plc));
  thread(&loop, stack_loop, sizeof(stack_loop));
  VRTS_Init();
  while(1);
}
```

### Pomiar temperatury `RTD`

W przykładzie przeprowadzany jest pomiar temperatury przy użyciu **4**-przewodowego czujnika rezystancyjnego **PT1000**. W funkcji `RTD_Loop` pomiar jest wykonywany `10` razy, a następnie uzyskane wartości są uśredniane i przeliczane na temperaturę. Odczyt temperatury odbywa się z poziomu głównej pętli `loop`.

```c
#include "opencplc-uno.h"

int loop(void);

static uint32_t stack_plc[64];
static uint32_t stack_rtd[64];
static uint32_t stack_loop[512];

int main(void)
{
  PLC_Init();
  RTD.type = MAX31865_Type_PT1000;
  RTD.wire = MAX31865_Wire_4;
  RTD.oversampling = 10;
  thread(&PLC_Loop, stack_plc, sizeof(stack_plc));
  thread(&RTD_Main, stack_rtd, sizeof(stack_rtd));
  thread(&loop, stack_loop, sizeof(stack_loop));
  VRTS_Init();
  while(1);
}

int loop(void)
{
  while(1) {
    float temperature = RTD_Temperature();
    DBG_String("TEMP ");
    DBG_Float(temperature, 3);
    DBG_Char("C");
    DBG_Enter();
    delay(1000);
  }
}
```