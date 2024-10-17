




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