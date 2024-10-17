## ⚓ Content

- 👋 [OpenCPLC](#-opencplc-) - Wstęp
- 🥇 [Uno](#-uno-) - Nasz pierwszy sterownik
- 🪜 [Code](#-code-) - Porównanie języków SCL, LAD oraz C
- ⚙️ [Essential-tools](#%EF%B8%8F-essential-tools-) - Konfiguracja środowiska
- 🐞 [Programing-debugging](#-programing-debugging-) - Programowanie i debugowanie
- 🧵 [Multi-thread](#-multi-thread-) - Programowanie wielowątkowe
- 🧩 Examples - Przykłady _(not ready yet)_
    - 1\. [General IO](./doc/guide-io.md) - 🕹️ Wyjścia i wejścia
    - 2\. [Communication](./doc/guide-com.md) - 🔗 Komunikacja RS485 i I2C
    - 3\. [Time & Utils](./doc/guide-time.md) - ⌚ Zarządzanie czasem i przydatne funkcje

# 👋 OpenCPLC

Projekt zapewnia warstwę pośrednią pomiędzy Twoją aplikacją, a peryferiami mikrokontrolera. Trochę podobnie jak w **Arduino**, jednak bardziej w kierunku automatyki. Bez własnego IDE oraz angażowania C++.

W świecie technologii, programowanie staje się coraz bardziej złożone _(często na życzenie samych programistów)_. Niekiedy poziom skomplikowania aplikacji jest nieproporcjonalny do problemu, który rozwiązuje lub wartości, jaką dostarcza. Chcemy, aby nasze biblioteki były możliwie proste, interfejs intuicyjny, a nakład technologiczny minimalny. Wykorzystujemy dobrze znane narzędzia, takie jak [**Visual Studio Code**](https://code.visualstudio.com/), system kontroli wersji [**Git**](https://git-scm.com/) oraz język [**C**](https://www.learn-c.org/pl/), który pomimo swojego wieku nadal jest numerem jeden wśród programistów Embedded. Nic nie stoi więc na przeszkodzie, aby pojawiło się go trochę więcej w automatyce, co pozwoli iść branży z duchem IT!

Zapotrzebowanie na automatyków było, jest i będzie bardzo duże. W przeszłości, kiedy programistów było niewiele, a za automatykę brali się elektrycy, zaprojektowanie języka, jakim jest ladder logic _(LAD)_ było strzałem w dziesiątkę, bo wykorzystywało logikę znaną z elektryki. Obecnie sytuacja jest odwrotna, a kod w języku C często jest bardziej czytelny dla absolwentów kierunków technicznych niż drzewo logiczne złożone ze styków i cewek.

## 🥇 Uno [➥](#-content)

Pierwszy sterownik z linii **OpenCPLC** jakim jest **Uno** ma cechować się wszechstronnością ze względu na różnorodność peryferii.
Po zakupie urządzenie jest zaprogramowane jako moduł rozszerzeń do współpracy z zewnętrznym sterownikiem lub komputerem.
Istnieje możliwość programowania urządzenia bezpośrednio, aby w rezultacie urządzenie mogło działać jako sterownik **PLC**.
Ten sterownik jest kompatybilny zarówno z systemami **24V**, jak i **12V**, co może wyróżniać ten produkt na rynku.
Może być zasilany i płynnie sterować tymi napięciami z wyjść oraz odczytywać napięcie jako logiczną `1` na wejściach.
Za jego pomocą będzie można wdrażać systemy automatyki, gdzie standardem jest 24V, oraz systemy maszyn przemysłowych ze standardem 12V.


| Face                       | View                       |
| -------------------------- | -------------------------- |
| ![Face](/img/uno-face.png) | ![View](/img/uno-view.png) |

Sterownik najlepiej sprawdzi się w małych i średnich projektach z zakresu automatyki, takich jak:

- System nawadniania/naświetlania roślin
- Regulator temperatury/natlenienia w akwarium
- Automatyczne drzwi
- System monitoringu zużycia energii
- Automatyczny podlewacz trawnika/szklarni
- Automatyczny regulator prędkości wentylatora
- Inteligentny termostat - sterowanie piecem
- Automatyczny system podawania karmy dla zwierząt
- Inteligentny kurnik, symulujący krótszą dobę
- Inkubator jajek utrzymujący odpowiednią temperaturę

### Specyfikcaja

- Zasilanie **24V**/**12V**
- Mikrokontroler `STM32G0B1`
- Pamięć programu FLASH `512kB`
- Pamięć operacyjna RAM `144kB`
- Układy peryferyjne
  - 4x`TO` - Wyjścia tranzystorowe _(Płynna regulacja obwodów prądu stałego DC)_
  - 4x`RO` - Wyjścia przekaźnikowe 
  - 2x`XO` - Wyjścia triakowe _(Płynna regulacja obwodów prądu przemiennego AC)_
  - 1x`I2C` - Kanały dla czujników I2C/1WIRE
  - 4x`DI` - Wejścia cyfrowe AC/DC _(W tym szybki licznik)_
  - 1x`PT100` - Kanały pomiaru rezystancji
  - 2x`AI` - Wejścia analogowe _(0-10V lub 0-20mA)_
  - 2x`RS485` - Interfejs komunikacyjny _(Modbus RTU, BACnet MS/TP lub cBash)_
  - Przycisk _(np. do przywracania ustawień fabrycznych)_
  - Dioda LED ogólnego zastosowania _(info)_
- Wskaźniki LED dla układów peryferyjnych.
- Złącza o rozstawie `5.00mm`, które są rozłączne. _(prostsza instalacja)_
- Listwy z różną liczbą wyprowadzeń. _(utrudniając błędne połączenie)_
- Orientacja urządzenia od frontu, zapewniająca jak najlepszy dostęp do wyprowadzeń z rozdzielnicy

![Vect](./img/uno-vect.png)

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

| LAD Classic                    | LAD Set/Reset                      |
| ------------------------------ | ---------------------------------- |
| ![LAD-Classic](/img/lader.png) | ![LAD-SetReset](/img/lader-sr.png) |

#### System start-stop ANSI C

**_(mapowanie z użyciem zmiennych)_**

```c
#import "opencplc-uno.h"

bool start_button = false;
bool stop_button = false;
bool motor_running = false;

int main(void)
{
  PLC_Init();
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
#import "opencplc-uno.h"

DIN_t *start_button = &DI1;
DIN_t *stop_button = &DI2;
DOUT_t *motor_running = &RO1;

int main(void)
{
  PLC_Init();
  while(1) {
    if(DIN_Rais(stop_button)) {
      DOUT_Rst(motor_running);
    }
    else if(DIN_Rais(start_button)) {
      DOUT_Set(motor_running);
    }
    PLC_Loop();
  }
}
```

Nie zapominajmy, że język [C](https://pl.wikipedia.org/wiki/C_(j%C4%99zyk_programowania)) powstał jako język ogólnego przeznaczenia, zatem charakteryzuje się dużą uniwersalnością, szczególnie względem sandbox'ów dostarczanych przez producentów sterowników PLC.

## ⚙️ Essential-tools [➥](#-content)

Programowanie sterownika **Uno** oraz całej linii **OpenCPLC** należy rozpocząć od sklonowania repozytorium, co jest równoważne ze skopiowaniem wszystkich plików projektowych. Potrzeby jest do tego [klient GIT](https://git-scm.com/download/win). Po jego instalacji wystarczy włączyć konsolę systemową _(koniecznie w lokalizacji, gdzie chcemy, aby projekt został utworzony!)_ oraz wpisać komendę:

```bash
git clone https://github.com/OpenCPLC/Uno
```

W miejscu, gdzie została otwarta konsola stworzy się folder `Uno`, który zawiera całość projektu. Aby praca z projektem była efektywniejsza zalecamy zainstalować IDE [Visual Studio Code](https://code.visualstudio.com/). Jest to popularne, otwarto źródłowe rozwiązanie i właśnie dla niego zapewniamy wsparcie. Te narzędzia są dość uniwersalne i duża szansa, że już jest znasz i wykorzystujesz, jeśli nie to na pewno znajdziesz do nich wiele zastosowań. Dobrze podczas instalacji zaznaczyć dwa checkbox'y.

- [x] Add "Open with Code" action to Windows Explorer file context menu
- [x] Add "Open with Code" action to Windows Explorer directory context menu

Aby otworzyć projekt **VSCode** można skorzystać z menu kontekstowego:

![VSCode](/img/open-with-code.png)

Poza samym VSCode _(który póki co jest po prostu zaawansowanym edytorem tekstu)_ musimy zainstalować rozszerzenia, które uczynią z niego profesjonalne narzędzie do tworzenia, kompilowania i debugowania kodu w języku C.

| C/C++                    | Cortex-Debug                      |
| ------------------------ | --------------------------------- |
| ![Ext-C](/img/ext-c.png) | ![Cortex-Debug](/img/ext-dbg.png) |

Najkrótszą drogą do uruchomienia pierwszego projektu jest uruchomienie aplikacji 🔮`wizard.exe`. Zainstaluje ona **GNU Arm Embedded Toolchain**, **OpenOCD**, **Make** oraz ustawi odpowiednio zmienne systemowe, a także stworzy pliki konfiguracyjne dla projektu. Jeżeli nie chcemy, aby ktoś grzebał w naszym systemie, możemy przygotować sobie [konfiguracje ręcznie](./doc/custom-env.md). Niemniej, 🪄`wizard.exe` może okazać się pomocny, gdy będziemy chcieli, aby nowo dodane pliki zostały dołączone do projektu lub zmienić jego nazwę. Aby go uruchomić trzeba otworzyć konsolę jako 🛡️administrator w miejscu z projektem oraz wpisać:

```bash
./wizard.exe -n [naza-projektu]
```

Za flagą `-n` należy wprowadzić nazwę projektu _(oczywiście, nazwę należy wprowadzić bez nawiasów `[]`)_. Wizard umożliwia także wykorzystanie wersji sterownika z mniejszą ilością pamięci `-m`, wymuszenie innego [poziomu optymalizacji debugera](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html) `-o` oraz nadpisanie plików konfiguracyjnych projektu `-r`.

```bash
./wizard.exe -n [naza-projektu] -m 128kB -o 0g -r
```

Na zakończenie należy otworzyć ponownie konsolę i zweryfikować, czy wszystkie pakiety zostały zainstalowane poprawnie. Można to zrobić przy użyciu komendy `--version`.

```bash
arm-none-eabi-gcc --version
openocd --version
make --version
```

## 🐞 Programing-debugging [➥](#-content)

Narzędziem, które wykorzystujemy do programowania i debugowania, jest [STLINK-V3MODS](https://www.st.com/en/development-tools/stlink-v3mods.html) zamontowany na przejściówce umożliwiającej podłączenie kabla [SKEED8](https://www.we-online.com/en/components/products/WST_IDC_PRE_PRESSED_CONNECTOR?sq=490107670812S#490107670812S). Niechętnie zastosowaliśmy własny standard, jednak firma **ST Microelectronics** ❤️ wykorzystuje złącze STDC14, które jest drogie i trudno dostępne, oraz złącze TC2050-IDC, które okraja stlink o interfejs komunikacji `UART`.

Aby zacząć programować mikrokontroler na płytce sterownika, trzeba podłączyć zasilanie `VCC` i `GND` od 12V do 24V oraz podłączyć z jednej strony programator przewodem SKEED do płytki, a z drugiej strony przewodem micro `USB` do komputera.

![Stlink](./img/uno-stlink.png)

Konieczna może okazać się instalacja [sterownika do programatora](https://www.st.com/en/development-tools/stsw-link009.html).

Gdy przeszliśmy kroki opisane w [rozdziale wyżej](#%EF%B8%8F-essential-tools-) to wszystko jest gotowe do pracy. Kompilacja i programowanie odbywa się przy użyciu komendy **`make`**

```bash
make # kompilacja programu
make clr # usunięcie plików kompilacyjnych
make run # kompilacja i wgranie programu
```

Miejsce, w którym należy tworzyć aplikacje, to katalog [`src`](./src/), a głównym plikiem projektu jest [`main.c`](./src/main.c), znajdujący się w nim. Plik konfiguracyjny, w którym nadpisujemy definicje `#define`, to [`main.h`](./src/main.h). Pozostałe zasady ustala sam programista i mogą się różnić w zależności od projektu.

Aby debugować program, wystarczy nacisnąć `F5` w otwartym projekcie Visual Studio Code.

Podczas kompilacji tworzy się folde `build`, a w nim plik o nazwie projektu z rozszerzeniem `.hex`. Na etapie produkcyjnym może być on wgrywany na mikrokontroler za pomocą narzędzia [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html).

### Strumień danych wyjściowych `DBG`

W procesie tworzenia i testowania oprogramowania kluczową rolę odgrywa etap debugowania, który polega na identyfikowaniu, lokalizowaniu i eliminowaniu błędów w kodzie źródłowym. W tym celu przygotowano zestaw funkcji `DBG`, które wykorzystują interfejs UART do wypisywania zmiennych różnych typów. To rozwiązanie jest zdecydowanie bardziej efektywne od korzystania z implementacji funkcji `sprintf`.

```c
#include "opencplc-uno.h"

int main(void)
{
  PLC_Init();
  while(1) {
    char *text = "text";
    DBG_String("String "); DBG_String(text); DBG_Enter();
    DBG_String("Char   "); DBG_Char('$'); DBG_Enter();
    uint8_t array[] = { 49, 50, 51 };
    DBG_String("Array  "); DBG_Array(array, sizeof(array)); DBG_Enter();
    DBG_String("uDec   "); DBG_Dec(-69); DBG_Enter();
    DBG_String("Dec    "); DBG_uDec(48); DBG_Enter();
    DBG_String("Float  "); DBG_Float(21.37, 2); DBG_Enter();
    DBG_String("Hex    0x"); DBG_Hex8(0x6D); DBG_Enter();
    DBG_String("Bin    0b"); DBG_Bin8(0b11001010); DBG_Enter();
    DBG_String("Now    "); DBG_Now(); DBG_Enter();
    DBG_Enter();
    PLC_Loop();
    delay(1000);
  }
}
```

Wiadomości, które tworzymy, są wysyłane do komputera za pomocą `UART`'a wbudowanego w programator. Z poziomu komputera będą widziane jako **serial port** _(`COM` na systemie Windows)_. Wiadomości możemy odebrać za pomocą dowolnego terminala obsługującego komunikację szeregową, takiego jak [Realterm](https://realterm.sourceforge.io/) - _[download](https://sqrt.pl/Realterm-3.0.1.45.exe)_. Należy ustawić prędkość na **115200**bps, używając 8 bitów danych, 1 bitu stopu bez kontroli parzystości.

## 🧵 Multi-thread [➥](#-content)

Podczas implementacji operacji/funkcji blokujących w projekcie, czyli tych, gdzie rozpoczynamy pewne zadanie i oczekujemy na jego zakończenie, korzystanie z programowania wielowątkowego jest dobrą praktyką. W projekcie został zaimplementowany system zwalnia wątków [**VRTS**](https://github.com/Xaeian/VRTS). Pozwala to na tworzenie czytelnego kodu, gdzie w każdym wątku możemy obsłużyć różne funkcjonalności. Taką funkcjonalnością może być obsługa komunikacji **RS485**, gdzie jako master wysyłamy ramkę nadawczą, oczekujemy na odpowiedź urządzenia slave, a następnie analizujemy ją. Warto, aby w trakcie oczekiwania procesor zajmował się innymi zadaniami.

Aby lepiej to zobrazować, do [przykładu start-stop](#system-start-stop-ansi-c-mapowanie-z-użyciem-wskaźników) dodajmy miganie lampką, podłączoną do wyjścia `TO1`, gdy silnik pracuje. W głównej funkcji `main` zainicjujemy peryferia sterownika za pomocą `PLC_Init`. Następnie przekazujemy funkcje dla trzech wątków:

- `PLC_Thread` - główna pętla sterownika,
- `start_stop` - pętla obsługująca funkcję start-stop,
- `blinking` - pętla odpowiedzialna za miganie lampki.

Dla każdego wątku konieczne jest zarezerwowanie stosu _(`stack1`, `stack2`, `stack3`)_. Ważne jest precyzyjne oszacowanie potrzebnej pamięci dla każdego wątku. Po tej operacji wystarczy uruchomić system przełączania wątków za pomocą `VRTS_Init`. Trochę dużo, ale dzięki takiemu podejściu mamy trzy główne pętle, z których każda odpowiada za inny aspekt funkcjonalny programu, co będzie z łatwością się skalować, jak nasza aplikacja będzie rosła.

```c
#include "opencplc-uno.h"

static uint32_t stack1[64];
static uint32_t stack2[64];
static uint32_t stack3[64];

DIN_t *start_button = &DI1;
DIN_t *stop_button = &DI2;
DOUT_t *motor_running = &RO1;

int start_stop(void)
{
  while(1) {
    if(DIN_Rise(stop_button)) {
      DOUT_Rst(motor_running);
    }
    else if(DIN_Rise(start_button)) {
      DOUT_Set(motor_running);
    }
  }
  let();
}

DOUT_t *blink_light = &TO1;

void blinking(void)
{
  while(1) {
    if(DOUT_State(motor_running)) {
      DOUT_Set(blink_light);
      delay(200);
      DOUT_Rst(blink_light);
      delay(200);
    }
    else {
      DOUT_Rst(blink_light);
      delay(200);
    }
  }
}

int main(void)
{
  thread(&PLC_Thread, stack1, sizeof(stack1));
  thread(&start_stop, stack2, sizeof(stack2));
  thread(&blinking, stack3, sizeof(stack3));
  VRTS_Init();
  while(1);
}
```

Podczas korzystania z VRTS należy uwzględnić dwie istotne kwestie:

- Każdy wątek musi zawierać co najmniej jedną funkcję zwalniającą, taką jak `let` czy `delay`. W przeciwnym razie wątek zajmie rdzeń na stałe i uniemożliwi innym wątkom pracę.
- Każdy wątek musi być wyposażony w nieskończoną pętlę, która zapobiegnie opuszczeniu funkcji wątku. Tak jak robi się to w funkcji głownej `main`.

Gdy zdecydujemy się nie korzystać z wielowątkowości _(np. ramach testów pojedyńczego wątku)_ trzeba, w pliku [`main.h`](./src/main.h), ustawić definicję `VRTS_SWITCHING` na `0`
