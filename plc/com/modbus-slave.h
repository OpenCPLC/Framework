#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

#include "uart.h"
#include "modbus.h"
#include "crc.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  MODBUS_Status_FrameForMe = 0,
  MODBUS_Status_UartBusy = 1,
  MODBUS_Status_NoFrame = 2,
  MODBUS_Status_FrameTooShort = 3,
  MODBUS_Status_FrameNotForMe = 4,
  MODBUS_Status_ErrorCRC = 5,
  MODBUS_Status_FrameWrongSize = 6,
  MODBUS_Status_ErrorUartSend = 7,
} MODBUS_Status_e;

#define MODBUS_STATUS_ERROR(status) status >= MODBUS_Status_ErrorCRC

typedef struct {
  UART_t *uart;
  uint8_t address;
  uint16_t *regmap;
  bool *write_mask; // Ustaw 1 jeżeli pozwala na wpisywanie do danego rejestru
  bool update_global_flag;
  bool *update_flag; // Ustawia 1, gdy wartość została odświerzona
  uint16_t regmap_size;
  uint8_t *buffer_tx;
  uint8_t *buffer_rx;
} MODBUS_Slave_t;

MODBUS_Status_e MODBUS_Loop(MODBUS_Slave_t *modbus);
bool MODBUS_IsUpdate(MODBUS_Slave_t *modbus);

//-------------------------------------------------------------------------------------------------
#endif