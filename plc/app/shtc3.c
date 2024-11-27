#include "shtc3.h"

//-------------------------------------------------------------------------------------------------

const CRC_t shtc3_crc = {
  .width = 8,
  .polynomial = 0x31,
  .initial = 0xFF,
  .reflect_data_in = false,
  .reflect_data_out = false,
  .final_xor = 0x00,
  .invert_out = false
};

//-------------------------------------------------------------------------------------------------

static bool SHTC3_SendCommand(SHTC3_t *shtc3, SHTC3_CMD_t cmd)
{
	shtc3->buff_tx[0] = (uint8_t)(cmd >> 8);
	shtc3->buff_tx[1] = (uint8_t)(cmd);
	return TWI_Write(SHTC3_ADRRESS, shtc3->buff_tx, 2);
}

static bool SHTC3_WakeUp(SHTC3_t *shtc3)
{
  bool ok = SHTC3_SendCommand(shtc3, SHTC3_CMD_WakuUp);
  if(!ok) return false;
  delay(20);
  return true;
}

static bool SHTC3_SetMode(SHTC3_t *shtc3)
{
  return SHTC3_SendCommand(shtc3, shtc3->mode);
}

static bool SHTC3_Read(SHTC3_t *shtc3)
{
  return TWI_Read(SHTC3_ADRRESS, shtc3->buff_rx, 6);
}

static bool SHTC3_Sleep(SHTC3_t *shtc3)
{
  return SHTC3_SendCommand(shtc3, SHTC3_CMD_Sleep);
}

static bool SHTC3_Calculate(SHTC3_t *shtc3)
{
  if((uint8_t)CRC_Run(&shtc3_crc, &shtc3->buff_rx[0], 2) != shtc3->buff_rx[2]) return false;
	if((uint8_t)CRC_Run(&shtc3_crc, &shtc3->buff_rx[3], 2) != shtc3->buff_rx[5]) return false;
	shtc3->raw.humidity = 10000 * (((int32_t) shtc3->buff_rx[3] << 8 | shtc3->buff_rx[4])) / 65536;
	shtc3->raw.temperature = (17500 * (((int32_t) shtc3->buff_rx[0] << 8 | shtc3->buff_rx[1])) / 65536) - 4500;
	return true;
}

SHTC3_t SHTC3 = {
  .mode = SHTC3_Mode_NormalClockEnabled,
  .oversampling = 10
};

state_t SHTC3_Loop(void)
{
  // jak licznik przegnie pałe NAN

  

  if(TWI_IsBusy()) return BUSY;
  SHTC3_t *shtc3 = &SHTC3;
  float humidity = 0;
  float temperature = 0;
  if(!SHTC3_WakeUp(shtc3)) return ERR;
  if(!SHTC3_SetMode(shtc3)) return ERR;
  uint16_t ovrs = shtc3->oversampling ? shtc3->oversampling : 1;
  for(uint16_t i; i < ovrs; i++) {
    if(!SHTC3_Read(shtc3)) return ERR;
    if(!SHTC3_Calculate(shtc3)) return ERR;
    humidity += (float)shtc3->raw.humidity;
    temperature += (float)shtc3->raw.temperature;
  }
  if(!SHTC3_Sleep(shtc3)) return ERR;
  shtc3->humidity = (humidity + 0.5) / ovrs;
  shtc3->temperature = (temperature + 0.5) / ovrs;
  // resetuj licznik
  return OK;
}

float SHTC3_Temperature_C(SHTC3_t *shtc3)
{
  return SHTC3.temperature;
}

float SHTC3_Humidity_RH(SHTC3_t *shtc3)
{
  return SHTC3.humidity;
}

//-------------------------------------------------------------------------------------------------
