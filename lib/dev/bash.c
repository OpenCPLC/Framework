#include "bash.h"
#include "dbg.h"

//------------------------------------------------------------------------------------------------- STRUCT

struct {
  FILE_t *file[BASH_FILE_LIMIT];
  uint8_t file_count;
  bool (*callback[BASH_CALLBACK_LIMIT])(char **, uint16_t);
  uint8_t callback_count;
  bool flash_autosave;
  FILE_t *cache;
  FILE_t *src;
  FILE_t *dsc;
  void (*Sleep)(PWR_SleepMode_e);
  void (*Reset)(void);
} bash_state;

void BASH_AddFile(FILE_t *file)
{
  bash_state.file[bash_state.file_count] = file;
  if(!bash_state.file_count) {
    bash_state.cache = file;
    bash_state.src = file;
    bash_state.dsc = file;
  }
  bash_state.file_count++;
  FILE_FlashLoad(file);
}

void BASH_AddCallback(bool (*callback)(char **, uint16_t))
{
  bash_state.callback[bash_state.callback_count] = callback;
  bash_state.callback_count++;
}

void BASH_SetFlashAutosave(void (*callback)(char **, uint16_t))
{
  bash_state.flash_autosave = true;
}

//------------------------------------------------------------------------------------------------- DATA

static void BASH_Data(uint8_t *data, uint16_t size, STREAM_t *stream)
{
  stream->counter--;
  #if(BASH_DBG)
    DBG_String("FILE data ");
    DBG_String((char *)bash_state.cache->name);
    DBG_String(" frame:"); DBG_uDec(stream->counter);
    DBG_Enter();
  #endif
  bash_state.cache->mutex = false;
  FILE_Append(bash_state.cache, data, size);
  if(!stream->counter) {
    stream->mode = STREAM_Mode_String;
    if(bash_state.flash_autosave) FILE_FlashSave(bash_state.cache);
  }
  else bash_state.cache->mutex = true;
}

//------------------------------------------------------------------------------------------------- FILE

static void _BASH_AccessDenied(FILE_t *file)
{
  #if(BASH_DBG)
    DBG_String("FILE ");
    DBG_String((char *)file->name);
    DBG_String(" access-denied");
    DBG_Enter();
  #endif
}

