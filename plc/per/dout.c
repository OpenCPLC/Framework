#include "dout.h"
#include "log.h"

//-------------------------------------------------------------------------------------------------

/**
 * @brief Odczytuje aktualną częstotliwość sygnału PWM.
 * @param pwm Wskaźnik do struktury reprezentującej sygnał PWM.
 * @return Aktualna częstotliwość sygnału PWM [Hz].
 */
float PWM_GetFrequency(PWM_t *pwm)
{
  return (float)SystemCoreClock / pwm->prescaler  / pwm->auto_reload / (pwm->center_aligned + 1);
}

/**
 * @brief Ustawia częstotliwość dla sygnału PWM.
 * Kontroler PWM_t może być powiązany z kilkoma wyjściami cyfrowymi.
 * Zatem częstotliwość zostanie zmieniona na wszystkich powiązanych wyjściach cyfrowych.
 * @param pwm Wskaźnik do struktury reprezentującej sygnał PWM.
 * @param frequency Docelowa częstotliwość sygnału PWM.
 * @return Rzeczywista ustawiona częstotliwość sygnału PWM.
 */
float PWM_Frequency(PWM_t *pwm, float frequency)
{
  uint32_t auto_reload_prev = pwm->auto_reload;
  uint32_t prescaler_prev = pwm->prescaler;
  uint32_t prescaler = 1;
  uint32_t auto_reload;
  do {
    PWM_SetPrescaler(pwm, prescaler++);
    auto_reload = (float)SystemCoreClock / pwm->prescaler / frequency / (pwm->center_aligned + 1);
  } while(auto_reload > 0xFFFF);
  PWM_SetAutoreload(pwm, auto_reload);
  for(TIM_Channel_t CH = TIM_CH1; CH <= TIM_CH4; CH++) {
    if(pwm->value[CH]) {
      PWM_SetValue(pwm, CH, (uint32_t)((uint64_t)pwm->value[CH] * pwm->prescaler * pwm->auto_reload / auto_reload_prev / prescaler_prev));
    }
  }
  return PWM_GetFrequency(pwm);
}

/**  
 * @brief Odczytuje aktualną częstotliwość sygnału PWM powiązanego z wyjściem cyfrowym.
 * @param dout Wskaźnik do struktury reprezentującej wyjście tranzystorowe (TO) lub triakowe (XO).
 * @return Aktualna częstotliwość sygnału PWM [Hz]. Wartość 0 dla wyjść bez licznika, np. przekaźnikowych (RO).
 */
float DOUT_GetFrequency(DOUT_t *dout)
{
  if(!dout->pwm) return 0;
  return PWM_GetFrequency(dout->pwm);
}

/**
 * @brief Ustawia częstotliwość sygnału PWM powiązanego z wyjściem cyfrowym.
 * Kontroler PWM_t może być powiązany z kilkoma wyjściami cyfrowymi.
 * Zatem częstotliwość zostanie zmieniona na wszystkich powiązanych wyjściach cyfrowych.
 * @param dout Wskaźnik do struktury reprezentującej wyjście tranzystorowe (TO) lub triakowe (XO).
 * @param frequency Docelowa częstotliwość sygnału PWM.
 * @return Aktualna częstotliwość sygnału PWM [Hz]. Wartość 0 dla wyjść bez licznika, np. przekaźnikowych (RO).
 */
float DOUT_Frequency(DOUT_t *dout, float frequency)
{
  if(!dout->pwm) return 0;
  return PWM_Frequency(dout->pwm, frequency);
}

/**
 * @brief Ustawia wartość wypełnienia `duty` sygnału PWM na wyjściu cyfrowym.
 * Jeśli opcja `save` jest włączona, aktualna wartość jest zapisywana do EEPROM.
 * @param dout Wskaźnik do struktury reprezentującej wyjście tranzystorowe (TO) lub triakowe (XO).
 * @param duty Wartość wypełnienia [%] sygnału PWM (0% - 100%).
 * @return Rzeczywista ustawiona wartość wypełnienia `duty` na wyjściu cyfrowym.
 */
