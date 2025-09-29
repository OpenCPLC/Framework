#include "gpio.h"

//------------------------------------------------------------------------------------------------- RCC

/**
 * @brief Initialize GPIO pin according to `GPIO_t` configuration.
 * @param gpio Pointer to GPIO configuration structure.
 */
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

/**
 * @brief Initialize multiple GPIO pins.
 * @param gpio First `GPIO_t` pointer, followed by more `GPIO_t *` arguments.
 *   List must be terminated with `NULL`.
 * @note Example: GPIO_InitList(&pin_led, &pin_button, NULL);
 */
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

/**
 * @brief Initialize GPIO pin in alternate mode.
 * @param gpio_map Pointer to `GPIO_Map_t` with port, pin and alternate.
 * @param open_drain_pull_up If `true`: configure as open-drain with pull-up (I2C-style).
 *   If `false`: push-pull, very high speed.
 */
void GPIO_InitAlternate(const GPIO_Map_t *gpio_map, bool open_drain_pull_up)
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

/**
 * @brief Set GPIO pin mode.
 * @param gpio Pointer to GPIO configuration.
 * @param mode New mode (`GPIO_Mode_t`).
 */
void GPIO_Mode(GPIO_t *gpio, GPIO_Mode_t mode)
{
  gpio->mode = mode;
  gpio->port->MODER = (gpio->port->MODER & ~(3U << (2U * gpio->pin))) | (gpio->mode << (2 * gpio->pin));
}

/**
 * @brief Set GPIO pin high.
 * @param gpio Pointer to `GPIO_t` structure.
 */
void GPIO_Set(GPIO_t *gpio)
{
  gpio->set = true;
  if(gpio->reverse) gpio->port->BRR = (1U << gpio->pin);
  else gpio->port->BSRR = (1U << gpio->pin);
}

/**
 * @brief Reset GPIO pin low.
 * @param gpio Pointer to `GPIO_t` structure.
 */
void GPIO_Rst(GPIO_t *gpio)
{
  gpio->set = false;
  if(gpio->reverse) gpio->port->BSRR = (1U << gpio->pin);
  else gpio->port->BRR = (1U << gpio->pin);
}

/**
 * @brief Toggle GPIO pin.
 * @param gpio Pointer to `GPIO_t` structure.
 */
void GPIO_Tgl(GPIO_t *gpio)
{
  if(gpio->set) GPIO_Rst(gpio);
  else GPIO_Set(gpio);
}

/**
 * @brief Read GPIO input state.
 * @param gpio Pointer to `GPIO_t` structure.
 * @return `true` if pin is high, `false` if low.
 */
bool GPIO_In(GPIO_t *gpio)
{
  bool raw = (gpio->port->IDR & (1U << gpio->pin)) != 0;
  return gpio->reverse ? !raw : raw;
}

//------------------------------------------------------------------------------------------------- EXTI

static void EXTI_Interrupt(EXTI_t *exti)
{
  if(EXTI->FPR1 & (1 << exti->pin)) {
    EXTI->FPR1 |= (1 << exti->pin);
    exti->fall_events++;
    if(exti->FallHandler) exti->FallHandler(exti->fall_arg);
    if(exti->oneshot) EXTI_Off(exti);
  }
  if(EXTI->RPR1 & (1 << exti->pin)) {
    EXTI->RPR1 |= (1 << exti->pin);
    exti->rise_events++;
    if(exti->RiseHandler) exti->RiseHandler(exti->rise_arg);
    if(exti->oneshot) EXTI_Off(exti);
  }
}

inline void EXTI_On(EXTI_t *exti)
{
  exti->irq_enable = true;
  exti->fall_events = 0;
  exti->rise_events = 0;
  EXTI->IMR1 |= (1 << exti->pin);
}

inline void EXTI_Off(EXTI_t *exti)
{
  exti->irq_enable = false;
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
  if(exti->fall_detect) EXTI->FTSR1 |= (1 << exti->pin);
  if(exti->rise_detect) EXTI->RTSR1 |= (1 << exti->pin);
  if(exti->irq_enable) EXTI_On(exti);
  else EXTI_Off(exti);
  IRQ_EnableEXTI(exti->pin, exti->irq_priority, (void (*)(void *))&EXTI_Interrupt, exti);
}

uint16_t EXTI_Events(EXTI_t *exti)
{
  if(exti->rise_events || exti->fall_events) {
    uint16_t response = exti->rise_events + exti->fall_events;
    exti->rise_events = 0;
    exti->fall_events = 0;
    return response;
  }
  return 0;
}

uint16_t EXTI_Rise(EXTI_t *exti)
{
  if(exti->rise_events) {
    uint16_t response = exti->rise_events;
    exti->rise_events = 0;
    return response;
  }
  return 0;
}

uint16_t EXTI_Fall(EXTI_t *exti)
{
  if(exti->fall_events) {
    uint16_t response = exti->fall_events;
    exti->fall_events = 0;
    return response;
  }
  return 0;
}

