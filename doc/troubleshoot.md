




Debuger skończył pracę na `HardFault_Handler`.


- Zbyt mały stos `stack` pamięci przypisany do jednego z wątków.
- Błąd pamięci







```
undefined reference to `thread'
undefined reference to `VRTS_Init'
```

Ustaw wartość definicji `VRTS_SWITCHING` na `1` w pliku `main.h` 

```c
#define VRTS_SWITCHING 1
```






Brak obsługi przycisku
Brak migającej diody
Innych standardowych funkcji, które dostaracz wątek PLC

- Brak lub skrajnie żadnie wywoływanie funkcji zwalnjającej rdzeń w naszym wątku, takiej jaki let() czy delay()

rozwiązanie:

dodaj funkcję let() lub delay() na końcu pętli while(1) w każdym z dodanych wątków

Obecnie występuje pewien problem: czyszczenie pamięci procesora _(erase full chip)_ powoduje jego zawieszenie. Aby przywrócić działanie procesora, należy wgrać jakiś działający program z nijcjacją, który nie dochodzi do końca instrukcji, a następnie odłączyć i ponownie podłączyć zasilanie. Rozwiązaniem będzie przygotowanie wbudowanego skryptu, który zamiast usuwać, wgra przykładowy program _(np. zmienianie koloru diody LED)_




Podczas uruchamiania `make` lub `make build` wyskakuj błąd `Access denied` nawet gdy `VSCode` jest włączony jako admin.


```
Access denied - .
File not found - -NAME
Access denied - .
File not found - -NAME
mkdir -p build/
The syntax of the command is incorrect.
make: *** [makefile:98: build/] Error 1
```

Prawdopoddnie CSVocde wykozystuje powłokę powershell, której już nie otwiera jako administrator