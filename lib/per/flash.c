#include "flash.h"

//-------------------------------------------------------------------------------------------------

void FLASH_Init(void)
{
  while(FLASH->SR & FLASH_SR_BSY1) __DSB();
  if((FLASH->CR & FLASH_CR_LOCK) != 0) {
    FLASH->KEYR = ((uint32_t)0x45670123);
    FLASH->KEYR = ((uint32_t)0xCDEF89AB);
  }
}

uint8_t FLASH_Erase(uint16_t page)
{
  while(FLASH->SR & FLASH_SR_BSY1) __DSB();
  FLASH->CR &= ~FLASH_CR_PNB;
  #ifdef STM32G081xx
    FLASH->CR |= (page << 3) | FLASH_CR_PER;
  #endif
  #ifdef STM32G0C1xx
    FLASH->CR |= ((page > 127 ? 1 : 0) << 13) | (page << 3) | FLASH_CR_PER;
  #endif
  FLASH->CR |= FLASH_CR_STRT;
  while(FLASH->SR & FLASH_SR_BSY1) __DSB();
  FLASH->CR &= ~FLASH_CR_PER;
  __DSB();
  if(FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP;
  else return FLASH->SR;
  return 0;
}

//-------------------------------------------------------------------------------------------------

uint32_t FLASH_GetAddress(uint16_t page, int16_t offset)
{
	return FLASH_START_ADDR + (FLASH_PAGE_SIZE * page) + offset;
}

uint32_t FLASH_Read(uint32_t addr)
{
  return *(uint32_t *)addr;
}

//-------------------------------------------------------------------------------------------------

uint8_t FLASH_Write(uint32_t addr, uint32_t data1, uint32_t data2)
{
  while(FLASH->SR & FLASH_SR_BSY1) __DSB();
  FLASH->SR = 0;
  FLASH->CR |= FLASH_CR_PG;
  *(uint32_t *)addr = data1;
  __ISB();
  *(uint32_t *)(addr + 4U) = data2;
  while(FLASH->SR & FLASH_SR_BSY1);
  FLASH->CR &= ~FLASH_CR_PG;
  if(FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP;
  else return FLASH->SR;
  return 0;
}

uint8_t FLASH_WriteFast(uint32_t addr, uint32_t *ary)
{
  while(FLASH->SR & FLASH_SR_BSY1);
  FLASH->CR |= FLASH_CR_FSTPG;
  __disable_irq();
  for(int i=0; i<64; i++) {
	  *(uint32_t *)addr = ary[i];
	  addr += 4U;
  }
  while(FLASH->SR & FLASH_SR_BSY1);
  FLASH->CR &= ~FLASH_CR_FSTPG;
  __enable_irq();
  if(FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP;
  else return FLASH->SR;
  return 0;
}

uint8_t FLASH_WritePage(uint16_t page, uint8_t *ary)
{
	if(FLASH_Erase(page)) return 1;
	uint32_t addr = FLASH_GetAddress(page, 0);
	for(uint32_t i = 0; i < FLASH_PAGE_SIZE; i += 256){
		if(FLASH_WriteFast(addr, (uint32_t *)ary)) return 1;
		addr += 256;
		ary += 256;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

bool FLASH_Compare(uint16_t page, uint8_t *data, uint16_t size)
{
  uint32_t addr = FLASH_GetAddress(page, 0);
  uint16_t size_flash = FLASH_Read(addr);
  if(size_flash != size) return false;
  addr += 4;
  if(memcmp(data, (uint8_t *)addr, size)) return false;
  return true;
}

uint8_t FLASH_Save(uint16_t page, uint8_t *data, uint16_t size)
{
  if(FLASH_Erase(page)) return 1;
  if(page >= FLASH_PAGES) return 1;
  uint32_t addr = FLASH_GetAddress(page, 0);
  uint32_t end = FLASH_GetAddress(page + 1, 0);
  int32_t length = (int32_t)size;
  if(addr + size >= FLASH_GetAddress(FLASH_PAGES, 0)) return 1;
  if(FLASH_Write(addr, (uint32_t)size, *(uint32_t *)data)) return 1;
  length -= 4;
  data += 4;
  if(length <= 0) return 0;
  addr += 8;
  while(length > 0){
    if(addr >= end) {
      addr = end;
      page++;
      if(FLASH_Erase(page)) return 1;
      end = FLASH_GetAddress(page + 1, 0);
    }
    if(FLASH_Write(addr, *(uint32_t *)data, *((uint32_t *)data + 1))) return 1;
    length -= 8;
    addr += 8;
    data += 8;
  }
  return 0;
}

uint16_t FLASH_Load(uint16_t page, uint8_t *data)
{
  uint32_t addr = FLASH_GetAddress(page, 0);
  uint16_t size = FLASH_Read(addr);
  if(addr + size >= FLASH_GetAddress(FLASH_PAGES, 0)) return 0;
  if(size == 0xFFFF) return 0;
  addr += 4;
  memcpy(data, (uint8_t *)addr, size);
  return size;
}

//-------------------------------------------------------------------------------------------------
