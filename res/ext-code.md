# 📜 Co

## 🪜 Code [➥](#-content)

Porównajmy implementacje systemu **start-stop** w języku **ST**, **LAD _(ladder logic)_** oraz **ANSI C** z wykorzystaniem bibliotek OpenCPLC, biorąc pod uwagę zastosowanie dwóch różnych stylów mapowania zmiennych. Jeśli kod w języku C wydaje Ci się najbardziej przystępny i zrozumiały to prawdopodobnie ta droga jest dla Ciebie 😃

#### System start-stop ST

```st
PROGRAM main

VAR
  start_button: BOOL := FALSE;
  stop_button: BOOL := FALSE;
  motor_running: BOOL := FALSE;
END_VAR

start_button := I0.1
stop_button := I0.2
motor_running := Q0.1

IF stop_button THEN
  motor_running := FALSE;
ELSIF (start_button OR motor_running) THEN
  motor_running := TRUE;
END_IF

Q0.1 := motor_running

END_PROGRAM
```

#### System start-stop LAD

| LAD Classic                          | LAD Set/Reset                            |
| ------------------------------------ | ---------------------------------------- |
| ![LAD-Classic](res/images/lader.png) | ![LAD-SetReset](res/images/lader-sr.png) |

#### System start-stop ANSI C

**_(mapowanie z użyciem zmiennych)_**

```c
bool start_button = false;
bool stop_button = false;
bool motor_running = false;

int main(void)
{
  while(1) {
    start_button = DIN_State(&DI1);
    stop_button = DIN_State(&DI2);
    if(stop_button) {
      motor_running = false;
    }
    else if(start_button || motor_running) {
      motor_running = true;
    }
    RELAY_Preset(&RO1, motor_running);
    PLC_Loop();
  }
}
```

**_(mapowanie z użyciem wskaźników)_**

```c
DIN_t *start_button = &DI1;
DIN_t *stop_button = &DI2;
DOUT_t *motor_running = &RO1;

int main(void)
{
  PLC_Init();
  while(1) {
    if(DIN_Rise(stop_button)) {
      DOUT_Rst(motor_running);
    }
    else if(DIN_Rise(start_button)) {
      DOUT_Set(motor_running);
    }
    PLC_Loop();
  }
}
```












[^1]: _Sterowniki programowalne_, M. Pawlak, Politechnika Wrocławska, 2010

[^2]:	_Systematically reviewing the layered architectural pattern principles and their use to reconstruct software architectures_, Alvine B. Belle, Ghizlane El Boussaidi, Timothy C. Lethbridge, Segla Kpodjedo, Hafedh Mili, Andres Paz, arXiv preprint, `DOI: 10.48550/arXiv.2112.01644`