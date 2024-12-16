# Uruchamianie kodu:
Nie zalecamy wgrywać zapewnionego kodu na płytki przekazane do analizy, gdyż klucze szyfrujące w niniejszym kodzie zostały zmienione, zatem wgranie niniejszego kodu pozwoli na uruchomienie urządzeń, jednakże bez możliwości dołączenia do sieci projektowej. W przypadku chęci uruchomienia niniejszego kodu zaleca się wykorzystanie dodatkowego modułu ewaluacyjnego niewchodzącego w skład sieci projektowej.

Bezpieczeństwo projektu oparte jest o tajność kluczy zgodnie z [zasadą Kerckhoffsa](https://pl.wikipedia.org/wiki/Zasada_Kerckhoffsa) zatem nie będą one przekazane wprost w kodzie. Jest to jedyny element którego w przekazanym oprogramowaniu świadomie nie umieściliśmy.

Uruchomienie projektu:
 1. Dostosuj odpowiedni plik app_template.c do danego STM32. Masz do wyboru:
    * Smoke detector - wykrywacz dymu (zamiast wykrywacza jest przycisk)
    * LCD Display - węzeł wyświetlający odpowiednie dane (klimatyczne i stan czujnika dymu)
    * Environmental sensor - Czujnik środowiskowy
    * Door lock - zamek który otwiera drzwi w muzeum na wypadek wybuchu pożaru
 2. Dostosuj #define NODE_ID w pliku parser.c, aby odpowiadało wybranemu węzłowi poprzez odkomentowanie odpowiedniego i zakomentowanie pozostałych
 3. Wybór trybu pracy za pomocą GUZIK zostało wyłączone w pliku main.h i nie powinno być włączane ponownie - ma zastosowanie tylko do celów dydaktycznych
 4. Zadbaj, aby klucze umieszczone w kodzie STM32 odpowiadały kluczom w projekcie w pythonie
