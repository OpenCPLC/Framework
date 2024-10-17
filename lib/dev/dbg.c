#include "dbg.h"

//-------------------------------------------------------------------------------------------------

struct {
  FILE_t *file;
  UART_t *uart;
} dbg_state;

void DBG_Init(UART_t *uart, FILE_t *file)
{
  dbg_state.uart = uart;
  dbg_state.file = file;
  UART_Init(uart);
}

//-------------------------------------------------------------------------------------------------

uint16_t DBG_ReadArray(uint8_t *array)
{
  return UART_ReadArray(dbg_state.uart, array);
}

uint16_t DBG_ReadSize(void)
{
  return UART_ReadSize(dbg_state.uart);
}

char *DBG_ReadString(void)
{
  return UART_ReadString(dbg_state.uart);
}

//-------------------------------------------------------------------------------------------------

uint8_t DBG_Array(uint8_t *ary, uint16_t len)
{
  return FILE_Array(dbg_state.file, (uint8_t *)ary, len);
}

uint8_t DBG_Char(char data)
{
  return FILE_Char(dbg_state.file, (uint8_t)data);
}

uint8_t DBG_Char16(uint16_t data)
{
  return FILE_Char16(dbg_state.file, data);
}

uint8_t DBG_Char32(uint32_t data)
{
  return FILE_Char32(dbg_state.file, data);
}

uint8_t DBG_Char64(uint64_t data)
{
  return FILE_Char64(dbg_state.file, data);
}

uint8_t DBG_String(char *str)
{
  return FILE_String(dbg_state.file, str);
}

uint8_t DBG_Enter(void)
{
  return FILE_Enter(dbg_state.file);
}

//-------------------------------------------------------------------------------------------------

uint8_t DBG_Int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  return FILE_Int(dbg_state.file, nbr, base, sign, fill_zero, fill_space);
}

uint8_t DBG_Float(float nbr, uint8_t accuracy)
{
  return FILE_Float(dbg_state.file, nbr, accuracy, 1);
}

uint8_t DBG_FloatInt(float nbr, uint8_t accuracy)
{
  uint32_t factor = 1;
  for(uint8_t i = 0; i < accuracy; i++) factor *= 10;
  return FILE_Float(dbg_state.file, nbr / factor, accuracy, 1);
}

uint8_t DBG_Dec(int32_t nbr)
{
  return FILE_Dec(dbg_state.file, nbr);
}

uint8_t DBG_uDec(uint32_t nbr)
{
  return FILE_uDec(dbg_state.file, nbr);
}

uint8_t DBG_Hex8(uint32_t nbr)
{
  return FILE_Hex8(dbg_state.file, nbr);
}

uint8_t DBG_Hex16(uint32_t nbr)
{
  return FILE_Hex16(dbg_state.file, nbr);
}

uint8_t DBG_Hex32(uint32_t nbr)
{
  return FILE_Hex32(dbg_state.file, nbr);
}

uint8_t DBG_Bin8(uint8_t nbr)
{
  return FILE_Bin8(dbg_state.file, nbr);
}

uint8_t DBG_Bin16(uint16_t nbr, char *sep)
{
  return FILE_Bin16(dbg_state.file, nbr, sep);
}

uint8_t DBG_Bin32(uint32_t nbr, char *sep)
{
  return FILE_Bin32(dbg_state.file, nbr, sep);
}

uint8_t DBG_Bool(bool value)
{
  return FILE_Bool(dbg_state.file, value);
}

uint8_t DBG_Status(bool value)
{
  return FILE_Status(dbg_state.file, value);
}

//------------------------------------------------------------------------------------------------- RTC

uint8_t DBG_Date(RTC_Datetime_t *datetime)
{
  return FILE_Date(dbg_state.file, datetime);
}

uint8_t DBG_Time(RTC_Datetime_t *datetime)
{
  return FILE_Time(dbg_state.file, datetime);
}

uint8_t DBG_Datetime(RTC_Datetime_t *datetime)
{
  return FILE_Datetime(dbg_state.file, datetime);
}

uint8_t DBG_AlarmTime(RTC_Alarm_t *alarm)
{
  return FILE_AlarmTime(dbg_state.file, alarm);
}

uint8_t DBG_Alarm(RTC_Alarm_t *alarm)
{
  return FILE_Alarm(dbg_state.file, alarm);
}

uint8_t DBG_DatetimeMs(RTC_Datetime_t *datetime)
{
  return FILE_DatetimeMs(dbg_state.file, datetime);
}

uint8_t DBG_Now(void)
{
  return FILE_Now(dbg_state.file);
}

uint8_t DBG_NowMs(void)
{
  return FILE_NowMs(dbg_state.file);
}

//-------------------------------------------------------------------------------------------------

uint8_t DBG_Ip(uint8_t *ip)
{
  return FILE_Ip(dbg_state.file, ip);
}

uint8_t DBG_ClearLastLine(void)
{
  return FILE_ClearLastLine(dbg_state.file);
}

uint8_t DBG_Clear(void)
{
  return FILE_Clear(dbg_state.file);
}

static inline uint16_t _DBG_PrintArrayHeader(char *name, uint16_t count, uint16_t *limit, uint16_t *offset)
{
  if(!offset) {
    uint16_t offset_void = 0;
    offset = &offset_void;
  }
  if(!limit) limit = &count;
  if(!count) { *limit = 0; return 0; }
  if(!*limit) *limit = count;
  if(*limit > count) *limit = count;
  if(*offset >= count) *offset = 0;
  uint16_t end = *offset + *limit;
  if(end > count) end -= count;
  else end--;
  DBG_String((char *)name); DBG_Char(' ');
  DBG_uDec(count); DBG_Char(' ');
  DBG_uDec(*offset); DBG_String("..");
  DBG_uDec(end); DBG_Enter();
  return end;
}

void DBG_PrintArray(FILE_t *file, uint16_t limit, uint16_t offset, void (*Print)(void *))
{
  uint16_t count = FILE_StructCount(file);
  uint8_t *p = file->buffer + (file->struct_size * offset);
  char *name = strtoupper((char *)file->name);
  _DBG_PrintArrayHeader(name, count, &limit, &offset);
  while(limit) {
    DBG_String("  ");
    Print(p);
    DBG_Enter();
    p += file->struct_size;
    offset++;
    limit--;
    if(offset >= count) {
      p = file->buffer;
      offset = 0;
    }
  }
}

void DBG_PrintArrayBash(FILE_t *file, char **argv, uint16_t argc, uint8_t limit_index, uint8_t offset_index, void (*Print)(void *))
{
  uint16_t limit = 0, offset = 0;
  if(argc > limit_index) limit = atoi(argv[limit_index]);
  if(argc > offset_index) offset = atoi(argv[offset_index]);
  DBG_PrintArray(file, limit, offset, Print);
}

//-------------------------------------------------------------------------------------------------

access_t DBG_Send(uint8_t *array, uint16_t length)
{
  return UART_Send(dbg_state.uart, array, length);
}

access_t DBG_SendFile(FILE_t *file)
{
  return UART_SendFile(dbg_state.uart, file);
}

//-------------------------------------------------------------------------------------------------

void DBG_BeforeReset(void)
{
  while(UART_During(dbg_state.uart)) {
    let();
  }
}