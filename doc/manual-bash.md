## BASH [➥](#./readme.md)

Interfejs deweloperski, który umożliwia diagnozę, testowanie oraz konfigurację urządzenia.
Dział podobnie jak **bash** znany z systemów linux z tą różnicą, że komendy należy przesyłać w paczkach, ponieważ `Enter` nie oznacza końca instrukcji.
Instrukcja kończy się w momencie, kiedy poprzez okres ok. `3.5` znaku nic nie zostanie wysyłane/odebrane.
Podczas wpisywania komend wielkość znaków nie ma znaczenia. Dostępne są komendy:

- [FILE](#bash-file-) - Zarządzanie plikami
- [PWR](#bash-pwr-) - Kontroler zasilania
- [RTC](#bash-rtc-) - Zegar czasu rzeczywistego
- [UID](#bash-uid-) - Unikalny numer ID
- [RGB](#bash-rgb-) - Dioda RGB
- [BTN](#bash-btn-) - Przycisk
- [ADC](#bash-adc-) - Przetwornik analogowo-cyfrowy
- [SIM](#bash-sim-) - Modem SIM
- [EVENT](#bash-event-) - Zdarzenia i logi
- [TIMETABLE](#bash-timetable-) - Tablica zadań

Komendy z grup `FILE`, `PWR`, `RTC`,` UID` są zawsze dostępne. Pozostałe należy włączć poprzez dodanie definicji:

```cpp
#define INCLUDE_APPNAME 1
```

Najlepiej w pliku konfiguracyjnym `cfg.h`. Wówczas domyślnie funkcje **BASH** dla danej aplikacji są dostępne.
Jednak zawsze można je wyłączyć:

```cpp
#define APPNAME_BASH 0
```

Gdy włączymy debugger, cały moduł bash zostanie dezaktywowany:

```cpp
#define INCLUDE_DBG 0
```

### Bash FILE [➥](#bash-)

Wyświetlanie listy plików

    FILE list
    >> FILE [count] 0..[count-1]
    >>   [file-nbr]:[file-name]
    >>   ...

W aplikacjach potrzebujące pliki wywoływane przez powłokę **bash** wykorzystują plik podręczny `cache`. Ustawiając pliki podręczne, należy przy pomocy numeru `file-nbr`.

    FILE cache [cache-nbr]
    >> FILE [name] size:[size] limit:[limit] mutex:{0|1} (flash:{page})

W aplikacjach gdzie realizowany jest jednocześnie zapis i odczyt, stosowane są pliki podręczne `tx` oraz `rx`.
Rolę `tx` i `rx` może pełnić ten sam plik.

    FILE cache [tx-nbr] [rx-nbr]
    >> FILE [tx-name] ...
    >> FILE [rx-name] ...

    FILE load
    FILE load [limit]
    FILE load [limit] [offset]

Zapis do pliku. Argument `save` spowoduje wyczyszczenie pliku przed zapisem, w odróżnieniu od `append`, które będzie skutkować dopisaniem do pliku.

    FILE {save|append}
    FILE {save|append} [pack]

W parametrze `pack` przekazać liczbę paczek, z którym będzie składać się przesyłany plik (`save`) lub jego fragment (`append`).

### Bash PWR [➥](#bash-)

Resetowanie aplikacji.
W przypadku użycia parametru `now` aplikacja zostanie zresetowana natychmiastowo i nie odpowie.

    PWR restart {now}
    >> PWR restart

Usypianie mikrokontrolera.
W przypadku użycia parametru `now` mikrokontroler natychmiastowo pójdzie spać i nie odpowie.

    PWR sleep [mode] {now}
    >> PWR sleep:[mode]

Lista opcji: `mode`

- stop 0: `0`
- stop 1: `1`
- standby with sRAM: `2`
- standby: `3`
- shutdown: `4`

### Bash RTC [➥](#bash-)

Pobranie daty i godziny `RTC_Datatime_t` w systemie

    RTC
    >> RTC [Y]-[M]-[D] [h]-[m]-[s] [day]

Ustawienie daty i godziny `RTC_Datatime_t` w systemie

    RTC [Y]-[M]-[D] [h]-[m]-[s]
    >> RTC set-by-datatime

    RTC [timestamp]
    >> RTC set-by-timestamp

Resetowanie daty i godziny w systemie

    RTC reset
    RTC rst
    >> RTC reset

### Bash UID [➥](#bash-)

Pobranie numeru **UID** z mikroprocesora odbywa się przy użyciu komendy `uid`

    UID
    >> UID [12-byte-uid]

### Bash RGB [➥](#bash-)

Zapalenie lub miganie diody RGB wywołuje się przy pomocy komendy `rgb` wysłać po kolei następujące kody stanów diod.
Stany wysyłane są jako łańcuch string. Ostatni stan z łańcucha utrzyma się na diodzie do momentu wysłania następnej komendy `rgb`:
Jeżeli projekt wykorzystuje diodę z 2 kolorami, niektóre stany będą nieobsługiwane.

    RGB [code-1] [code-2] ... [code-last]

Lista stanów: `code`

- dioda wyłączona _(off)_: `0`
- kolor czerwony _(red)_: `1`
- kolor zielony _(green)_: `2`
- kolor niebieski _(blue)_: `3`
- kolor żółty _(yellow)_: `4`
- kolor turkusowy _(cyan)_: `5`
- kolor fioletowy _(magenta)_: `6`
- kolor biały _(white)_: `7`

Przykład mignięcia 2 razy czerwoną diodą

    RGB 1 0 1 0

### Bash BTN [➥](#bash-)

Aby wymusić zdarzenie `click` lub `press` bez kliknięcia lub przyciśnięcia przycisku można użyć komendy `btn`.
Nawet w przypadku włączonego debuggera, tak wywołane zdarzenie, nie będzie zwracało w debuggerze wpisu.

    BTN {click|press}

### Bash ADC [➥](#bash-)

Uruchamia konwersje analogowo-cyfrową zgodnie z wykonaną konfiguracją. Dostępne są tryby pojedynczy `signle`:

    ADC
    ADC single
    >> ADC single [adc-0] [adc-1] ... [adc-last]

A także tryb ciągły `record`, który przekazuje zarejestowane przebiegi do pliku podręcznego `cache`.
W prypadku 

Trzeba podać częstotliwość pomiarów, jednak trzeba mieć na uwadzę, aby konfiguracja pozwalała 

    ADC record [freq]Hz [time]s
    >> ADC record [file] [freq]Hz [time]ms

Zw Wymaga włączenia funkcji `RECORD`

```cpp
#define ADC_INCLUDE_RECORD 1
```

### Bash SIM [➥](#bash-)

Pozwala wywoływać funkcję aplikacji `sim`. Podczas wysyłania i odbierania wykorzystuje pliki podręczne `tx` i `rx`.

Inicjalizacja modułu sim

    SIM init
    >> SIM init {ok|err} [power]dbm [I.P.V.4] [Y]-[M]-[D] [h]-[m]-[s] +[phone]

Gdy moduł jest gotowy do pracy, zwracane są:

- Połączenie z kartą sim
- Moc sygnału odbieranego
- Adres IP
- Datę i czas
- Numer kary sim

Połączenie `tcp` lub `udp`. Wysyłany jest plik `tx`, a odpowiedź umieszczana jest w pliku `rx`.

    SIM {tcp|udp}
    >> SIM {tcp|udp} {ok|err:[x]}

Wymaga włączenia funkcji `TCPUDP` oraz ustawienia testowych parametrów serwera.

```cpp
#define SIM_INCLUDE_TCPUDP 1
#define SIM_TCP_SERVER "177.100.166.133"
#define SIM_TCP_PORT 7063
#define SIM_UDP_SERVER "177.100.166.133"
#define SIM_UDP_PORT 7063
```

Połączenie `http(s)`. Wysyłany jest plik `tx`, a odpowiedź umieszczana jest w pliku `rx`.

    SIM http
    >> SIM http res:[x]

Wymaga włączenia funkcji `HTTP` oraz ustawienia testowych parametrów serwera.

```cpp
#define SIM_INCLUDE_HTTP 1
#define SIM_HTTP_URL "sqrt.pl"
#define SIM_HTTP_APP "/app/"
```

### Bash EVENT [➥](#bash-)

W systemie działa baza danych `record` w której umieszczona jest tabela `event`. Za pomocą komend z tej grupy można umieszczać w tej tabeli nowe wpisy oraz pobierać wybrane.

Dopisywanie nowych zdarzeń: `insert`

    EVENT insert [code]
    >> EVENT insert [code] [datatime]

Usuwanie wszystkich wpisów z tabeli

    EVENT delete
    >> EVENT delete

Pobranie wpisów z bazy danych: `select`
Pobrane dane i przekazanie ich do pliku podręcznego.
Można go ustawić przy pomocy komendy [`file cache ...`](#file)

    EVENT select [limit] [offset] [tmin] [tmax] {adc|desc}
    >> EVENT return [count]

Nie jest wymagane podawanie wszystkich parametrów.
W takim przypadku parametry będą ustawione domyślnie:

- limit `0`: zwróć wszystkie wpisy
- offset `0`: zacznij od początku
- tmin `0`: brak ograniczenia dolnego dla czasu
- tmin `0`: brak ograniczenia górnego dla czasu
- dir `desc`: sortowanie od najświeższych wpisów

Wyświetlanie eventów z pliku podręcznego: `print`
Bufor debuggera jest ograniczony, więc czasami nie będzie możliwe wyświetlenie wszystkich wpisów z debuggera. Wówczas trzeba sprytnie operować parametrami `limit` i `offset`

    EVENT print [limit] [offset]
    EVENT [count] [start]..[end]
    [Y]-[M]-[D] [h]-[m]-[s] code:[code]
      ...

### Bash TIMETABLE [➥](#bash-)

Używając komend `timetable` trzeba upewnić się, czy jako plik podręczny jest faktycznie umieszczony plik zawierający tablicę `timetable`
Ustawienie najbliższego alarmu lub inicjacja nowej/zaktualizowanej tablicy
Wówczas w tablicy zostaje znaleziony `weekstamp` najbliższy aktualnie ustawionej dacie oraz zostaje on ustawiony jako alarm

    TIMETABLE init
    TIMETABLE move
    >> TIMETABLE move [day] [h]-[m]-[s] [0] task:[task]

Aby wyświetlić tablice `timetable` należy użyć komendy.

    TIMETABLE print
    TIMETABLE print [length]
    TIMETABLE print [length] [offset]
    >> TIMETABLE [count] [start]..[end]
    >>   [day] [h]-[m]-[s] 0x[task]
    >>   ...
