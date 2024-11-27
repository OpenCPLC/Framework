#include "gpio.h"

//----------------------------------------------------------------------------- RCC

void GPIO_Init(GPIO_t *gpio)
{
  RCC_EnableGPIO(gpio->port);
  gpio->port->PUPDR = (gpio->port->PUPDR & ~(3 << (2 * gpio->pin))) | (gpio->pull << (2 * gpio->pin));
  gpio->port->OSPEEDR = (gpio->port->OSPEEDR & ~(3 << (2 * gpio->pin))) | (gpio->speed << (2 * gpio->pin));
  if(gpio->out_type) gpio->port->OTYPER |= (1 << gpio->pin);
  else gpio->port->OTYPER &= ~(1 << gpio->pin);
  if(gpio->mode == GPIO_Mode_Alternate) {
    if(gpio->pin < 8) gpio->port->AFR[0] = (gpio->port->AFR[0] & ~(0x0F << (4 * gpio->pin))) | (gpio->alternate << (4 * gpio->pin));
    else gpio->port->AFR[1] = (gpio->port->AFR[1] & ~(0x0F << (4 * (gpio->pin - 8)))) | (gpio->alternate << (4 * (gpio->pin - 8)));
  }
  gpio->port->MODER = (gpio->port->MODER & ~(3 << (2 * gpio->pin))) | (gpio->mode << (2 * gpio->pin));
  if(gpio->set) GPIO_Set(gpio);
  else GPIO_Rst(gpio);
  #if(GPIO_INCLUDE_WAKEUP)
  if(gpio->wakeup_pull) {
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    PWR->CR3 |= PWR_CR3_APC;
    if(gpio->wakeup_pull == GPIO_WakeupPull_Up) {
      switch((uint32_t)gpio->port) {
        case (uint32_t)GPIOA: PWR->PUCRA |= (1 << gpio->pin); break;
        case (uint32_t)GPIOB: PWR->PUCRB |= (1 << gpio->pin); break;
        case (uint32_t)GPIOC: PWR->PUCRC |= (1 << gpio->pin); break;
        case (uint32_t)GPIOD: PWR->PUCRD |= (1 << gpio->pin); break;
        case (uint32_t)GPIOF: PWR->PUCRF |= (1 << gpio->pin); break;
      }
    }
    else {
      switch((uint32_t)gpio->port) {
        case (uint32_t)GPIOA: PWR->PDCRA |= (1 << gpio->pin); break;
        case (uint32_t)GPIOB: PWR->PDCRB |= (1 << gpio->pin); break;
        case (uint32_t)GPIOC: PWR->PDCRC |= (1 << gpio->pin); break;
        case (uint32_t)GPIOD: PWR->PDCRD |= (1 << gpio->pin); break;
        case (uint32_t)GPIOF: PWR->PDCRF |= (1 << gpio->pin); break;
      }
    }
  }
  #endif
}

void GPIO_InitList(GPIO_t *gpio, ...)
{
  va_list args;
  va_start(args, gpio);
  while(gpio) {
    GPIO_Init(gpio);
    gpio = va_arg(args, GPIO_t *);
  }
  va_end(args);
}

void GPIO_AlternateInit(const GPIO_Map_t *gpio_map, bool open_drain_pull_up)
{
  GPIO_t gpio = GPIO_ALTERNATE;
  gpio.port = gpio_map->port;
  gpio.pin = gpio_map->pin;
  gpio.alternate = gpio_map->alternate;
  if(open_drain_pull_up) {
    gpio.speed = GPIO_Speed_VeryLow;
    gpio.out_type = GPIO_OutType_OpenDrain;
    gpio.pull = GPIO_Pull_Up;
  }
  else gpio.speed = GPIO_Speed_VeryHigh;
  GPIO_Init(&gpio);
}

void GPIO_SupplyInit(GPIO_t *gpio)
{
  gpio->mode = GPIO_Mode_Output;
  gpio->speed = GPIO_Speed_VeryHigh;
  GPIO_Init(gpio);
}

void GPIO_Set(GPIO_t *gpio)
{
  gpio->set = true;
  if(gpio->reverse) gpio->port->BRR = (1<<gpio->pin);
  else gpio->port->BSRR = (1<<gpio->pin);
}

void GPIO_Rst(GPIO_t *gpio)
{
  gpio->set = false;
  if(gpio->reverse) gpio->port->BSRR = (1<<gpio->pin);
  else gpio->port->BRR = (1<<gpio->pin);
}

void GPIO_Tgl(GPIO_t *gpio)
{
  if(gpio->set) GPIO_Rst(gpio);
  else GPIO_Set(gpio);
}

bool GPIO_In(GPIO_t *gpio)
{
  uint16_t in;
  in = gpio->port->IDR & (1<<gpio->pin);
  if(in) { if(gpio->reverse) return false; else return true; }
  else { if(gpio->reverse) return true; else return false; }
}

bool GPIO_NotIn(GPIO_t *gpio)
{
  return !GPIO_In(gpio);
}

void GPIO_ModeInput(GPIO_t *gpio)
{
  gpio->mode = GPIO_Mode_Input;
  gpio->port->MODER = (gpio->port->MODER & ~(3 << (2 * gpio->pin))) | (gpio->mode << (2 * gpio->pin));
}

void GPIO_ModeOutput(GPIO_t *gpio)
{
  gpio->mode = GPIO_Mode_Output;
  gpio->port->MODER = (gpio->port->MODER & ~(3 << (2 * gpio->pin))) | (gpio->mode << (2 * gpio->pin));
}

//----------------------------------------------------------------------------- GPIF

bool GPIF_Input(GPIF_t *gpif)
{
  return gpif->value;
}

