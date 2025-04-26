#include "dbg.h"
#include "bash.h"
#include "log.h"

//------------------------------------------------------------------------------------------------- Basic

static int32_t DBG_ConsolePrintSkip(void) {
  #if(LOG_COLORS)
    return DBG_String("\e[91m^C\e[0m ");
  #else
    return DBG_String("^C");
  #endif
}

static int32_t DBG_ConsolePrintExecute(void)
{
  #if(LOG_COLORS)
    return DBG_String("\e[32m^E\e[0m ");
  #else
    return DBG_String("^E");
  #endif
}

static void DBG_ConsoleRun(bool logs_print)
{
  if(!logs_print) {
    #if(LOG_COLORS)
      DBG_String("\e[93m^S\e[0m ");
    #else
      DBG_String("^S");
    #endif
  }
  LogPrintFlag = logs_print;
}

static BUFF_Console_t dbg_cosole = {
  .Echo = DBG_Char,
  .Enter = DBG_Enter,
  .Skip = DBG_ConsolePrintSkip,
  .Execute = DBG_ConsolePrintExecute,
  .Run = DBG_ConsoleRun
};

static uint8_t dbg_buffer_rx[DBG_RX_SIZE];
static uint8_t dbg_buffer_tx[DBG_TX_SIZE];

static BUFF_t dbg_buff = {
  .mem = dbg_buffer_rx,
  .size = DBG_RX_SIZE,
  .console = &dbg_cosole
};

static FILE_t dbg_file = {
  .name = "debug",
  .buffer = dbg_buffer_tx,
  .limit = DBG_TX_SIZE
};

UART_t *DbgUart;
FILE_t *DbgFile = &dbg_file;
bool DbgSendFlag = true;

void DBG_SwitchMode(bool data_mode)
{
  if(data_mode) {
    DbgUart->buff->console = NULL;
    UART_SetTimeout(DbgUart, DBG_DATAMODE_TIMEOUT);
  }
  else {
    DbgUart->buff->console = &dbg_cosole;
    UART_SetTimeout(DbgUart, 0);
  }
}

STREAM_t dbg_stream = {
  .name = "debug",
  .modify = STREAM_Modify_Lowercase,
  .Size = DBG_ReadSize,
  .Read = DBG_ReadString,
  .SwitchMode = DBG_SwitchMode
};

void DBG_Init(UART_t *uart)
{
  DbgUart = uart;
  DbgUart->buff = &dbg_buff;
  UART_Init(DbgUart);
}

void DBG_Loop(void)
{
  new_init(NEW_DEFAULT_LIMIT + 32);
  while(1) {
    BASH_Loop(&dbg_stream);
    if(UART_IsFree(DbgUart)) {
      clear();
      if(DbgSendFlag && DbgFile->size) {
        uint8_t *buffer = (uint8_t *)new(DbgFile->size);
        memcpy(buffer, DbgFile->buffer, DbgFile->size);
        UART_Send(DbgUart, buffer, DbgFile->size);
        FILE_Clear(DbgFile);
      }
    }
    let();
  }
}

void DBG_WaitForFree(void)
{
  while(UART_IsBusy(DbgUart)) let();
}

void DBG_WaitForFreeBlock(void)
{
  while(UART_IsBusy(DbgUart)) __NOP();
}

void DBG_Send(uint8_t *array, uint16_t length)
{
  DBG_WaitForFree();
  UART_Send(DbgUart, array, length);
  DBG_WaitForFree();
}

void DBG_SendFile(FILE_t *file)
{
  DBG_Send(file->buffer, file->size);
}

void DBG_DefaultFile(void)
{
  DbgFile = &dbg_file;
}

void DBG_SetFile(FILE_t *file)
{
  DbgFile = file;
}

//------------------------------------------------------------------------------------------------- Read

uint16_t DBG_ReadArray(uint8_t *array)
{
  return UART_ReadArray(DbgUart, array);
}

uint16_t DBG_ReadSize(void)
{
  return UART_ReadSize(DbgUart);
}

char *DBG_ReadString(void)
{
  return UART_ReadString(DbgUart);
}

//------------------------------------------------------------------------------------------------- Add

