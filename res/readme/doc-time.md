# ⌚ Time tools [➥](../readme.md)

## Synchronizacja procesu

Zmienne nieulotrne

Gdy nie jest wymagana duża precyzja...

```c
#include "opencplc-uno.h"
#include "rng.h"

void Task(void)
{
  uint32_t delay_ms = rng(1000, 3000);
  delay(delay_ms);
}

int main(void)
{
  PLC_Init();
  while(1) {
    uint64_t tick = 0;
    while(1)
    {
      delay_until(&tick); // zaczekaj na zakończenie odliczania
      tick = gettick(seconds(5)); // rozpocznij odliczanie 5s
      DBG_String("TASK synchronized"); // wyświetlane co 5s
      Task(); // zadanie trwające 1-3 sekundy
      PLC_Loop();
    }
  }
}
```

## Wykonanie z opóźnieniem

Wykonywanie zadania z opóźnieniem

```c
#include "opencplc-uno.h"

int main(void)
{
  PLC_Init();
  while(1) {
    uint64_t tick = 0;
    while(1)
    {
      if(DIN_Rais(&DI1)) {
        tick = gettick(minutes(2));
        DBG_String("TASK scheduled 2m");
        // zadanie zaplanowane za 2 minuty
      }
      // something
      if(waitfor(&tick)) {
        RELAY_Set(&RO1);
        DBG_String("TASK completed");
        // zadanie wykonane
      }
      PLC_Loop();
    }
  }
}
```

### Harmonogram zadań CRON

```c
#include "opencplc-uno.h"
#include "cron.h"

void WednesdayNight(void)
{
  DBG_String("CRON wednesday-night");
  DBG_Enter();
}

void CheckInput(DIN_t *din)
{
  DBG_String("CRON din-");
  if(DIN_State(din)) DBG_String("high");
  else DBG_String("low");
  DBG_Enter();
}

void FirstDayMonth(void)
{
  DBG_String("CRON first-day-month");
  DBG_Enter();
}

static uint32_t stack_plc[64];
static uint32_t stack_cron[512];

int main(void)
{
  PLC_Init();
  CRON_Task(&WednesdayNight, NULL, CRON_NULL, RTC_WEDNESDAY, 23, 00); // Wednesday  23:00
  CRON_Task(&CheckInput,     &DI1, CRON_NULL, CRON_NULL,     06, 30); // Everyday   06:30
  CRON_Task(&FirstDayMonth,  NULL, 1,         CRON_NULL,     17, 45); // xxxx-xx-01 17:45
  thread(&PLC_Loop, stack_plc, sizeof(stack_plc));
  thread(&CRON_Main, stack_cron, sizeof(stack_cron));
  VRTS_Init();
  while(1);
}
```