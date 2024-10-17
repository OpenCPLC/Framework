#ifndef I2C_H_
#define I2C_H_

#include "gpio.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#if(SYS_CLOCK_FREQ == 2000000)
  #define I2C_TIMING_100KHz timing = 0x00000509, .filter = 0
#elif(SYS_CLOCK_FREQ == 16000000)
  #define I2C_TIMING_100kHz timing = 0x00503D5A, .filter = 0
  #define I2C_TIMING_400kHz timing = 0x0010061A, .filter = 0
#elif(SYS_CLOCK_FREQ == 18432000)


#elif(SYS_CLOCK_FREQ == 48000000)
  #define I2C_TIMING_100KHz timing = 0x00C0D6FF, .filter = 0
  #define I2C_TIMING_400kHz timing = 0x00501855, .filter = 0
  #define I2C_TIMING_1MHz timing = 0x0020091C, .filter = 0
  #define I2C_TIMING_OVERCLOCK timing = 0x00100207, .filter = 0
#endif

//-------------------------------------------------------------------------------------------------

typedef enum {
  I2C1_SCL_PA9 = 1,
  I2C1_SCL_PB6 = 2,
  I2C1_SCL_PB8 = 3,
  I2C2_SCL_PA11 = 4,
  I2C2_SCL_PB10 = 5,
  I2C2_SCL_PB13 = 6,
} I2C_SCL_e;

typedef enum {
  I2C1_SDA_PA10 = 1,
  I2C1_SDA_PB7 = 2,
  I2C1_SDA_PB9 = 3,
  I2C2_SDA_PA12 = 4,
  I2C2_SDA_PB11 = 5,
  I2C2_SDA_PB14 = 6,
} I2C_SDA_e;

void I2C_Reset(I2C_TypeDef *i2c_typedef);

extern const GPIO_Map_t i2c_scl_map[];
extern const GPIO_Map_t i2c_sdc_map[];

//-------------------------------------------------------------------------------------------------
#endif
