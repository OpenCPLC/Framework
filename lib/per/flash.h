#ifndef FLASH_H_
#define FLASH_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32g0xx.h"

//-------------------------------------------------------------------------------------------------

#define FLASH_PAGE_SIZE ((uint32_t)0x00000800)
#define FLASH_START_ADDR ((uint32_t)0x08000000)
#define FLASH_PAGES ((uint16_t)64)

//-------------------------------------------------------------------------------------------------

void FLASH_Init(void);
uint8_t FLASH_Erase(uint16_t page);
uint32_t FLASH_GetAddress(uint16_t page, int16_t offset);
uint32_t FLASH_Read(uint32_t addr);
uint8_t FLASH_Write(uint32_t addr, uint32_t data1, uint32_t data2);
uint8_t FLASH_WriteFast(uint32_t addr, uint32_t *ary) __attribute__((section(".data#")));
uint8_t FLASH_WritePage(uint16_t page, uint8_t *ary);

//-------------------------------------------------------------------------------------------------

bool FLASH_Compare(uint16_t page, uint8_t *data, uint16_t size);
uint8_t FLASH_Save(uint16_t page, uint8_t *data, uint16_t size);
uint16_t FLASH_Load(uint16_t page, uint8_t *data);

//-------------------------------------------------------------------------------------------------

#endif
