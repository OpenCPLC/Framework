#include "ain.h"

/**
 * @brief Sortuje dane z bufora ADC na osobne kanały AIN.
 * @param buff Wskaźnik do bufora z danymi ADC.
 * @param data Tablica wskaźników do danych dla poszczególnych kanałów.
 * @param channels Liczba kanałów ADC.
 * @param samples Liczba próbek na kanał.
 */
void AIN_Sort(uint16_t *buff, uint16_t channels, uint16_t samples, uint16_t data[channels][samples])
{
  for(uint16_t n = 0; n < samples; n++) {
    for(uint8_t cha = 0; cha < channels; cha++) {
      data[cha][n] = buff[(n * channels) + cha];
    }
  }
}

/**
 * @brief Ustawia dolny i górny próg błędu dla wejścia analogowego.
 * @param ain Wskaźnik na strukturę AIN_t.
 * @param down Dolny próg błędu.
 * @param up Górny próg błędu.
 * @param scale Opcja: napięcie[V], prąd[mA], procenty[%]
 */
void AIN_Threshold(AIN_t *ain, float down, float up, AIN_Threshold_e scale)
{
  ain->threshold_down = down / scale;
  ain->threshold_up = up / scale;
}

/**
 * @brief Zwraca przefiltrowaną wartość pomiaru ADC, bez konwersji.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Wartość przetwornika ADC w zakresie liczby 16-bitowej.
 */
float AIN_Raw(AIN_t *ain)
{
  if(tick_over(&ain->tick)) return ain->value;
  uint16_t size = ain->count / 3;
  sort_asc_uint16(ain->data, ain->count);
  ain->value = average_uint16(&(ain->data[size]), size);
  ain->tick = tick_keep(AIN_AVERAGE_TIME_MS / 2);
  LOG_Debug("Analog input %s raw-value: %F", ain->name, ain->value);
  return ain->value;
}

/**
 * @brief Zwraca napięcie [V] na wejściu analogowym 'ain' po przeliczeniu wartości pomiaru ADC.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Napięcie [V].
 */
static float AIN_Volts(AIN_t *ain)
{
  float raw = AIN_Raw(ain);
  float volts = resistor_divider_factor(3.3, 340, 160, 16) * raw;
  if(volts > 10.25 || (ain->threshold_up && volts > ain->threshold_up)) return AIN_Error_OverValue;
  if((ain->mode_4_20mA && (volts < 0.1)) || volts < ain->threshold_down) return AIN_Error_UnderValue;
  return volts;
}

static void AIN_PrintError(AIN_t *ain, float value, char *subject)
{
  if(value == AIN_Error_OverValue) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s over-%s", ain->name, subject);
  }
  else if(value == AIN_Error_UnderValue) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s under-%s", ain->name, subject);
  }
}

/**
 * @brief Zwraca napięcie [V] na wejściu analogowym 'ain' z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Napięcie [V] lub kod błędu AIN_Error (over-voltage/under-voltage).
 */
float AIN_Voltage_V(AIN_t *ain)
{
  float volts = AIN_Volts(ain);
  AIN_PrintError(ain, volts, "voltage");
  return volts;  
}

/**
 * @brief Zwraca prąd [V] na wejściu analogowym 'ain' z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Prąd [mA] lub kod błędu AIN_Error (over-current/under-current).
 */
float AIN_Current_mA(AIN_t *ain)
{
  float volts = AIN_Volts(ain);
  AIN_PrintError(ain, volts, "current");
  return volts * 2;
}

/**
 * @brief Zwraca wartość wejścia analogowego 'ain' w procentach [%] z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Wartość w procentach [0-100%] lub kod błędu AIN_Error (over-value/under-value).
 */
float AIN_Percent(AIN_t *ain)
{
  float volts = AIN_Volts(ain);
  AIN_PrintError(ain, volts, "value");
  float percent = 10 * volts;
  if(ain->mode_4_20mA) {
    percent = (percent - 20) * 5 / 4;
    if(percent < 0) percent = 0;
  }
  if(percent > 100) percent = 100;
  return percent;
}

/**
 * @brief Zwraca wartość wejścia analogowego 'ain' w postaci znormalizowanej [0-1].
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Znormalizowana wartość w zakresie [0-1].
 */
float AIN_Normalized(AIN_t *ain)
{
  return AIN_Percent(ain) / 100;
}

/**
 * @brief Zwraca napięcie [V] na potencjometrze na podstawie przeliczonej wartości pomiaru ADC.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Napięcie w zakresie [0-3.3V].
 */
float POT_Voltage_V(AIN_t *ain)
{
  float raw = AIN_Raw(ain);
  return volts_factor(3.3, 16) * raw;
}

/**
 * @brief Zwraca wartość na potencjometrz wyrażoną w procentach [%].
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Wskazanie potencjometru w proecntach [0-100%].
 */
float POT_Percent(AIN_t *ain)
{
  float volts = POT_Voltage_V(ain);
  return volts * 100 / 3.3;
}

/**
 * @brief Zwraca wartość na potencjometrze w postaci znormalizowanej [0-1].
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Znormalizowana wartość wskazania potencjometru w zakresie [0-1].
 */
float POT_Normalized(AIN_t *ain)
{
  return POT_Percent(ain) / 100;
}
