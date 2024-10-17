#include "opencplc-dio.h"

//------------------------------------------------------------------------------------------------- EEPROM

#ifdef STM32G081xx
  EEPROM_t io_eeprom[] = {
    { .page_a = 60, .page_b = 61 },
    { .page_a = 58, .page_b = 59 },
    { .page_a = 56, .page_b = 57 },
    { .page_a = 54, .page_b = 55 },
  };
#endif
#ifdef STM32G0C1xx
  EEPROM_t io_eeprom[] = {
    { .page_a = 252, .page_b = 253 },
    { .page_a = 250, .page_b = 251 },
    { .page_a = 248, .page_b = 249 },
    { .page_a = 246, .page_b = 247 }
  };
#endif

//------------------------------------------------------------------------------------------------- DOUT-TO

PWM_t to149c_pwm = {
  .reg = TIM1,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 92,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM1_CH1_PA8,
  .channel[TIM_CH2] = TIM1_CH2_PA9,
  .channel[TIM_CH3] = TIM1_CH3_PC10,
  .channel[TIM_CH4] = TIM1_CH4_PC11,
  .auto_reload = 1000,
  .center_aligned = true
};

DOUT_t TO1 = { .name = "TO1", .pwm = &to149c_pwm, .channel = TIM_CH4, .eeprom = &io_eeprom[0], .save = false };
DOUT_t TO4 = { .name = "TO4", .pwm = &to149c_pwm, .channel = TIM_CH3, .eeprom = &io_eeprom[0], .save = false };
DOUT_t TO9 = { .name = "TO9", .pwm = &to149c_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[1], .save = false };
DOUT_t TO12 = { .name = "TO12", .pwm = &to149c_pwm, .channel = TIM_CH2, .eeprom = &io_eeprom[1], .save = false };

void TO149C_Frequency(float frequency)
{
  PWM_Frequency(&to149c_pwm, frequency);
};

PWM_t to2_pwm = {
  .reg = TIM14,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 92,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM14_CH1_PC12,
  .auto_reload = 1000,
  .center_aligned = false
};

DOUT_t TO2 = { .name = "TO2", .pwm = &to2_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[0], .save = false };

void TO2_Frequency(float frequency)
{
  PWM_Frequency(&to2_pwm, frequency);
};

PWM_t to3_pwm = {
  .reg = TIM17,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 92,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM17_CH1_PB9,
  .auto_reload = 1000,
  .center_aligned = false
};

DOUT_t TO3 = { .name = "TO3", .pwm = &to3_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[0], .save = false };

void TO3_Frequency(float frequency)
{
  PWM_Frequency(&to2_pwm, frequency);
};

PWM_t to6_pwm = {
  .reg = TIM16,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 92,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM16_CH1_PB8,
  .auto_reload = 1000,
  .center_aligned = false
};

DOUT_t TO6 = { .name = "TO6", .pwm = &to6_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[0], .save = false };

void TO6_Frequency(float frequency)
{
  PWM_Frequency(&to6_pwm, frequency);
};

PWM_t to7a_pwm = {
  .reg = TIM15,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 92,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM15_CH1_PB14,
  .channel[TIM_CH2] = TIM15_CH2_PB15,
  .auto_reload = 1000,
  .center_aligned = false
};

DOUT_t TO7 = { .name = "TO7", .pwm = &to7a_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[1], .save = false };
DOUT_t TO10 = { .name = "TO10", .pwm = &to7a_pwm, .channel = TIM_CH2, .eeprom = &io_eeprom[1], .save = false };

void TO7A_Frequency(float frequency)
{
  PWM_Frequency(&to7a_pwm, frequency);
};

DOUT_t TO5 = { .name = "TO5", .gpio = { .port = GPIOB, .pin = 4 }, .eeprom = &io_eeprom[0], .save = false };
DOUT_t TO8 = { .name = "TO8", .gpio = { .port = GPIOB, .pin = 13 }, .eeprom = &io_eeprom[1], .save = false };
DOUT_t TO11 = { .name = "TO11", .gpio = { .port = GPIOC, .pin = 6 }, .eeprom = &io_eeprom[1], .save = false };

void TO_Frequency(float frequency)
{
  TO149C_Frequency(frequency);
  TO2_Frequency(frequency);
  TO3_Frequency(frequency);
  TO6_Frequency(frequency);
  TO7A_Frequency(frequency);
}

