#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#include <string.h>
#include "irq.h"
#include "i2c.h"
#include "heap.h"
#include "extdef.h"
#include "main.h"

//------------------------------------------------------------------------------------------------

#ifndef I2C_DMA_TX
  #define I2C_DMA_TX 0
#endif

#ifndef I2C_DMA_RX
  #define I2C_DMA_RX 0
#endif

//------------------------------------------------------------------------------------------------

typedef struct {
  I2C_TypeDef *reg;
  I2C_SCL_e scl_pin;
  I2C_SDA_e sda_pin;
  bool pull_up;
  IRQ_Priority_t int_prioryty;
  uint32_t timing;
  uint8_t filter; // Digital noise filter (0..15)
  #if(I2C_DMA_TX)
    uint8_t tx_dma_channel;
    DMA_Channel_TypeDef *tx_dma;
    DMAMUX_Channel_TypeDef *tx_dmamux;
  #endif
  #if(I2C_DMA_RX)
    uint8_t rx_dma_channel;
    DMA_Channel_TypeDef *rx_dma;
    DMAMUX_Channel_TypeDef *rx_dmamux;
  #endif
  volatile bool busy;
  uint8_t *tx_buffer;
  #if(!I2C_DMA_TX || !I2C_DMA_RX)
    volatile uint8_t *tx_location;
    volatile uint16_t tail;
    uint16_t head;
  #endif
  uint8_t address;
  uint8_t *rx_location;
  uint16_t size;
} I2C_Master_t;

void I2C_Master_Init(I2C_Master_t *i2c);
void I2C_Master_Disable(I2C_Master_t *i2c);
bool I2C_Master_IsBusy(I2C_Master_t *i2c);
bool I2C_Master_IsFree(I2C_Master_t *i2c);
status_t I2C_Master_Write(I2C_Master_t *i2c, uint8_t addr, uint8_t *ary, uint16_t n);
status_t I2C_Master_Read(I2C_Master_t *i2c, uint8_t addr, uint8_t *ary, uint16_t n);
status_t I2C_Master_WriteReg(I2C_Master_t * i2c, uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n);
status_t I2C_Master_ReadReg(I2C_Master_t * i2c, uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n);
status_t I2C_Master_WriteRead(I2C_Master_t *i2c, uint8_t addr, uint8_t *write_ary, uint16_t write_n, uint8_t *read_ary, uint16_t read_n);

//------------------------------------------------------------------------------------------------

#endif
