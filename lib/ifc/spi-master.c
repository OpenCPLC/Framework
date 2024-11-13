#include "spi-master.h"

//-------------------------------------------------------------------------------------------------

static void SPI_Master_InterruptDMA(SPI_Master_t *spi)
{
  if(DMA1->ISR & (2 << (4 * (spi->rx_dma_channel - 1)))) {
    DMA1->IFCR |= (2 << (4 * (spi->rx_dma_channel - 1)));
    if(spi->cs_gpio) GPIO_Rst(spi->cs_gpio);
    spi->_busy_flag = false;
    if(!spi->mosi_pin) spi->spi_typedef->CR1 &= ~SPI_CR1_SPE;
  }
}

void SPI_Master_Init(SPI_Master_t *spi)
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  GPIO_AlternateInit(&spi_sck_map[spi->sck_pin], false);
  if(spi->miso_pin) {
    spi->_rx_dma = (DMA_Channel_TypeDef *)(DMA1_BASE + 8 + (20 * (spi->rx_dma_channel - 1)));
    spi->_rx_dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + (4 * (spi->rx_dma_channel - 1)));
    switch((uint32_t)spi->spi_typedef) {
      case (uint32_t)SPI1: spi->_rx_dmamux->CCR = (spi->_rx_dmamux->CCR & 0xFFFFFFC0) | 16; break;
      case (uint32_t)SPI2: spi->_rx_dmamux->CCR = (spi->_rx_dmamux->CCR & 0xFFFFFFC0) | 18; break;
    }
    INT_EnableDMA(spi->rx_dma_channel, spi->interrupt_level, (void (*)(void*))&SPI_Master_InterruptDMA, spi);
    GPIO_AlternateInit(&spi_miso_map[spi->miso_pin], false);
    spi->_rx_dma->CPAR = (uint32_t)&(spi->spi_typedef->DR);
    spi->_rx_dma->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE;
  }
  if(spi->mosi_pin) {
    spi->_tx_dma = (DMA_Channel_TypeDef *)(DMA1_BASE + 8 + (20 * (spi->tx_dma_channel - 1)));
    spi->_tx_dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE + (4 * (spi->tx_dma_channel - 1)));
    switch((uint32_t)spi->spi_typedef) {
      case (uint32_t)SPI1: spi->_tx_dmamux->CCR = (spi->_tx_dmamux->CCR & 0xFFFFFFC0) | 17; break;
      case (uint32_t)SPI2: spi->_tx_dmamux->CCR = (spi->_tx_dmamux->CCR & 0xFFFFFFC0) | 19; break;
    }
    GPIO_AlternateInit(&spi_mosi_map[spi->mosi_pin], false);
    spi->_tx_dma->CPAR = (uint32_t)&(spi->spi_typedef->DR);
    spi->_tx_dma->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
  }
  uint32_t cr2 = SPI_CR2_RXDMAEN | SPI_CR2_FRXTH | SPI_CR2_SSOE | 0x00000700;
  uint32_t cr1 = SPI_CR1_MSTR | (spi->lsb << 7) | (spi->prescaler << 3) | SPI_CR1_SPE | (spi->cpol << 1) | spi->cpha;
  if(spi->cs_gpio) {
    spi->cs_gpio->mode = GPIO_Mode_Output;
    GPIO_Init(spi->cs_gpio);
  }
  else cr2 |= SPI_CR1_SSM | SPI_CR1_SSI;
  if(spi->mosi_pin) cr2 |= SPI_CR2_TXDMAEN;
  else cr1 = SPI_CR1_RXONLY;
  RCC_EnableSPI(spi->spi_typedef);
  spi->spi_typedef->CR2 = cr2;
  spi->spi_typedef->CR1 = cr1;
}

//-------------------------------------------------------------------------------------------------

bool SPI_Master_IsBusy(SPI_Master_t *spi)
{
  return spi->_busy_flag;
}

bool SPI_Master_IsFree(SPI_Master_t *spi)
{
  return !(spi->_busy_flag);
}

static void SPI_Master_Start(SPI_Master_t *spi)
{
  spi->_tx_dma->CCR &= ~DMA_CCR_EN;
  spi->_rx_dma->CCR &= ~DMA_CCR_EN;
}

static void SPI_Master_End(SPI_Master_t *spi, uint16_t n)
{
  spi->_tx_dma->CNDTR = n;
  spi->_rx_dma->CNDTR = n;
  if(spi->cs_gpio) {
    GPIO_Set(spi->cs_gpio);
    delay(spi->cs_delay_ms);
  }
  spi->_rx_dma->CCR |= DMA_CCR_EN;
  spi->_tx_dma->CCR |= DMA_CCR_EN;
  spi->_busy_flag = true;
}

