#include "bash.h"

//------------------------------------------------------------------------------------------------- STRUCT

static struct {
  FILE_t *files[BASH_FILE_LIMIT];
  uint32_t files_hash[BASH_FILE_LIMIT];
  uint8_t files_count;
  void (*callbacks[BASH_CALLBACK_LIMIT])(char **, uint16_t);
  uint32_t callbacks_hash[BASH_CALLBACK_LIMIT];
  uint8_t callbacks_count;
  bool flash_autosave;
  FILE_t *file_active;
  void (*Sleep)(PWR_SleepMode_e);
  void (*Reset)(void);
} bash;

/**
 * @brief Dodaje plik do systemu BASH.
 * @param file Wskaźnik do struktury pliku do dodania.
 */
void BASH_AddFile(FILE_t *file)
{
  if(bash.files_count >= BASH_FILE_LIMIT) {
    LOG_Error("BASH Exceeded file limit (max %u)", BASH_FILE_LIMIT);
    return;
  }
  bash.files[bash.files_count] = file;
  char *name = strcopy(file->name);
  bash.files_hash[bash.files_count] = hash(str2lower_this(name));
  if(!bash.files_count) {
    bash.file_active = file;
  }
  bash.files_count++;
  FILE_Flash_Load(file);
}

/**
 * @brief Dodaje nową komendę do systemu BASH, powiązaną z funkcją callback.
 * @param callback Funkcja obsługująca komendę
 * @param argv0 Nazwa komendy, przypisywana do funkcji callback.
 */
void BASH_AddCallback(void (*callback)(char **, uint16_t), char *argv0)
{
  if(bash.callbacks_count >= BASH_CALLBACK_LIMIT) {
    LOG_Error("BASH Exceeded callback limit (max %u)", BASH_CALLBACK_LIMIT);
    return;
  }
  bash.callbacks[bash.callbacks_count] = callback;
  char *argv = strcopy(argv0);
  bash.callbacks_hash[bash.callbacks_count] = hash(str2lower_this(argv));
  bash.callbacks_count++;
}

/**
 * @brief Włącza lub wyłącza automatyczne zapisywanie danych do pamięci flash.
 * @param autosave Flaga włączająca (true) lub wyłączająca (false) autosave.
 */
void BASH_FlashAutosave(bool autosave)
{
  bash.flash_autosave = autosave;
}

/**
 * @brief Wyświetla ostrzeżenie o nieprawidłowej liczbie argumentów dla danej komendy.
 * @param command Nazwa komendy, która została wywołana z błędną liczbą argumentów.
 */
void BASH_WrongArgc(char *command)
{
  #if(LOG_COLORS)
    LOG_Warning("Command " ANSI_YELLOW "%s" ANSI_END " called with incorrect arguments count", command);
  #else
    LOG_Warning("Command %s called with incorrect arguments count", command);
  #endif
}

/**
 * @brief Wyświetla ostrzeżenie o nieprawidłowym argumencie dla danej komendy.
 * @param command Nazwa komendy, która została wywołana z błędnym argumentem.
 * @param nbr Numer argumentu, który jest nieprawidłowy.
 */
void BASH_WrongArgv(char *command, char nbr)
{
  #if(LOG_COLORS)
    LOG_Warning("Command " ANSI_YELLOW "%s" ANSI_END " received invalid argument " ANSI_BLUE "%u" ANSI_END, command, nbr);
  #else
    LOG_Warning("Command '%s' received invalid argument %u", command, nbr);
  #endif
}

//------------------------------------------------------------------------------------------------- DATA

static void BASH_Data(uint8_t *data, uint16_t size, STREAM_t *stream)
{
  stream->packages--;
  if(stream->packages) LOG_Bash("File %s transfer, packages left: %d ", (char *)bash.file_active->name, stream->packages);
  else LOG_Bash("File %s transfer end", (char *)bash.file_active->name);
  bash.file_active->mutex = false;
  FILE_Append(bash.file_active, data, size);
  if(!stream->packages) {
    STREAM_ArgsMode(stream);
    if(bash.flash_autosave) FILE_Flash_Save(bash.file_active);
  }
  else {
    bash.file_active->mutex = true;
  }
}

