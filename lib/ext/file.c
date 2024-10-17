#include "file.h"
#include "dbg.h"

//------------------------------------------------------------------------------------------------- PRINT-Basic

status_t FILE_Char(FILE_t *file, uint8_t data)
{
  if(file->mutex) return ERR;
  if(file->size >= file->limit) return ERR;
  file->buffer[file->size] = data;
  file->size++;
  return OK;
}

status_t FILE_Char16(FILE_t *file, uint16_t data)
{
  if(FILE_Char(file, (uint8_t)(data >> 8))) return ERR;
  if(FILE_Char(file, (uint8_t)data)) return ERR;
  return OK;
}

status_t FILE_Char32(FILE_t *file, uint32_t data)
{
  if(FILE_Char16(file, (uint16_t)(data >> 16))) return ERR;
  if(FILE_Char16(file, (uint16_t)(data))) return ERR;
  return OK;
}

status_t FILE_Char64(FILE_t *file, uint64_t data)
{
  if(FILE_Char32(file, (uint32_t)(data >> 32))) return ERR;
  if(FILE_Char32(file, (uint32_t)(data))) return ERR;
  return OK;
}

status_t FILE_Array(FILE_t *file, uint8_t *array, uint16_t length)
{
  if(file->mutex) return ERR;
  if(((file->size) + length) >= file->limit) return ERR;
  memcpy(&(file->buffer[file->size]), array, length);
  file->size += length;
  return OK;
}

status_t FILE_String(FILE_t *file, char *string)
{
  if(file->mutex) return ERR;
  while(*string) {
    if(file->size >= file->limit) return ERR;
    file->buffer[file->size] = *string;
    file->size++;
    string++;
  }
  return OK;
}

status_t FILE_Enter(FILE_t *file)
{
  if(FILE_Char(file, 13)) return ERR;
  return FILE_Char(file, 10);
}

//------------------------------------------------------------------------------------------------- PRINT-Number

status_t FILE_Int(FILE_t *file, int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  if(file->mutex) return ERR;
  char str[32];
  uint16_t len = itoa_base(nbr, str, base, sign, fill_zero, fill_space);
  if(((file->size)+len) >= file->limit) return ERR;
  while(len) {
    len--;
    file->buffer[file->size] = str[len];
    file->size++;
  }
  return OK;
}

status_t FILE_Float(FILE_t *file, float nbr, uint8_t accuracy, uint8_t fill_space)
{
  if(file->mutex) return ERR;
  char str[32];
  for(uint16_t i = 0; i<accuracy; i++) nbr *= 10;
  uint16_t length = itoa_base((int32_t)nbr, str, 10, true, accuracy + 1, fill_space - 1);
  if(accuracy) {
    if(((file->size)+ length + 1) >= file->limit) return ERR;
  }
  else {
    if(((file->size) + length) >= file->limit) return ERR;
  }
  while(length) {
    if(length == accuracy) {
      file->buffer[file->size] = '.';
      file->size++;
    }
    length--;
    file->buffer[file->size] = str[length];
    file->size++;
  }
  return OK;
}

status_t FILE_Dec(FILE_t *file, int32_t nbr) { return FILE_Int(file, nbr, 10, true, 0, 0); }
status_t FILE_uDec(FILE_t *file, uint32_t nbr) { return FILE_Int(file, (int32_t)nbr, 10, false, 0, 0); }
status_t FILE_Hex8(FILE_t *file, uint32_t nbr) { return FILE_Int(file, (int32_t)nbr, 16, false, 2, 2); }
status_t FILE_Hex16(FILE_t *file, uint32_t nbr) { return FILE_Int(file, (int32_t)nbr, 16, false, 4, 4); }
status_t FILE_Hex32(FILE_t *file, uint32_t nbr) { return FILE_Int(file, (int32_t)nbr, 16, false, 8, 8); }
status_t FILE_Bin8(FILE_t *file, uint8_t nbr) { return FILE_Int(file, (int32_t)nbr, 2, false, 8, 8); }

status_t FILE_Bin16(FILE_t *file, uint16_t nbr, char *sep)
{
  if(FILE_Bin8(file, (uint8_t)(nbr >> 8))) return ERR;
  if(FILE_String(file, sep)) return ERR;
  if(FILE_Bin8(file, (uint8_t)(nbr))) return ERR;
  return OK;
}

status_t FILE_Bin32(FILE_t *file, uint32_t nbr, char *sep)
{
  if(FILE_Bin16(file, (uint16_t)nbr >> 16, sep)) return ERR;
  if(FILE_String(file, sep)) return ERR;
  if(FILE_Bin16(file, (uint16_t)nbr, sep)) return ERR;
  return OK;
}

status_t FILE_Bool(FILE_t *file, bool value)
{
  if(value) return FILE_String(file, "true");
  else return FILE_String(file, "false");
}

status_t FILE_Status(FILE_t *file, bool value)
{
  if(value) return FILE_String(file, "err");
  else return FILE_String(file, "ok");
}

