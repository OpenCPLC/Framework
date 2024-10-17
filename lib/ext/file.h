#ifndef FILE_H_
#define FILE_H_

#include "flash.h"
#include "vrts.h"
#include "rtc.h"
#include "crc.h"
#include "exstring.h"
#include "main.h"

#ifndef FILE_DBG
  #define FILE_DBG 1
#endif

//-------------------------------------------------------------------------------------------------

typedef struct {
  const char *name;
  uint8_t *buffer;
  uint16_t size;
  uint16_t limit;
  bool mutex;
  uint8_t flash_page;
  uint16_t _offset;
  uint16_t struct_size;
} FILE_t;

//-------------------------------------------------------------------------------------------------

status_t FILE_Char(FILE_t *file, uint8_t data);
status_t FILE_Char16(FILE_t *file, uint16_t data);
status_t FILE_Char32(FILE_t *file, uint32_t data);
status_t FILE_Char64(FILE_t *file, uint64_t data);
status_t FILE_Array(FILE_t *file, uint8_t *array, uint16_t length);
status_t FILE_String(FILE_t *file, char *string);
status_t FILE_Enter(FILE_t *file);

status_t FILE_Int(FILE_t *file, int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space);
status_t FILE_Float(FILE_t *file, float nbr, uint8_t accuracy, uint8_t fill_space);
status_t FILE_Dec(FILE_t *file, int32_t nbr);
status_t FILE_uDec(FILE_t *file, uint32_t nbr);
status_t FILE_Hex8(FILE_t *file, uint32_t nbr);
status_t FILE_Hex16(FILE_t *file, uint32_t nbr);
status_t FILE_Hex32(FILE_t *file, uint32_t nbr);
status_t FILE_Bin8(FILE_t *file, uint8_t nbr);
status_t FILE_Bin16(FILE_t *file, uint16_t nbr, char *sep);
status_t FILE_Bin32(FILE_t *file, uint32_t nbr, char *sep);
status_t FILE_Bool(FILE_t *file, bool value);
status_t FILE_Status(FILE_t *file, bool value);
status_t FILE_Struct(FILE_t *file, uint8_t *object);
uint16_t FILE_StructCount(FILE_t *file);
uint16_t FILE_StructFreeSpace(FILE_t *file, uint16_t margin);
status_t FILE_StructMove(FILE_t *file, uint8_t count);
status_t FILE_StructDrop(FILE_t *file, uint8_t count);

status_t FILE_Date(FILE_t *file, RTC_Datetime_t *datetime);
status_t FILE_Time(FILE_t *file, RTC_Datetime_t *datetime);
status_t FILE_Datetime(FILE_t *file, RTC_Datetime_t *datetime);
status_t FILE_DatetimeMs(FILE_t *file, RTC_Datetime_t *datetime);
status_t FILE_AlarmTime(FILE_t *file, RTC_Alarm_t *alarm);
status_t FILE_Alarm(FILE_t *file, RTC_Alarm_t *alarm);
status_t FILE_Now(FILE_t *file);
status_t FILE_NowMs(FILE_t *file);

status_t FILE_Ip(FILE_t *file, uint8_t *ip);
status_t FILE_ClearLastLine(FILE_t *file);

status_t FILE_Crc(FILE_t *file, const CRC_t *crc);
status_t FILE_CrcError(FILE_t *file, const CRC_t *crc);

status_t FILE_Clear(FILE_t *file);
status_t FILE_Copy(FILE_t *file_to, FILE_t *file_from);
status_t FILE_Save(FILE_t *file, uint8_t *data, uint16_t size);
status_t FILE_Append(FILE_t *file, uint8_t *data, uint16_t size);

status_t FILE_GetAccess(FILE_t *file);
status_t FILE_Get2Access(FILE_t *primary, FILE_t *secondary);
void FILE_AllowAccess(FILE_t *file);

status_t FILE_FlashSave(FILE_t *file);
status_t FILE_FlashLoad(FILE_t *file);

status_t FILE_OffsetDrop(FILE_t *file);
status_t FILE_OffsetSet(FILE_t *file, uint16_t offset);

void FILE_Print(FILE_t *file);
void FILE_PointerPrint(FILE_t **file);

//-------------------------------------------------------------------------------------------------
#endif