//------------------------------------------------------------------------------------------------- FILE

static FILE_t *BASH_FindFile(char *file_name)
{
  uint32_t file_hash = hash(file_name);
  for(uint8_t i = 0; i < bash.files_count; i++) {
    if(file_hash == bash.files_hash[i]) {
      return bash.files[i];
    }
  }
  #if(LOG_COLORS)
    LOG_Warning("File " ANSI_ORANGE "%s" ANSI_END " not exist", (char *)file_name);
  #else
    LOG_Warning("File %s not exist", (char *)file_name);
  #endif
  return NULL;
}

static void BASH_AccessDenied(FILE_t *file)
{
  LOG_Warning("File %s access denied", (char *)file->name);
}

static void BASH_File(char **argv, uint16_t argc, STREAM_t *stream)
{
  if(!bash.files_count) {
    LOG_Warning("No file added to bash");
    return;
  }
  if(argc < 2) { BASH_WrongArgc(argv[0]); return; }
  switch(hash(argv[1])) {
    case BASH_Hash_List: { // FILE list
      if(argc != 2) { BASH_WrongArgc(argv[0]); return; }
      const char *file_names[bash.files_count];
      for(uint16_t i = 0; i < bash.files_count; i++) {
        file_names[i] = bash.files[i]->name;
      }
      #if(LOG_COLORS)
        LOG_Bash("File list: %a"ANSI_GREY", "ANSI_END"%s", bash.files_count, file_names);
      #elif
        LOG_Bash("File list: %a, %s", bash.files_count, file_names);
      #endif
      break;
    }
    case BASH_Hash_Active:
    case BASH_Hash_Select: { // FILE select <file_name:str>
      if(argc != 3) { BASH_WrongArgc(argv[0]); return; }
      FILE_t *file = BASH_FindFile(argv[2]);
      if(!file) return;
      bash.file_active = file;
      LOG_Bash("File %s was selected", bash.file_active->name);
      break;
    }
    case BASH_Hash_Info: {  // FILE info
      if(argc != 2) { BASH_WrongArgc(argv[0]); return; }
      LOG_Bash("File %o", bash.file_active, &DBG_File);
      break;
    }
    case BASH_Hash_Clear: { // FILE clear
      if(argc != 2) { BASH_WrongArgc(argv[0]); return; }
      if(FILE_Clear(bash.file_active)) {
        BASH_AccessDenied(bash.file_active);
        return;
      }
      LOG_Bash("File %s is empty", bash.file_active->name);
      return;
    }
    case BASH_Hash_Save: { // FILE save <packages:uint16>?
      if(argc > 3) { BASH_WrongArgc(argv[0]); return; }
      if(FILE_Clear(bash.file_active)) BASH_AccessDenied(bash.file_active);
      else {
        uint16_t packages = 1;
        if(argc == 3) {
          if(str2uint16_fault(argv[2])) { BASH_WrongArgv(argv[0], 2); return; }
          packages = str2nbr(argv[2]);
        }
        STREAM_DataMode(stream);
        stream->packages = packages;
        bash.file_active->mutex = true;
        LOG_Bash("File %s save, packages: %d", bash.file_active->name, stream->packages);
      }
      break;
    }
    case BASH_Hash_Append: { // FILE append <packages:uint16>?
      if(argc > 3) { BASH_WrongArgc(argv[0]); return; }
      if(bash.file_active->mutex) BASH_AccessDenied(bash.file_active);
      else {
        STREAM_DataMode(stream);
        if(str2uint16_fault(argv[2])) { BASH_WrongArgv(argv[0], 2); return; }
        stream->packages = argc == 3 ? str2nbr(argv[2]) : 1;
        bash.file_active->mutex = true;
        LOG_Bash("File %s append, packages: %d", bash.file_active->name, stream->packages);
      }
      break;
    }
    case BASH_Hash_Load: { // FILE load <limit:uint16>? <offset:uint16>?
      if(argc > 4) { BASH_WrongArgc(argv[0]); return; }
      if(bash.file_active->mutex) BASH_AccessDenied(bash.file_active);
      uint16_t limit = bash.file_active->size;
      uint16_t offset = 0;
      if(argc >= 3) {
        if(str2uint16_fault(argv[2])) { BASH_WrongArgv(argv[0], 2); return; }
        limit = str2nbr(argv[2]);
      }
      if(argc == 4) {
        if(str2uint16_fault(argv[3])) { BASH_WrongArgv(argv[0], 3); return; }
        offset = str2nbr(argv[3]);
      }
      if(offset >= bash.file_active->size) offset = 0;
      if(limit + offset > bash.file_active->size) limit = bash.file_active->size - offset;
      DBG_Data(&bash.file_active->buffer[offset], limit);
      DBG_Enter();
      break;
    }
    case BASH_Hash_Flash: { // FILE flash {save|load}
      if(argc != 3) { BASH_WrongArgc(argv[0]); return; }
      switch(hash(argv[2])) {
        case BASH_Hash_Save:
          if(FILE_Flash_Save(bash.file_active)) LOG_Error("File %s flash save fault", bash.file_active->name);
          else LOG_Bash("File %s flash save success", bash.file_active->name);
          break;
        case BASH_Hash_Load: case BASH_Hash_Reset: 
          if(FILE_Flash_Load(bash.file_active)) LOG_Error("File %s flash load fault", bash.file_active->name);
          else LOG_Bash("File %s flash load success", bash.file_active->name);
          break;
        default: BASH_WrongArgv("file flash", 2);
      }
    }
    case BASH_Hash_Mutex: { // FILE mutex {set|rst}
      if(argc != 3) { BASH_WrongArgc("file mutex"); return; }
      switch(hash(argv[2])) {
        case BASH_Hash_Set: bash.file_active->mutex = true; break;
        case BASH_Hash_Rst: case BASH_Hash_Reset: bash.file_active->mutex = false; break;
        default: BASH_WrongArgv("file mutex", 2);
      }
      break;
    }
    case BASH_Hash_Copy: { // FILE copy {from|to} <file_name:str>
      if(argc != 4) { BASH_WrongArgc("file copy"); return; }
      FILE_t *file = BASH_FindFile(argv[3]);
      if(!file) return;
      switch(hash(argv[2])) {
        case BASH_Hash_To:
          if(FILE_Copy(file, bash.file_active)) LOG_Error("File copy fault");
          else LOG_Bash("File copy %s → %s success", bash.file_active->name, file->name);
          break;
        case BASH_Hash_From:
          if(FILE_Copy(bash.file_active, file)) LOG_Error("File copy fault");
          else LOG_Bash("File copy %s → %s success", file->name, bash.file_active->name);
          break;
        default: BASH_WrongArgv("file copy", 2);
      }
      break;
    }
    case BASH_Hash_Print: { // FILE print {int8|uint8|int16|uint16|int32|uint32|struce} <limit:uint16>? <offset:uint16>?
      LOG_Bash("%02a %d", 50, bash.file_active->buffer);
      break;
    }


    // 
    // TODO: FILE (struct) print <limit:uint16>? <offset:uint16>?
    default: {
      #if(LOG_COLORS)
        LOG_Error("File command doesn't support "ANSI_YELLOW"%s"ANSI_END" option", argv[1]);
      #else
        LOG_Error("File command doesn't support %s option", argv[1]);
      #endif
    }
  }
}

