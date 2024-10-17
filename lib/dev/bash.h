#ifndef BASH_H_
#define BASH_H_

#include "stream.h"
#include "file.h"
#include "main.h"

#ifndef BASH_FILE_LIMIT
  #define BASH_FILE_LIMIT 12
#endif

#ifndef BASH_CALLBACK_LIMIT
  #define BASH_CALLBACK_LIMIT 12
#endif

#ifndef BASH_DBG
  #define BASH_DBG 1
#endif

#ifndef BASH_RTC
  #define BASH_RTC 1
#endif

//-------------------------------------------------------------------------------------------------

void BASH_AddFile(FILE_t *file);
void BASH_AddCallback(bool (*callback)(char **, uint16_t));
void BASH_SetFlashAutosave(void (*callback)(char **, uint16_t));
bool BASH_Loop(STREAM_t *stream);

//-------------------------------------------------------------------------------------------------
#endif
