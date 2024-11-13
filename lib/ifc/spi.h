#ifndef SPI_H_
#define SPI_H_

#include "gpio.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
	SPI_Prescaler_2 = 0,
	SPI_Prescaler_4 = 1,
	SPI_Prescaler_8 = 2,
	SPI_Prescaler_16 = 3,
	SPI_Prescaler_32 = 4,
	SPI_Prescaler_64 = 5,
	SPI_Prescaler_128 = 6,
	SPI_Prescaler_256 = 7,
} SPI_Prescaler_e;

typedef enum {
  SPI1_MISO_Free, SPI1_MISO_PA6, SPI1_MISO_PA11, SPI1_MISO_PB4, SPI1_MISO_PD5,
  SPI2_MISO_PA3, SPI2_MISO_PA9, SPI2_MISO_PB2, SPI2_MISO_PB6, SPI2_MISO_PB14, SPI2_MISO_PC2, SPI2_MISO_PD3
} SPI_MISO_e;

typedef enum {
  SPI1_MOSI_Free, SPI1_MOSI_PA2, SPI1_MOSI_PA7, SPI1_MOSI_PA12, SPI1_MOSI_PB5, SPI1_MOSI_PD6,
  SPI2_MOSI_PA4, SPI2_MOSI_PA10, SPI2_MOSI_PB7, SPI2_MOSI_PB11, SPI2_MOSI_PB15, SPI2_MOSI_PC3, SPI2_MOSI_PD4
} SPI_MOSI_e;

typedef enum {
  SPI1_SCK_Free, SPI1_SCK_PA1, SPI1_SCK_PA5, SPI1_SCK_PB3, SPI1_SCK_PD8,
  SPI2_SCK_PA0, SPI2_SCK_PB8, SPI2_SCK_PB10, SPI2_SCK_PB13, SPI2_SCK_PD1
} SPI_SCK_e;

extern const GPIO_Map_t spi_sck_map[];
extern const GPIO_Map_t spi_miso_map[];
extern const GPIO_Map_t spi_mosi_map[];

//-------------------------------------------------------------------------------------------------
#endif
