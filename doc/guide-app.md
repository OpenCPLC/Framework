

Test HD44780

```c
HD44780_t lcd = {
  .address = 0x27,
  .columns = 16,
  .rows = 4
};

void loop(void)
{
  HD44780_Init(&lcd);
  HD44780_ExtraChars(&lcd);
  HD44780_Str(&lcd, "Test", 0, 0);
  HD44780_Write(&lcd, HD44780_Char_ArrowRight);
  HD44780_Write(&lcd, HD44780_Char_ArrowLeft);
  HD44780_Write(&lcd, HD44780_Char_ArrowUp);
  HD44780_Write(&lcd, HD44780_Char_ArrowDown);
  HD44780_Write(&lcd, HD44780_Char_BoxSet);
  HD44780_Write(&lcd, HD44780_Char_BoxRst);
  HD44780_Write(&lcd, HD44780_Char_Degree);
  HD44780_Write(&lcd, 'C');
  while(1) {
    let();
  }
}
```



1WIRE DS18x20

```c
// Import podstawowych funkcji sterownika.
#include "opencplc-uno.h"
// Import biblioteki wyświetlacza alfanumerycznego HD44780
#include "hd44780.h"

// Stos pamięci dla wątku PLC
static uint32_t stack_plc[256];
// Stos pamięci dla funkcji loop
static uint32_t stack_loop[256];



uint8_t data = 12;

void prug(void)
{
  while(1) {
    __NOP();
  }
}

#define DS18X_START_CONVERSION 0x44
#define DS18X_READ_SCRATCHPAD 0xBE
#define DS18X_DATA_LENGNT 9

  // int16_t raw = (data[1] << 8) | data[0];
  // if (type_s) {
  //   raw = raw << 3; // 9 bit resolution default
  //   if (data[7] == 0x10) {
  //     // "count remain" gives full 12 bit resolution
  //     raw = (raw & 0xFFF0) + 12 - data[6];
  //   }
  // } else {
  //   byte cfg = (data[4] & 0x60);
  //   // at lower res, the low bits are undefined, so let's zero them
  //   if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  //   else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  //   else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //   //// default is 12 bit resolution, 750 ms conversion time
  // }
  // celsius = (float)raw / 16.0;
  // fahrenheit = celsius * 1.8 + 32.0;
  // Serial.print("  Temperature = ");
  // Serial.print(celsius);
  // Serial.print(" Celsius, ");


void loop(void)
{
  uint8_t addr[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t data[DS18X_DATA_LENGNT];

  if(!ONE_WIRE_Init()) prug();


  ONE_WIRE_Search(addr);
  delay(250);
  // Check CRC addr[7] -- CRC8
  
  while(1) {
    ONE_WIRE_Reset();
    ONE_WIRE_Select(addr);
    ONE_WIRE_WriteParasitePower(DS18X_START_CONVERSION);
    delay(1000);

    ONE_WIRE_Reset();
    ONE_WIRE_Select(addr);  
    ONE_WIRE_Write(DS18X_READ_SCRATCHPAD);

    for(uint8_t i = 0; i < DS18X_DATA_LENGNT; i++) {
      data[i] = ONE_WIRE_Read();
      DBG_Hex8(data[i]);
      DBG_Char(' ');
    }
    DBG_Enter();
    delay(250);
    

    // Check CRC data[DS18X_DATA_LENGNT - 1] -- CRC8
  }

}

// 0x28

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
```