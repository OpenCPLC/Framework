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
 * @brief Zwraca przefiltrowaną wartość pomiaru ADC, bez konwersji.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Wartość przetwornika ADC w zakresie liczby 16-bitowej.
 */
float AIN_Raw(AIN_t *ain)
{
  if(waitfor(&ain->tick)) return ain->value;
  uint16_t size = ain->count / 3;
  sort_inc_uint16(ain->data, ain->count);
  ain->value = average_uint16(&(ain->data[size]), size);
  ain->tick = gettick(AIN_AVERAGE_TIME_MS / 2);
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
  return resistor_divider_factor(3.3, 340, 160, 16) * raw;
}

/**
 * @brief Zwraca napięcie [V] na wejściu analogowym 'ain' z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Napięcie [V] lub kod błędu AIN_Error (over-voltage/under-voltage).
 */
float AIN_Voltage_V(AIN_t *ain)
{
  float volts = AIN_Volts(ain);
  if(volts > 10.2) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s over-voltage", ain->name);
    return AIN_Error_OverValue;
  }
  if(ain->mode_4_20mA & (volts < 1.8)) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s under-voltage", ain->name);
    return AIN_Error_UnderValue;
  }
  return volts;  
}

/**
 * @brief Zwraca prąd [V] na wejściu analogowym 'ain' z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Prąd [mA] lub kod błędu AIN_Error (over-current/under-current).
 */
float AIN_Current_mA(AIN_t *ain)
{
  float mamps = 2 * AIN_Volts(ain);
  if(mamps > 20.4) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s over-current", ain->name);
    return AIN_Error_OverValue;
  }
  if(ain->mode_4_20mA & (mamps < 3.6)) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s under-current", ain->name);
    return AIN_Error_UnderValue;
  }
  return mamps;
}

/**
 * @brief Zwraca wartość wejścia analogowego 'ain' w procentach [%] z walidacją błędów.
 * @param ain Wskaźnik na strukturę AIN_t reprezentującą wejście analogowe (AI).
 * @return Wartość w procentach [0-100%] lub kod błędu AIN_Error (over-value/under-value).
 */
float AIN_Percent(AIN_t *ain)
{
  float value = 10 * AIN_Volts(ain);
  if(value > 102) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s over-value", ain->name);
    return AIN_Error_OverValue;
  }
  if(ain->mode_4_20mA & (value < 18)) {
    LOG_Message(AIN_LOG_LEVEL, "Analog input %s under-value", ain->name);
    return AIN_Error_UnderValue;
  }
  if(ain->mode_4_20mA) {
    value = (value - 20) * 5 / 4;
    if(value < 0) value = 0;
  }
  if(value > 100) value = 100;
  return value;
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