status_t FILE_Struct(FILE_t *file, uint8_t *object)
{
  if(file->mutex) return ERR;
  if(file->size + file->struct_size > file->limit) return ERR;
  memcpy(&file->buffer[file->size], object, file->struct_size);
  file->size += file->struct_size;
  return OK;
}

uint16_t FILE_StructCount(FILE_t *file)
{
  return file->size / file->struct_size;
}

// Returns the number of structures still fit in the file
uint16_t FILE_StructFreeSpace(FILE_t *file, uint16_t margin)
{
  if((int32_t)file->limit - margin <= 0) return 0;
  return ((file->limit - margin) / file->struct_size) - (file->size / file->struct_size);
}

status_t FILE_StructMove(FILE_t *file, uint8_t count)
{
  if(file->mutex) return ERR;
  int32_t size = (int32_t)file->size - (file->struct_size * count);
  if(size > 0) {
    memcpy(file->buffer, &file->buffer[file->struct_size * count], (size_t)size);
    file->size = size;
    return ERR;
  }
  return OK;
}

status_t FILE_StructDrop(FILE_t *file, uint8_t count)
{
  if(file->mutex) return ERR;
  if(!count) return ERR;
  int32_t size = file->struct_size * count;
  if(size > file->size) {
    file->size = 0;
    return OK;
  }
  else {
    file->size -= size;
    return ERR;
  }
}

//------------------------------------------------------------------------------------------------- RTC

status_t FILE_Date(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(FILE_String(file, "20")) return ERR;
  if(FILE_Int(file, datetime->year, 10, false, 2, 2)) return ERR;
  if(FILE_Char(file, '-')) return ERR;
  if(FILE_Int(file, datetime->month, 10, false, 2, 2)) return ERR;
  if(FILE_Char(file, '-')) return ERR;
  if(FILE_Int(file, datetime->month_day, 10, false, 2, 2)) return ERR;
  return OK;
}

status_t FILE_Time(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(FILE_Int(file, datetime->hour, 10, false, 2, 2)) return ERR;
  if(FILE_Char(file, ':')) return ERR;
  if(FILE_Int(file, datetime->minute, 10, false, 2, 2)) return ERR;
  if(FILE_Char(file, ':')) return ERR;
  if(FILE_Int(file, datetime->second, 10, false, 2, 2)) return ERR;
  return OK;
}

status_t FILE_Datetime(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(FILE_Date(file, datetime)) return ERR;
  if(FILE_Char(file, ' ')) return ERR;
  if(FILE_Time(file, datetime)) return ERR;
  return OK;
}

status_t FILE_DatetimeMs(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(FILE_Datetime(file, datetime)) return ERR;
  if(FILE_Char(file, '.')) return ERR;
  if(FILE_Int(file, datetime->ms, 10, false, 3, 3)) return ERR;
  return OK;
}

status_t FILE_AlarmTime(FILE_t *file, RTC_Alarm_t *alarm)
{
  if(alarm->hour_mask) FILE_String(file, "**:");
  else {
    if(FILE_Int(file, alarm->hour, 10, false, 2, 2)) return ERR;
    if(FILE_Char(file, ':')) return ERR;
  }
  if(alarm->minute_mask) FILE_String(file, "**:");
  else {
    if(FILE_Int(file, alarm->minute, 10, false, 2, 2)) return ERR;
    if(FILE_Char(file, ':')) return ERR;
  }
  if(alarm->second_mask) FILE_String(file, "**");
  else {
    if(FILE_Int(file, alarm->second, 10, false, 2, 2)) return ERR;
  }
  return OK;
}

status_t FILE_Alarm(FILE_t *file, RTC_Alarm_t *alarm)
{
  if(alarm->day_mask) { if(FILE_String(file, "Everyday")) return ERR; }
  else { if(FILE_String(file, (char *)rtc_weak_day_string[alarm->day])) return ERR; }
  if(FILE_Char(file, ' ')) return ERR;
  if(FILE_AlarmTime(file, alarm)) return ERR;
  return OK;
}

status_t FILE_Now(FILE_t *file)
{
  RTC_Datetime_t datetime = RTC_Datetime();
  return FILE_Datetime(file, &datetime);
}

status_t FILE_NowMs(FILE_t *file)
{
  RTC_Datetime_t datetime = RTC_Datetime();
  return FILE_DatetimeMs(file, &datetime);
}

//-------------------------------------------------------------------------------------------------

status_t FILE_Ip(FILE_t *file, uint8_t *ip)
{
  if(FILE_uDec(file, ip[0])) return ERR;
  if(FILE_Char(file, '.')) return ERR;
  if(FILE_uDec(file, ip[1])) return ERR;
  if(FILE_Char(file, '.')) return ERR;
  if(FILE_uDec(file, ip[2])) return ERR;
  if(FILE_Char(file, '.')) return ERR;
  if(FILE_uDec(file, ip[3])) return ERR;
  return OK;
}