bool GPIF_Toggling(GPIF_t *gpif)
{
  if(gpif->_toggle_tick) return true;
  else return false;
}

bool GPIF_Rise(GPIF_t *gpif)
{
  if(gpif->rise) {
    gpif->rise = false;
    return true;
  }
  return false;
}

bool GPIF_Fall(GPIF_t *gpif)
{
  if(gpif->fall) {
    gpif->fall = false;
    return true;
  }
  return false;
}

bool GPIF_Edge(GPIF_t *gpif)
{
  bool rise = GPIF_Rise(gpif);
  bool fall = GPIF_Fall(gpif);
  if(rise || fall) return true;
  return false;
}

void GPIF_Init(GPIF_t *gpif)
{
  gpif->gpio.mode = GPIO_Mode_Input;
  if(!gpif->ton_ms) gpif->ton_ms = GPIF_TON_DELAULT;
  if(!gpif->toff_ms) gpif->toff_ms = GPIF_TOFF_DELAULT;
  if(!gpif->toggle_ms) gpif->toggle_ms = GPIF_TOGGLE_DELAULT;
  GPIO_Init(&gpif->gpio);
  gpif->value = GPIO_In(&gpif->gpio);
  gpif->_tio_tick = gpif->value ? tick_keep(gpif->toff_ms) : tick_keep(gpif->ton_ms);
}

bool GPIF_Loop(GPIF_t *gpif)
{
  bool in = GPIO_In(&gpif->gpio);
  if(in == gpif->value) {
    gpif->_tio_tick = in ? tick_keep(gpif->toff_ms) : tick_keep(gpif->ton_ms);
  }
  else {
    if(tick_over(&gpif->_tio_tick)) {
      gpif->value = in;
      gpif->_toggle_tick = tick_keep(gpif->toggle_ms);
      if(gpif->value) {
        gpif->_res_rise_tick = tick_keep(GPIF_TRESET);
        gpif->rise = true;
      }
      else {
        gpif->_res_fall_tick = tick_keep(GPIF_TRESET);
        gpif->fall = true;
      }
    }
  }
  tick_over(&gpif->_toggle_tick);
  if(tick_over(&gpif->_res_rise_tick)) gpif->rise = false;
  if(tick_over(&gpif->_res_fall_tick)) gpif->fall = false;
  return gpif->value;
}

//----------------------------------------------------------------------------- EXTI

static void EXTI_Interrupt(EXTI_t *exti)
{
  if(EXTI->FPR1 & (1 << exti->pin)) {
    EXTI->FPR1 |= (1 << exti->pin);
    exti->_fall_inc++;
    if(exti->fall_function) exti->fall_function(exti->fall_struct);
    if(exti->one_shot) EXTI_Off(exti);
  }
  if(EXTI->RPR1 & (1 << exti->pin)) {
    EXTI->RPR1 |= (1 << exti->pin);
    exti->_rise_inc++;
    if(exti->rise_function) exti->rise_function(exti->rise_struct);
  }
}

inline void EXTI_On(EXTI_t *exti)
{
  exti->enable = true;
  EXTI->IMR1 |= (1 << exti->pin);
}

inline void EXTI_Off(EXTI_t *exti)
{
  exti->enable = false;
  EXTI->IMR1 &= ~(1 << exti->pin);
}

void EXTI_Init(EXTI_t *exti)
{
  RCC_EnableGPIO(exti->port);
  exti->port->MODER = (exti->port->MODER & ~(3 << (2 * exti->pin))) | (exti->mode << (2 * exti->pin));
  exti->port->PUPDR = (exti->port->PUPDR & ~(3 << (2 * exti->pin))) | (exti->pull << (2 * exti->pin));
  uint32_t exticr_reg = exti->pin / 4;
  uint32_t exticr_move = 8 * (exti->pin % 4);
  uint32_t exticr = EXTI->EXTICR[exticr_reg];
  exticr &= ~(0xF << exticr_move);
  switch((uint32_t)exti->port) {
    case (uint32_t)GPIOA: break;
    case (uint32_t)GPIOB: exticr |= (1 << exticr_move); break;
    case (uint32_t)GPIOC: exticr |= (2 << exticr_move); break;
    case (uint32_t)GPIOD: exticr |= (3 << exticr_move); break;
    case (uint32_t)GPIOF: exticr |= (5 << exticr_move); break;
  }
  EXTI->EXTICR[exticr_reg] = exticr;
  if(exti->fall) EXTI->FTSR1 |= (1 << exti->pin);
  if(exti->rise) EXTI->RTSR1 |= (1 << exti->pin);
  if(exti->enable) EXTI_On(exti);
  else EXTI_Off(exti);
  INT_EnableEXTI(exti->pin, exti->interrupt_level, (void (*)(void *))&EXTI_Interrupt, exti);
}

uint16_t EXTI_In(EXTI_t *exti)
{
  if(exti->_rise_inc || exti->_fall_inc) {
    uint16_t response = exti->_rise_inc + exti->_fall_inc;
    exti->_rise_inc = 0;
    exti->_fall_inc = 0;
    return response;
  }
  return 0;
}

uint16_t EXTI_Rise(EXTI_t *exti)
{
  if(exti->_rise_inc) {
    uint16_t response = exti->_rise_inc;
    exti->_rise_inc = 0;
    return response;
  }
  return 0;
}

uint16_t EXTI_Fall(EXTI_t *exti)
{
  if(exti->_fall_inc) {
    uint16_t response = exti->_fall_inc;
    exti->_fall_inc = 0;
    return response;
  }
  return 0;
}

//----------------------------------------------------------------------------- END
