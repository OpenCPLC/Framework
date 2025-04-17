## 🩺 Troubleshoot


#### 

Po wyczyszczeniu upamięci do zera



#### Nie mogę uruchomić przykładu. Co może być przyczyną?

Przykłady mają stały schemat i wyglądają następująco ⤵️

🧩 Kompletny przykład: **_"Nazwa Przykładu"_**

Możesz go szybko uruchomić, bedąc w przestrzeni roboczej, wpisując w konsoli:

```bash
./wizard.exe -n <name> -p opencplc/doc/examples/<example>
make run
```

➡️ Powodów może być kilka:

1️⃣ Projekt istnieje

2️⃣ Być może przykład jest już dodany do projektów. Aby to sprawdzić, wyświetl listę dostępnych przykładów. Jeśli znajdziesz poszukiwany przykład, możesz się na niego przełączyć:

```bash
./wizard.exe -l # Wyświetl listę projektów
./wizard.exe -s <name> # Przełącz się na przykład <name>
```

3️⃣ Inna ścieżka framework'u

Jeśli modyfikujesz kod i używasz flagi `-f` `--framework`, wskazując inny katalog framework'u, musisz również dostosować ścieżkę do przykładu:

```bash
./wizard.exe -n <name> -p <framework-path>/doc/examples/<example> -f <framework-path>
make run
```

Jeśli utworzyłeś folder `opencplc`, upewnij się, że jego zawartość jest zgodna z oczekiwaniami frameworka. Prawdopodobnie nie, więc usuń ten folder.

#### VSCode przegląda złe plki oraz źle rozpoznaje  `#define`

Przeładuj okno VS Code: Jeśli zmiany nie zostaną od razu uwzględnione, przeładuj okno edytora:

Otwórz paletę poleceń (Ctrl+Shift+P) li b `F1`
Wpisz Reload Window i wybierz odpowiednią opcję.
Wyczyść pamięć podręczną IntelliSense: W przypadku dalszych problemów z wczytywaniem zmian, wyczyść pamięć podręczną IntelliSense:

Otwórz paletę poleceń (Ctrl+Shift+P).
Wpisz C/C++: Reset IntelliSense Database i wybierz tę opcję.





```c

```



Debuger skończył pracę na `HardFault_Handler`.


- Zbyt mały stos `stack` pamięci przypisany do jednego z wątków.
- Błąd pamięci


```
undefined reference to `vrts_thread'
undefined reference to `vrts_init'
```

Ustaw wartość definicji `VRTS_SWITCHING` na `1` w pliku `main.h` 

```c
#define VRTS_SWITCHING 1
```

Trzeba dodać najczęstrze logi !

- Nie przełączanie się w wątkach
- Kontroler czy pamięć się na nadpisuje w kółko
- Czy memory dynamiczne się nie przepełnia

Obecnie występuje pewien problem: czyszczenie pamięci procesora _(erase full chip)_ powoduje jego zawieszenie. Aby przywrócić działanie procesora, należy wgrać jakiś działający program z nijcjacją, który nie dochodzi do końca instrukcji, a następnie odłączyć i ponownie podłączyć zasilanie. Rozwiązaniem będzie przygotowanie wbudowanego skryptu, który zamiast usuwać, wgra przykładowy program _(np. zmienianie koloru diody LED)_
