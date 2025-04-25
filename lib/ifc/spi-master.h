#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include "spi.h"
#include "exdef.h"
#include "main.h"

#ifndef SPI_SOFTWARE_ENABLE
  #define SPI_SOFTWARE_ENABLE 0
#endif

#ifndef SPI_Delay
  #define SPI_Delay(value) delay(value)
#endif

//-------------------------------------------------------------------------------------------------

typedef struct {
  SPI_TypeDef *reg;
  DMA_Nbr_t tx_dma_channel;
  DMA_Nbr_t rx_dma_channel;
  INT_Prioryty_t int_prioryty;
  SPI_SCK_t sck_pin;
  SPI_MISO_t miso_pin;
  SPI_MOSI_t mosi_pin;
  GPIO_t *cs_gpio;
  uint32_t cs_delay;
  SPI_Prescaler_t prescaler;
  bool lsb;
  bool cpol;
  bool cpha;
  DMA_Channel_TypeDef *tx_dma;
  DMA_Channel_TypeDef *rx_dma;
  uint8_t const_reg;
  bool busy_flag;
} SPI_Master_t;

void SPI_Master_Init(SPI_Master_t *spi);
bool SPI_Master_IsBusy(SPI_Master_t *spi);
bool SPI_Master_IsFree(SPI_Master_t *spi);

void SPI_Master_OnlyRead(SPI_Master_t *spi, uint8_t *rx_buff, uint16_t n);
state_t SPI_Master_Run(SPI_Master_t *spi, uint8_t *rx_buff, uint8_t *tx_buff, uint16_t n);
state_t SPI_Master_Read(SPI_Master_t *spi, uint8_t addr, uint8_t *rx_buff, uint16_t n);
state_t SPI_Master_Write(SPI_Master_t *spi, uint8_t *tx_buff, uint16_t n);

//-------------------------------------------------------------------------------------------------
#if(SPI_SOFTWARE_ENABLE)

#ifndef SPI_SOFTWARE_LBS
  #define SPI_SOFTWARE_LBS 0
#endif
#ifndef SPI_SOFTWARE_CPOL
  #define SPI_SOFTWARE_CPOL 0
#endif
#ifndef SPI_SOFTWARE_CPHA
  #define SPI_SOFTWARE_CPHA 1
#endif

typedef struct {
  GPIO_t *cs;
  GPIO_t *sck;
  GPIO_t *miso;
  GPIO_t *mosi;
  uint32_t delay;
} SPI_Software_t;

void SPI_Software_Init(SPI_Software_t *spi);
void SPI_Software_Run(SPI_Software_t *spi, uint8_t *rx_buff, uint8_t *tx_buff, uint16_t n);

#endif
//-------------------------------------------------------------------------------------------------
#endif
