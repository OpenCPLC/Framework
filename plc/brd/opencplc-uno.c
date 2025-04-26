#ifdef OPENCPLC_UNO
#include "opencplc-uno.h"

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

//------------------------------------------------------------------------------------------------- DOUT-RO

DOUT_t RO1 = { .name = "RO1", .relay = true, .gpio = { .port = GPIOB, .pin = 7 }, .eeprom = &io_eeprom[0], .save = true };
DOUT_t RO2 = { .name = "RO2", .relay = true, .gpio = { .port = GPIOB, .pin = 6 }, .eeprom = &io_eeprom[0], .save = true };
DOUT_t RO3 = { .name = "RO3", .relay = true, .gpio = { .port = GPIOB, .pin = 5 }, .eeprom = &io_eeprom[1], .save = true };
DOUT_t RO4 = { .name = "RO4", .relay = true, .gpio = { .port = GPIOB, .pin = 4 }, .eeprom = &io_eeprom[1], .save = true };

//------------------------------------------------------------------------------------------------- DOUT-TO

PWM_t to_pwm = {
  .reg = TIM1,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 46,
  #elif(SYS_CLOCK_FREQ == 18432000)
    // .prescaler = TODO, 
  #endif
  .channel[TIM_CH1] = TIM1_CH1_PC8,
  .channel[TIM_CH2] = TIM1_CH2_PC9,
  .channel[TIM_CH3] = TIM1_CH3_PC10,
  .channel[TIM_CH4] = TIM1_CH4_PC11,
  .auto_reload = 1000,
  .center_aligned = true
};

DOUT_t TO1 = { .name = "TO1", .pwm = &to_pwm, .channel = TIM_CH4, .eeprom = &io_eeprom[2], .save = false };
DOUT_t TO2 = { .name = "TO2", .pwm = &to_pwm, .channel = TIM_CH3, .eeprom = &io_eeprom[2], .save = false };
DOUT_t TO3 = { .name = "TO3", .pwm = &to_pwm, .channel = TIM_CH2, .eeprom = &io_eeprom[2], .save = false };
DOUT_t TO4 = { .name = "TO4", .pwm = &to_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[2], .save = false };

void TO_Frequency(float frequency)
{
  PWM_Frequency(&to_pwm, frequency);
};

//------------------------------------------------------------------------------------------------- DOUT-XO

PWM_t xo_pwm = {
  .reg = TIM2,
  #if(SYS_CLOCK_FREQ == 16000000)
    .prescaler = 460,
  #elif(SYS_CLOCK_FREQ == 18432000)

  #endif
  .channel[TIM_CH1] = TIM2_CH1_PA15,
  .channel[TIM_CH2] = TIM2_CH2_PB3,
  .auto_reload = 1000,
};

DOUT_t XO1 = { .name = "XO1", .pwm = &xo_pwm, .channel = TIM_CH2, .eeprom = &io_eeprom[3], .save = false };
DOUT_t XO2 = { .name = "XO2", .pwm = &xo_pwm, .channel = TIM_CH1, .eeprom = &io_eeprom[3], .save = false };

void XO_Frequency(float frequency)
{
  PWM_Frequency(&xo_pwm, frequency);
}

//------------------------------------------------------------------------------------------------- DIN

EXTI_t din_trig3, din_trig4;

