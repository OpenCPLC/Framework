#include "file.h"
#include "dbg.h"

//------------------------------------------------------------------------------------------------- Strings

int32_t FILE_Char(FILE_t *file, uint8_t data)
{
  if(file->mutex) return 0;
  if(file->size + 1 >= file->limit) return 0;
  file->buffer[file->size] = data;
  file->size++;
  return 1;
}

int32_t FILE_Char16(FILE_t *file, uint16_t data)
{
  if(file->mutex) return 0;
  if(file->size + 2 >= file->limit) return 0;
  FILE_Char(file, (uint8_t)(data >> 8));
  FILE_Char(file, (uint8_t)data);
  return 2;
}

int32_t FILE_Char32(FILE_t *file, uint32_t data)
{
  if(file->mutex) return 0;
  if(file->size + 4 >= file->limit) return 0;
  FILE_Char16(file, (uint16_t)(data >> 16));
  FILE_Char16(file, (uint16_t)(data));
  return 4;
}

int32_t FILE_Char64(FILE_t *file, uint64_t data)
{
  if(file->mutex) return 0;
  if(file->size + 8 >= file->limit) return 0;
  FILE_Char32(file, (uint32_t)(data >> 32));
  FILE_Char32(file, (uint32_t)(data));
  return 8;
}

int32_t FILE_Data(FILE_t *file, uint8_t *array, uint16_t length)
{
  if(file->mutex) return 0;
  if((file->size + length) >= file->limit) return 0;
  memcpy(&(file->buffer[file->size]), array, length);
  file->size += length;
  return length;
}

int32_t FILE_String(FILE_t *file, char *string)
{
  if(file->mutex) return 0;
  if(!string) return 0;
  uint16_t length = 0;
  while(*string) {
    if(file->size >= file->limit) {
      file->size -= length;
      return 0;
    }
    file->buffer[file->size] = *string;
    file->size++;
    string++;
    length++;
  }
  return length;
}

int32_t FILE_Enter(FILE_t *file)
{
  if(file->mutex) return 0;
  #if(FILE_ENTER_RETURN)
    if(file->size + 2 >= file->limit) return 0;
    FILE_Char(file, '\r');
    FILE_Char(file, '\n');
    return 2;
  #else
    return FILE_Char(file, '\n');
  #endif
}

int32_t FILE_ClearLastLine(FILE_t *file)
{
  if(file->mutex) return 0;
  int32_t length = 0;
  bool end = false;
  uint16_t size = file->size;
  for(int32_t i = file->size - 1; i >= 0; i--) {
    if(file->buffer[i] == '\r' || file->buffer[i] == '\n') {
      if(end) return length;
    }
    else end = true;
    file->size--;
    length++;
  }
  file->size = size;
  return length;
}

int32_t FILE_Bool(FILE_t *file, bool value)
{
  if(value) return FILE_String(file, "true");
  else return FILE_String(file, "false");
}

//------------------------------------------------------------------------------------------------- Numbers

int32_t FILE_Int(FILE_t *file, int64_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  if(file->mutex) return 0;
  int32_t length = (int32_t)itoa_encode(nbr, StrTempMem, base, sign, fill_zero, fill_space);
  if(((file->size)+length) >= file->limit) return ERR;
  while(length) {
    length--;
    file->buffer[file->size] = StrTempMem[length];
    file->size++;
  }
  return length;
}

int32_t FILE_Float(FILE_t *file, float nbr, uint8_t accuracy, uint8_t fill_space)
{
  if(file->mutex) return 0;
  if(isNaN(nbr)) {
    // uint8_t len = fill_space > 3 ? fill_space : 3;
    // char nan[len + 1];
    // memset(nan, ' ', len - 3);
    // nan[len - 3] = 'N';
    // nan[len - 2] = 'a';
    // nan[len - 1] = 'N';
    uint8_t len = fill_space > 1 ? fill_space : 1; 
    char nan[len + 1];
    memset(nan, ' ', len - 1);
    nan[len - 1] = '-'; 
    nan[len] = '\0';
    return FILE_String(file, nan);
  }
  for(uint16_t i = 0; i<accuracy; i++) nbr *= 10;
  if(!fill_space) fill_space = 1;
  int32_t length = (int32_t)itoa_encode((int32_t)nbr, StrTempMem, 10, true, nbr < 0 ? accuracy + 2 : accuracy + 1, fill_space - 1);
  if(accuracy) {
    if(((file->size)+ length + 1) >= file->limit) return 0;
  }
  else {
    if(((file->size) + length) >= file->limit) return 0;
  }
  while(length) {
    if(length == accuracy) {
      file->buffer[file->size] = '.';
      file->size++;
    }
    length--;
    file->buffer[file->size] = StrTempMem[length];
    file->size++;
  }
  return length;
}