//------------------------------------------------------------------------------------------------- UID

static void BASH_Uid(char **argv, uint16_t argc)
{
  if(argc > 1) {
    BASH_WrongArgc(argv[0]);
    return;
  }
  uint8_t *uid = (uint8_t *)UID_BASE;
  LOG_Bash("UID %a%02x", 12, uid);
}

//------------------------------------------------------------------------------------------------- RTC
#ifdef RTC_H_

static RTC_Weekday_e RTC_Str2Weekday(const char *str)
{
  switch(hash(str)) {
    case RTC_Hash_Everyday: case RTC_Hash_Evd: case HASH_Number_0: return RTC_Weekday_Everyday;
    case RTC_Hash_Monday: case RTC_Hash_Mon: case HASH_Number_1: return RTC_Weekday_Monday;
    case RTC_Hash_Tuesday: case RTC_Hash_Tue: case HASH_Number_2: return RTC_Weekday_Tuesday;
    case RTC_Hash_Wednesday: case RTC_Hash_Wed: case HASH_Number_3: return RTC_Weekday_Wednesday;
    case RTC_Hash_Thursday: case RTC_Hash_Thu: case HASH_Number_4: return RTC_Weekday_Thursday;
    case RTC_Hash_Friday: case RTC_Hash_Fri: case HASH_Number_5: return RTC_Weekday_Friday;
    case RTC_Hash_Saturday: case RTC_Hash_Sat: case HASH_Number_6: return RTC_Weekday_Saturday;
    case RTC_Hash_Sunday: case RTC_Hash_Sun: case HASH_Number_7: return RTC_Weekday_Sunday;
    default: return RTC_Weekday_Error;
  }
}

