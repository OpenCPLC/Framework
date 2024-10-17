#include "eeprom.h"

//-------------------------------------------------------------------------------------------------

EEPROM_Page_Status EEPROM_PageStatus(EEPROM_t *eeprom, uint16_t page)
{
  if(page >= FLASH_PAGES) return EEPROM_NO_PAGE;
  uint32_t i;
  uint32_t addr = eeprom->_page_address_end[page] - 8;
  if(*(uint64_t *) (addr) != (uint64_t) 0xFFFFFFFFFFFFFFFF) return EEPROM_FULL_PAGE;
  addr -= 8;
  for(i = addr; i >= eeprom->_page_address_start[page]; i -= 8) {
    if(*(uint64_t *) (i) != (uint64_t) 0xFFFFFFFFFFFFFFFF) return EEPROM_FILLEDOUT_PAGE;
  }
  return EEPROM_EMPTY_PAGE;
}

static uint32_t _EEPROM_FindPointner(EEPROM_t *eeprom)
{
  uint32_t i;
  for(i = eeprom->_page_address_end[eeprom->_page_active] - 8; i >= eeprom->_page_address_start[eeprom->_page_active]; i -= 8) {
    if(*(uint64_t *) (i) != (uint64_t) 0xFFFFFFFFFFFFFFFF) break;
  }
  return i + 8;
}

static void _EEPROM_Rewrite(EEPROM_t *eeprom, uint8_t full)
{
  uint32_t data, cell, tmp, i, j;
  uint8_t empty = (full) ? 0 : 1;
  eeprom->_page_active = empty;
  eeprom->_page_pointner = eeprom->_page_address_start[empty];
  for(i = eeprom->_page_address_end[full] - 8; i >= eeprom->_page_address_start[full]; i -= 8) {
    cell = *(uint32_t *) (i);
    data = *(uint32_t *) (i + 4);
    if(cell != 0xFFFFFFFF) {
      for(j = eeprom->_page_address_start[empty]; j < eeprom->_page_address_end[empty]; j += 8) {
        tmp = *(uint32_t *) (j);
        if(cell == tmp) break;
        else if(tmp == 0xFFFFFFFF) {
          FLASH_Write(j, cell, data);
          eeprom->_page_pointner += 8;
          break;
        }
      }
    }
  }
  if(full) FLASH_Erase(eeprom->page_b);
  else FLASH_Erase(eeprom->page_a);
}

//-------------------------------------------------------------------------------------------------

void EEPROM_Clear(EEPROM_t *eeprom)
{
  FLASH_Erase(eeprom->page_a);
  FLASH_Erase(eeprom->page_b);
  eeprom->_page_active = 0;
  eeprom->_page_pointner = eeprom->_page_address_start[0];
}

//-------------------------------------------------------------------------------------------------