int32_t DBG_Char(uint8_t data) { return FILE_Char(DbgFile, data); }
int32_t DBG_Char16(uint16_t data) { return FILE_Char16(DbgFile, data); }
int32_t DBG_Char32(uint32_t data) { return FILE_Char32(DbgFile, data); }
int32_t DBG_Char64(uint64_t data) { return FILE_Char64(DbgFile, data); }
int32_t DBG_Data(uint8_t *array, uint16_t length) { return FILE_Data(DbgFile, array, length); }
int32_t DBG_String(char *string) { return FILE_String(DbgFile, string); }
int32_t DBG_Enter(void) { return FILE_Enter(DbgFile); }
int32_t DBG_ClearLastLine(void) { return FILE_ClearLastLine(DbgFile); }
int32_t DBG_Bool(bool value) { return FILE_Bool(DbgFile, value); }
int32_t DBG_Int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space) {
  return FILE_Int(DbgFile, nbr, base, sign, fill_zero, fill_space);
}
int32_t DBG_Float(float nbr, uint8_t accuracy) {
  return FILE_Float(DbgFile, nbr, accuracy, 1);
}
int32_t DBG_FloatSpace(float nbr, uint8_t accuracy, uint8_t fill_space) {
  return FILE_Float(DbgFile, nbr, accuracy, fill_space);
}
int32_t DBG_Dec(int32_t nbr) { return FILE_Dec(DbgFile, nbr); }
int32_t DBG_uDec(uint32_t nbr) { return FILE_uDec(DbgFile, nbr); }
int32_t DBG_Hex8(uint32_t nbr) { return FILE_Hex8(DbgFile, nbr); }
int32_t DBG_Hex16(uint32_t nbr) { return FILE_Hex16(DbgFile, nbr); }
int32_t DBG_Hex32(uint32_t nbr) { return FILE_Hex32(DbgFile, nbr); }
int32_t DBG_Bin8(uint8_t nbr) { return FILE_Bin8(DbgFile, nbr); }

int32_t DBG_Date(RTC_Datetime_t *datetime) { return FILE_Date(DbgFile, datetime); }
int32_t DBG_Time(RTC_Datetime_t *datetime) { return FILE_Time(DbgFile, datetime); }
int32_t DBG_TimeMs(RTC_Datetime_t *datetime) { return FILE_TimeMs(DbgFile, datetime); }
int32_t DBG_Datetime(RTC_Datetime_t *datetime) { return FILE_Datetime(DbgFile, datetime); }
int32_t DBG_DatetimeMs(RTC_Datetime_t *datetime) { return FILE_DatetimeMs(DbgFile, datetime); }
int32_t DBG_AlarmTime(RTC_Alarm_t *alarm) { return FILE_AlarmTime(DbgFile, alarm); }
int32_t DBG_Alarm(RTC_Alarm_t *alarm) { return FILE_Alarm(DbgFile, alarm); }

int32_t DBG_File(FILE_t *file)
{
  int32_t size = 0;
  size += DBG_String((char *)file->name);
  size += DBG_Char(' ');
  size += DBG_uDec(file->size);
  size += DBG_Char('/');
  size += DBG_uDec(file->limit);
  if(file->mutex) size += DBG_String(" mutex");
  if(file->flash_page) {
    size += DBG_String(" flash:");
    size += DBG_uDec(file->flash_page);
  }
  return size;
}

//-------------------------------------------------------------------------------------------------

int32_t DBG_int8_Print(int8_t *nbr) { return DBG_Dec(*nbr); }
int32_t DBG_uint8_Print(uint8_t *nbr) { return DBG_uDec(*nbr); }
int32_t DBG_int16_Print(int16_t *nbr) { return DBG_Dec(*nbr); }
int32_t DBG_uint16_Print(uint16_t *nbr) { return DBG_uDec(*nbr); }
int32_t DBG_int32_Print(int32_t *nbr) { return DBG_Dec(*nbr); }
int32_t DBG_uint32_Print(uint32_t *nbr) { return DBG_uDec(*nbr); }
int32_t DBG_float_Print(float *nbr) { return DBG_Float(*nbr, 6); }

//-------------------------------------------------------------------------------------------------