static void BASH_Rtc(char **argv, uint16_t argc)
{
  switch(argc) {
    case 1: { // RTC
      RTC_Datetime_t dt = RTC_Datetime();
      LOG_Bash("RTC %o %s", &dt, &DBG_Datetime, (char *)rtc_weakdays[dt.week_day]);
      break;
    }
    case 2: { // RTC rst|<timestamp:uint32>
      uint32_t argv1_hash = hash(argv[1]);
      if(argv1_hash == BASH_Hash_Rst || argv1_hash == BASH_Hash_Reset) {
        RTC_Reset();
        LOG_Bash("RTC was reset");
        return;
      }
      if(str2uint64_fault(argv[1])) { BASH_WrongArgv(argv[0], 1); return; }
      uint64_t stamp = str2nbr64(argv[1]);
      LOG_Bash("RTC was set by timestamp");
      RTC_SetTimestamp(stamp);
      break;
    }
    case 3: { // RTC <date:str(YYYY-MM-DD)> <time:str(hh:mm:ss)>
      char *date = replace_char("\"/,:+-_", ',', argv[1]);
      char *year_str = extraction(date, ',', 0);
      char *month_str = extraction(date, ',', 1);
      char *day_str = extraction(date, ',', 2);
      if(str2uint16_fault(year_str) || str2uint16_fault(month_str) || str2uint16_fault(day_str)) {
        BASH_WrongArgv(argv[0], 1);
        return;
      }
      uint16_t year_nbr = str2nbr(year_str);
      if(year_nbr >= 2000) year_nbr -= 2000;
      uint16_t month_nbr = str2nbr(month_str);
      uint16_t day_nbr = str2nbr(day_str);
      if(year_nbr >= 100 || month_nbr == 0 || month_nbr > 12 || day_nbr == 0 || day_nbr > 31) {
        BASH_WrongArgv(argv[0], 1);
        return;
      }
      char *time = replace_char("\"/,:+-_", ',', argv[2]);
      char *hour_str = extraction(time, ',', 0);
      char *minute_str = extraction(time, ',', 1);
      char *second_str = extraction(time, ',', 2);
      if(str2uint16_fault(hour_str) || str2uint16_fault(minute_str) || str2uint16_fault(second_str)) {
        BASH_WrongArgv(argv[0], 2);
        return;
      }
      uint16_t hour_nbr = str2nbr(hour_str);
      if(hour_nbr == 24) hour_nbr = 0;
      uint16_t minute_nbr = str2nbr(minute_str);
      uint16_t second_nbr = str2nbr(second_str);
      if(hour_nbr >= 24 || minute_nbr >= 60 || second_nbr >= 60) {
        BASH_WrongArgv(argv[0], 2);
        return;
      }
      RTC_Datetime_t dt = {
        .year = year_nbr,
        .month = month_nbr,
        .month_day = day_nbr,
        .hour = hour_nbr,
        .minute = minute_nbr,
        .second = second_nbr
      };
      RTC_SetDatetime(&dt);
      LOG_Bash("RTC was set by datatime");
      break;
    }
    default: {
      BASH_WrongArgc(argv[0]);
      break;
    }
  }
}

