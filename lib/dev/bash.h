#ifndef BASH_H_
#define BASH_H_

#include "stream.h"
#include "log.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef BASH_FILE_LIMIT
  #define BASH_FILE_LIMIT 8
#endif

#ifndef BASH_CALLBACK_LIMIT
  #define BASH_CALLBACK_LIMIT 16
#endif

typedef enum {
  BASH_Hash_Ping = 2090616627,
  BASH_Hash_File = 2090257189,
  BASH_Hash_Uid = 193507975,
  BASH_Hash_Rtc = 193505070,
  BASH_Hash_Alarm = 253177266,
  BASH_Hash_Pwr = 193503006,
  BASH_Hash_Power = 271097426,
  BASH_Hash_Addr = 2090071808,
  BASH_Hash_List = 2090473057,
  BASH_Hash_Active = 4049882593,
  BASH_Hash_Select = 461431749,
  BASH_Hash_Info = 2090370257,
  BASH_Hash_Clear = 255552908,
  BASH_Hash_Load = 2090478981,
  BASH_Hash_Save = 2090715988,
  BASH_Hash_Append = 4065151197,
  BASH_Hash_Flash = 259106899,
  BASH_Hash_Mutex = 267752024,
  BASH_Hash_Set = 193505681,
  BASH_Hash_Rst = 193505054,
  BASH_Hash_Reset = 273105544,
  BASH_Hash_Copy = 2090156064,
  BASH_Hash_To = 5863848,
  BASH_Hash_From = 2090267097,
  BASH_Hash_A = 177670,
  BASH_Hash_B = 177671,
  BASH_Hash_Sleep = 274527774,
  BASH_Hash_Reboot = 421948272,
  BASH_Hash_Restart = 1059716234,
  BASH_Hash_Now = 193500569,
} BASH_Hash_e;

#ifdef RTC_H_
typedef enum {
  RTC_Hash_Everyday = 552618222,
  RTC_Hash_Monday = 238549325,
  RTC_Hash_Tuesday = 4252182340,
  RTC_Hash_Wednesday = 1739173961,
  RTC_Hash_Thursday = 3899371353,
  RTC_Hash_Friday = 4262946948,
  RTC_Hash_Saturday = 3744646578,
  RTC_Hash_Sunday = 480477209,
  RTC_Hash_Evd = 193490980,
  RTC_Hash_Mon = 193499471,
  RTC_Hash_Tue = 193507283,
  RTC_Hash_Wed = 193510021,
  RTC_Hash_Thu = 193506870,
  RTC_Hash_Fri = 193491942,
  RTC_Hash_Sat = 193505549,
  RTC_Hash_Sun = 193506203
} RTC_Hash_e;
#endif

typedef enum {
  PWR_Hash_Stop = 2090736459,
  PWR_Hash_Stop0 = 274826459,
  PWR_Hash_Stop1 = 274826460,
  PWR_Hash_StandbySram = 950227578,
  PWR_Hash_Standbysram = 1332813965,
  PWR_Hash_Standby = 2916655642,
  PWR_Hash_Shutdown = 4232446817,
} PWR_Hash_e;

void BASH_AddFile(FILE_t *file);
void BASH_AddCallback(void (*callback)(char **, uint16_t), char *argv0);
void BASH_FlashAutosave(bool autosave);
void BASH_WrongArgc(char *command);
void BASH_WrongArgv(char *command, char nbr);
bool BASH_Loop(STREAM_t *stream);

//-------------------------------------------------------------------------------------------------
#endif
