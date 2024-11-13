#include "adc.h"

//-------------------------------------------------------------------------------------------------

static void ADC_SetChannels(uint8_t *cha, uint8_t count)
{
  uint32_t chselr = 0;
  while(count) {
    chselr |= (1 << *cha);
    cha++; count--;
  }
  ADC1->CHSELR = chselr;
}

uint8_t ADC_Measurements(ADC_t *adc)
{
  if(!adc->_busy_flag) {
    adc->_busy_flag = ADC_StatusSingle;
    adc->measurements._active = 0;
    ADC1->CFGR2 =
        (adc->measurements.oversampling_shift << 5) |
        (adc->measurements.oversampling_ratio << 2) |
         adc->measurements.oversampling_enable;
    ADC1->SMPR = adc->measurements.sampling_time;
    ADC_SetChannels(adc->measurements.channels, adc->measurements.count);
    #if(ADC_RECORD)
      ADC1->CFGR1 &= ~(ADC_CFGR1_EXTEN);
      ADC1->CFGR1 |= ADC_CFGR1_CONT;
    #endif
    ADC1->IER |= ADC_IER_EOCIE;
    ADC1->CR |= ADC_CR_ADSTART;
    return OK;
  }
  return BUSY;
}

#if(ADC_RECORD)
uint8_t ADC_Record(ADC_t *adc)
{
  if(!adc->_busy_flag) {
    adc->_busy_flag = ADC_StatusRecord;
    ADC1->CFGR2 =
        (adc->record.oversampling_shift << 5) |
        (adc->record.oversampling_ratio << 2) |
         adc->record.oversampling_enable;
    ADC1->SMPR = adc->record.sampling_time;
    ADC_SetChannels(adc->record.channels, adc->record.count);
    adc->record._dma->CMAR = (uint32_t)(adc->record.buffer);
    adc->record._dma->CNDTR = (uint32_t)(adc->record.buffer_length);
    if(adc->record.tim) {
      TIM_Enable(adc->record.tim);
      ADC1->CFGR1 |= ADC_CFGR1_EXTEN;
      ADC1->CFGR1 &= ~ADC_CFGR1_CONT;
    } else {
      ADC1->CFGR1 &= ~(ADC_CFGR1_EXTEN);
      ADC1->CFGR1 |= ADC_CFGR1_CONT;
    }
    adc->record._dma->CCR |= DMA_CCR_EN;
    ADC1->CR |= ADC_CR_ADSTART;
    return FREE;
  }
  return BUSY;
}
#endif

//-------------------------------------------------------------------------------------------------

void ADC_Stop(ADC_t *adc)
{
  ADC1->CR |= ADC_CR_ADSTP;
  switch(adc->_busy_flag) {
    case ADC_StatusSingle: ADC1->IER &= ~ADC_IER_EOCIE; break;
    #if(ADC_RECORD)
      case ADC_StatusRecord:
        if(adc->record.tim) TIM_Disable(adc->record.tim);
        adc->record._dma->CCR &= ~DMA_CCR_EN;
        break;
    #endif
  }
  adc->_busy_flag = ADC_StatusFree;
}

bool ADC_IsBusy(ADC_t *adc)
{
  if(adc->_busy_flag) return true;
  return false;
}

bool ADC_IsFree(ADC_t *adc)
{
  if(adc->_busy_flag) return false;
  return true;
}

void ADC_Wait(ADC_t *adc)
{
  while(ADC_IsBusy(adc)) __DSB();
}

void ADC_Enabled(void)
{
  do {
    ADC1->CR |= ADC_CR_ADEN;
  } while((ADC1->ISR & ADC_ISR_ADRDY) == 0);
}

void ADC_Disabled(void)
{
  if((ADC1->CR & ADC_CR_ADSTART)) ADC1->CR |= ADC_CR_ADSTP;
  while((ADC1->CR & ADC_CR_ADSTP)) __DSB();
  ADC1->CR |= ADC_CR_ADDIS;
  while((ADC1->CR & ADC_CR_ADEN) != 0) __DSB();
}

//-------------------------------------------------------------------------------------------------

static void ADC_InterruptEV(ADC_t *adc)
{
  if(ADC1->ISR & ADC_ISR_OVR) {
    ADC1->ISR |= ADC_ISR_OVR;
    #if(INCLUDE_DBG)
      adc->overrun++;
    #endif
    ADC_Stop(adc);
  }
  else if(ADC1->ISR & ADC_ISR_EOC) {
    ADC1->ISR |= ADC_ISR_EOC;
    adc->measurements.output[adc->measurements._active] = ADC1->DR;
    adc->measurements._active++;
    if(adc->measurements._active >= adc->measurements.count) ADC_Stop(adc);
  }
}