static void BASH_File(char **argv, uint16_t argc, STREAM_t *stream)
{
  if(argc < 2) return;
  #if(BASH_DBG)
  if(!strcmp(argv[1], "list") && argc == 2) { // FILE list
    DBG_String("FILE "); DBG_uDec(bash_state.file_count);
    DBG_String(" 0.."); DBG_uDec(bash_state.file_count - 1);
    DBG_Enter();
    for(uint8_t i = 0; i < bash_state.file_count; i++) {
      DBG_String("  "); DBG_uDec(i);
      DBG_Char(':'); DBG_String((char *)bash_state.file[i]->name);
      DBG_Enter();
    }
    return;
  }
  #endif
  if(!strcmp(argv[1], "cache") && argc == 3) { // FILE cache [file-nbr]
    uint8_t nbr = atoi(argv[2]);
    if(nbr < bash_state.file_count) bash_state.cache = bash_state.file[nbr];
    #if(BASH_DBG)
      FILE_Print(bash_state.cache);
    #endif
    return;
  }
  if(!strcmp(argv[1], "src-dsc") && argc == 4) { // FILE src-dsc [src-file-nbr] [dsc-file-nbr]
    uint8_t a = atoi(argv[2]);
    uint8_t b = atoi(argv[3]);
    if(a < bash_state.file_count) bash_state.src = bash_state.file[a];
    if(b < bash_state.file_count) bash_state.dsc = bash_state.file[b];
    #if(BASH_DBG)
      FILE_Print(bash_state.src);
      FILE_Print(bash_state.dsc);
    #endif
    return;
  }
  if(!strcmp(argv[1], "clear") && argc == 2) { // FILE clear
    if(FILE_Clear(bash_state.cache)) _BASH_AccessDenied(bash_state.cache);
    FILE_Print(bash_state.cache);
    return;
  }
  if(!strcmp(argv[1], "load") && argc <= 4) { // FILE load [limit] [offset]
    uint16_t limit = bash_state.cache->size;
    uint16_t offset = 0;
    if(argc >= 3) limit = atoi(argv[2]);
    if(argc >= 4) offset = atoi(argv[3]);
    if(offset >= bash_state.cache->size) offset = 0;
    if(limit + offset > bash_state.cache->size) limit = bash_state.cache->size - offset;
    if(!bash_state.cache->mutex) {
      DBG_SendFile(bash_state.cache);
    }
    else _BASH_AccessDenied(bash_state.cache);
    return;
  }
  if(!strcmp(argv[1], "save") && (argc == 2 || argc == 3)) { // FILE save [frames]
    if(FILE_Clear(bash_state.cache)) _BASH_AccessDenied(bash_state.cache);
    else {
      stream->mode = STREAM_Mode_Data;
      stream->counter = argc == 3 ? atoi(argv[2]) : 1;
      bash_state.cache->mutex = true;
      #if(BASH_DBG)
        DBG_String("FILE save ");
        DBG_String((char *)bash_state.cache->name);
        DBG_String(" frame:"); DBG_uDec(stream->counter);
        DBG_Enter();
      #endif
    }
    return;
  }
  if(!strcmp(argv[1], "append") && argc == 3) { // FILE append [frames]
    if(bash_state.cache->mutex) _BASH_AccessDenied(bash_state.cache);
    else {
      stream->mode = STREAM_Mode_Data;
      stream->counter = atoi(argv[2]);
      bash_state.cache->mutex = true;
      #if(BASH_DBG)
        DBG_String("FILE append ");
        DBG_String((char *)bash_state.cache->name);
        DBG_String(" frame:"); DBG_uDec(stream->counter);
        DBG_Enter();
      #endif
    }
    return;
  }
  if(!strcmp(argv[1], "flash") && argc == 3) { // FILE flash {save|load}
    if(!strcmp(argv[2], "save")) FILE_FlashSave(bash_state.cache);
    else if(!strcmp(argv[2], "load")) FILE_FlashLoad(bash_state.cache);
    return;
  }
  if(!strcmp(argv[1], "mutex") && argc == 3) { // FILE mutex {set|rst}
    if(!strcmp(argv[2], "set")) bash_state.cache->mutex = true;
    else if(!strcmp(argv[2], "rst")) bash_state.cache->mutex = false;
    #if(BASH_DBG)
      FILE_Print(bash_state.cache);
    #endif
    return;
  }
  if(!strcmp(argv[1], "copy") && (argc == 2 || argc == 4)) { // FILE copy ([tx-nbr] [rx-nbr])
    FILE_t *src = NULL;
    FILE_t *dsc = NULL;
    if(argc == 2) {
      src = bash_state.src;
      dsc = bash_state.dsc;
    } else if(argc == 4) {
      src = bash_state.file[atoi(argv[2])];
      dsc = bash_state.file[atoi(argv[3])];
    }
    if(FILE_Copy(dsc, src))
      _BASH_AccessDenied(dsc);
    else {
      FILE_Print(src);
      FILE_Print(dsc);
    }
  }
}

//------------------------------------------------------------------------------------------------- UID

void BASH_Uid(char **argv, uint16_t argc)
{
  if(argc != 1) return;
  DBG_String("UID ");
  uint8_t *uid = (uint8_t *)UID_BASE;
  for(uint8_t i = 0; i < 12; i++) DBG_Hex8(uid[i]);
  DBG_Enter();
}

//------------------------------------------------------------------------------------------------- RTC
#ifdef RTC_H_

void BASH_Rtc(char **argv, uint16_t argc)
{
  if(argc == 1) {
    RTC_Datetime_t datetime = RTC_Datetime();
    #if(BASH_DBG)
      DBG_String("RTC ");
      DBG_Datetime(&datetime);
      DBG_Char(' ');
      DBG_String((char *)rtc_weak_day_string[datetime.week_day]);
      DBG_Enter();
    #else
      DBG_Dec(RTC_DatetimeUnix(&datetime));
    #endif
    return;
  }
  if(argc == 2) {
    if(!strcmp(argv[1], "reset") || !strcmp(argv[1], "rst")) {
      RTC_Reset();
      DBG_String("RTC reset");
      DBG_Enter();
      return;
    }
    #if(BASH_DBG)
      DBG_String("RTC set-by-timestamp");
      DBG_Enter();
    #endif
    RTC_SetTimestamp(strtoul(argv[1], NULL, 10));
    return;
  }
  if(argc == 3) {
    #if(BASH_DBG)
      DBG_String("RTC set-by-datatime");
      DBG_Enter();
    #endif
    char *date = replace_this_char("\"/,:+-_", ',',  argv[1]);
    char *time = replace_this_char("\"/,:+-_", ',',  argv[2]);
    RTC_Datetime_t datetime = {
      .year = atoi(extraction(date, ',', 0)) % 100,
      .month = atoi(extraction(date, ',', 1)),
      .month_day = atoi(extraction(date, ',', 2)),
      .hour = atoi(extraction(time, ',', 0)),
      .minute = atoi(extraction(time, ',', 1)),
      .second = atoi(extraction(time, ',', 2))
    };
    RTC_SetDatetime(&datetime);
    return;
  }
}

