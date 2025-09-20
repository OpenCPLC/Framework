#include "i2c-master.h"

//-------------------------------------------------------------------------------------------------

static inline void I2C_Master_ReadEV(I2C_Master_t *i2c)
{
  i2c->busy = 0;
  I2C_Master_Read(i2c, i2c->address, i2c->rx_location, i2c->size);
  i2c->size = 0;
}

static void I2C_Master_InterruptEV(I2C_Master_t *i2c)
{
  if((i2c->reg->CR1 & I2C_CR1_TCIE) && (i2c->reg->ISR & I2C_ISR_TC)) {
    i2c->reg->CR1 &= ~I2C_CR1_TCIE;
    if(i2c->size) I2C_Master_ReadEV(i2c);
  }
  #if(!I2C_DMA_TX)
  if((i2c->reg->CR1 & I2C_CR1_TXIE) && (i2c->reg->ISR & I2C_ISR_TXE)) {
    i2c->reg->TXDR = i2c->tx_location[i2c->tail];
    i2c->tail++;
    if(i2c->tail >= i2c->head) {
      i2c->reg->CR1 &= ~I2C_CR1_TXIE;
      if(i2c->size) I2C_Master_ReadEV(i2c);
      else {
        i2c->reg->CR1 |= I2C_CR1_STOPIE;
      }
    }
  }
  #endif
  #if(!I2C_DMA_RX)
  if((i2c->reg->CR1 & I2C_CR1_RXIE) && (i2c->reg->ISR & I2C_ISR_RXNE)) {
    i2c->rx_location[i2c->tail] = i2c->reg->RXDR;
    i2c->tail++;
    if(i2c->tail >= i2c->head) {
      i2c->reg->CR1 &= ~I2C_CR1_RXIE;
      i2c->reg->CR1 |= I2C_CR1_STOPIE;
    }
  }
  #endif
  if((i2c->reg->CR1 & I2C_CR1_STOPIE) && (i2c->reg->ISR & I2C_ISR_STOPF)) {
    i2c->reg->ICR |= I2C_ICR_STOPCF;
    i2c->reg->CR1 &= ~I2C_CR1_STOPIE;
    i2c->busy = 0;
    heap_free((void *)&i2c->tx_buffer);
    i2c->tx_buffer = 0;
  }
  if((i2c->reg->CR1 & I2C_CR1_NACKIE) && (i2c->reg->ISR & I2C_ISR_NACKF)) {
    i2c->reg->ICR |= I2C_ICR_NACKCF;
    i2c->reg->CR1 &= ~I2C_CR1_NACKIE;
    // TODO: error
  }
}

#if(I2C_DMA_TX || I2C_DMA_RX)
static void I2C_Master_InterruptDMA(I2C_Master_t *i2c)
{
  #if(I2C_DMA_TX)
  if(DMA1->ISR & (2 << (4 * (i2c->tx_dma_channel - 1)))) { // DMA_ISR_TCIF[i2c->Channel]
    DMA1->IFCR |= (2 << (4 * (i2c->tx_dma_channel - 1))); // DMA_IFCR_CTCIF[i2c->Channel]
    i2c->reg->CR1 |= I2C_CR1_STOPIE;
  }
  #endif
  #if(I2C_DMA_RX)
  if(DMA1->ISR & (2 << (4 * (i2c->rx_dma_channel - 1)))) { // DMA_ISR_TCIF[i2c->Channel]
    DMA1->IFCR |= (2 << (4 * (i2c->rx_dma_channel - 1))); // DMA_IFCR_CTCIF[i2c->Channel]
    i2c->reg->CR1 |= I2C_CR1_STOPIE;
  }
  #endif
}
#endif

//-------------------------------------------------------------------------------------------------

void I2C_Master_Init(I2C_Master_t *i2c)
{
  RCC_EnableI2C(i2c->reg);
  INT_EnableI2C(i2c->reg, i2c->int_prioryty, (void (*)(void *))&I2C_Master_InterruptEV, i2c);
  GPIO_AlternateInit(&i2c_scl_map[i2c->scl_pin], i2c->pull_up);
  GPIO_AlternateInit(&i2c_sdc_map[i2c->sda_pin], i2c->pull_up);
	i2c->reg->TIMINGR = i2c->timing;
	i2c->reg->CR1 &= ~I2C_CR1_DNF;
  #if(I2C_DMA_TX || I2C_DMA_RX)
	  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  #endif
  #if(I2C_DMA_TX)
	  i2c->reg->CR1 |= I2C_CR1_TXDMAEN;
	  i2c->tx_dma = (DMA_Channel_TypeDef *)(DMA1_BASE+8+(20*(i2c->tx_dma_channel-1)));
	  i2c->tx_dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+(4*(i2c->tx_dma_channel-1)));
	  i2c->tx_dma->CPAR = (uint32_t) &(i2c->reg->TXDR);
	  i2c->tx_dma->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
	  INT_EnableDMA(i2c->tx_dma_channel, i2c->int_prioryty, (void (*)(void *))&I2C_Master_InterruptDMA, i2c);
	  switch((uint32_t) i2c->reg) {
	    case (uint32_t) I2C1: i2c->tx_dmamux->CCR = (i2c->tx_dmamux->CCR & 0xFFFFFFC0) | 11; break;
	    case (uint32_t) I2C2: i2c->tx_dmamux->CCR = (i2c->tx_dmamux->CCR & 0xFFFFFFC0) | 13; break;
	  }
  #endif
  #if(I2C_DMA_RX)
	  i2c->reg->CR1 |= I2C_CR1_RXDMAEN;
	  i2c->rx_dma = (DMA_Channel_TypeDef *)(DMA1_BASE+8+(20*(i2c->rx_dma_channel-1)));
	  i2c->rx_dmamux = (DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+(4*(i2c->rx_dma_channel-1)));
	  i2c->rx_dma->CPAR = (uint32_t) &(i2c->reg->RXDR);
	  i2c->rx_dma->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE;
	  INT_EnableDMA(i2c->rx_dma_channel, i2c->interrupt_level, (void (*)(void *))&I2C_Master_InterruptDMA, i2c);
	  switch((uint32_t) i2c->reg) {
	    case (uint32_t) I2C1: i2c->rx_dmamux->CCR = (i2c->rx_dmamux->CCR & 0xFFFFFFC0) | 10; break;
	    case (uint32_t) I2C2: i2c->rx_dmamux->CCR = (i2c->rx_dmamux->CCR & 0xFFFFFFC0) | 12; break;
	  }
  #endif
	i2c->reg->CR1 |= I2C_CR1_PE | (i2c->filter << I2C_CR1_DNF_Pos);
  i2c->busy = false;
}

