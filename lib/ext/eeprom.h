#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stdarg.h>
#include "stm32g0xx.h"
#include "flash.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#define EEPROM_PAGE_A 0
#define EEPROM_PAGE_B 1

typedef enum {
  EEPROM_NO_PAGE,
  EEPROM_EMPTY_PAGE,
  EEPROM_FILLEDOUT_PAGE,
  EEPROM_FULL_PAGE,
} EEPROM_Page_Status;

typedef struct {
  uint16_t page_a;
  uint16_t page_b;
  uint32_t _page_address_start[2];
  uint32_t _page_address_end[2];
  uint16_t _page_active;
  uint32_t _page_pointner;
} EEPROM_t;

EEPROM_Page_Status EEPROM_PageStatus(EEPROM_t *eeprom, uint16_t page);
void EEPROM_Clear(EEPROM_t *eeprom);
uint8_t EEPROM_Init(EEPROM_t *eeprom);
uint8_t EEPROM_WriteCell(EEPROM_t *eeprom, uint32_t cell, uint32_t *data);
uint8_t EEPROM_Write(EEPROM_t *eeprom, uint32_t *data);
uint8_t EEPROM_Read(EEPROM_t *eeprom, uint32_t *data);

uint8_t EEPROM_Cache(EEPROM_t *eeprom);
uint8_t EEPROM_Save(uint32_t *data);
uint8_t EEPROM_Load(uint32_t *data);
void EEPROM_SaveList(uint32_t *data, ...);
void EEPROM_LoadList(uint32_t *data, ...);
void EEPROM_Remove(void);

//-------------------------------------------------------------------------------------------------
#endif