state_t SPI_Master_Run(SPI_Master_t *spi, uint8_t *rx_buff, uint8_t *tx_buff, uint16_t n)
{
  if(spi->_busy_flag) return BUSY;
  SPI_Master_Start(spi);
	spi->_tx_dma->CCR |= DMA_CCR_MINC;
	spi->_rx_dma->CCR |= DMA_CCR_MINC;
	spi->_tx_dma->CMAR = (uint32_t)tx_buff;
	spi->_rx_dma->CMAR = (uint32_t)rx_buff;
	SPI_Master_End(spi, n);
  return FREE;
}

void SPI_Master_OnlyRead(SPI_Master_t *spi, uint8_t *rx_buff, uint16_t n)
{
  spi->_rx_dma->CCR &= ~DMA_CCR_EN;
  spi->_rx_dma->CMAR = (uint32_t)rx_buff;
  spi->_rx_dma->CNDTR = n;
  spi->_rx_dma->CCR |= DMA_CCR_EN;
  spi->_busy_flag = true;
  spi->spi_typedef->CR1 |= SPI_CR1_SPE;
}

state_t SPI_Master_Read(SPI_Master_t *spi, uint8_t addr, uint8_t *rx_buff, uint16_t n)
{
  if(spi->_busy_flag) return BUSY;
  spi->const_reg = addr;
  if(!spi->mosi_pin) { SPI_Master_OnlyRead(spi, rx_buff, n); return 0; }
  SPI_Master_Start(spi);
  spi->_tx_dma->CCR &= ~DMA_CCR_MINC;
  spi->_rx_dma->CCR |= DMA_CCR_MINC;
  spi->_tx_dma->CMAR = (uint32_t)&(spi->const_reg);
  spi->_rx_dma->CMAR = (uint32_t)rx_buff;
  SPI_Master_End(spi, n);
  return FREE;
}

state_t SPI_Master_Write(SPI_Master_t *spi, uint8_t *tx_buff, uint16_t n)
{
  if(spi->_busy_flag) return BUSY;
  SPI_Master_Start(spi);
  spi->_tx_dma->CCR |= DMA_CCR_MINC;
  spi->_rx_dma->CCR &= ~DMA_CCR_MINC;
  spi->_tx_dma->CMAR = (uint32_t)tx_buff;
  spi->_rx_dma->CMAR = (uint32_t)&(spi->const_reg);
  SPI_Master_End(spi, n);
  return FREE;
}

//-------------------------------------------------------------------------------------------------
#if(SPI_SOFTWARE_ENABLE)

void SPI_Software_Init(SPI_Software_t *spi)
{
  spi->cs->mode = GPIO_Mode_Output;
  spi->sck->mode = GPIO_Mode_Output;
  spi->miso->mode = GPIO_Mode_Input;
  spi->mosi->mode = GPIO_Mode_Output;
  GPIO_InitList(spi->cs, spi->sck, spi->miso, spi->mosi, NULL);
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Set(spi->sck);
  #else
    GPIO_Rst(spi->sck);
  #endif
  GPIO_Rst(spi->cs);
}

static void SPI_Software_Write(GPIO_t *mosi, uint8_t *byte_tx)
{
  #if(SPI_SOFTWARE_LBS)
    if(*byte_tx & 1) GPIO_Set(mosi);
    else GPIO_Rst(mosi);
    *byte_tx >>= 1;
  #else
    if(*byte_tx & (1 << 7)) GPIO_Set(mosi);
    else GPIO_Rst(mosi);
    *byte_tx <<= 1;
  #endif
}

static void SPI_Software_Read(GPIO_t *miso, uint8_t *byte_rx)
{
  #if(SPI_SOFTWARE_LBS)
    *byte_rx >>= 1;
    if(GPIO_In(miso)) *byte_rx += (1 << 7);
  #else
    *byte_rx <<= 1;
    if(GPIO_In(miso)) *byte_rx += 1;
  #endif
}

void SPI_Software_Run(SPI_Software_t *spi, uint8_t *rx_buff, uint8_t *tx_buff, uint16_t n)
{
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Set(spi->sck);
  #else
    GPIO_Rst(spi->sck);
  #endif
  GPIO_Set(spi->cs);
  for(uint32_t i = 0; i < spi->delay; i++) {}
  uint8_t byte_tx, byte_rx;
  for(uint16_t i = 0; i < n; i++) {
    byte_tx = tx_buff[i];
    byte_rx = 0;
    for(uint8_t j = 0; j < 8; j++) {
      GPIO_Tgl(spi->sck); __NOP();
      #if(!SPI_SOFTWARE_CPHA)
        SPI_Software_Write(spi->mosi, &byte_tx);
        SPI_Software_Read(spi->mosi, &byte_rx);
      #endif
      GPIO_Tgl(spi->sck); __NOP();
      #if(SPI_SOFTWARE_CPHA)
        SPI_Software_Write(spi->mosi, &byte_tx);
        SPI_Software_Read(spi->mosi, &byte_rx);
      #endif
    }
    rx_buff[i] = byte_rx;
  }
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Rst(spi->sck);
  #else
    GPIO_Set(spi->sck);
  #endif
  GPIO_Rst(spi->cs);
}

#endif
//-------------------------------------------------------------------------------------------------