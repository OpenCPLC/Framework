#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

#include <string.h>
#include "int.h"
#include "i2c.h"
#include "main.h"

typedef struct {
  I2C_TypeDef *reg;
  I2C_SCL_e scl_pin;
  I2C_SDA_e sda_pin;
  uint8_t addr;
  uint8_t *regmap;
  bool *write_mask;
  bool update_global_flag;
  bool *update_flag;
  uint16_t regmap_size;
  uint16_t i;
  bool sequence;
  bool pull_up;
  uint8_t interrupt_level;
  uint32_t timing;
  uint8_t filter; // Digital noise filter (0..15)
  bool _start;
  


  // volatile bool busy;
  // uint8_t *tx_buffer;
  // #if(!I2C_DMA_TX || !I2C_DMA_RX)
  //   volatile uint8_t *tx_location;
  //   volatile uint16_t tail;
  //   uint16_t head;
  // #endif
  // uint8_t address;
  // uint8_t *rx_location;
  // uint16_t size;
} I2C_Slave_t;

bool I2C_Slave_IsUpdate(I2C_Slave_t *i2c);
void I2C_Slave_Init(I2C_Slave_t *i2c);

#endif
