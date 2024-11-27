#ifndef FILE_H_
#define FILE_H_

#include <stdarg.h>
#include "flash.h"
#include "vrts.h"
#include "rtc.h"
#include "crc.h"
#include "exstring.h"
#include "main.h"

#ifndef FILE_ENTER_RETURN
  #define FILE_ENTER_RETURN 1
#endif

//-------------------------------------------------------------------------------------------------

typedef struct {
  const char *name;
  uint8_t *buffer;
  uint16_t size;
  uint16_t limit;
  bool mutex;
  uint8_t flash_page;
  uint16_t offset;
  uint16_t struct_size;
  int32_t (*struct_print)(void *);
} FILE_t;

//-------------------------------------------------------------------------------------------------

int32_t FILE_Char(FILE_t *file, uint8_t data);
int32_t FILE_Char16(FILE_t *file, uint16_t data);
int32_t FILE_Char32(FILE_t *file, uint32_t data);
int32_t FILE_Char64(FILE_t *file, uint64_t data);
int32_t FILE_Array(FILE_t *file, uint8_t *array, uint16_t length);
int32_t FILE_String(FILE_t *file, char *string);
int32_t FILE_Enter(FILE_t *file);
int32_t FILE_ClearLastLine(FILE_t *file);
int32_t FILE_Bool(FILE_t *file, bool value);

int32_t FILE_Int(FILE_t *file, int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space);
int32_t FILE_Float(FILE_t *file, float nbr, uint8_t accuracy, uint8_t fill_space);
int32_t FILE_Dec(FILE_t *file, int32_t nbr);
int32_t FILE_uDec(FILE_t *file, uint32_t nbr);
int32_t FILE_Hex8(FILE_t *file, uint32_t nbr);
int32_t FILE_Hex16(FILE_t *file, uint32_t nbr);
int32_t FILE_Hex32(FILE_t *file, uint32_t nbr);
int32_t FILE_Bin8(FILE_t *file, uint8_t nbr);

int32_t FILE_Date(FILE_t *file, RTC_Datetime_t *datetime);
int32_t FILE_Time(FILE_t *file, RTC_Datetime_t *datetime);
int32_t FILE_TimeMs(FILE_t *file, RTC_Datetime_t *datetime);
int32_t FILE_Datetime(FILE_t *file, RTC_Datetime_t *datetime);
int32_t FILE_DatetimeMs(FILE_t *file, RTC_Datetime_t *datetime);
int32_t FILE_AlarmTime(FILE_t *file, RTC_Alarm_t *alarm);
int32_t FILE_Alarm(FILE_t *file, RTC_Alarm_t *alarm);

int32_t FILE_Clear(FILE_t *file);
state_t FILE_Copy(FILE_t *file_to, FILE_t *file_from);
state_t FILE_Save(FILE_t *file, uint8_t *data, uint16_t size);
state_t FILE_Append(FILE_t *file, uint8_t *data, uint16_t size);
state_t FILE_Access_Get(FILE_t *file);
void FILE_Access_Allow(FILE_t *file);
state_t FILE_Access_Get2(FILE_t *primary, FILE_t *secondary);
state_t FILE_Flash_Save(FILE_t *file);
state_t FILE_Flash_Load(FILE_t *file);

int32_t FILE_Struct_Add(FILE_t *file, uint8_t *object);
uint16_t FILE_Struct_GetCount(FILE_t *file);
uint16_t FILE_Struct_GetFreeSpace(FILE_t *file, uint16_t margin);
int32_t FILE_Struct_Move(FILE_t *file, uint16_t count);
int32_t FILE_Struct_Drop(FILE_t *file, uint16_t count);
state_t FILE_Offset_Drop(FILE_t *file);
state_t FILE_Offset_Set(FILE_t *file, uint16_t offset);

int32_t FILE_Crc_Append(FILE_t *file, const CRC_t *crc);
bool FILE_Crc_IsError(FILE_t *file, const CRC_t *crc);

//-------------------------------------------------------------------------------------------------
#endif
