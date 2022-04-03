//              program do kontrolowania urzadzenia
//              za pomoca przyciskow i timera
//              w IL (STL Siemenes)

A   %I4.6         //przyciskiem start
S   %M1.0         //ustaw flage
A   %M1.0         //przypisz flage do urzadzenia (dioda)
=   %Q1.0
//---------------------------------------------------------
A   %I4.2         //przyciskiem stop
S   %M3.1         //zezwolenie timer
R   %Q4.2         //zeruj pierwszy bajt wyjscia - wylacz czerwone podswietlenie
//---------------------------------------------------------
A   %I4.6         //przyciskiem start
S   %Q4.2         //ustaw bajt czerownego przycisku stop
R   %Q5.6         //zeruj zerowy bajt - wylacz zielony start
R   %M3.1         //wylacz zezwolenie timer
//--------------------------------------------------------
A   %M3.1         //inicjalizacja timera
FR  %T1           
L   S5T#2s        //opoznienie 2s
SD  %T1           
L   %T1           //przenies wartosc timera do komorki pamieci
T   %MW10 
A   %T1           //wylacz urzadzenie (dioda)
R   %M1.0
S   %Q5.6
R   %Q4.2
R   %M3.1