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

float VCC_Value(void)
{
  uint16_t raw = ain_adc.measurements.output[2];
  float value = resistor_divider_factor(3.3, 110, 10, 16) * raw;
  return value;
}

float POT_Value(void)
{
  uint16_t raw = ain_adc.measurements.output[3];
  float value = volts_factor(3.3, 16) * raw;
  return value * 100 / 3.3;
}

//------------------------------------------------------------------------------------------------- RS485

uint8_t rs1_buff_buffer[RS_BUFFER_SIZE];
BUFF_t rs1_buff = { .mem = rs1_buff_buffer, .size = RS_BUFFER_SIZE };
GPIO_t rs1_gpio_direction = { .port = GPIOA, .pin = 4, .speed = GPIO_Speed_VeryHigh };
UART_t RS1 = {
  .reg = USART2,
  .tx_pin = UART2_TX_PA2,
  .rx_pin = UART2_RX_PA3,
  .dma_channel = 5,
  .UART_9600,
  .buff = &rs1_buff,
  .gpio_direction = &rs1_gpio_direction
};

uint8_t rs2_buff_buffer[RS_BUFFER_SIZE];
BUFF_t rs2_buff = { .mem = rs2_buff_buffer, .size = RS_BUFFER_SIZE };
GPIO_t rs2_gpio_direction = { .port = GPIOB, .pin = 2, .speed = GPIO_Speed_VeryHigh };
UART_t RS2 = {
  .reg = USART1,
  .tx_pin = UART1_TX_PC4,
  .rx_pin = UART1_RX_PC5,
  .dma_channel = 6,
  .UART_9600,
  .buff = &rs2_buff,
  .gpio_direction = &rs2_gpio_direction
};

//------------------------------------------------------------------------------------------------- I2C

I2C_Master_t i2c_master = {
  .reg = I2C1,
  .scl_pin = I2C1_SCL_PA9,
  .sda_pin = I2C1_SDA_PA10,
  .pull_up = true,
  .interrupt_level = 1,
  .I2C_TIMING_100kHz
};

bool I2C_JustRead(uint8_t addr, uint8_t *ary, uint16_t n)
{
  I2C_Master_JustRead(&i2c_master, addr, ary, n);
  if(timeout(21 + n, WAIT_&I2C_Master_IsFree, &i2c_master)) {
    return false;
  }
  return true;
}

bool I2C_JustWrite(uint8_t addr, uint8_t *ary, uint16_t n)
{
  I2C_Master_JustWrite(&i2c_master, addr, ary, n);
  if(timeout(21 + n, WAIT_&I2C_Master_IsFree, &i2c_master)) {
    return false;
  }
  return true;
}

bool I2C_Read(uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n)
{
  I2C_Master_Read(&i2c_master, addr, reg, ary, n);
  if(timeout(23 + n, WAIT_&I2C_Master_IsFree, &i2c_master)) {
    return false;
  }
  return true;
}

bool I2C_Write(uint8_t addr, uint8_t reg, uint8_t *ary, uint16_t n)
{
  I2C_Master_Write(&i2c_master, addr, reg, ary, n);
  if(timeout(22 + n, WAIT_&I2C_Master_IsFree, &i2c_master)) {
    return false;
  }
  return true;
}

GPIO_t onewire_power_gpio = { .port = GPIOA, .pin = 9, .mode = GPIO_Mode_Output, .set = true };
GPIO_t onewire_data_gpio = { .port = GPIOA, .pin = 10 };
ONEWIRE_t onewire = { .gpio = &onewire_power_gpio };

bool ONE_WIRE_Init(void)
{
  I2C_Master_Disable(&i2c_master);
  GPIO_Init(&onewire_data_gpio);
  return WIRE_Init(&onewire);
}