//------------------------------------------------------------------------------------------------- DIN

EXTI_t din_trig3, din_trig4;

PWMI_t din_pwmi3 = {
  .reg = TIM3,
  .prescaler = 49,
  // .channel[TIM_CH1] = TIM3_CH1_PA6,
  // .channel[TIM_CH2] = TIM3_CH2_PA7,
  // .channel[TIM_CH3] = TIM3_CH3_PB0,
  // .channel[TIM_CH4] = TIM3_CH4_PB1,
  .input_prescaler = PWMI_Prescaler_1,
  .filter = TIM_Filter_NoFilter,
  .oversampling = 4
};

DIN_t DI12 = { .name = "DI12", .gpif = { .gpio = { .port = GPIOA, .pin = 6, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI11 = { .name = "DI11", .gpif = { .gpio = { .port = GPIOA, .pin = 7, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI10 = { .name = "DI10", .gpif = { .gpio = { .port = GPIOB, .pin = 0, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI9 = { .name = "DI9", .gpif = { .gpio = { .port = GPIOB, .pin = 1, .reverse = true } }, .eeprom = &io_eeprom[2] };

bool din_pwmi3_init = false;

PWMI_t din_pwmi2 = {
  .reg = TIM2,
  .prescaler = 49,
  // .channel[TIM_CH1] = TIM2_CH1_PA15,
  // .channel[TIM_CH2] = TIM2_CH2_PB3,
  .input_prescaler = PWMI_Prescaler_1,
  .filter = TIM_Filter_NoFilter,
  .oversampling = 4
};

DIN_t DI1 = { .name = "DI1", .gpif = { .gpio = { .port = GPIOA, .pin = 15, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI6 = { .name = "DI6", .gpif = { .gpio = { .port = GPIOB, .pin = 3, .reverse = true } }, .eeprom = &io_eeprom[2] };

bool din_pwmi2_init = false;

DIN_t DI2 = { .name = "DI2", .gpif = { .gpio = { .port = GPIOD, .pin = 3, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI3 = { .name = "DI3", .gpif = { .gpio = { .port = GPIOD, .pin = 2, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI4 = { .name = "DI4", .gpif = { .gpio = { .port = GPIOD, .pin = 1, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI5 = { .name = "DI5", .gpif = { .gpio = { .port = GPIOD, .pin = 0, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI7 = { .name = "DI7", .gpif = { .gpio = { .port = GPIOC, .pin = 1, .reverse = true } }, .eeprom = &io_eeprom[2] };
DIN_t DI8 = { .name = "DI8", .gpif = { .gpio = { .port = GPIOC, .pin = 2, .reverse = true } }, .eeprom = &io_eeprom[2] };

//------------------------------------------------------------------------------------------------- AIN

uint8_t ain_channels[] = {
  ADC_IN_PA2, ADC_IN_PA3, ADC_IN_PA5, ADC_IN_PC4, ADC_IN_PC5 // TODO: POT
};

uint16_t ain_output[sizeof(ain_channels)];

ADC_t ain_adc = {
  .prescaler = ADC_Prescaler_2,
  .interrupt_level = 3,
  .measurements = {
    .channels = ain_channels,
    .count = sizeof(ain_channels),
    .output = ain_output,
    .sampling_time = ADC_SamplingTime_160,
    .oversampling_enable = true,
    .oversampling_ratio = ADC_OversamplingRatio_256,
    .oversampling_shift = 4
  }
};

AIN_t AI1 = { .adc = &ain_adc, .channel = 0, .type = AIN_Type_Volts };
AIN_t AI2 = { .adc = &ain_adc, .channel = 1, .type = AIN_Type_Volts };
AIN_t AI3 = { .adc = &ain_adc, .channel = 3, .type = AIN_Type_Volts };
AIN_t AI4 = { .adc = &ain_adc, .channel = 4, .type = AIN_Type_Volts };

float VCC_Value(void)
{
  uint16_t raw = ain_adc.measurements.output[2];
  float value = resistor_divider_factor(3.3, 110, 10, 16) * raw;
  return value;
}

//------------------------------------------------------------------------------------------------- RS485

uint8_t rs_buff_buffer[RS_BUFFER_SIZE];
BUFF_t rs_buff = { .mem = rs_buff_buffer, .size = RS_BUFFER_SIZE };
GPIO_t rs_gpio_direction = { .port = GPIOA, .pin = 4, .speed = GPIO_Speed_VeryHigh }; // TODO: port+pin 
UART_t RS = {
  .reg = USART1,
  .tx_pin = UART1_TX_PB6,
  .rx_pin = UART1_RX_PB7,
  .dma_channel = 5,
  .UART_9600,
  .buff = &rs_buff,
  .gpio_direction = &rs_gpio_direction
};

//------------------------------------------------------------------------------------------------- RGB+BTN

GPIO_t rgb_gpio_red = { .port = GPIOC, .pin = 7 };
GPIO_t rgb_gpio_green = { .port = GPIOA, .pin = 11 };
GPIO_t rgb_gpio_blue = { .port = GPIOA, .pin = 12 };

RGB_t RGB = { .red = &rgb_gpio_red, .green = &rgb_gpio_green, .blue = &rgb_gpio_blue };

//------------------------------------------------------------------------------------------------- DBG+Bash

uint8_t dbg_buff_buffer[2048];
BUFF_t dbg_buff = { .mem = dbg_buff_buffer, .size = sizeof(dbg_buff_buffer) };
UART_t dbg_uart = {
  .reg = USART2,
  .tx_pin = UART2_TX_PD5,
  .rx_pin = UART2_RX_PD6,
  .dma_channel = 4,
  .UART_115200,
  .buff = &dbg_buff
};
uint8_t dbg_file_buffer[2048];
FILE_t dbg_file = { .name = "debug", .buffer = dbg_file_buffer, .limit = sizeof(dbg_file_buffer) };
#ifdef STM32G081xx
  EEPROM_t cache_eeprom = { .page_a = 62, .page_b = 63 };
#endif
#ifdef STM32G0C1xx
  EEPROM_t cache_eeprom = { .page_a = 254, .page_b = 255 };
#endif
uint8_t cache_file_buffer[2048];
FILE_t cache_file = { .name = "cache", .buffer = cache_file_buffer, .limit = sizeof(cache_file_buffer) };

STREAM_t dbg_stream = {
  .name = "debug",
  .modify = STREAM_Modify_Lowercase,
  .Size = DBG_ReadSize,
  .Read = DBG_ReadString
};

//------------------------------------------------------------------------------------------------- PLC

void PLC_Init(void)
{
  #if PLC_BOOTLOADER
    // SCB->VTOR = FLASH_BASE | 0x00000000U;
  #endif
  // Konfiguracja systemowa
  RTC_Init();
  EEPROM_Cache(&cache_eeprom);
  SYSTICK_Init(PLC_BASETIME);
  RGB_Init(&RGB);
  DBG_Init(&dbg_uart, &dbg_file);
  BASH_AddFile(&cache_file);
  BASH_AddFile(&dbg_file);
  BASH_AddCallback(&LED_Bash);
  // Wyjścia cyfrowe tranzystorowe (TO)
  DOUT_Init(&TO1);
  DOUT_Init(&TO2);
  DOUT_Init(&TO3);
  DOUT_Init(&TO4);
  DOUT_Init(&TO5);
  DOUT_Init(&TO6);
  DOUT_Init(&TO7);
  DOUT_Init(&TO8);
  DOUT_Init(&TO9);
  DOUT_Init(&TO10);
  DOUT_Init(&TO11);
  DOUT_Init(&TO12);
  PWM_Init(&to149c_pwm);
  PWM_Init(&to2_pwm);
  PWM_Init(&to3_pwm);
  PWM_Init(&to6_pwm);
  PWM_Init(&to7a_pwm);
  // Wejścia cyfrowe (DI)
  if(DIN_Init(&DI1)) {
    din_pwmi2.channel[TIM_CH2] = TIM2_CH2_PB3;
    DI1.frequency = &din_pwmi2.frequency[0];
    DI1.fill = &din_pwmi2.fill[0];
    din_pwmi2_init = true;
  }
  DIN_Init(&DI2);
  DIN_Init(&DI3);
  DIN_Init(&DI4);
  DIN_Init(&DI5);
  if(DIN_Init(&DI6)) {
    din_pwmi2.channel[TIM_CH1] = TIM2_CH1_PA15;
    DI6.frequency = &din_pwmi2.frequency[0];
    DI6.fill = &din_pwmi2.fill[0];
    din_pwmi2_init = true;
  }
  if(din_pwmi2_init) {
    PWMI_Init(&din_pwmi2);
  }
  DIN_Init(&DI7);
  DIN_Init(&DI8);
  if(DIN_Init(&DI9)) {
    din_pwmi3.channel[TIM_CH1] = TIM3_CH1_PA6;
    DI9.frequency = &din_pwmi3.frequency[0];
    DI9.fill = &din_pwmi3.fill[0];
    din_pwmi3_init = true;
  }
  if(DIN_Init(&DI10)) {
    din_pwmi3.channel[TIM_CH2] = TIM3_CH2_PA7;
    DI10.frequency = &din_pwmi3.frequency[1];
    DI10.fill = &din_pwmi3.fill[1];
    din_pwmi3_init = true;
  }
  if(DIN_Init(&DI11)) {
    din_pwmi3.channel[TIM_CH3] = TIM3_CH3_PB0;
    DI11.frequency = &din_pwmi3.frequency[2];
    DI11.fill = &din_pwmi3.fill[2];
    din_pwmi3_init = true;
    din_pwmi3.trig3 = &din_trig3;
  }
  if(DIN_Init(&DI12)) {
    din_pwmi3.channel[TIM_CH4] = TIM3_CH4_PB1;
    DI12.frequency = &din_pwmi3.frequency[3];
    DI12.fill = &din_pwmi3.fill[3];
    din_pwmi3_init = true;
    din_pwmi3.trig4 = &din_trig4;
  }
  if(din_pwmi3_init) {
    PWMI_Init(&din_pwmi3);
  }
  // Wejścia analogowe (AI)
  ADC_Init(&ain_adc);
  ADC_Measurements(&ain_adc);
  ADC_Wait(&ain_adc);
  // Interfejsy RS485
  UART_Init(&RS);
}

void PLC_Loop(void)
{
  RGB_Loop(&RGB);
  // Obsługa debugera i powłoki bash
  BASH_Loop(&dbg_stream);
  if(UART_IsFree(&dbg_uart)) {
    clear();
    if(dbg_file.size) {
      uint8_t *buffer = (uint8_t *)new(dbg_file.size);
      memcpy(buffer, dbg_file.buffer, dbg_file.size);
      UART_Send(&dbg_uart, buffer, dbg_file.size);
      FILE_Clear(&dbg_file);
    }
  }
  // Wyjścia cyfrowe tranzystorowe (TO)
  DOUT_Loop(&TO1);
  DOUT_Loop(&TO2);
  DOUT_Loop(&TO3);
  DOUT_Loop(&TO4);
  DOUT_Loop(&TO5);
  DOUT_Loop(&TO6);
  DOUT_Loop(&TO7);
  DOUT_Loop(&TO8);
  DOUT_Loop(&TO9);
  DOUT_Loop(&TO10);
  DOUT_Loop(&TO11);
  DOUT_Loop(&TO12);
  // Wejścia cyfrowe (DI)
  DIN_Loop(&DI1);
  DIN_Loop(&DI2);
  DIN_Loop(&DI3);
  DIN_Loop(&DI4);
  DIN_Loop(&DI5);
  DIN_Loop(&DI6);
  if(din_pwmi2_init && PWMI_Loop(&din_pwmi2)) {
    // PWMI_Print(&din_pwmi2);
  }
  DIN_Loop(&DI7);
  DIN_Loop(&DI8);
  DIN_Loop(&DI9);
  DIN_Loop(&DI10);
  DIN_Loop(&DI11);
  DIN_Loop(&DI12);
  if(din_pwmi3_init && PWMI_Loop(&din_pwmi3)) {
    // PWMI_Print(&din_pwmi3);
  }
  // Wejścia analogowe (AI)
  ADC_Measurements(&ain_adc);
  if(ain_adc.overrun) {
    // DBG_String("ADC overrun:");
    // DBG_uDec(ain_adc.overrun);
    // DBG_Enter();
    ain_adc.overrun = 0;
  }
}

void PLC_Thread(void)
{
  PLC_Init();
  while(1) {
    PLC_Loop();
    let();
  }
}

//-------------------------------------------------------------------------------------------------
