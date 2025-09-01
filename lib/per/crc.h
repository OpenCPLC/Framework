#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx.h"
#include "exdef.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef CRC_STANDARD_USED
  #define CRC_STANDARD_USED 1
#endif

typedef struct {
  uint8_t width;
  uint32_t polynomial;
  uint32_t initial;
  uint8_t reflect_data_in;
  bool reflect_data_out;
  uint32_t final_xor;
  bool invert_out;
} CRC_t;

uint32_t CRC_Run(const CRC_t *crc, void *buffer, uint16_t count);
uint16_t CRC_Append(const CRC_t *crc, uint8_t *buffer, uint16_t count);
state_t CRC_Error(const CRC_t *crc, uint8_t *buffer, uint16_t count);
state_t CRC_Ok(const CRC_t *crc, uint8_t *buffer, uint16_t count);

#if(CRC_STANDARD_USED)
  extern const CRC_t crc32_iso;
  extern const CRC_t crc32_aixm;
  extern const CRC_t crc32_autosar;
  extern const CRC_t crc32_cksum;
  extern const CRC_t crc16_kermit;
  extern const CRC_t crc16_modbus;
  extern const CRC_t crc16_buypass;
  extern const CRC_t crc8_maxim;
  extern const CRC_t crc8_smbus;
#endif

//-------------------------------------------------------------------------------------------------
#endif
