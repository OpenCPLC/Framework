## ⌨️ Wejścia cyfrowe **`DI`**

Funkcja `DIN_RawState` zwraca bezpośrednią wartość z wejścia mikrokontrolera.
Wejście `TO1` zostaje aktywowane, gdy pojawia się sygnał rzeczywisty/chwilowy.
Funkcja `DIN_State` zwracają odfiltrowaną wartość, uwzględniając minimalne czasy narastania i opadania sygnału.
Wejście `TO2` zostaje aktywowane, gdy pojawia się sygnał odfiltrowany.
Przed uruchomieniem wątków, w tym inicjalizacją sterownika, można zastosować wartości opóźnień narastania sygnału **`ton_ms`** oraz opadania sygnału **`toff_ms`**
Test ten pozwala na obserwację opóźnień oraz poprawność działania wejścia cyfrowego `DI1`

```c
#include "opencplc.h"
#include "main.h"

static uint32_t stack_plc[256];
static uint32_t stack_loop[256];

void loop(void)
{
  delay(200); // Odczekanie chwili, aż sygnały na wejściach cyfrowych się ustabilizują
  DBG_String("DIN test");
  DBG_Enter();
  while(1) {
    if(DIN_RawState(&DI1)) DOUT_Set(&TO1);
    else DOUT_Rst(&TO1);
    if(DIN_State(&DI1)) DOUT_Set(&TO2);
    else DOUT_Rst(&TO2);
    let();
  }
}

// Funkcja główna
int main(void)
{
  DI1.gpif.ton_ms = 2000; // Ustawia czas narastania sygnału na 2 sekundy
  DI1.gpif.toff_ms = 500; // Ustawia czas opadania sygnału na 500 ms
  thread(&PLC_Thread, stack_plc, sizeof(stack_plc));
  thread(&loop, stack_loop, sizeof(stack_loop));
  VRTS_Init();
  while(1);
}
```