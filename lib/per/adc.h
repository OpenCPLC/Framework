#ifndef ADC_H_
#define ADC_H_
//-------------------------------------------------------------------------------------------------

#include "gpio.h"
#include "vrts.h"
#include "main.h"

#ifndef ADC_RECORD
  #define ADC_RECORD 1
#endif

#if(ADC_RECORD)
  #include "tim.h"
#endif

#define adc_record_buffer_size(time_ms, sample_time, oversampling, channel_count) \
  (uint16_t)(channel_count * (time_ms * 16000 / (sample_time + 12) / oversampling / channel_count))

#define resistor_divider_factor(vcc, up, down, resolution) \
  ((float)vcc * ((float)up + down) / down / ((1 << resolution) - 1))

#define volts_factor(vcc, resolution) \
  ((float)vcc / (float)((1 << resolution) - 1))

//-------------------------------------------------------------------------------------------------

typedef enum {
  ADC_StatusFree = 0,
  ADC_StatusSingle = 1,
  ADC_StatusRecord = 2
} ADC_Status_e;

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
} ADC_IN_e;

typedef enum {
  ADC_OversamplingRatio_2 = 0,
  ADC_OversamplingRatio_4 = 1,
  ADC_OversamplingRatio_8 = 2,
  ADC_OversamplingRatio_16 = 3,
  ADC_OversamplingRatio_32 = 4,
  ADC_OversamplingRatio_64 = 5,
  ADC_OversamplingRatio_128 = 6,
  ADC_OversamplingRatio_256 = 7
} ADC_OversamplingRatio_e;

typedef enum {
  ADC_SamplingTime_1 = 0,
  ADC_SamplingTime_3 = 1,
  ADC_SamplingTime_7 = 2,
  ADC_SamplingTime_12 = 3,
  ADC_SamplingTime_19 = 4,
  ADC_SamplingTime_39 = 5,
  ADC_SamplingTime_79 = 6,
  ADC_SamplingTime_160 = 7
} ADC_SamplingTime_e;

typedef enum {
  ADC_Frequency_16MHz = 0,  // Prescaler:1
  ADC_Frequency_8Mhz = 1, // Prescaler:2
  ADC_Frequency_4Mhz = 2, // Prescaler:4
  ADC_Frequency_2_7MHz = 3, // Prescaler:6
  ADC_Frequency_2Mhz = 4, // Prescaler:8
  ADC_Frequency_1_6MHz = 5, // Prescaler:10
  ADC_Frequency_1_3MHz = 6, // Prescaler:12
  ADC_Frequency_1MHz = 7, // Prescaler:16
  ADC_Frequency_500kHz = 8, // Prescaler:32
  ADC_Frequency_250kHz = 9, // Prescaler:64
  ADC_Frequency_125kHz = 10, // Prescaler:128
  ADC_Frequency_62_5kHz = 11 // Prescaler:256
} ADC_Frequency_e;

//-------------------------------------------------------------------------------------------------

typedef struct {
  uint8_t *channels; // TODO array
  uint8_t count;
  uint16_t *output; // TODO std output
  ADC_SamplingTime_e sampling_time;
  bool oversampling_enable;
  ADC_OversamplingRatio_e oversampling_ratio;
  uint8_t oversampling_shift; // 0-8 [bit]
  uint8_t _active;
} ADC_Measurements_t;

#if(ADC_RECORD)
typedef struct {
  uint8_t *channels;
  uint8_t count;
  TIM_t *tim;
  uint8_t dma_channel;
  ADC_SamplingTime_e sampling_time;
  bool oversampling_enable;
  ADC_OversamplingRatio_e oversampling_ratio;
  uint8_t oversampling_shift; // 0-8 [bit]
  uint16_t *buffer;
  uint16_t buffer_length;
  DMA_Channel_TypeDef* _dma;
  DMAMUX_Channel_TypeDef* _dmamux;
} ADC_Record_t;
#endif

typedef struct {
  uint8_t interrupt_level;
  ADC_Frequency_e frequency;
  ADC_Measurements_t measurements;
  #if(ADC_RECORD)
    ADC_Record_t record;
  #endif
  uint16_t overrun;
  uint8_t _busy_flag;
} ADC_t;

//-------------------------------------------------------------------------------------------------

uint8_t ADC_Measurements(ADC_t *adc);
uint8_t ADC_Record(ADC_t *adc);

bool ADC_IsBusy(ADC_t *adc);
bool ADC_IsFree(ADC_t *adc);
void ADC_Wait(ADC_t *adc);
void ADC_Stop(ADC_t *adc);
void ADC_Enabled(void);
void ADC_Disabled(void);

void ADC_Init(ADC_t *adc);

//-------------------------------------------------------------------------------------------------

extern const uint8_t(*ANA_Single_ErtsVector[])(ADC_t *adc);
extern const uint16_t ANA_Single_ErtsTime[];

#if(ADC_RECORD)
  extern const uint8_t(*ANA_Record_ErtsVector[])(ADC_t *adc);
  extern const uint16_t ANA_Record_ErtsTime[];
#endif

//-------------------------------------------------------------------------------------------------
#endif
