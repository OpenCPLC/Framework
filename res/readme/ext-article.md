## 📜 OpenCPLC Article

_Linia otwartych, warstwowo projektowanych sterowników PLC. Ogólna analiza wskazująca, dlaczego na rynku jest miejsce na tego typu rozwiązanie. Lektura dla bardzo ciekawskich_ 🧐

## 👋 Wstęp

**Sterowniki PLC** _(ang. Programable Logic Controllers)_ są integralną częścią przemysłu, ponieważ umożliwiają kontrolę i zarządzanie złożonymi procesami produkcyjnymi w czasie rzeczywistym i istotnym elementem systemów wbudowanych[^1]. Przy wyborze odpowiedniego sterownika, oprócz wymagań projektowych, należy wziąć pod uwagę jego skalowalność, łatwość programowania, niezawodność, bezpieczeństwo, wsparcie techniczne, warunki eksploatacji oraz koszt. Programiści często preferują sterowniki, które już znają, o ile nie są zobowiązani do użycia konkretnego rozwiązania. Temat ten jest bardziej złożony, niż mogłoby się wydawać, i obejmuje nie tylko aspekty techniczne. Dlatego celem pracy jest analiza dostępnych rozwiązań w kontekście projektowania warstwowego oraz zaproponowanie najbardziej odpowiedniej alternatywy w tym kontekście.

Artykuł podzielony został na dwie części. W pierwszej skupiono się na analizie poprawności technologicznej dostępnych rozwiązań pod kątem projektowania warstwowego. W drugiej części przedstawiono alternatywne, autorskie, otwarte rozwiązanie zaprojektowane w sposób warstwowy wraz z przykładową implementacją. W obu częściach został przeanalizowany zakres środowiska programistycznego i sterownika PLC z pominięciem perspektywy kompleksowych systemów, takich jak systemy **SCADA**.

## 🥪 Projektowanie warstwowe

Naturalnym procesem podczas tworzenia większych, bardziej skomplikowanych systemów, aplikacji, języków programowania i sposobów komunikacji jest ich podział na odseparowane funkcjonalnie warstwy abstrakcji, które upraszczają złożone systemy poprzez ukrycie szczegółów niższych poziomów przed wyższymi [^2]. Cechami charakterystycznymi projektowania warstwowego są:

- **specjalizacja**: warstwy mają określone zadania, co pozwala specjalistom skupić się na swoim obszarze;
- **modułowość**: umożliwia łatwe dodawanie, wymianę lub modyfikację poszczególnych części;
- **izolacja**: zmiany w jednej warstwie nie powinny wpływać na inne;
- **skalowalność**: warstwy mogą być rozwijane niezależnie, zapewniając efektywne dostosowanie do zwiększających się wymagań;
- **wzajemna wymienność**: warstwy można zastępować alternatywnymi rozwiązaniami bez naruszania całości systemu.

Te właściwości pomagają w zarządzaniu, testowaniu i rozwoju bardziej złożonych systemów, okazując się przydatne zarówno przy pracy ze sprzętem, jak i z oprogramowaniem.

Wspomniane warstwy mogą być zorganizowane w sposób liniowy, gdzie każda pełni swoją określoną funkcję, a komunikacja między nimi przebiega w układzie hierarchicznym, jak w modelach **TCP/IP**, **MVC** i oprogramowaniu **KiCad**, które jest zestawem aplikacji. Może się zdarzyć, że wyższa warstwa otacza niższą, zachowując separację, jak w frameworkach **Next.js** i **FastAPI**, a także w **systemach wbudowanych**, o czym może świadczyć sama budowa mikrokontrolera.

