#include "dbg.h"
#include "bash.h"
#include "opencplc.h"

//------------------------------------------------------------------------------------------------- Basic

static struct {
  FILE_t *file;
  UART_t *uart;
  bool run;
} dbg;

STREAM_t dbg_stream = {
  .name = "debug",
  .modify = STREAM_Modify_Lowercase,
  .Size = DBG_ReadSize,
  .Read = DBG_ReadString
};

void DBG_Init(UART_t *uart, FILE_t *file)
{
  dbg.uart = uart;
  dbg.file = file;
  dbg.run = true;
  UART_Init(uart);
}

void DBG_Loop(void)
{
  NEW_Init(8);
  while(1) {
    BASH_Loop(&dbg_stream);
    if(UART_IsFree(dbg.uart)) {
      clear();
      if(dbg.run == true && dbg.file->size) {
        uint8_t *buffer = (uint8_t *)new(dbg.file->size);
        memcpy(buffer, dbg.file->buffer, dbg.file->size);
        UART_Send(dbg.uart, buffer, dbg.file->size);
        FILE_Clear(dbg.file);
      }
    }
    let();
  }
}

void DBG_Enable(void)
{
  dbg.run = true;
}

void DBG_Disable(void)
{
  dbg.run = false;
}

void DBG_BeforeReset(void)
{
  while(UART_During(dbg.uart)) let();
}

static void DBG_Wait4Uart(void)
{
  while(UART_IsBusy(dbg.uart)) let();
}

void DBG_Send(uint8_t *array, uint16_t length)
{
  DBG_Wait4Uart();
  UART_Send(dbg.uart, array, length);
}

void DBG_SendFile(FILE_t *file)
{
  DBG_Send(file->buffer, file->size);
}

//------------------------------------------------------------------------------------------------- Read

uint16_t DBG_ReadArray(uint8_t *array)
{
  return UART_ReadArray(dbg.uart, array);
}

uint16_t DBG_ReadSize(void)
{
  return UART_ReadSize(dbg.uart);
}

char *DBG_ReadString(void)
{
  return UART_ReadString(dbg.uart);
}

//------------------------------------------------------------------------------------------------- Add

int32_t DBG_Char(uint8_t data) { return FILE_Char(dbg.file, data); }
int32_t DBG_Char16(uint16_t data) { return FILE_Char16(dbg.file, data); }
int32_t DBG_Char32(uint32_t data) { return FILE_Char32(dbg.file, data); }
int32_t DBG_Char64(uint64_t data) { return FILE_Char64(dbg.file, data); }
int32_t DBG_Array(uint8_t *array, uint16_t length) { return FILE_Array(dbg.file, array, length); }
int32_t DBG_String(char *string) { return FILE_String(dbg.file, string); }
int32_t DBG_Enter(void) { return FILE_Enter(dbg.file); }
int32_t DBG_ClearLastLine(void) { return FILE_ClearLastLine(dbg.file); }
int32_t DBG_Bool(bool value) { return FILE_Bool(dbg.file, value); }

int32_t DBG_Int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space) {
  return FILE_Int(dbg.file, nbr, base, sign, fill_zero, fill_space);
}

int32_t DBG_Float(float nbr, uint8_t accuracy) {
  return FILE_Float(dbg.file, nbr, accuracy, 1);
}

int32_t DBG_Dec(int32_t nbr) { return FILE_Dec(dbg.file, nbr); }
int32_t DBG_uDec(uint32_t nbr) { return FILE_uDec(dbg.file, nbr); }
int32_t DBG_Hex8(uint32_t nbr) { return FILE_Hex8(dbg.file, nbr); }
int32_t DBG_Hex16(uint32_t nbr) { return FILE_Hex16(dbg.file, nbr); }
int32_t DBG_Hex32(uint32_t nbr) { return FILE_Hex32(dbg.file, nbr); }
int32_t DBG_Bin8(uint8_t nbr) { return FILE_Bin8(dbg.file, nbr); }

int32_t DBG_Date(RTC_Datetime_t *datetime) { return FILE_Date(dbg.file, datetime); }
int32_t DBG_Time(RTC_Datetime_t *datetime) { return FILE_Time(dbg.file, datetime); }
int32_t DBG_TimeMs(RTC_Datetime_t *datetime) { return FILE_TimeMs(dbg.file, datetime); }
int32_t DBG_Datetime(RTC_Datetime_t *datetime) { return FILE_Datetime(dbg.file, datetime); }
int32_t DBG_DatetimeMs(RTC_Datetime_t *datetime) { return FILE_DatetimeMs(dbg.file, datetime); }
int32_t DBG_AlarmTime(RTC_Alarm_t *alarm) { return FILE_AlarmTime(dbg.file, alarm); }
int32_t DBG_Alarm(RTC_Alarm_t *alarm) { return FILE_Alarm(dbg.file, alarm); }

//------------------------------------------------------------------------------------------------- Log

void print(const char *template, ...)
{
  va_list args;
  va_start(args, template);
  FILE_Print(dbg.file, template, args);
  va_end(args);
}

