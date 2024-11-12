## 🎛️ Wejścia analogowe **`AI`**

Wejście analogowe umożliwia pomiar napięcia w zakresie **0-10V** oraz prądu w zakresie **0-20mA**. Zakres prądu można zawęzić do **4-20mA** poprzez ustawienie flagi `mode_4_20mA` na `true`. Do odczytu wskazań z wejść analogowych dostępne są następujące funkcje:

- `AIN_Voltage_V`: zwraca wartość napięcia,
- `AIN_Current_mA`: zwraca wartość prądu,
- `AIN_Percent`: przelicza wartość prądu lub napięcia na zakres **0-100%** _(min-max)_,
- `AIN_Normalized`: przelicza wartość prądu lub napięcia na skalę znormalizowaną **0-1**.

Wszystkie funkcje zwracają zmienną typu float, którą przed przeliczeniem należy sprawdzić za pomocą `AIN_Error` lub `AIN_OK`, aby upewnić się, że wartość mieści się w oczekiwanym zakresie.

W podstawowym przykładzie wykorzystane są dwa wejścia: **`AI1`** jako wejście **napięciowe** i **`AI2`** jako wejście **prądowe**:

```c
// Pobieranie wartości wejść analogowych AI
void loop(void)
{
  // Ustawienie trybu pracy AI2 na 4-20mA
  AI2.mode_4_20mA = true;
  while(1) {
    // Odczyt napięcia z wejścia analogowego AI1
    float V = AIN_Voltage_V(&AI1);
    if(AIN_Error(V)) {
      // Obsługa błędu dla napięcia poza spodziewanym zakresem
      // LOG z poziomem AIN_LOG_LEVEL będzie generowany automatycznie
    }
    else {
      // Logowanie odczytanego napięcia
      LOG_Info("Analog input AI1 voltage: %fV", V);
    }
    // Odczyt prądu z wejścia analogowego AI2
    float mA = AIN_Current_mA(&AI2);
    if(AIN_OK(mA)) {
      // Logowanie odczytanego prądu
      LOG_Info("Analog input AI2 current: %fmA", mA);
    }
    else {
      // Obsługa błędu dla prądu poza spodziewanym zakresem
      // LOG z poziomem AIN_LOG_LEVEL będzie generowany automatycznie
    }
    // Pomiary wykonywane co ok. 1s
    delay(1000);
  }
}
```

🧩 Kompletny przykład: [Wejścia analogowe AI](./examples/ain-basic/main.c)

Możesz go szybko uruchomić, bedąc w przestrzeni roboczej, wpisując w konsoli:

```bash
./wizard.exe -n example-ain-basic -p opencplc/res/examples/ain-basic
make run
```

Różne sterowniki PLC oferują zróżnicowaną liczbę wejść analogowych `AI`, co przedstawia poniższa tabela:

|         Uno         |         DIO          |       🎚️AIO          |         Eco          |
| :-----------------: | :------------------: | :------------------: | :------------------: |
| **2**: `AI1`, `AI2` | **4**: `AI1` – `AI4` | **8**: `AI1` – `AI8` | **4**: `AI1` – `AI4` |

### Skalowanie

W aplikacjach z wejściami i wyjściami analogowymi podłącza się czujniki, przetworniki oraz potencjometry, które mierzą różne wielkości fizyczne, takie jak temperatura, ciśnienie czy prędkość obrotowa. W celu uzyskania właściwych wyników, wartości te należy przeskalować na docelowe jednostki, uwzględniając charakterystykę danego czujnika.

#### Math-Style

Metoda _Math-Style_ polega na traktowaniu przetwarzanych wartości jako funkcji liniowej, dla której można obliczyć parametry `a` i `b`. Te parametry są następnie wykorzystywane do konwersji zmierzonych wartości na docelową jednostkę _(np. temperaturę)_. Funkcja liniowa przekształca prąd w zakresie 4-20mA na temperaturę, zgodnie z równaniem:

$$ temperatura = a × prąd + b $$

W przykładzie poniżej wartości `a`, jako `PARAM_A` i `b`, jako `PARAM_B` zostały wyliczone w odniesieniu do minimalnej i maksymalnej temperatury.

```c
// Definicja zakresu temperatury
#define TEMPERATURE_MIN_4mA  -40.0 // Minimalna temperatura [°C] przy 4mA
#define TEMPERATURE_MAX_20mA 125.0 // Maksymalna temperatura [°C] przy 20mA
// Obliczenia parametrów a, b dla funkcji liniowej,
// aby wyznaczyć temperaturę na podstawie prądu
#define PARAM_A ((TEMPERATURE_MAX_20mA - TEMPERATURE_MIN_4mA) / (20.0 - 4.0))
#define PARAM_B (TEMPERATURE_MIN_4mA - (PARAM_A * 4))

// Mapowanie AI1 na nazwę temperature_ai z użyciem wskaźnika
static AIN_t *temperature_ai = &AI1;

// Funkcja zwracająca wartość temperatury na podstawie aktualnego prądu
static float getTemperature(void)
{
  float current_mA = AIN_Current_mA(temperature_ai); // Pobierz aktualny prąd
  if(AIN_Error(current_mA)) { // Sprawdzenie błędu pomiaru
    LOG_Error("Temperature measurement fault"); // Logowanie błędu
    return (float)0xFFFF; // Zwróć wartość błędu
  }
  // Obliczenie temperatury z funkcji liniowej
  float temperature = (PARAM_A * current_mA) + PARAM_B;
  // Informacja o zmierzonej temperaturze
  LOG_Info("Temperature: %f°C", temperature);
  return temperature;
}

// Funkcja skalująca prąd na temperaturę (math-style)
static void scaleMathStyle(void)
{
  temperature_ai->mode_4_20mA = true; // Ustawienie trybu 4-20mA dla AI
  while(1) {
    float temperature = get_temperature(); // Pobierz wartość temperatury
    // TODO: Użycie wartości temperatury
    delay(1000); // Pomiar wykonywany co ok. 1s
  }
}
```

