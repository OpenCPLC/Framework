#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include "spi.h"
#include "main.h"

#ifndef SPI_SOFTWARE_ENABLE
  #define SPI_SOFTWARE_ENABLE 0
#endif

//-------------------------------------------------------------------------------------------------

typedef struct {
  SPI_TypeDef *spi_typedef;
  DMA_Channel_e tx_dma_channel;
  DMA_Channel_e rx_dma_channel;
  INT_Level_e interrupt_level;
  SPI_MISO_e miso_pin;
  SPI_MOSI_e mosi_pin;
  SPI_SCK_e sck_pin;
  GPIO_t *cs_gpio;
  uint32_t cs_delay_ms;
  SPI_Prescaler_e prescaler;
  bool lsb;
  bool cpol;
  bool cpha;
  DMA_Channel_TypeDef *_tx_dma;
  DMAMUX_Channel_TypeDef *_tx_dmamux;
  DMA_Channel_TypeDef *_rx_dma;
  DMAMUX_Channel_TypeDef *_rx_dmamux;
  uint8_t const_reg;
  bool _busy_flag;
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