int32_t FILE_Dec(FILE_t *file, int64_t nbr) { return FILE_Int(file, nbr, 10, true, 0, 0); }
int32_t FILE_uDec(FILE_t *file, uint64_t nbr) { return FILE_Int(file, (int64_t)nbr, 10, false, 0, 0); }
int32_t FILE_Hex8(FILE_t *file, uint8_t nbr) { return FILE_Int(file, (int64_t)nbr, 16, false, 2, 2); }
int32_t FILE_Hex16(FILE_t *file, uint16_t nbr) { return FILE_Int(file, (int64_t)nbr, 16, false, 4, 4); }
int32_t FILE_Hex32(FILE_t *file, uint32_t nbr) { return FILE_Int(file, (int64_t)nbr, 16, false, 8, 8); }
int32_t FILE_Bin8(FILE_t *file, uint8_t nbr) { return FILE_Int(file, (int64_t)nbr, 2, false, 8, 8); }

//------------------------------------------------------------------------------------------------- RTC

int32_t FILE_Date(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(file->mutex) return 0;
  if(file->size + 10 >= file->limit) return 0;
  FILE_String(file, "20");
  FILE_Int(file, datetime->year, 10, false, 2, 2);
  FILE_Char(file, '-');
  FILE_Int(file, datetime->month, 10, false, 2, 2);
  FILE_Char(file, '-');
  FILE_Int(file, datetime->month_day, 10, false, 2, 2);
  return 10;
}

int32_t FILE_Time(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(file->mutex) return 0;
  if(file->size + 8 >= file->limit) return 0;
  FILE_Int(file, datetime->hour, 10, false, 2, 2);
  FILE_Char(file, ':');
  FILE_Int(file, datetime->minute, 10, false, 2, 2);
  FILE_Char(file, ':');
  FILE_Int(file, datetime->second, 10, false, 2, 2);
  return 8;
}

int32_t FILE_TimeMs(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(file->mutex) return 0;
  if(file->size + 12 >= file->limit) return 0;
  FILE_Time(file, datetime);
  FILE_Char(file, '.');
  FILE_Int(file, datetime->ms, 10, false, 3, 3);
  return 12;
}

int32_t FILE_Datetime(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(file->mutex) return 0;
  if(file->size + 19 >= file->limit) return 0;
  FILE_Date(file, datetime);
  FILE_Char(file, ' ');
  FILE_Time(file, datetime);
  return 19;
}

int32_t FILE_DatetimeMs(FILE_t *file, RTC_Datetime_t *datetime)
{
  if(file->mutex) return 0;
  if(file->size + 23 >= file->limit) return 0;
  FILE_Datetime(file, datetime);
  FILE_Char(file, '.');
  FILE_Int(file, datetime->ms, 10, false, 3, 3);
  return 23;
}

int32_t FILE_AlarmTime(FILE_t *file, RTC_Alarm_t *alarm)
{
  if(file->mutex) return 0;
  if(file->size + 8 >= file->limit) return 0;
  if(alarm->hour_mask) FILE_String(file, "**:");
  else {
    FILE_Int(file, alarm->hour, 10, false, 2, 2);
    FILE_Char(file, ':');
  }
  if(alarm->minute_mask) FILE_String(file, "**:");
  else {
    FILE_Int(file, alarm->minute, 10, false, 2, 2);
    FILE_Char(file, ':');
  }
  if(alarm->second_mask) FILE_String(file, "**");
  else {
    FILE_Int(file, alarm->second, 10, false, 2, 2);
  }
  return 8;
}

int32_t FILE_Alarm(FILE_t *file, RTC_Alarm_t *alarm)
{
  if(file->mutex) return 0;
  int32_t length;
  length = alarm->day_mask ?
    FILE_String(file, (char *)rtc_weakdays[0]) :
    FILE_String(file, (char *)rtc_weakdays[alarm->day]);
  if(!length) return 0;
  if(file->size + 9 >= file->limit) {
    file->size -= length;
    return 0;
  }
  length += 9;
  FILE_Char(file, ' ');
  FILE_AlarmTime(file, alarm);
  return length;
}

//------------------------------------------------------------------------------------------------- General

state_t FILE_Clear(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->size = 0;
  return OK;
}

state_t FILE_Copy(FILE_t *file_to, FILE_t *file_from)
{
  if(file_to->mutex) return ERR;
  else if(file_to->size > file_from->limit) return ERR;
  memcpy(file_to->buffer, file_from->buffer, file_from->size);
  file_to->size = file_from->size;
  return OK;
}