float DOUT_Duty(DOUT_t *dout, float duty)
{
  if(!dout->pwm) {
    return 0;
  }
  uint32_t old_value = dout->pwm->value[dout->channel];
  PWM_SetValue(dout->pwm, dout->channel, duty * dout->pwm->auto_reload / 100);
  dout->value = dout->pwm->value[dout->channel];
  duty = (float)dout->value * 100 / dout->pwm->auto_reload;
  if(dout->eeprom && (old_value != dout->value) && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
  return duty;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Ustawia stan wysoki na wyjściu cyfrowym.
 * Wyjściu przekaźnikowym (RO): wykonuje zwarcie na styku przekaźnika.
 * Wyjście tranzystorowe (TO): wystawia potencjał zasilania lub TCOM.
 * Wyjście triakowe (XO): przenosi napięcie przemienne z XCOM.
 * Jeśli opcja `save` jest włączona, aktualny stan jest zapisywany do EEPROM.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 */
void DOUT_Set(DOUT_t *dout)
{
  if(dout->pwm) {
    if(dout->value == dout->pwm->auto_reload) return;
    dout->value = dout->pwm->auto_reload;
    PWM_SetValue(dout->pwm, dout->channel, dout->value);
    if(dout->eeprom && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
  }
  else {
    if(dout->value) return;
    dout->value = true;
    if(dout->eeprom && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
  }
}

/**
 * @brief Ustawia stan niski na wyjściu cyfrowym.
 * Wyjściu przekaźnikowym (RO): pozostawia rozwarty styk przekaźnika.
 * Wyjście tranzystorowe (TO): pozostawia brak potencjału (floating).
 * Wyjście triakowe (XO): pozostawia brak potencjału (floating).
 * Jeśli opcja `save` jest włączona, aktualny stan jest zapisywany do EEPROM.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 */
void DOUT_Rst(DOUT_t *dout)
{
  if(dout->pwm) {
    if(dout->value == 0) return;
    dout->value = 0;
    PWM_SetValue(dout->pwm, dout->channel, dout->value);
    if(dout->eeprom && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
  }
  else {
    if(!dout->value) return;
    dout->value = false;
    if(dout->eeprom && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
    if(dout->relay) dout->_stun = tick_keep(DOUT_RELAY_DELAY);
  }
}

/**
 * @brief Zmienia stan na wyjściu cyfrowym.
 * Jeśli opcja `save` jest włączona, aktualny stan jest zapisywany do EEPROM.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 */
void DOUT_Tgl(DOUT_t *dout)
{
  if(dout->pwm) {
    if(dout->value) DOUT_Rst(dout);
    else DOUT_Set(dout);
  }
  else {
    dout->value = !dout->value;
    if(dout->eeprom && dout->save) EEPROM_Save(dout->eeprom, &dout->value);
  }
}

/**
 * @brief Ustawia wyjście cyfrowe na określony stan wysoki `true`, lub niski `false`.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @param value Jeśli `true`, funkcja jest równoważna `DOUT_Set`, w przeciwnym razie `DOUT_Rst`.
 */
void DOUT_Preset(DOUT_t *dout, bool value)
{
  if(value) DOUT_Set(dout);
  else DOUT_Rst(dout); 
}

/**
 * @brief Aktywuje impuls, czyli zmienia stan wyjścia cyfrowego na czas `time_ms`.
 * W przypadku aktywnego trybu PWM, na czas `time_ms` wyjście zostanie wyłączone.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @param time_ms Czas trwania impulsu w milisekundach.
 * @return Impuls zostanie wykonany jeśli `true`
 */
bool DOUT_Pulse(DOUT_t *dout, uint16_t time_ms)
{
  if(dout->pulse) return false;
  dout->_pulse = time_ms;
  return true;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Pobiera aktualny stan wyjścia.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @return Wartość `true` dla stanu wysokiego, `false` stanu dla niskiego.
 * W przypadku kanału PWM, wartoś cyfrową wypełnienia sygnału.
 */
uint32_t DOUT_State(DOUT_t *dout)
{
  if(dout->pwm) return dout->pwm->value[dout->channel];
  else return dout->gpio.set;
}

/**
 * @brief Sprawdza, czy wyjście cyfrowe jest w trakcie wykonywania impulsu.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @return Wartość `true` dla trwającego impulsu, `false` w przeciwnym razie.
 */
bool DOUT_IsPulse(DOUT_t *dout)
{
  return dout->pulse;
}

/**
 * @brief Pobiera aktualną wartość wypełnienia sygnału [%] na wyjściu cyfrowym.
 * Funkcja tylko dla wyjść z kanałami PWM.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @return Wartość `true` dla aktywnego stanu, `false` dla nieaktywnego.
 */
float DOUT_GetDuty(DOUT_t *dout)
{
  if(!dout->pwm) return 0;
  return (float)dout->value * 100 / dout->pwm->auto_reload;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Inicjalizuje określone wyjście cyfrowe.
 * W przypadku wyjść z kanałami PWM konieczna jest jego inicjalizacja z funkcji wywołującej.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 */
void DOUT_Init(DOUT_t *dout)
{
  if(dout->eeprom)
  {
    EEPROM_Init(dout->eeprom);
    if(!dout->save) EEPROM_Load(dout->eeprom, &dout->save);
    if(dout->save) EEPROM_Load(dout->eeprom, &dout->value);
    if(dout->relay) EEPROM_Load(dout->eeprom, &dout->cycles);
  }
  if(!dout->pwm) {
    dout->gpio.mode = GPIO_Mode_Output;
    GPIO_Init(&dout->gpio);
  }
}

static inline void DOUT_RelayCyclesInc(DOUT_t *dout)
{
  dout->cycles++;
  if(dout->eeprom) EEPROM_Save(dout->eeprom, &dout->cycles);
}

/**
 * @brief Pętla obsługująca wyjście cyfrowe.
 * Funkcję należy wywoływać w każdej iteracji pętli głównej lub dowolnego wątku.
 * Zalecane jest, aby była uruchamiana nie rzadziej niż 100 ms.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 */
void DOUT_Loop(DOUT_t *dout)
{
  if(tick_over(&dout->_stun)) return;
  if(dout->_pulse) { // Gdy zostanie ustawiony tryb pulse
    if(dout->relay && !DOUT_State(dout)) DOUT_RelayCyclesInc(dout);
    if(dout->pwm) {
      uint32_t value = dout->value ? 0 : dout->pwm->auto_reload;
      PWM_SetValue(dout->pwm, dout->channel, value);
    }
    else GPIO_Tgl(&dout->gpio);
    dout->_stun = tick_keep(dout->_pulse);
    dout->_pulse = 0;
    dout->pulse = true;
    return;
  }
  dout->pulse = false;
  if(DOUT_State(dout) != dout->value) {
    if(dout->pwm) PWM_SetValue(dout->pwm, dout->channel, dout->value);
    else {
      if(dout->value) {
        GPIO_Set(&dout->gpio);
        dout->value = true;
        if(dout->relay) DOUT_RelayCyclesInc(dout);
      }
      else {
        GPIO_Rst(&dout->gpio);
        dout->value = false;
      }
    }
    if(dout->relay) dout->_stun = tick_keep(DOUT_RELAY_DELAY);
  }
  return;
}

/**
 * @brief Określa, czy zachować wartość wyjścia cyfrowego po resecie.
 * @param dout Wskaźnik do struktury reprezentującej wyjście cyfrowe.
 * @param save Wartość `true` zapisywanie stanu wyjścia, natomiast `false` brak zapisu.
 */
void DOUT_Settings(DOUT_t *dout, bool save)
{
  if(dout->eeprom && save != dout->save) {
    EEPROM_Save(dout->eeprom, &dout->save);
  }
}

//-------------------------------------------------------------------------------------------------

#include "bash.h"

void DOUT_Print(DOUT_t *dout)
{
  #if(LOG_COLORS)
    DBG_String(ANSI_TEAL);
  #endif
  DBG_String(dout->name);
  #if(LOG_COLORS)
    DBG_String(ANSI_END);
  #endif
  if(dout->pwm) {
    float duty = DOUT_GetDuty(dout);
    #if(LOG_COLORS)
       DBG_String(ANSI_GREY" duty:"ANSI_END);
    #else
       DBG_String(" duty:");
    #endif
    DBG_Float(duty, 2);
    DBG_Char('%');
    float freq = PWM_GetFrequency(dout->pwm);
    #if(LOG_COLORS)
      DBG_String(ANSI_GREY" freq:"ANSI_END);
    #else
      DBG_String(" freq:");
    #endif
    DBG_Float(freq, 0);
    DBG_String("Hz");
  }
  else {
    if(DOUT_State(dout)) DBG_String(" ON ");
    else DBG_String(" OFF");
    if(DOUT_IsPulse(dout)) DBG_Char('*');
    if(dout->relay) {
      #if(LOG_COLORS)
        DBG_String(ANSI_GREY" cycles:"ANSI_END);
      #else
        DBG_String(" cycles:");
      #endif
      DBG_uDec(dout->cycles);
    }
  }
}

static struct {
  DOUT_t *dout[DOUT_BASH_LIMIT];
  uint16_t count;
  uint32_t hash[DOUT_BASH_LIMIT];
} bash;

void DOUT_Add2Bash(DOUT_t *dout)
{
  if(bash.count >= DOUT_BASH_LIMIT) {
    #if(LOG_COLORS)
      LOG_Error("Exceeded "ANSI_TURQUS"DOUT_t"ANSI_END" limit (max %u)", DOUT_BASH_LIMIT);
    #else
      LOG_Error("Exceeded DOUT_t limit (max %u)", DOUT_BASH_LIMIT);
    #endif
    return;
  }
  if(!dout->name) {
    #if(LOG_COLORS)
      LOG_Error("Object "ANSI_TURQUS"DOUT_t"ANSI_END" passed to BASH must be named", DOUT_BASH_LIMIT);
    #else
      LOG_Error("Object DOUT_t passed to BASH must be named", DOUT_BASH_LIMIT);
    #endif
    return;
  }
  bash.dout[bash.count] = dout;
  bash.hash[bash.count] = hash_djb2_ci(dout->name);
  bash.count++;
}

void DOUT_Bash(char **argv, uint16_t argc)
{
  BASH_Argc(2, 4);
  uint32_t argv1_hash = hash_djb2(argv[1]);
  if(argv1_hash == HASH_List) {
    const char *names[bash.count];
    for(uint16_t i = 0; i < bash.count; i++) {
      names[i] = bash.dout[i]->name;
    }
    #if(LOG_COLORS)
      LOG_Info("Digital outputs: "ANSI_TEAL"%a %s"ANSI_END, bash.count, names);
    #elif
      LOG_Info("Digital outputs: %a, %s", bash.count, names);
    #endif
    return;
  }
  DOUT_t *dout = NULL;
  for(uint16_t i = 0; i < bash.count; i++) {
    if(bash.hash[i] == argv1_hash) {
      dout = bash.dout[i];
      break;
    }
  }
  if(!dout) {
    #if(LOG_COLORS)
      LOG_Warning("Digital output "ANSI_TEAL"%s"ANSI_END" not found", argv[1]);
    #else
      LOG_Warning("Digital output %s not found", argv[1]);
    #endif
    return;
  }
  bool pulse_todo = false;
  if(argc >= 3) {
    switch(hash_djb2(argv[2])) {
      case HASH_Set: case HASH_On: case HASH_Enable: DOUT_Set(dout); break;
      case HASH_Rst: case HASH_Reset: case HASH_Off: case HASH_Disable: DOUT_Rst(dout); break;
      case HASH_Tgl: case HASH_Toggle: case HASH_Sw: case HASH_Switch: DOUT_Tgl(dout); break;
      case HASH_Pulse: case HASH_Impulse: case HASH_Burst: pulse_todo = true;
      case HASH_Duty: case HASH_Fill: {
        BASH_Argc(4);
        if(!str_is_u16(argv[3])) {
          LOG_ErrorParse(argv[3], "uint16_t");
          BASH_ArgvExit(3);
        }
        uint16_t value = str_to_int(argv[3]);
        if(pulse_todo) DOUT_Pulse(dout, value);
        else DOUT_Duty(dout, value);
        break;
      }
      default: BASH_ArgvExit(2);
    }
  }
  LOG_Info("Digital output %o", dout, &DOUT_Print);
}

//-------------------------------------------------------------------------------------------------