![article-tcpip-uc](http://sqrt.pl/img/opencplc/article-tcpip-uc.png)

Relacje między warstwami mogą być bardziej złożone, jak w przypadku systemów operacyjnych. Kluczowe jest jednak oddzielenie części funkcjonalnych oraz zapewnienie dostępu do poszczególnych modułów.

## 🔒 Close PLC 

Najpopularniejsze komercyjne sterowniki PLC zazwyczaj są dostarczane z dedykowanym oprogramowaniem, które umożliwia tworzenie i wgrywanie własnych programów. W centrum każdego sterownika znajduje się mikrokontroler, który wykonuje obliczenia, steruje układami peryferyjnymi oraz komunikuje się z innymi urządzeniami poprzez interfejsy.

Niestety, dedykowane środowiska do programowania narzucają użytkownikom pewne ograniczenia. Umożliwiają korzystanie tylko z funkcji i bloków zapewnionych przez producenta. Takie rozwiązanie oczywiście pozwala szybko tworzyć programy o przejrzystej strukturze, jednak ograniczenie dla programisty dostępu do niższej warstwy odbiera możliwość pełnego wykorzystania mikroprocesora, co czasem jest niezbędne przy bardziej zaawansowanych lub nietypowych zadaniach.

W tego typu rozwiązaniach środowisko programistyczne pozwala na programowanie jedynie sterowników danego producenta, a sterowniki mogą być programowane tylko za pomocą dedykowanego środowiska. Prawdopodobnie istnieje wewnętrzny podział na warstwy, ale dostęp do nich, jak wspomniano wcześniej jest niedostępny dla programistów. Taka strategia jest zdecydowanie bardziej korzystna dla producentów niż dla programistów i konsumentów końcowych. Ogranicza to bowiem wybór i elastyczność dla programistów oraz może prowadzić do większej zależności od jednego dostawcy, co może wpływać na konkurencyjność rynkową i innowacje[^3].

Pomimo tych ograniczeń, popularność sterowników takich jak **Siemens S7-1200**, **Wago 750-881** i **Allen-Bradley MicroLogix 1400** świadczy o ich niezawodności i dostosowaniu do potrzeb branży. Dlatego firmy często wybierają te produkty, nawet mimo pewnych wad.

## 🔓 Open PLC

Zgodnie z ideą projektowania warstwowego, środowisko programistyczne powinno być łatwo dostosowywane do różnych urządzeń sprzętowych, a sterownik powinien umożliwiać programowanie bez konieczności korzystania z dodatkowej warstwy skoncentrowanej na obsłudze systemów automatyki. Dzięki temu programiści mieliby większą swobodę w wyborze rozwiązań, co sprzyjałoby innowacjom i konkurencyjności na rynku. Sukces otwartych standardów w branży serwerów i sieci jest dowodem na to, że tego typu podejście może działać również w automatyce. Otwarte interfejsy i projektowanie warstwowe w obszarze serwerów znacznie przyczyniły się do rozwoju chmur obliczeniowych i rozwiązań hybrydowych, takich jak np.[^4].

Najbardziej naturalnym kierunkiem rozwoju sterowników PLC w kontekście projektowania warstwowego będzie dodanie warstwy sprzętowej do mikrokomputera z otwartym systemem operacyjnym, takim jak **Linux**[^5]. W ostatnim czasie na rynku pojawiło się wiele rozwiązań adaptujących mikrokomputery do zastosowań przemysłowych, takich jak **UniPi**, **Revolution Pi** oparte na **Raspberry Pi** oraz **PLCnext** od **Phoenix Contact**.'''

Zatem opracowanie sterownika opartego na RPI wymaga utworzenia warstwy fizycznej, zgodnej z wyprowadzeniami mikrokomputera Raspberry Pi, obejmującej wybrane peryferia automatyki, takie jak

- **RO**: Wyjścia przekaźnikowe
- **TO**: - Wyjścia tranzystorowe
- **XO**: - Wyjścia triakowe
- **DI**: - Wejścia cyfrowe
- **AI**: - Wejścia analogowe
- **AO**: - Wyjścia analogowe
- **RS485**: Interfejs komunikacyjny

Warto także zapewnić pakiet narzędzi i sterowników umożliwiających sterowanie peryferiami automatyki oraz ułatwiających tworzenie aplikacji. Całość należy zawrzeć w automatycznym instalatorze, obrazie systemu lub obrazie Docker, co pozwoli na stworzenie kompletnego środowiska pracy z PLC.

![article-rpi](http://sqrt.pl/img/opencplc/article-rpi.png)

Na rynku jest miejsce dla sterowników PLC zarówno opartych na mikrokomputerach [^6], które oferują większą moc obliczeniową, zaawansowaną obsługę interfejsów, takich jak USB, Ethernet czy Wi-Fi, oraz szeroką gamę oprogramowania, w tym języki interpretowane, bazy danych, systemd i cron, jak i tych opartych na mikrokontrolerach [^7], które wyróżniają się wyższą niezawodnością, krótszym czasem uruchamiania, niższym zużyciem energii i kosztami.

Pojawia się zatem wyraźna luka, której wypełnienie mogłoby korzystnie wpłynąć zarówno na konkurencyjność, jak i na edukację w obszarze komputerowych systemów sterowania. Projektowanie warstwowe, z klarownym podziałem na poszczególne komponenty, pozwala lepiej zrozumieć działanie systemów i ułatwia proces ich nauki. Dzięki temu uczniowie i specjaliści mogą szybciej zdobywać praktyczne umiejętności, zwiększając swoją biegłość w nowoczesnych technologiach sterowania.

## 💡 Project OpenCPLC

...

[^1]: _Sterowniki programowalne_, M. Pawlak, Politechnika Wrocławska, 2010

[^2]:	_Systematically reviewing the layered architectural pattern principles and their use to reconstruct software architectures_, Alvine B. Belle, Ghizlane El Boussaidi, Timothy C. Lethbridge, Segla Kpodjedo, Hafedh Mili, Andres Paz, arXiv preprint, `DOI: 10.48550/arXiv.2112.01644`