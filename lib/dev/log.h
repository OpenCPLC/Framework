#ifndef LOG_H_
#define LOG_H_

#include "dbg.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef LOG_ARYSEP_MAX
  #define LOG_ARYSEP_MAX 8
#endif

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

//-------------------------------------------------------------------------------------------------

extern bool LogPrintFlag;

void print(const char *template, ...);
void LOG_Init(const char *version, const char *board);
void LOG_Bash(const char *message, ...);
void LOG_Debug(const char *message, ...);
void LOG_Info(const char *message, ...);
void LOG_Warning(const char *message, ...);
void LOG_Error(const char *message, ...);
void LOG_Critical(const char *message, ...);
void LOG_Panic(const char *message);
void LOG_Message(LOG_Level_e lvl, char *message, ...);

//-------------------------------------------------------------------------------------------------
#endif