uint8_t EEPROM_Init(EEPROM_t *eeprom)
{
  if(eeprom->_page_address_start[0]) return 0;
  FLASH_Init();
  eeprom->_page_address_start[0] = FLASH_START_ADDR + (FLASH_PAGE_SIZE * (eeprom->page_a));
  eeprom->_page_address_start[1] = FLASH_START_ADDR + (FLASH_PAGE_SIZE * (eeprom->page_b));
  eeprom->_page_address_end[0] = FLASH_START_ADDR + (FLASH_PAGE_SIZE * (eeprom->page_a + 1));
  eeprom->_page_address_end[1] = FLASH_START_ADDR + (FLASH_PAGE_SIZE * (eeprom->page_b + 1));
  EEPROM_Page_Status s0 = EEPROM_PageStatus(eeprom, 0);
  EEPROM_Page_Status s1 = EEPROM_PageStatus(eeprom, 1);
  if(s0 == EEPROM_NO_PAGE || s1 == EEPROM_NO_PAGE) return 1;
  else if(s0 == EEPROM_FULL_PAGE && s1 == EEPROM_FULL_PAGE) { // Error
    EEPROM_Clear(eeprom);
  }
  else if(s0 == EEPROM_EMPTY_PAGE && s1 == EEPROM_EMPTY_PAGE) { // First Use
    eeprom->_page_active = 0;
    eeprom->_page_pointner = eeprom->_page_address_start[0];
  }
  else if(s0 == EEPROM_FULL_PAGE || s1 == EEPROM_FULL_PAGE) { // Restart in progress
    if(s1 == EEPROM_FULL_PAGE) {
      FLASH_Erase(eeprom->page_a); _EEPROM_Rewrite(eeprom, 1);
    } else {
      FLASH_Erase(eeprom->page_b); _EEPROM_Rewrite(eeprom, 0);
    }
  }
  else if(s0 == EEPROM_EMPTY_PAGE || s1 == EEPROM_EMPTY_PAGE) { // Standard Work
    if(s0 == EEPROM_FILLEDOUT_PAGE) eeprom->_page_active = 0;
    else eeprom->_page_active = 1;
    eeprom->_page_pointner = _EEPROM_FindPointner(eeprom);
  }
  return 0;
}

//-------------------------------------------------------------------------------------------------

uint8_t EEPROM_WriteCell(EEPROM_t * eeprom, uint32_t cell, uint32_t * data)
{
  if(FLASH_Write(eeprom->_page_pointner, cell, *data)) return 1;
  eeprom->_page_pointner += 8;
  if(eeprom->_page_pointner >= eeprom->_page_address_end[eeprom->_page_active]) {
    _EEPROM_Rewrite(eeprom, eeprom->_page_active);
  }
  return 0;
}

uint8_t EEPROM_Write(EEPROM_t *eeprom, uint32_t *data)
{
  if(FLASH_Write(eeprom->_page_pointner, (uint32_t)data, *data)) return ERR;
  eeprom->_page_pointner += 8;
  if(eeprom->_page_pointner >= eeprom->_page_address_end[eeprom->_page_active]) {
    _EEPROM_Rewrite(eeprom, eeprom->_page_active);
  }
  return OK;
}

uint8_t EEPROM_Read(EEPROM_t *eeprom, uint32_t *data)
{
  uint32_t i, pointner, temp_data;
  for(i = eeprom->_page_address_end[eeprom->_page_active] - 8; i >= eeprom->_page_address_start[eeprom->_page_active]; i -= 8) {
    pointner = *(uint32_t *) (i);
    temp_data = *(uint32_t *) (i + 4);
    if((uint32_t)data == pointner) {
      *data = temp_data;
      return OK;
    }
  }
  return ERR;
}

//------------------------------------------------------------------------------------------------- Cache

EEPROM_t *eeprom_cache;

uint8_t EEPROM_Cache(EEPROM_t *eeprom)
{
  eeprom_cache = eeprom;
  return EEPROM_Init(eeprom_cache);
}

uint8_t EEPROM_Save(uint32_t *data)
{
  if(!eeprom_cache) return ERR;
  return EEPROM_Write(eeprom_cache, data);
}

uint8_t EEPROM_Load(uint32_t *data)
{
  if(!eeprom_cache) return ERR;
  return EEPROM_Read(eeprom_cache, data);
}

void EEPROM_SaveList(uint32_t *data, ...)
{
  va_list args; va_start(args, data);
  while(data) { EEPROM_Save(data); data = va_arg(args, uint32_t *); }
  va_end(args);
}

void EEPROM_LoadList(uint32_t *data, ...)
{
  va_list args; va_start(args, data);
  while(data) { EEPROM_Load(data); data = va_arg(args, uint32_t *); }
  va_end(args);
}

void EEPROM_Remove(void)
{
  if(!eeprom_cache) return;
  EEPROM_Clear(eeprom_cache);
}

//-------------------------------------------------------------------------------------------------
