#ifndef ADC_H_
#define ADC_H_
//-------------------------------------------------------------------------------------------------

#include "int.h"
#include "gpio.h"
#include "extdef.h"
#include "vrts.h"
#include "main.h"

#ifndef ADC_RECORD
  #define ADC_RECORD 1
#endif

#if(ADC_RECORD)
  #include "tim.h"
#endif

#define adc_record_buffer_size(time_ms, sample_time, oversampling, channel_count) \
  (uint16_t)(channel_count * (time_ms * 16000 / sample_time / oversampling / channel_count))

#define resistor_divider_factor(vcc, up, down, resolution) \
  ((float)vcc * ((float)up + down) / down / ((1 << resolution) - 1))

#define volts_factor(vcc, resolution) \
  ((float)vcc / (float)((1 << resolution) - 1))

//-------------------------------------------------------------------------------------------------

typedef enum {
  ADC_State_Free = 0,
  ADC_State_Measure = 1,
  ADC_State_Record = 2
} ADC_State_t;

typedef enum {
  ADC_IN_PA0 = 0,
  ADC_IN_PA1 = 1,
  ADC_IN_PA2 = 2,
  ADC_IN_PA3 = 3,
  ADC_IN_PA4 = 4,
  ADC_IN_PA5 = 5,
  ADC_IN_PA6 = 6,
  ADC_IN_PA7 = 7,
  ADC_IN_PB0 = 8,
  ADC_IN_PB1 = 9,
  ADC_IN_PB2 = 10,
  ADC_IN_PB10 = 11,
  ADC_IN_TSEN = 12,
  ADC_IN_VREFEN = 13,
  ADC_IN_VBATEN = 14,
  ADC_IN_PB11 = 15,
  ADC_IN_PB12 = 16,
  ADC_IN_PC4 = 17,
  ADC_IN_PC5 = 18
} ADC_IN_t;

typedef enum {
  ADC_OversamplingRatio_2 = 0,
  ADC_OversamplingRatio_4 = 1,
  ADC_OversamplingRatio_8 = 2,
  ADC_OversamplingRatio_16 = 3,
  ADC_OversamplingRatio_32 = 4,
  ADC_OversamplingRatio_64 = 5,
  ADC_OversamplingRatio_128 = 6,
  ADC_OversamplingRatio_256 = 7
} ADC_OversamplingRatio_t;

typedef enum {
  ADC_SamplingTime_14 = 0,
  ADC_SamplingTime_16 = 1,
  ADC_SamplingTime_20 = 2,
  ADC_SamplingTime_25 = 3,
  ADC_SamplingTime_32 = 4,
  ADC_SamplingTime_52 = 5,
  ADC_SamplingTime_92 = 6,
  ADC_SamplingTime_173 = 7
} ADC_SamplingTime_t;

typedef enum {
  ADC_Prescaler_1 = 0,
  ADC_Prescaler_2 = 1,
  ADC_Prescaler_4 = 2,
  ADC_Prescaler_6 = 3,
  ADC_Prescaler_8 = 4,
  ADC_Prescaler_10 = 5,
  ADC_Prescaler_12 = 6,
  ADC_Prescaler_16 = 7,
  ADC_Prescaler_32 = 8,
  ADC_Prescaler_64 = 9,
  ADC_Prescaler_128 = 10,
  ADC_Prescaler_256 = 11
} ADC_Prescaler_t;

//-------------------------------------------------------------------------------------------------

typedef struct {
  bool enable;
  ADC_OversamplingRatio_t ratio;
  uint8_t shift; // 0-8 [bit]
} ADC_Oversampling_t;

typedef struct {
  uint8_t *channels;
  uint8_t count;
  uint16_t *output;
  ADC_Prescaler_t prescaler;
  ADC_SamplingTime_t sampling_time;
  ADC_Oversampling_t oversampling;
  uint8_t active;
} ADC_Measurements_t;

#if(ADC_RECORD)
typedef struct {
  uint8_t *channels;
  uint8_t count;
  DMA_Nbr_t dma_nbr;
  ADC_Prescaler_t prescaler;
  ADC_SamplingTime_t sampling_time;
  ADC_Oversampling_t oversampling;
  uint16_t *buffer;
  uint16_t buffer_length;
  TIM_t *tim;
  DMA_t dma;
} ADC_Record_t;
#endif

typedef struct {
  INT_Prioryty_t int_prioryty;
  bool freq_16Mhz;
  ADC_Prescaler_t prescaler;
  ADC_Measurements_t measure;
  #if(ADC_RECORD)
    ADC_Record_t record;
  #endif
  uint16_t overrun;
  volatile uint8_t busy;
} ADC_t;

//-------------------------------------------------------------------------------------------------

uint8_t ADC_Measure(ADC_t *adc);
uint8_t ADC_Record(ADC_t *adc);
float ADC_RecordSampleTime_s(ADC_t *adc);

bool ADC_IsBusy(ADC_t *adc);
bool ADC_IsFree(ADC_t *adc);
void ADC_Wait(ADC_t *adc);
void ADC_Stop(ADC_t *adc);
void ADC_Enable(void);
void ADC_Disable(void);

void ADC_Init(ADC_t *adc);

//-------------------------------------------------------------------------------------------------

extern const uint32_t AdcFrequencyTab[];
extern const uint8_t AdcSamplingTimeTab[];
extern const uint16_t AdcOversamplingRatioTab[];

//-------------------------------------------------------------------------------------------------
#endif