static void BASH_Alarm(char **argv, uint16_t argc)
{
  RTC_ALARM_e alarm_type;
  char alarm_char;
  if(argc >= 2) {
    uint32_t argv1_hash = hash(argv[1]);
    switch(argv1_hash) {
      case BASH_Hash_A: alarm_type = RTC_ALARM_A; alarm_char = 'A'; break;
      case BASH_Hash_B: alarm_type = RTC_ALARM_B; alarm_char = 'B'; break;
      default:
        BASH_WrongArgv(argv[0], 1);
        return;
    }
  }
  switch(argc) {
    case 2: {
      RTC_Alarm_t alarm = RTC_Alarm(alarm_type);
      if(RTC_Alarm_IsEnabled(alarm_type)) return LOG_Bash("Alarm %c %o", alarm_char, &alarm, &DBG_Alarm);
      else return LOG_Bash("Alarm %c disabled", alarm_char);
      break;
    }
    case 4: {
      RTC_Weekday_e weekday = RTC_Str2Weekday(argv[2]);
      if(weekday == RTC_Weekday_Error) {
        BASH_WrongArgv(argv[0], 2);
        return;
      }
      bool weekday_mask = weekday ? false : true;
      char *time = replace_char("\"/,:+-_", ',', argv[3]);
      char *hour_str = extraction(time, ',', 0);
      char *minute_str = extraction(time, ',', 1);
      char *second_str = extraction(time, ',', 2);
      if(str2uint16_fault(hour_str) || str2uint16_fault(minute_str) || str2uint16_fault(second_str)) {
        BASH_WrongArgv(argv[0], 3);
        return;
      }
      uint16_t hour_nbr = str2nbr(hour_str);
      if(hour_nbr == 24) hour_nbr = 0;
      uint16_t minute_nbr = str2nbr(minute_str);
      uint16_t second_nbr = str2nbr(second_str);
      if(hour_nbr >= 24 || minute_nbr >= 60 || second_nbr >= 60) {
        BASH_WrongArgv(argv[0], 3);
        return;
      }
      RTC_Alarm_t alarm = {
        .week = true,
        .day_mask = weekday_mask,
        .day = weekday,
        .hour_mask = false,
        .hour = hour_nbr,
        .minute_mask = false,
        .minute = minute_nbr,
        .second_mask = false,
        .second = second_nbr
      };
      RTC_Alarm_Enable(alarm_type, &alarm);
      LOG_Bash("Alarm %c %o", alarm_char, &alarm, &DBG_Alarm);
      break;
    }
    default: {
      BASH_WrongArgc(argv[0]);
      break;
    }
  }
}

#endif
//------------------------------------------------------------------------------------------------- PWR

PWR_SleepMode_e PWR_Str2SleepMode(const char *str)
{
  switch(hash(str)) {
    case PWR_Hash_Stop0: case HASH_Number_0: return PWR_SleepMode_Stop0;
    case PWR_Hash_Stop: case PWR_Hash_Stop1: case HASH_Number_1: return PWR_SleepMode_Stop1;
    case PWR_Hash_StandbySram: case PWR_Hash_Standbysram: case HASH_Number_2: return PWR_SleepMode_StandbySRAM;
    case PWR_Hash_Standby: case HASH_Number_3: return PWR_SleepMode_Standby;
    case PWR_Hash_Shutdown: case HASH_Number_4: return PWR_SleepMode_Shutdown;
    default: return PWR_SleepMode_Error;
  }
}