#### PLC-Style

W metodzie PLC-Style wartości są najpierw **normalizowane** do zakresu **0-1**, a następnie przeskalowane do oczekiwanego przedziału. Takie podejście jest często stosowane w programowalnych sterownikach logicznych _(PLC)_, gdzie po przetworzeniu i normalizacji można łatwo przeliczyć wynik na docelowe jednostki. Na przykład, prąd w zakresie `4-20mA` jest najpierw normalizowany do wartości **0-1**, a następnie przeskalowany na określony zakres temperatury.

```c
// Definicja zakresu temperatury
#define TEMPERATURE_MIN  -60.0 // Minimalna temperatura [°C]
#define TEMPERATURE_MAX 100.0 // Maksymalna temperatura
// Definicja zakresu prądu wejściowego
#define ERROR_VAL (float)0xFFFF // Wartość zwracana w przypadku błędu

// Mapowanie AI1 na nazwę AI_Temperature z użyciem definicji
#define AI_Temperature AI1

// Funkcja skalująca prąd na temperaturę (plc-style)
static void scalePLCStyle(void)
{
  float temperature;
  AI_Temperature.mode_4_20mA = true; // Ustawienie trybu 4-20mA dla AI
  while(1) {
    // Pobierz znormalizowaną wartość prądu [0-1]
    float normalized_current = AIN_Normalized(&AI_Temperature);
    if(AIN_OK(normalized_current)) { // Sprawdzenie, czy pomiar jest prawidłowy
      // Przeskalowanie do temperatury
      temperature = normalized_current * (TEMPERATURE_MAX - TEMPERATURE_MIN) + TEMPERATURE_MIN;
      LOG_Info("Temperature: %f°C", temperature); // Logowanie zmierzonej temperatury
    }
    else {
      temperature = ERROR_VAL; // Wartość w przypadku błędu pomiaru
      LOG_Error("Temperature measurement fault"); // Logowanie błędu pomiaru
    }
    // TODO: Użycie wartości temperatury
    delay(1000); // Pomiar wykonywany co ok. 1s
  }
}
```

🧩 Kompletny przykład: [Skalowanie wejść analogowych AI](./examples/ain-scale/main.c)

Możesz go szybko uruchomić, bedąc w przestrzeni roboczej, wpisując w konsoli:

```bash
./wizard.exe -n example-ain-scale -p opencplc/res/examples/ain-scale
make run
```

### Extras

W sterownikach **OpenCPLC** do wejściowych wartości analogowych zaliczamy wskazania z potencjometrów oraz napięcie zasilania**.

**Potencjometry** są ciekawym sposobem regulacji parametrów ciągłych wpływających na działanie maszyny lub systemu. Szczególnie sprawdzają się w małych projektach, gdzie wygodniej jest użyć śrubokrętu do odpowiedniego przekręcenia potencjometru w lewo lub w prawo, niż korzystać z komputera z odpowiednim interfejsem.

W różnych sterownikch są różne wartości

|     Uno      |              DIO              |  AIO  |          Eco           |
| :----------: | :---------------------------: | :---: | :--------------------: |
| **1**: `POT` | **3**: `POT1`, `POT2`, `POT3` | **0** | **6**: `POT1` – `POT6` |


Monitorowanie **napięcia zasilania** jest istotne w systemach, gdzie może ono być niestabilne lub pochodzić z różnych źródeł o różnej wartości nominalnej. Znając napięcie zasilania, aplikacja może reagować na zbyt niskie, zbyt wysokie lub niestabilne napięcie, unikając wykonywania operacji obarczonych ryzykiem w takich warunkach.

W przykładzie odczytywana jest wartość z potencjometru **`POT1`** oraz napięcie zasilania sterownika:

```c
// Pobieranie nastaw z potencjometrów POT oraz pamiar napięcia zasilania
void loop(void)
{
  while(1) {
    // Odczyt wartości potencjometru
    float value = POT_Percent(&POT1);
    LOG_Info("Potentiometer value: %f%%", value);
    // Odczyt napięcia zasilania
    float supply = VCC_Voltage_V();
    LOG_Info("Power supply: %fV", supply);
    // Odczekaj 1s
    delay(1000);
  }
}
```

🧩 Kompletny przykład: [Obsługa potencjometru i pomiar napięcia zasilania](./examples/ain-extras/main.c)

Możesz go szybko uruchomić, bedąc w przestrzeni roboczej, wpisując w konsoli:

```bash
./wizard.exe -n example-ain-extras -p opencplc/res/examples/ain-extras
make run
```