bool EXTI_In(EXTI_t *exti)
{
  return (exti->port->IDR >> exti->pin) & 0x0001;
}

//------------------------------------------------------------------------------------------------- GPIF

/**
 * @brief Initialize GPIF input filter.
 * Sets GPIO as input, applies default values if zero,
 * reads initial state, clears flags and resets timers.
 * @param gpif Pointer to `GPIF_t` instance
 */
void GPIF_Init(GPIF_t *gpif)
{
  gpif->gpio.mode = GPIO_Mode_Input;
  if(!gpif->ton_ms) gpif->ton_ms = GPIF_DEFAULT_TON_ms;
  if(!gpif->toff_ms) gpif->toff_ms = GPIF_DEFAULT_TOFF_ms;
  if(!gpif->ton_long_ms) gpif->ton_long_ms = GPIF_DEFAULT_TON_LONG_ms;
  if(!gpif->toff_long_ms) gpif->toff_long_ms = GPIF_DEFAULT_TOFF_LONG_ms;
  if(!gpif->toggle_ms) gpif->toggle_ms = GPIF_DEFAULT_TOGGLE_ms;
  GPIO_Init(&gpif->gpio);
  gpif->input_state = GPIO_In(&gpif->gpio);
  gpif->toggle_state = false;
  gpif->rise = false;
  gpif->fall = false;
  gpif->rise_long = false;
  gpif->fall_long = false;
  gpif->tick_debounce = gpif->input_state ? tick_keep(gpif->toff_ms) : tick_keep(gpif->ton_ms);
  gpif->tick_long = 0;
  gpif->tick_toggle = 0;
}

/**
 * @brief Update GPIF state, edges, long events and toggle.
 * Call this function periodically. It reads raw GPIO, applies debounce,
 * detects edges, long press/release, and updates toggle output.
 */
void GPIF_Loop(GPIF_t *gpif)
{
  bool raw = GPIO_In(&gpif->gpio);
  if(raw == gpif->input_state) gpif->tick_debounce = raw ? tick_keep(gpif->ton_ms) : tick_keep(gpif->toff_ms);
  else if(tick_over(&gpif->tick_debounce)) {
    gpif->input_state = raw;
    if(!gpif->tick_reset) gpif->tick_toggle = tick_keep(gpif->toggle_ms);
    if(!gpif->toggle_state) gpif->tick_reset = tick_keep(gpif->toggle_ms / 2);
    if(raw) {
      gpif->rise = true;
      gpif->tick_long = tick_keep(gpif->ton_long_ms);
    }
    else {
      gpif->fall = true;
      gpif->tick_long = tick_keep(gpif->toff_long_ms);
    }
  }
  if(tick_over(&gpif->tick_long)) gpif->input_state ? (gpif->rise_long = true) : (gpif->fall_long = true);
  if(tick_over(&gpif->tick_reset)) gpif->tick_toggle = 0;
  if(tick_over(&gpif->tick_toggle)) gpif->toggle_state = !gpif->toggle_state;
}

/**
 * @brief Get debounced input state.
 * @param gpif Pointer to `GPIF_t` instance
 * @return Current input state
 */
bool GPIF_Input(GPIF_t *gpif)
{
  return gpif->input_state;
}

/**
 * @brief Get toggle output state.
 * @param gpif Pointer to `GPIF_t` instance
 * @return Current toggle state
 */
bool GPIF_Toggle(GPIF_t *gpif)
{
  return gpif->toggle_state;
}

/**
 * @brief Check rising edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if rising edge occurred
 */
bool GPIF_Rise(GPIF_t *gpif)
{
  if(gpif->rise) {
    gpif->rise = false;
    return true;
  }
  return false;
}

/**
 * @brief Check falling edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if falling edge occurred
 */
bool GPIF_Fall(GPIF_t *gpif)
{
  if(gpif->fall) {
    gpif->fall = false;
    return true;
  }
  return false;
}

/**
 * @brief Check any edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if rising or falling edge occurred
 */
bool GPIF_Edge(GPIF_t *gpif)
{
  return GPIF_Rise(gpif) || GPIF_Fall(gpif);
}

/**
 * @brief Check long rising edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if long rising edge occurred
 */
bool GPIF_RiseLong(GPIF_t *gpif)
{
  if(gpif->rise_long) {
    gpif->rise_long = false;
    return true;
  }
  return false;
}

/**
 * @brief Check long falling edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if long falling edge occurred
 */
bool GPIF_FallLong(GPIF_t *gpif)
{
  if(gpif->fall_long) {
    gpif->fall_long = false;
    return true;
  }
  return false;
}

/**
 * @brief Check any long edge and clear flag.
 * @param gpif Pointer to `GPIF_t` instance
 * @return True if long rising or long falling edge occurred
 */
bool GPIF_EdgeLong(GPIF_t *gpif)
{
  return GPIF_RiseLong(gpif) || GPIF_FallLong(gpif);
}

//-------------------------------------------------------------------------------------------------