#if(ADC_RECORD)
static void ADC_InterruptDMA(ADC_t *adc)
{
  if(DMA1->ISR & (2 << (4 * (adc->record.dma_channel - 1)))) {// DMA_ISR_TCIF[adc->dma_channel]
    DMA1->IFCR |= (2 << (4 * (adc->record.dma_channel - 1))); // DMA_IFCR_CTCIF[adc->dma_channel]
    ADC_Stop(adc);
    if(adc->record.tim) TIM_Disable(adc->record.tim);
  }
}
#endif

//-------------------------------------------------------------------------------------------------

static void ADC_InitGPIO(uint32_t *chselr, uint8_t *cha, uint8_t count)
{
  while(count) {
    *chselr |= (1 << *cha);
    if(*cha < 8) GPIOA->MODER |= (3 << (2 * *cha));
    else if(*cha < 11) GPIOB->MODER |= (3 << (2 * (*cha - 8)));
    else if(*cha == 11) GPIOB->MODER |= (3 << (2 * 10));
    else if(*cha == 12) ADC->CCR |= ADC_CCR_TSEN;
    else if(*cha == 13) ADC->CCR |= ADC_CCR_VREFEN;
    else if(*cha == 14) ADC->CCR |= ADC_CCR_VBATEN;
    else if(*cha < 17) GPIOB->MODER |= (3 << (2 * (*cha - 4)));
    else if(*cha < 19) GPIOC->MODER |= (3 << (2 * (*cha - 13)));
    cha++; count--;
  }
}

void ADC_Init(ADC_t *adc)
{
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;
  ADC1->CR |= ADC_CR_ADVREGEN;
  for(uint32_t i = 0; i < SystemCoreClock / 500000; i++) __DSB();
  ADC1->CR &= ~ADC_CR_ADEN;
  ADC1->CR |= ADC_CR_ADCAL;
  while(!(ADC1->ISR & ADC_ISR_EOCAL)) __DSB();
  ADC1->ISR |= ADC_ISR_EOCAL;
  #if(ADC_RECORD)
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
    adc->record._dma = (DMA_Channel_TypeDef *) (DMA1_BASE + 8 + (20 * (adc->record.dma_channel - 1)));
    adc->record._dmamux = (DMAMUX_Channel_TypeDef *) (DMAMUX1_BASE + (4 * (adc->record.dma_channel - 1)));
    adc->record._dmamux->CCR = (adc->record._dma->CCR & 0xFFFFFFC0) | 5;
    adc->record._dma->CPAR = (uint32_t)(&(ADC1->DR));
    adc->record._dma->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_TCIE;
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
    INT_EnableDMA(adc->record.dma_channel, adc->interrupt_level, (void (*)(void *))&ADC_InterruptDMA, adc);
  #endif
  ADC->CCR |= (adc->frequency << ADC_CCR_PRESC_Pos);
  uint32_t chselr = 0;
  ADC_InitGPIO(&chselr, adc->measurements.channels, adc->measurements.count);
  #if(ADC_RECORD)
    if(adc->record.channels) ADC_InitGPIO(&chselr, adc->record.channels, adc->record.count);
  #endif
  ADC1->CHSELR = chselr;
  ADC1->IER |= ADC_IER_OVRIE;
  INT_EnableADC(adc->interrupt_level, (void (*)(void *))&ADC_InterruptEV, adc);
  #if(ADC_RECORD)
    if(adc->record.tim) {
      switch((uint32_t)adc->record.tim->reg) {
        case (uint32_t)TIM1: ADC1->CFGR1 |= (0 << 6); break;
        case (uint32_t)TIM3: ADC1->CFGR1 |= (3 << 6); break;
        case (uint32_t)TIM6: ADC1->CFGR1 |= (5 << 6); break;
        case (uint32_t)TIM15: ADC1->CFGR1 |= (4 << 6); break;
      }
      adc->record.tim->interrupt_level = adc->interrupt_level;
      adc->record.tim->one_pulse_mode = false;
      adc->record.tim->enable = false;
      adc->record.tim->enable_interrupt = false;
      TIM_Init(adc->record.tim);
      TIM_MasterMode(adc->record.tim, TIM_MasterMode_Update);
    }
  #else
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;
    ADC1->CFGR1 |= ADC_CFGR1_CONT;
  #endif
  ADC_Enabled();
}

//-------------------------------------------------------------------------------------------------
