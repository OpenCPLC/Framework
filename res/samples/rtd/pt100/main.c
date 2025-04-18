#include "opencplc.h" // Import funkcji sterownika

void loop(void)
{
  while(1) {    
    // Pobieranie wartości temperatury
    float temp = RTD_Temperature_C(&RTD1);
    // Wyświetlanie wartości temperatury
    LOG_Info("Temperature: %f°C", temp);
    // Oczekiwanie 2 sekundy
    delay(1000);
  }
}

stack(stack_plc, 256); // Stos pamięci dla wątku PLC
stack(stack_dbg, 256); // Stos pamięci dla wątku debug'era (logs + bash)
stack(stack_rtd, 256); // Stos pamięci dla funkcji rtd
stack(stack_loop, 1024); // Stos pamięci dla funkcji loop

int main(void)
{
  // Konfiguracja czujnika RTD do pracy z termometrem PT100
  // Wartości konfiguracyjne muszą być ustawione przed uruchomieniem systemu VRTS
  RTD1.nominal_ohms = RTD_Type_PT100; // Czujnik PT100 o nominalnej rezystancji 100Ω
  RTD1.wire = RTD_Wire_3; // Czujnik 3-przewodowy
  RTD1.reject = RTD_Reject_50Hz; // Odfiltrowywanie zakłóceń z sieci 50Hz
  // RTD1.oversampling = false; // Wykonaj pojedynczy pomiar
  RTD1.oversampling = 7; // Wykonaj 7 pomiarów i uśrednij ich wartość
  RTD1.interval_ms = 1000; // Okres wykonywania pomiarów (zbyt częste pomiary mogą powodować samonagrzewanie)
  RTD1.expiry_ms = 2000; // TODO...
  thread(PLC_Thread, stack_plc); // Dodanie wątku sterownika
  thread(DBG_Loop, stack_dbg); // Dodanie wątku debug'era (logs + bash)
  thread(loop, stack_loop); // Dodanie funkcji loop jako wątek
  vrts_init(); // Włączenie systemy przełączania wątków VRTS
  while(1); // W to miejsce program nigdy nie powinien dojść
}