void I2C_Master_Disable(I2C_Master_t *i2c)
{
  i2c->busy = true;
  i2c->reg->CR1 &= ~(I2C_CR1_PE);
  RCC_DisableI2C(i2c->reg);
}

//------------------------------------------------------------------------------------------------- BASIC

bool I2C_Master_IsBusy(I2C_Master_t *i2c)
{
	return i2c->busy;
}

bool I2C_Master_IsFree(I2C_Master_t *i2c)
{
  return !(i2c->busy);
}

state_t I2C_Master_Write(I2C_Master_t *i2c, uint8_t addr, uint8_t *ary, uint16_t n)
{
	if(i2c->busy) return BUSY;
  #if(I2C_DMA_TX)
    i2c->tx_dma->CCR &= ~ DMA_CCR_EN;
    i2c->tx_dma->CMAR = (uint32_t) ary;
    i2c->tx_dma->CNDTR = n;
    i2c->tx_dma->CCR |= DMA_CCR_EN;
  #else
    i2c->tx_location = ary;
    i2c->tail = 1;
    i2c->head = n;
    i2c->reg->TXDR = ary[0];
    if(n == 1) i2c->reg->CR1 |= I2C_CR1_STOPIE | I2C_CR1_NACKIE;
    else i2c->reg->CR1 |= I2C_CR1_TXIE | I2C_CR1_NACKIE;
  #endif
  i2c->reg->CR2 = I2C_CR2_AUTOEND | (n << 16) | (addr << 1) | I2C_CR2_START;
  i2c->busy = 1;
  return FREE;
}

state_t I2C_Master_Read(I2C_Master_t *i2c, uint8_t addr, uint8_t *ary, uint16_t n)
{
	if(i2c->busy) return BUSY;
  #if(I2C_DMA_RX)
    i2c->rx_dma->CCR &= ~ DMA_CCR_EN;
    i2c->rx_dma->CMAR = (uint32_t) ary;
    i2c->rx_dma->CNDTR = n;
    i2c->rx_dma->CCR |= DMA_CCR_EN;
  #else
    i2c->rx_location = ary;
    i2c->tail = 0;
    i2c->head = n;
    i2c->reg->CR1 |= I2C_CR1_RXIE;
  #endif
  i2c->reg->CR2 = I2C_CR2_AUTOEND | (n << 16) | I2C_CR2_RD_WRN | (addr << 1) | I2C_CR2_START;
  i2c->busy = 1;
  return FREE;
}

//-------------------------------------------------------------------------------------------------

state_t I2C_Master_WriteReg(I2C_Master_t *i2c, uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n)
{
	if(i2c->busy) return BUSY;
	i2c->tx_buffer = heap_alloc(n + 1);
  i2c->tx_buffer[0] = reg;
  memcpy(&i2c->tx_buffer[1], ary, n);
  return I2C_Master_Read(i2c, addr, i2c->tx_buffer, n + 1);
}

state_t I2C_Master_ReadReg(I2C_Master_t *i2c, uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n)
{
  if(i2c->busy) return BUSY;
  i2c->address = addr;
	i2c->rx_location = ary;
	i2c->size = n;
	i2c->reg->CR1 |= I2C_CR1_TCIE;
	i2c->reg->CR1 |= I2C_CR1_NACKIE;
	i2c->reg->CR2 = (1 << 16) | (addr << 1) | I2C_CR2_START;
	i2c->reg->TXDR = reg;
	i2c->busy = 1;
	return FREE;
}

//-------------------------------------------------------------------------------------------------

state_t I2C_Master_WriteRead(I2C_Master_t *i2c, uint8_t addr, uint8_t *write_ary, uint16_t write_n, uint8_t *read_ary, uint16_t read_n)
{
  if(i2c->busy) return BUSY;
  i2c->address = addr;
  i2c->rx_location = read_ary;
  i2c->size = read_n;
  return I2C_Master_Read(i2c, addr, write_ary, write_n);
}

//-------------------------------------------------------------------------------------------------
