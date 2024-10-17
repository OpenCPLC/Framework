#include "stream.h"

//-------------------------------------------------------------------------------------------------

uint16_t STREAM_Read(STREAM_t *stream, char ***argv)
{
  uint16_t length = stream->Size();
  if(length) {
    char *buffer = stream->Read();
    #if(STREAM_ADDRESS)
      uint8_t address = *buffer;
      if(address != stream->address) return 0;
      buffer++;
      length--;
    #endif
    #if(STREAM_CRC)
      if(CRC_Error(stream->crc, buffer, length)) return 0;
      length -= stream_crc->width / 8;
    #endif
    #if(STREAM_DBG)
      DBG_String("STREAM "); DBG_String(stream->name);
    #endif
    if(stream->mode == STREAM_Mode_String) {
      buffer = trim(buffer);
      int argc = explode(argv, buffer, ' ');
      #if(STREAM_DBG)
        DBG_String(" mode:string");
      #endif
      if(stream->modify == STREAM_Modify_Lowercase || stream->modify == STREAM_Modify_Uppercase) {
        for(int i = 0; i < argc; i++) {
          if(stream->modify == STREAM_Modify_Lowercase) strtolower_this((*argv)[i]);
          else if(stream->modify == STREAM_Modify_Uppercase) strtoupper_this((*argv)[i]);
          #if(STREAM_DBG)
            DBG_String(" arg[");
            DBG_Dec(i);
            DBG_String("]:");
            DBG_String((*argv)[i]);
          #endif
        }
      }
      #if(STREAM_DBG)
        DBG_Enter();
      #endif
      return (uint8_t)argc;
    }
    else {
      #if(STREAM_DBG)
        DBG_String(" mode:file\r\n");
      #endif
      char **file;
      char *loc;
      file = new(sizeof(char*) + (length * sizeof(char)));
      loc = (char*)file + sizeof(char*);
      memcpy(loc, buffer, length);
      file[0] = loc;
      *argv = file;
      return length;
    }
  }
  return 0;
}

//-------------------------------------------------------------------------------------------------