status_t FILE_ClearLastLine(FILE_t *file)
{
  if(file->mutex) return ERR;
  bool end = false;
  uint16_t size = file->size;
  for(int32_t i = file->size - 1; i >= 0; i--) {
    if(file->buffer[i] == '\r' || file->buffer[i] == '\n') {
      if(end) return OK;
    }
    else end = true;
    file->size--;
  }
  file->size = size;
  return ERR;
}

//-------------------------------------------------------------------------------------------------

status_t FILE_Crc(FILE_t *file, const CRC_t *crc)
{
  if(file->limit - file->size < crc->width / 8) return ERR;
  file->size = CRC_Append(crc, file->buffer, file->size);
  return OK;
}

status_t FILE_CrcError(FILE_t *file, const CRC_t *crc)
{
  if(file->size < crc->width / 8) return ERR;
  if(CRC_Error(crc, file->buffer, file->size)) return ERR;
  file->size -= crc->width / 8;
  return OK;
}

//------------------------------------------------------------------------------------------------- Save/Load/Copy

status_t FILE_Clear(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->size = 0;
  return OK;
}

status_t FILE_Copy(FILE_t *file_to, FILE_t *file_from)
{
  if(file_to->mutex) return ERR;
  else if(file_to->size > file_from->limit) return ERR;
  memcpy(file_to->buffer, file_from->buffer, file_from->size);
  file_to->size = file_from->size;
  return OK;
}

status_t FILE_Save(FILE_t *file, uint8_t *data, uint16_t size)
{
  if(file->mutex) return ERR;
  else if(size > file->limit) return ERR;
  memcpy(file->buffer, data, size);
  file->size = size;
  return OK;
}

status_t FILE_Append(FILE_t *file, uint8_t *data, uint16_t size)
{
  if(file->mutex) return ERR;
  if(size + file->size > file->limit) return ERR;
  memcpy(file->buffer + file->size, data, size);
  file->size += size;
  return OK;
}

status_t FILE_GetAccess(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->mutex = true;
  return OK;
}

status_t FILE_Get2Access(FILE_t *primary, FILE_t *secondary)
{
  if(FILE_GetAccess(primary)) return ERR;
  if(secondary && secondary != primary) {
    if(FILE_GetAccess(secondary)) {
      FILE_AllowAccess(primary);
      return ERR;
    }
  }
  return OK;
}

void FILE_AllowAccess(FILE_t *file)
{
  if(file) file->mutex = false;
}

status_t FILE_FlashSave(FILE_t *file)
{
  if(!file->flash_page) return ERR;
  #if(FILE_DBG)
    DBG_String("FILE flash save ");
    DBG_String((char *)file->name);
  #endif
  if(FLASH_Compare(file->flash_page, file->buffer, file->size)) {
    #if(FILE_DBG)
      DBG_String("no-change\r\n");
    #endif
  }
  else if(FLASH_Save(file->flash_page, file->buffer, file->size)) {
    #if(FILE_DBG)
      DBG_String("error\r\n");
    #endif
    return ERR;
  }
  else {
    #if(FILE_DBG)
      DBG_String("ok\r\n");
    #endif
  }
  return OK;
}

status_t FILE_FlashLoad(FILE_t *file)
{
  if(file->mutex) return ERR;
  else if(!file->flash_page) return ERR;
  file->size = FLASH_Load(file->flash_page, file->buffer);
  #if(FILE_DBG)
    DBG_String("FILE flash load ");
    DBG_String((char*)file->name);
  #endif
  if(file->size) {
    #if(FILE_DBG)
      DBG_String(" ok\r\n");
    #endif
    return OK;
  }
  else {
    #if(FILE_DBG)
      DBG_String(" error\r\n");
    #endif
    return ERR;
  }
}

status_t FILE_OffsetDrop(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->buffer -= file->_offset;
  file->limit += file->_offset;
  file->size += file->_offset;
  file->_offset = 0;
  return OK;
}

status_t FILE_OffsetSet(FILE_t *file, uint16_t offset)
{
  if(FILE_OffsetDrop(file)) return ERR;
  if(offset > file->limit) return ERR;
  file->buffer += offset;
  file->limit -= offset;
  if(file->size > offset) file->size -= offset;
  else file->size = OK;
  file->_offset = offset;
  return OK;
}

//-------------------------------------------------------------------------------------------------

void FILE_Print(FILE_t *file)
{
  #if(FILE_DBG)
    DBG_String("FILE "); DBG_String((char *)file->name); DBG_Char(' ');
    DBG_uDec(file->size); DBG_Char('/'); DBG_uDec(file->limit);
    if(file->mutex) DBG_String(" mutex");
    if(file->flash_page) { DBG_String(" flash:"); DBG_uDec(file->flash_page); }
    DBG_Enter();
  #endif
}

void FILE_PointerPrint(FILE_t **file)
{
  FILE_Print(*file);
}

//-------------------------------------------------------------------------------------------------
