#ifndef SHTC3_H_
#define SHTC3_H_

#include <stdint.h>
#include <math.h>
#include "twi.h"

//-------------------------------------------------------------------------------------------------

#define SHTC3_ADRRESS 0x70

typedef enum {
	SHTC3_CMD_Sleep = 0xB098,
	SHTC3_CMD_WakuUp = 0x3517,
	SHTC3_Mode_NormalClockEnabled = 0x7CA2,
	SHTC3_Mode_NormalClockDisable = 0x7866,
	SHTC3_Mode_LowPowerClockEnabled = 0x6458,
	SHTC3_Mode_LowPowerClockDisable = 0x609C
} SHTC3_CMD_t;

typedef struct {
  int16_t humidity; // [%RH]
  int16_t temperature; // [°C]
} SHTC3_Raw_t;


typedef struct {
  uint16_t mode; // Tryp bracy Normal|Low-Powe, Clock Enable|Disable
	uint16_t oversampling; // Liczba próbek dla nadpróbkowania software'owego
  SHTC3_Raw_t raw;
  float humidity;
  float temperature;
	uint8_t buff_tx[2];
	uint8_t buff_rx[6];
} SHTC3_t;

state_t SHTC3_Measurement(SHTC3_t *shtc3, uint16_t oversampling);

//-------------------------------------------------------------------------------------------------
#endif