bool ONE_WIRE_Reset(void) { return WIRE_Reset(&onewire); }
void ONE_WIRE_Write(uint8_t value) { WIRE_Write(&onewire, value); }
void ONE_WIRE_WriteParasitePower(uint8_t value) { WIRE_WriteParasitePower(&onewire, value); }
uint8_t ONE_WIRE_Read(void) { return WIRE_Read(&onewire); }
void ONE_WIRE_Select(uint8_t *addr) { WIRE_Select(&onewire, addr); }
void ONE_WIRE_Skip(void) { WIRE_Skip(&onewire); }
bool ONE_WIRE_Search(uint8_t *addr) { return WIRE_Search(&onewire, addr); }

//------------------------------------------------------------------------------------------------- RGB+BTN

GPIO_t rgb_gpio_red = { .port = GPIOC, .pin = 7 };
GPIO_t rgb_gpio_green = { .port = GPIOA, .pin = 11 };
GPIO_t rgb_gpio_blue = { .port = GPIOA, .pin = 12 };

RGB_t RGB = { .red = &rgb_gpio_red, .green = &rgb_gpio_green, .blue = &rgb_gpio_blue };
DIN_t BTN = { .gpif = { .gpio = { .port = GPIOC, .pin = 12 } } };

//------------------------------------------------------------------------------------------------- DBG+Bash

uint8_t dbg_buff_buffer[2048];
BUFF_t dbg_buff = { .mem = dbg_buff_buffer, .size = sizeof(dbg_buff_buffer) };
#ifdef STM32G081xx
  TIM_t dbg_tim = { .reg = TIM6 };
#endif
UART_t dbg_uart = {
  .reg = USART3,
  .tx_pin = UART3_TX_PB8,
  .rx_pin = UART3_RX_PB9,
  .dma_channel = 4,
  .UART_115200,
  .buff = &dbg_buff,
  #ifdef STM32G081xx
    .tim = &dbg_tim
  #endif
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

//------------------------------------------------------------------------------------------------- Functions PLC

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
  DIN_Init(&BTN);
  DBG_Init(&dbg_uart, &dbg_file);
  BASH_AddFile(&cache_file);
  BASH_AddFile(&dbg_file);
  BASH_AddCallback(&LED_Bash);
  // Magistrala I2C
  I2C_Master_Init(&i2c_master);
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
  ADC_Measurements(&ain_adc);
  ADC_Wait(&ain_adc);
  // Interfejsy RS485
  UART_Init(&RS1);
  UART_Init(&RS2);
}

void PLC_Loop(void)
{
  RGB_Loop(&RGB);
  DIN_Loop(&BTN);
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

//------------------------------------------------------------------------------------------------- RTD

GPIO_t rtd_gpio_drdy = { .port = GPIOB, .pin = 11, .reverse = true };
GPIO_t rtd_gpio_cs = { .port = GPIOB, .pin = 12, .reverse = true };

SPI_Master_t rtd_spi = {
  .spi_typedef = SPI2,
  .tx_dma_channel = 2,
  .rx_dma_channel = 3,
  .interrupt_level = 2,
  .miso_pin = SPI2_MISO_PB14,
  .mosi_pin = SPI2_MOSI_PB15,
  .sck_pin = SPI2_SCK_PB13,
  .cs_gpio = &rtd_gpio_cs,
  .cs_delay = 10,
  .prescaler = SPI_PS_16,
  .cpol = true,
  .cpha = true
};

//------------------------------------------------------------------------------------------------- MAX31865

MAX31865_t RTD = {
  .spi = &rtd_spi,
  .ready = &rtd_gpio_drdy,
  .type = MAX31865_Type_PT100,
  .wire = MAX31865_Wire_3,
  .reject = MAX31865_Reject_60Hz
};

//------------------------------------------------------------------------------------------------- Functions RTD

void RTD_Main(void)
{
  MAX31865_Init(&RTD);
  while(1) {
    MAX31865_Loop(&RTD);
    let();
  }
}

float RTD_Temperature(void)
{
  return RTD.temperature;
}

//-------------------------------------------------------------------------------------------------