state_t FILE_Save(FILE_t *file, uint8_t *data, uint16_t size)
{
  if(file->mutex) return ERR;
  else if(size > file->limit) return ERR;
  memcpy(file->buffer, data, size);
  file->size = size;
  return OK;
}

state_t FILE_Append(FILE_t *file, uint8_t *data, uint16_t size)
{
  if(file->mutex) return ERR;
  if(size + file->size > file->limit) return ERR;
  memcpy(file->buffer + file->size, data, size);
  file->size += size;
  return OK;
}

//------------------------------------------------------------------------------------------------- Access

state_t FILE_Access_Get(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->mutex = true;
  return OK;
}

void FILE_Access_Allow(FILE_t *file)
{
  if(file) file->mutex = false;
}

state_t FILE_Access_Get2(FILE_t *primary, FILE_t *secondary)
{
  if(FILE_Access_Get(primary)) return ERR;
  if(secondary && secondary != primary) {
    if(FILE_Access_Get(secondary)) {
      FILE_Access_Allow(primary);
      return ERR;
    }
  }
  return OK;
}

//------------------------------------------------------------------------------------------------- Flash

state_t FILE_Flash_Save(FILE_t *file)
{
  if(!file->flash_page) return ERR;
  if(FLASH_Compare(file->flash_page, file->buffer, file->size)) {
    return OK; // no-change
  }
  if(FLASH_Save(file->flash_page, file->buffer, file->size)) {
    return ERR;
  }
  return OK;
}

state_t FILE_Flash_Load(FILE_t *file)
{
  if(file->mutex) return ERR;
  else if(!file->flash_page) return ERR;
  file->size = FLASH_Load(file->flash_page, file->buffer);
  if(file->size) return OK;
  else return ERR;
}

//------------------------------------------------------------------------------------------------- Struct

int32_t FILE_Struct_Add(FILE_t *file, uint8_t *object)
{
  if(file->mutex) return 0;
  if(file->size + file->struct_size >= file->limit) return 0;
  memcpy(&file->buffer[file->size], object, file->struct_size);
  file->size += file->struct_size;
  return file->struct_size;
}

uint16_t FILE_Struct_GetCount(FILE_t *file)
{
  return file->size / file->struct_size;
}

// Returns the number of structures still fit in the file
uint16_t FILE_Struct_GetFreeSpace(FILE_t *file, uint16_t margin)
{
  if((int32_t)file->limit - margin <= 0) return 0;
  return ((file->limit - margin) / file->struct_size) - (file->size / file->struct_size);
}

int32_t FILE_Struct_Move(FILE_t *file, uint16_t count)
{
  if(file->mutex) return 0;
  int32_t size = (int32_t)file->size - (file->struct_size * count);
  if(size > 0) {
    memcpy(file->buffer, &file->buffer[file->struct_size * count], (size_t)size);
    file->size = size;
    return -(int32_t)file->struct_size;
  }
  return 0;
}

int32_t FILE_Struct_Drop(FILE_t *file, uint16_t count)
{
  if(file->mutex) return 0;
  if(!count) return 0;
  int32_t size = file->struct_size * count;
  if(size > file->size) {
    size = file->size;
    file->size = 0;
  }
  else {
    file->size -= size;
  }
  return -size;
}

//------------------------------------------------------------------------------------------------- Offset

state_t FILE_Offset_Drop(FILE_t *file)
{
  if(file->mutex) return ERR;
  file->buffer -= file->offset;
  file->limit += file->offset;
  file->size += file->offset;
  file->offset = 0;
  return OK;
}

state_t FILE_Offset_Set(FILE_t *file, uint16_t offset)
{
  if(FILE_Offset_Drop(file)) return ERR;
  if(offset > file->limit) return ERR;
  file->buffer += offset;
  file->limit -= offset;
  if(file->size > offset) file->size -= offset;
  else file->size = 0;
  file->offset = offset;
  return OK;
}

//------------------------------------------------------------------------------------------------- Crc

int32_t FILE_CrcAppend(FILE_t *file, const CRC_t *crc)
{
  if(file->limit - file->size < crc->width / 8) return 0;
  file->size = CRC_Append(crc, file->buffer, file->size);
  return crc->width / 8;
}

bool FILE_CrcError(FILE_t *file, const CRC_t *crc)
{
  if(file->size < crc->width / 8) return true;
  if(CRC_Error(crc, file->buffer, file->size)) return true;
  file->size -= crc->width / 8;
  return false;
}

//-------------------------------------------------------------------------------------------------