inline static void LOG_Datetime(void)
{
  RTC_Datetime_t dt = RTC_Datetime();
  #if(LOG_MILLISECONDS)
    #if(LOG_TIME_ONLY)
      DBG_TimeMs(&dt);
    #else
      DBG_DatetimeMs(&dt);
    #endif
  #else
    #if(LOG_TIME_ONLY)
      DBG_Time(&dt);
    #else
      DBG_Datetime(&dt);
    #endif
  #endif
}

void LOG_Init(const char *board)
{
  #if(LOG_INIT)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[36mINIT:\e[0m ");
      DBG_String("OpenCPLC \e[90mversion:\e[0m");
      DBG_String(OPENCPLC_VERSION);
      DBG_String(" \e[90mboard:\e[0m");
    #else
      DBG_String(" INIT: ");
      DBG_String("OpenCPLC version:");
      DBG_String(OPENCPLC_VERSION);
      DBG_String(" board:");
    #endif
    DBG_String((char *)board);
    DBG_Enter();
  #endif
}

void LOG_Debug(const char *message, ...)
{
  #if(LOG_LEVEL <= LEG_Level_Debug)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[92mDBUG:\e[0m ");
    #else
      DBG_String(" DBUG: ");
    #endif
    va_list args;
    va_start(args, message);
    print(message, args);
    va_end(args);
    DBG_Enter();
  #endif
}

void LOG_Info(const char *message, ...)
{
  #if(LOG_LEVEL <= LEG_Level_Info)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[94mINFO:\e[0m ");
    #else
      DBG_String(" INFO: ");
    #endif
    va_list args;
    va_start(args, message);
    print(message, args);
    va_end(args);
    DBG_Enter();
  #endif
}

void LOG_Warning(const char *message, ...)
{
  #if(LOG_LEVEL <= LEG_Level_Warning)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[93mWARN:\e[0m ");
    #else
      DBG_String(" WARN: ");
    #endif
    va_list args;
    va_start(args, message);
    print(message, args);
    va_end(args);
    DBG_Enter();
  #endif
}

void LOG_Error(const char *message, ...)
{
  #if(LOG_LEVEL <= LEG_Level_Error)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[91mERRO:\e[0m ");
    #else
      DBG_String(" ERRO: ");
    #endif
    va_list args;
    va_start(args, message);
    print(message, args);
    va_end(args);
    DBG_Enter();
  #endif
}

void LOG_Critical(const char *message, ...)
{
  #if(LOG_LEVEL <= LEG_Level_Critical)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[95mCRIT:\e[0m ");
    #else
      DBG_String(" CRIT: ");
    #endif
    va_list args;
    va_start(args, message);
    print(message, args);
    va_end(args);
    DBG_Enter();
    dbg.run = false;
    DBG_Send(dbg.file->buffer, dbg.file->size);
    DBG_Wait4Uart();
    FILE_Clear(dbg.file);
    dbg.run = true;
  #endif
}

void LOG_Panic(const char *message)
{
  #if(LOG_LEVEL <= LEG_Level_Panic)
    LOG_Datetime();
    #if(LOG_COLORS)
      DBG_String(" \e[31mPANC:\e[0m ");
    #else
      DBG_String(" PANC: ");
    #endif
    DBG_String((char *)message);
    DBG_Enter();
    dbg.run = false;
    DBG_Send(dbg.file->buffer, dbg.file->size);
    DBG_Wait4Uart();
    FILE_Clear(dbg.file);
    dbg.run = true;
  #endif
}

//------------------------------------------------------------------------------------------------- Array

static inline uint16_t _DBG_Array_PrintHeader(char *name, uint16_t count, uint16_t *limit, uint16_t *offset)
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

void DBG_Array_Print(FILE_t *file, uint16_t limit, uint16_t offset, void (*Print)(void *))
{
  uint16_t count = FILE_Struct_GetCount(file);
  uint8_t *p = file->buffer + (file->struct_size * offset);
  char *name = strtoupper((char *)file->name);
  _DBG_Array_PrintHeader(name, count, &limit, &offset);
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

void DBG_Array_PrintBash(FILE_t *file, char **argv, uint16_t argc, uint16_t limit_index, uint16_t offset_index, void (*Print)(void *))
{
  uint16_t limit = 0, offset = 0;
  if(argc > limit_index) limit = (uint16_t)atoi(argv[limit_index]);
  if(argc > offset_index) offset = (uint16_t)atoi(argv[offset_index]);
  DBG_Array_Print(file, limit, offset, Print);
}

//------------------------------------------------------------------------------------------------- File

void DBG_File_Print(FILE_t *file)
{
  #if(FILE_DBG)
    DBG_String("FILE "); DBG_String((char *)file->name); DBG_Char(' ');
    DBG_uDec(file->size); DBG_Char('/'); DBG_uDec(file->limit);
    if(file->mutex) DBG_String(" mutex");
    if(file->flash_page) { DBG_String(" flash:"); DBG_uDec(file->flash_page); }
    DBG_Enter();
  #endif
}

void DBG_File_PPrint(FILE_t **file)
{
  DBG_File_Print(*file);
}

//-------------------------------------------------------------------------------------------------
