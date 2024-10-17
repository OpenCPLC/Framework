#ifndef DBG_H_
#define DBG_H_

#include "main.h"
#include "uart.h"

//-------------------------------------------------------------------------------------------------

void DBG_Init(UART_t *uart, FILE_t *file);
uint16_t DBG_ReadArray(uint8_t *array);
uint16_t DBG_ReadSize(void);
char *DBG_ReadString(void);

uint8_t DBG_Array(uint8_t *ary, uint16_t len);
uint8_t DBG_Char(char data);
uint8_t DBG_Char16(uint16_t data);
uint8_t DBG_Char32(uint32_t data);
uint8_t DBG_Char64(uint64_t data);
uint8_t DBG_String(char *str);
uint8_t DBG_Enter(void);

uint8_t DBG_Int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space);
uint8_t DBG_Float(float nbr, uint8_t accuracy);
uint8_t DBG_FloatInt(float nbr, uint8_t accuracy);
uint8_t DBG_Dec(int32_t nbr);
uint8_t DBG_uDec(uint32_t nbr);
uint8_t DBG_Hex8(uint32_t nbr);
uint8_t DBG_Hex16(uint32_t nbr);
uint8_t DBG_Hex32(uint32_t nbr);
uint8_t DBG_Bin8(uint8_t nbr);
uint8_t DBG_Bin16(uint16_t nbr, char *sep);
uint8_t DBG_Bin32(uint32_t nbr, char *sep);
uint8_t DBG_Bool(bool value);
uint8_t DBG_Status(bool value);

uint8_t DBG_Date(RTC_Datetime_t *datetime);
uint8_t DBG_Time(RTC_Datetime_t *datetime);
uint8_t DBG_Datetime(RTC_Datetime_t *datetime);
uint8_t DBG_DatetimeMs(RTC_Datetime_t *datetime);
uint8_t DBG_AlarmTime(RTC_Alarm_t *alarm);
uint8_t DBG_Alarm(RTC_Alarm_t *alarm);
uint8_t DBG_Now(void);
uint8_t DBG_NowMs(void);

uint8_t DBG_Ip(uint8_t *ip);
uint8_t DBG_ClearLastLine(void);
uint8_t DBG_Clear(void);
void DBG_PrintArray(FILE_t *file, uint16_t limit, uint16_t offset, void (*Print)(void *));
void DBG_PrintArrayBash(FILE_t *file, char **argv, uint16_t argc, uint8_t limit_index, uint8_t offset_index, void (*Print)(void *));

access_t DBG_Send(uint8_t *array, uint16_t length);
access_t DBG_SendFile(FILE_t *file);

void DBG_BeforeReset(void);

//-------------------------------------------------------------------------------------------------
#endif
