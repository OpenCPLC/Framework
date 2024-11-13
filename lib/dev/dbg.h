#ifndef DBG_H_
#define DBG_H_

#include "uart.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef LOG_MILLISECONDS
  #define LOG_MILLISECONDS 0
#endif

#ifndef LOG_TIME_ONLY
  #define LOG_TIME_ONLY 0
#endif

#ifndef LOG_COLORS
  #define LOG_COLORS 1
#endif

#ifndef LOG_INIT
  #define LOG_INIT 1
#endif

#define LOG_LEVEL_DBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERRO 3
#define LOG_LEVEL_CRIT 4
#define LOG_LEVEL_PANC 5
#define LOG_LEVEL_VOID 6

typedef enum {
  LOG_Level_Debug = LOG_LEVEL_DBUG,
  LOG_Level_Info = LOG_LEVEL_INFO,
  LOG_Level_Warning = LOG_LEVEL_WARN,
  LOG_Level_Error = LOG_LEVEL_ERRO,
  LOG_Level_Critical = LOG_LEVEL_CRIT,
  LOG_Level_Panic = LOG_LEVEL_PANC,
  LOG_Level_Void = LOG_LEVEL_VOID
} LOG_Level_e;

#ifndef LOG_LEVEL
  #define LOG_LEVEL LOG_LEVEL_INFO
#endif

void print(const char *template, ...);
void LOG_Init(const char *board);
void LOG_Debug(const char *message, ...);
void LOG_Info(const char *message, ...);
void LOG_Warning(const char *message, ...);
void LOG_Error(const char *message, ...);
void LOG_Critical(const char *message, ...);
void LOG_Panic(const char *message);
void LOG_Message(LOG_Level_e lvl, char *message, ...);

//-------------------------------------------------------------------------------------------------

void DBG_Init(UART_t *uart, FILE_t *file);
void DBG_Loop(void);
void DBG_Enable(void);
void DBG_Disable(void);
void DBG_BeforeReset(void);
void DBG_Send(uint8_t *array, uint16_t length);
void DBG_SendFile(FILE_t *file);

uint16_t DBG_ReadArray(uint8_t *array);
uint16_t DBG_ReadSize(void);
char *DBG_ReadString(void);

int32_t DBG_Char(uint8_t data);
int32_t DBG_Char16(uint16_t data);
int32_t DBG_Char32(uint32_t data);
int32_t DBG_Char64(uint64_t data);
int32_t DBG_Array(uint8_t *array, uint16_t length);
int32_t DBG_String(char *string);
int32_t DBG_Enter(void);
int32_t DBG_ClearLastLine(void);
int32_t DBG_Bool(bool value);

int32_t DBG_Int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space);
int32_t DBG_Float(float nbr, uint8_t accuracy);
int32_t DBG_Dec(int32_t nbr);
int32_t DBG_uDec(uint32_t nbr);
int32_t DBG_Hex8(uint32_t nbr);
int32_t DBG_Hex16(uint32_t nbr);
int32_t DBG_Hex32(uint32_t nbr);
int32_t DBG_Bin8(uint8_t nbr);

int32_t DBG_Date(RTC_Datetime_t *datetime);
int32_t DBG_Time(RTC_Datetime_t *datetime);
int32_t DBG_TimeMs(RTC_Datetime_t *datetime);
int32_t DBG_Datetime(RTC_Datetime_t *datetime);
int32_t DBG_DatetimeMs(RTC_Datetime_t *datetime);
int32_t DBG_AlarmTime(RTC_Alarm_t *alarm);
int32_t DBG_Alarm(RTC_Alarm_t *alarm);

void DBG_Array_Print(FILE_t *file, uint16_t limit, uint16_t offset, void (*Print)(void *));
void DBG_Array_PrintBash(FILE_t *file, char **argv, uint16_t argc, uint16_t limit_index, uint16_t offset_index, void (*Print)(void *));
void DBG_File_Print(FILE_t *file);
void DBG_File_PPrint(FILE_t **file);

//-------------------------------------------------------------------------------------------------
#endif
