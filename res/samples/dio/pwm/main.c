#include "opencplc.h" // Import funkcji sterownika

/**
 * @name  Projekt: dio/pwm
 * @brief Modyfikacja parametrów sygnału proporcjonalnego PWM (wypełnienie, częstotliwość)
 *        wraz z równoległym pomiarem tych parametrów za pomocą szybkiego licznika.
 *        Wątek `loop` zmienia częstotliwość (lub wypełnienie) sygnału na wyjściu TO1.
 *        Zmiana wartości jest realizowana po wciśnięciu przycisku BTN1.
 *        Wątek `monit` mierzy i wyświetla zmierzoną częstotliwość i wypełnienie.
 * @short TO1 --> DI1 (połączenie wymagane)
 *        TO1 --> 1kΩ --> GND (połączenie dodatkowe)
 * @note  Wyjście cyfrowe jest typu P (Source), a nie Push-Pull,
 *        więc żeby przebieg był lepszej jakości, trzeba wymusić przepływ prądu.
 */

// Zmiana wypełnienia sygnału proporcjonalnego PWM
float next_duty(float duty)
{
  duty += 10.0; // Zwiększenie wypełnienia o 10%
  if(duty > 100.0) duty = 0.0; // Reset wypełnienia do 0% po przekroczeniu 100%
  DOUT_Duty(&TO1, duty); // Ustaw wartość wypełnienia wyjścia TO1
  LOG_Warning("Modify duty: %.0f%%", duty);
  return duty;
}

// Zmiana częstotliwości sygnału proporcjonalnego PWM
float next_freq(float freq)
{
  freq /= 2; // Zmniejszenie częstotliwości 2-krotnie
  if(freq < 10.0) freq = 2000.0; // Ustawienie częstotliwości na 2kHz, gdy spadnie poniżej 10Hz
  // Ustaw wartość częstotliwości wyjścia TO1 (oraz pozostałych, które dzielą ten sam TIM'er)
  DOUT_Frequency(&TO1, freq); 
  LOG_Warning("Modify frequency: %.0fHz", freq);
  return freq;
}

// Wątek kontrolujący sygnał na wyjciu TO1
void loop(void)
{
  float duty = 50.0; // Początkowo ustaw 50% wypełnienia
  DOUT_Duty(&TO1, duty); // Ustaw wartość wypełnienia wyjścia TO1
  LOG_Info("Init duty: %.0f%%", duty);
  // Pobranie wartości częstotliwości sygnału proporcjonalnego PWM
  float freq = DOUT_GetFrequency(&TO1);
  // Częstotliwość jest często wspólna dla kilku wyjść, gdyż wykorzystują ten sam TIM'er
  LOG_Info("Init frequency: %.2fHz", freq);
  while(1) {
    if(DIN_Fall(&BTN1)) { // Gdy przycisk wciśnięty...
      // duty = next_duty(duty); // Zmiana wypełnienia sygnału
      freq = next_freq(freq); // Zmiana częstotliwości sygnału
    }
    let(); // Zwolnienie rdzenia
  }
}

// Wątek monitorujący (mierzący) sygnał na wejściu DI1
void monit(void)
{
  float duty, freq;
  while(1) {
    duty = DIN_Duty_Percent(&DI1); // Pobierz pomiar wypełnienia [%]
    freq = DIN_Frequency_Hz(&DI1); // Pobierz pomiar częstotliwości [Hz]
    LOG_Info("Fast counter: %.0f%% %.2fHz", duty, freq); // Wyświetl pomiary
    delay(1000); // ... z interwałem 1s
  }
}

stack(stack_plc, 256); // Stos pamięci dla wątku PLC
stack(stack_dbg, 256); // Stos pamięci dla wątku debug'era (logs + bash)
stack(stack_loop, 1024); // Stos pamięci dla funkcji loop
stack(stack_monit, 1024); // Stos pamięci dla funkcji monit

int main(void)
{
  DI1.fast_counter = true; // Aktywacja szybkiego licznika na wejściu DI1
  // DI1.pwmi->prescaler = 1; // Pomiar wysokich częstotliwości: > 10kHz
  // DI1.pwmi->prescaler = 1024; // Pomiar niskich częstotliwości: < 10Hz
  thread(PLC_Thread, stack_plc); // Dodanie wątku sterownika
  thread(DBG_Loop, stack_dbg); // Dodanie wątku debug'era (logs + bash)
  thread(loop, stack_loop); // Dodanie funkcji loop jako wątek
  thread(monit, stack_monit); // Dodanie funkcji loop jako wątek
  vrts_init(); // Włączenie systemy przełączania wątków VRTS
  while(1); // W to miejsce program nigdy nie powinien dojść
}