







## 💬 Q&A

#### W jaki sposób dołączać pliki nagłówkowe `.h` sterownika?

```c
#include "opencplc.h"
// vs
#include "opencplc-uno.h"
#include "opencplc-dio.h"
#include "opencplc-aio.h"
```

Można użyć ogólnej nazwy `opencplc.h` lub nazwy skojarzonej z konkretnym sterownikiem, np. `opencplc-uno.h`. Wybór zależy od kontekstu aplikacji. Jeśli jest to aplikacja demonstracyjna, test jednostkowy pojedynczego peryferium i ogólnie rzecz biorąc może być wykonana na różnych sterownikach, lepiej wybrać `opencplc.h`. Natomiast jeśli kod aplikacji jest ściśle powiązany z konkretnym sterownikiem PLC, lepszym wyborem będzie nagłówek wskazujący bezpośrednio na wykorzystywany sterownik, jak `opencplc-uno.h` dla płyty Uno.

#### Kiedy używac funkcji clear / clear_all

Kidy zmienyamy konfiguracje #define w pliku main