void BASH_Alarm(char **argv, uint16_t argc)
{
  if(argc < 2) return;
  RTC_ALARM_e ab;
  if(!strcmp(argv[1], "a")) ab = RTC_ALARM_A;
  else if(!strcmp(argv[1], "b")) ab = RTC_ALARM_B;
  else return;
  if(argc == 4) {
    uint8_t week_day = atoi(argv[2]);
    bool week_day_mask = true;
    if(week_day) week_day_mask = false;
    char *hms = replace_this_char("\"/,.:+-_", ',',  argv[3]);
    RTC_Alarm_t alarm = {
      .week = true,
      .day_mask = week_day_mask,
      .day = week_day,
      .hour_mask = false,
      .hour = atoi(extraction(hms, ',', 0)),
      .minute_mask = false,
      .minute = atoi(extraction(hms, ',', 1)),
      .second_mask = false,
      .second = atoi(extraction(hms, ',', 2))
    };
    #if(BASH_DBG)
      DBG_String("ALARM "); DBG_String(" set-"); DBG_String(argv[1]);
      DBG_Enter();
    #endif
    RTC_Alarm_Enable(ab, &alarm);
    return;
  }
  #if(BASH_DBG)
  if(argc == 2) {
    RTC_Alarm_t alarm = RTC_Alarm(ab);
    DBG_String("ALARM "); DBG_Alarm(&alarm);
    DBG_Enter();
    return;
  }
  #endif
}

#endif
//------------------------------------------------------------------------------------------------- PWR

static void BASH_Pwr(char **argv, uint16_t argc)
{
  if(!strcmp(argv[1], "sleep")) {
    uint8_t mode = atoi(argv[1]);
    if(argc == 3) {
      if(bash_state.Sleep) bash_state.Sleep(mode);
      else PWR_Sleep(mode);
    }
    else if(!strcmp(argv[2], "now") && argc == 4) PWR_Sleep(mode);
    return;
  }
  if(!strcmp(argv[1], "reset")) {
    if(argc == 2) {
      if(bash_state.Reset) bash_state.Reset();
      else PWR_Reset();
    }
    else if(!strcmp(argv[2], "now") && argc == 3) PWR_Reset();
    return;
  }
}

//------------------------------------------------------------------------------------------------- LOOP

bool BASH_Loop(STREAM_t *stream)
{
  char **argv = NULL;
  uint16_t argc = STREAM_Read(stream, &argv);
  if(argc) {
    if(stream->mode == STREAM_Mode_Data) BASH_Data((uint8_t *)argv[0], argc, stream);
    else {
      #if(BASH_DBG)
        if(!strcmp(argv[0], "ping") && argc == 1) {
          DBG_String("PING pong");
          DBG_Enter();
        }
        else if(!strcmp(argv[0], "enter") && argc == 1) {
          DBG_Enter();
        }
        else
      #endif
      if(!strcmp(argv[0], "file") && bash_state.file_count) BASH_File(argv, argc, stream);
      else if(!strcmp(argv[0], "uid")) BASH_Uid(argv, argc);
      #if(BASH_RTC)
        else if(!strcmp(argv[0], "rtc")) BASH_Rtc(argv, argc);
        else if(!strcmp(argv[0], "alarm")) BASH_Alarm(argv, argc);
      #endif
      else if(!strcmp(argv[0], "pwr")) BASH_Pwr(argv, argc);
      #if(STREAM_ADDRESS)
        else if(!strcmp(argv[0], "addr")) {
          if(argc == 2) {
            stream->address = atoi(argv[1]);
            if(stream->Readdress) stream->Readdress(stream->address);
          }
          DBG_String("ADDR ");
          DBG_uDec(stream->address);
          DBG_Enter();
        }
      #endif
      else {
        for(uint8_t i = 0; i < bash_state.callback_count; i++) {
          if(bash_state.callback[i](argv, argc)) break;
        }
      }
    }
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------------------------