PWMI_t din_pwmi = {
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

DIN_t DI1 = { .name = "DI1", .gpif = { .gpio = { .port = GPIOA, .pin = 6, .reverse = true } }, .eeprom = &io_eeprom[3] };
DIN_t DI2 = { .name = "DI2", .gpif = { .gpio = { .port = GPIOA, .pin = 7, .reverse = true } }, .eeprom = &io_eeprom[3] };
DIN_t DI3 = { .name = "DI3", .gpif = { .gpio = { .port = GPIOB, .pin = 0, .reverse = true } }, .eeprom = &io_eeprom[3] };
DIN_t DI4 = { .name = "DI4", .gpif = { .gpio = { .port = GPIOB, .pin = 1, .reverse = true } }, .eeprom = &io_eeprom[3] };

bool din_pwmi_init = false;

//------------------------------------------------------------------------------------------------- AIN

uint8_t ain_channels[] = {
  ADC_IN_PA0, ADC_IN_PA1, ADC_IN_PA5, ADC_IN_PB10
};

#define AIN_BUFFER_SIZE adc_record_buffer_size(AIN_AVERAGE_TIME_MS, 160, 64, sizeof(ain_channels))
#define AIN_SAMPLES (AIN_BUFFER_SIZE / sizeof(ain_channels))

uint16_t ain_buffer[AIN_BUFFER_SIZE];
uint16_t ain_data[sizeof(ain_channels)][AIN_SAMPLES];

ADC_t ain_adc = {
  .frequency = ADC_Frequency_16MHz,
  .interrupt_level = 3,
  .record = {
    .channels = ain_channels,
    .count = sizeof(ain_channels),
    .dma_nbr = 1,
    .sampling_time = ADC_SamplingTime_160,
    .oversampling_enable = true,
    .oversampling_ratio = ADC_OversamplingRatio_64,
    .oversampling_shift = 2,
    .buffer = ain_buffer,
    .buffer_length = AIN_BUFFER_SIZE
  }
};

AIN_t AI1 = { .name = "AI1", .data = ain_data[0], .count = AIN_SAMPLES };
AIN_t AI2 = { .name = "AI2", .data = ain_data[1], .count = AIN_SAMPLES };
AIN_t POT = { .name = "POT", .data = ain_data[3], .count = AIN_SAMPLES };
AIN_t VCC = { .name = "VCC", .data = ain_data[2], .count = AIN_SAMPLES };

float VCC_Voltage_V(void)
{
  float raw = AIN_Raw(&VCC);
  float value = resistor_divider_factor(3.3, 110, 10, 16) * raw;
  return value;
}

//------------------------------------------------------------------------------------------------- RS485

uint8_t rs1_buff_buffer[RS_BUFFER_SIZE];
BUFF_t rs1_buff = { .mem = rs1_buff_buffer, .size = RS_BUFFER_SIZE };
GPIO_t rs1_gpio_direction = { .port = GPIOA, .pin = 4, .speed = GPIO_Speed_VeryHigh };
UART_t RS1 = {
  .reg = USART2,
  .tx_pin = UART2_TX_PA2,
  .rx_pin = UART2_RX_PA3,
  .dma_nbr = 5,
  .UART_9600,
  .buff = &rs1_buff,
  .gpio_direction = &rs1_gpio_direction,
  .timeout = 40
};

uint8_t rs2_buff_buffer[RS_BUFFER_SIZE];
BUFF_t rs2_buff = { .mem = rs2_buff_buffer, .size = RS_BUFFER_SIZE };
GPIO_t rs2_gpio_direction = { .port = GPIOB, .pin = 2, .speed = GPIO_Speed_VeryHigh };
UART_t RS2 = {
  .reg = USART1,
  .tx_pin = UART1_TX_PC4,
  .rx_pin = UART1_RX_PC5,
  .dma_nbr = 6,
  .UART_9600,
  .buff = &rs2_buff,
  .gpio_direction = &rs2_gpio_direction,
  .timeout = 40
};

//------------------------------------------------------------------------------------------------- I2C+1WIRE

I2C_Master_t i2c_master = {
  .reg = I2C1,
  .scl_pin = I2C1_SCL_PA9,
  .sda_pin = I2C1_SDA_PA10,
  .pull_up = true,
  .interrupt_level = 1,
  .I2C_TIMING_100kHz
};

GPIO_t _1wire_gpio = { .port = GPIOA, .pin = 10 };
TIM_t sleep_us_tim = { .reg = TIM7 };

void _1WIRE_Active(void)
{
  sleep_us_init(&sleep_us_tim);
  I2C_Master_Disable(&i2c_master);
  // WIRE_Init(&onewire);
  return;
}

//------------------------------------------------------------------------------------------------- RGB+BTN

GPIO_t rgb_gpio_red = { .port = GPIOC, .pin = 7 };
GPIO_t rgb_gpio_green = { .port = GPIOA, .pin = 11 };
GPIO_t rgb_gpio_blue = { .port = GPIOA, .pin = 12 };

RGB_t RGB = { .red = &rgb_gpio_red, .green = &rgb_gpio_green, .blue = &rgb_gpio_blue };
DIN_t BTN = { .gpif = { .gpio = { .port = GPIOC, .pin = 12 } } };

//------------------------------------------------------------------------------------------------- DBG+Bash

#ifdef STM32G081xx
  TIM_t dbg_tim = { .reg = TIM6 };
#endif
UART_t dbg_uart = {
  .reg = USART3,
  .tx_pin = UART3_TX_PB8,
  .rx_pin = UART3_RX_PB9,
  .dma_nbr = DMA_Nbr_4,
  .int_prioryty = INT_Prioryty_Low,
  .UART_115200,
  #ifdef STM32G081xx
    .tim = &dbg_tim
  #endif
};
#ifdef STM32G081xx
  EEPROM_t cache_eeprom = { .page_a = 62, .page_b = 63 };
#endif
#ifdef STM32G0C1xx
  EEPROM_t cache_eeprom = { .page_a = 254, .page_b = 255 };
#endif
uint8_t cache_file_buffer[2048];
FILE_t cache_file = { .name = "cache", .buffer = cache_file_buffer, .limit = sizeof(cache_file_buffer) };

//------------------------------------------------------------------------------------------------- Functions PLC

void PLC_Init(void)
{
  #if PLC_BOOTLOADER
    // SCB->VTOR = FLASH_BASE | 0x00000000U;
  #endif
  // Konfiguracja systemowa
  system_clock_init();
  systick_init(PLC_BASETIME);
  RTC_Init();
  EEPROM_Cache(&cache_eeprom);
  RGB_Init(&RGB);
  DIN_Init(&BTN);
  DBG_Init(&dbg_uart);
  BASH_AddFile(&cache_file);
  BASH_AddCallback(&LED_Bash, "led");
  // Magistrala I2C
  TWI_Init(&i2c_master);
  // Wyjścia cyfrowe przekaźnikowe (RO)
  DOUT_Init(&RO1);
  DOUT_Init(&RO2);
  DOUT_Init(&RO3);
  DOUT_Init(&RO4);
  // Wyjścia cyfrowe tranzystorowe (TO)
  DOUT_Init(&TO1);
  DOUT_Init(&TO2);
  DOUT_Init(&TO3);
  DOUT_Init(&TO4);
  PWM_Init(&to_pwm);
  // Wyjścia cyfrowe triakowe (XO)
  DOUT_Init(&XO1);
  DOUT_Init(&XO2);
  PWM_Init(&xo_pwm);
  // Wejścia cyfrowe (DI)
  if(DIN_Init(&DI1)) {
    din_pwmi.channel[TIM_CH1] = TIM3_CH1_PA6;
    DI1.frequency = &din_pwmi.frequency[0];
    DI1.fill = &din_pwmi.fill[0];
    din_pwmi_init = true;
  }
  if(DIN_Init(&DI2)) {
    din_pwmi.channel[TIM_CH2] = TIM3_CH2_PA7;
    DI2.frequency = &din_pwmi.frequency[1];
    DI2.fill = &din_pwmi.fill[1];
    din_pwmi_init = true;
  }
  if(DIN_Init(&DI3)) {
    din_pwmi.channel[TIM_CH3] = TIM3_CH3_PB0;
    DI3.frequency = &din_pwmi.frequency[2];
    DI3.fill = &din_pwmi.fill[2];
    din_pwmi_init = true;
    din_pwmi.trig3 = &din_trig3;
  }
  if(DIN_Init(&DI4)) {
    din_pwmi.channel[TIM_CH4] = TIM3_CH4_PB1;
    DI4.frequency = &din_pwmi.frequency[3];
    DI4.fill = &din_pwmi.fill[3];
    din_pwmi_init = true;
    din_pwmi.trig4 = &din_trig4;
  }
  if(din_pwmi_init) {
    PWMI_Init(&din_pwmi);
  }
  // Wejścia analogowe (AI)
  ADC_Init(&ain_adc);
  ADC_Record(&ain_adc);
  ADC_Wait(&ain_adc);
  // Interfejsy RS485
  UART_Init(&RS1);
  UART_Init(&RS2);
  LOG_Init(PLC_GREETING, PRO_VERSION);
}

void PLC_Loop(void)
{
  // Dioda LED i przycisk (BTN)
  RGB_Loop(&RGB);
  DIN_Loop(&BTN);
  // Wyjścia przekaźnikowe (RO)
  DOUT_Loop(&RO1);
  DOUT_Loop(&RO2);
  DOUT_Loop(&RO3);
  DOUT_Loop(&RO4);
  // Wyjścia cyfrowe tranzystorowe (TO)
  DOUT_Loop(&TO1);
  DOUT_Loop(&TO2);
  DOUT_Loop(&TO3);
  DOUT_Loop(&TO4);
  // Wyjścia cyfrowe triakowe (XO)
  DOUT_Loop(&XO1);
  DOUT_Loop(&XO2);
  // Wejścia cyfrowe (DI)
  DIN_Loop(&DI1);
  DIN_Loop(&DI2);
  DIN_Loop(&DI3);
  DIN_Loop(&DI4);
  if(din_pwmi_init && PWMI_Loop(&din_pwmi)) {
    // PWMI_Print(&din_pwmi);
  }
  // Wejścia analogowe (AI)
  if(ADC_IsFree(&ain_adc)) {
    AIN_Sort(ain_buffer, sizeof(ain_channels), AIN_SAMPLES, ain_data);
    ADC_Record(&ain_adc);
  }
  if(ain_adc.overrun) {
    LOG_Warning("ADC over-run %d", ain_adc.overrun);
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

//------------------------------------------------------------------------------------------------- RTD

GPIO_t rtd_gpio_drdy = { .port = GPIOB, .pin = 11, .reverse = true };
GPIO_t rtd_gpio_cs = { .port = GPIOB, .pin = 12, .reverse = true };

SPI_Master_t rtd_spi = {
  .reg = SPI2,
  .tx_dma_nbr = DMA_Nbr_2,
  .rx_dma_nbr = DMA_Nbr_3,
  .int_prioryty = INT_Prioryty_Medium,
  .miso_pin = SPI2_MISO_PB14,
  .mosi_pin = SPI2_MOSI_PB15,
  .sck_pin = SPI2_SCK_PB13,
  .cs_gpio = &rtd_gpio_cs,
  .cs_delay = 10,
  .prescaler = SPI_Prescaler_32,
  .cpol = true,
  .cpha = true
};

MAX31865_t RTD = {
  .name = "RTD",
  .spi = &rtd_spi,
  .ready = &rtd_gpio_drdy,
  .nominal_ohms = RTD_Type_PT100,
  .wire = RTD_Wire_3,
  .reject = RTD_Reject_60Hz
};

void RTD_Thread(void)
{
  SPI_Master_Init(&rtd_spi);
  MAX31865_Init(&RTD);
  while(1) {
    MAX31865_Loop(&RTD);
    let();
  }
}

//-------------------------------------------------------------------------------------------------
#endif
