#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include "spi.h"
#include "main.h"

#ifndef SPI_SOFTWARE_ENABLE
  #define SPI_SOFTWARE_ENABLE 0
#endif

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
#endif

//-------------------------------------------------------------------------------------------------

#if(SPI_SOFTWARE_ENABLE)
typedef struct {
  GPIO_t *cs;
  GPIO_t *sck;
  GPIO_t *miso;
  GPIO_t *mosi;
  uint32_t delay;
} SPI_Software_t;
#endif

//-------------------------------------------------------------------------------------------------

typedef enum {
	SPI_PS_2 = 0,
	SPI_PS_4 = 1,
	SPI_PS_8 = 2,
	SPI_PS_16 = 3,
	SPI_PS_32 = 4,
	SPI_PS_64 = 5,
	SPI_PS_128 = 6,
	SPI_PS_256 = 7,
} SPI_Master_Prescaler;

typedef struct {
  SPI_TypeDef *spi_typedef;
  uint8_t tx_dma_channel;
  uint8_t rx_dma_channel;
  uint8_t interrupt_level;
  SPI_MISO_e miso_pin;
  SPI_MOSI_e mosi_pin;
  SPI_SCK_e sck_pin;
  GPIO_t *cs_gpio;
  uint32_t cs_delay;
  SPI_Master_Prescaler prescaler;
  bool lsb;
  bool cpol;
  bool cpha;
  DMA_Channel_TypeDef *_tx_dma;
  DMAMUX_Channel_TypeDef *_tx_dmamux;
  DMA_Channel_TypeDef *_rx_dma;
  DMAMUX_Channel_TypeDef *_rx_dmamux;
  uint8_t _void_register;
  bool _busy_flag;
} SPI_Master_t;

//-------------------------------------------------------------------------------------------------

#if(SPI_SOFTWARE_ENABLE)
  void SPI_Software_Init(SPI_Software_t *spi);
  void SPI_Software_Run(SPI_Software_t *spi, uint8_t *rx_ary, uint8_t *tx_ary, uint16_t n);
#endif

void SPI_Master_Init(SPI_Master_t *spi);
bool SPI_Master_IsBusy(SPI_Master_t *spi);
bool SPI_Master_IsFree(SPI_Master_t *spi);

access_t SPI_Master_Run(SPI_Master_t *spi, uint8_t *rx_ary, uint8_t *tx_ary, uint16_t n);
access_t SPI_Master_Read(SPI_Master_t *spi, uint8_t *rx_ary, uint16_t n);
access_t SPI_Master_Write(SPI_Master_t *spi, uint8_t *tx_ary, uint16_t n);
access_t SPI_Master_ReadSequence(SPI_Master_t *spi, uint8_t reg, uint8_t *rx_ary, uint16_t n);

//-------------------------------------------------------------------------------------------------
#endif