static void BASH_Power(char **argv, uint16_t argc)
{
  if(argc == 1) { BASH_WrongArgc(argv[0]); return; }
  switch(hash(argv[1])) {
    case BASH_Hash_Sleep: {
      PWR_SleepMode_e mode = PWR_Str2SleepMode(argv[2]);
      if(mode == PWR_SleepMode_Error) { BASH_WrongArgv(argv[0], 2); return; }
      if(argc == 3) {
        if(bash.Sleep) bash.Sleep(mode);
        else PWR_Sleep(mode);
      }
      else if(argc == 4) {
        if(hash(argv[3]) == BASH_Hash_Now) PWR_Sleep(mode);
        else { BASH_WrongArgv(argv[0], 3); return; }
      }
      else { BASH_WrongArgc(argv[0]); return; }
      break;
    }
    case BASH_Hash_Reboot:
    case BASH_Hash_Restart:
    case BASH_Hash_Reset: {
      if(argc == 2) {
        if(bash.Reset) bash.Reset();
        else PWR_Reset();
      }
      else if(argc == 3) {
        if(hash(argv[2]) == BASH_Hash_Now) PWR_Reset();
        else { BASH_WrongArgv(argv[0], 2); return; }
      }
      else { BASH_WrongArgc(argv[0]); return; }
      break;
    }
    default: {
      BASH_WrongArgv(argv[0], 1);
      break;
    }
  }
}

//------------------------------------------------------------------------------------------------- Addr

#if(STREAM_ADDRESS)
static void BASH_Addr(char **argv, uint16_t argc)
{
  if(argc > 2) { BASH_WrongArgc(argv[0]); return; }
  if(argc == 2) {
    stream->address = atoi(argv[1]);
    if(stream->Readdress) stream->Readdress(stream->address);
  }
  LOG_Bash("ADDR %u", stream->address);
}
#endif

//------------------------------------------------------------------------------------------------- LOOP

/**
 * @brief Obsługuje komendy odczytane ze strumienia w systemie BASH.
 * Obsługuje wbudowane komendy oraz te dodane przez użytkownika.
 * @param stream Strumień wejściowy (np. UART) z komendami lub danymi.
 * @return `true` jeśli komenda została obsłużona, `false` w przypadku błędu lub nieznanej komendy.
 */
bool BASH_Loop(STREAM_t *stream)
{
  char **argv = NULL;
  uint16_t argc = STREAM_Read(stream, &argv);
  if(argc) {
    if(stream->data_mode) BASH_Data((uint8_t *)argv[0], argc, stream);
    else {
      uint32_t argv0_hash = hash(argv[0]);
      switch(argv0_hash) {
        case BASH_Hash_Ping: {
          if(argc == 1) LOG_Bash("PING pong");
          else BASH_WrongArgc(argv[0]);
          break;
        }
        case BASH_Hash_File: BASH_File(argv, argc, stream); break;
        case BASH_Hash_Uid: BASH_Uid(argv, argc); break;
        case BASH_Hash_Power: case BASH_Hash_Pwr: BASH_Power(argv, argc); break;
        #ifdef RTC_H_
          case BASH_Hash_Rtc: BASH_Rtc(argv, argc); break;
          case BASH_Hash_Alarm: BASH_Alarm(argv, argc); break;
        #endif
        #if(STREAM_ADDRESS)
          case BASH_Hash_Addr: break;
        #endif
        default: {
          for(uint8_t i = 0; i < bash.callbacks_count; i++) {
            if(argv0_hash == bash.callbacks_hash[i]) {
              bash.callbacks[i](argv, argc);
              return true;
            }
          }
          #if(LOG_COLORS)
            LOG_Warning("Command " ANSI_YELLOW "%s" ANSI_END " not found", argv[0]);
          #else
            LOG_Warning("Command %s not found", argv[0]);
          #endif
          return false;
        }
      }
    }
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------------------------
