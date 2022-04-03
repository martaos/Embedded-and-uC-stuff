//***********************************************
// Funkcje obslugi tekstowego wyswietlacza LCD 
// Dotyczy uklad�w zgodnych ze sterownikiem: 
//             S 6 A 0 0 6 9  !!!!
//--------------------------------------------------
// Definicja portow procesora i trybu pracy LCD
// Dla interfejsu 4-bitowego zakladamy: 
//  starsza czesc LCD_DATA_PORT  -> do D4-D7 LCD 

#include <avr/io.h>
#define F_CPU 8000000L
#include <util/delay.h>
#include <stdio.h>

#define LCD_DATA_PORT	PORTA	//port danych
#define LCD_DATA_DDR	DDRA	//rejestr kierunku portu danych
#define LCD_CTR_PORT	PORTA	//port sterowania
#define LCD_CTR_DDR		DDRA	//rejestr kierunku portu sterowania
#define LCD_PIN_E		PA3		//piny sterujace
#define LCD_PIN_RS		PA2
//--------------------------------------------------
//Pomocnicze definicje ustawiajace sygnaly 	sterowania				
#define LCD_SET_E()   (LCD_CTR_PORT |=  (1<<LCD_PIN_E))
#define LCD_CLR_E()   (LCD_CTR_PORT &= ~(1<<LCD_PIN_E))
#define LCD_SET_RS()  (LCD_CTR_PORT |=  (1<<LCD_PIN_RS))
#define LCD_CLR_RS()  (LCD_CTR_PORT &= ~(1<<LCD_PIN_RS))

//------------------------------------------------
// Generacja impulsu strobuj�cego E
// Uwagi czasowe:
//   -min czas pomiedzy kolejnymi cyklami E - 500ns
//   -min dl. impulsu E - 230 ns
//   -min czas od ustalenia RS do zbocza narastajacego E - 40ns
//	 -min czas od danych do zbocza opadajacego  E - 80ns
// Dla CPU 16 MHz  - czas wykonania instrukcji -62 ns
static void lcd_wr(char dd)	// cykl zapisu do LCD 
{
	asm volatile ("nop ");
	LCD_SET_E();				// E stan wysoki
	// ustawienie starszej cz�sci portu danych przy zachowaniu 
	// stanu mlodszej czesci w asemblerze ok 6 instrukcji
	LCD_DATA_PORT &= 0x0F;
	LCD_DATA_PORT |= (0xF0 & dd);	
	asm volatile ("nop ");	//_delay_us(1);
	asm volatile ("nop ");
	LCD_CLR_E();		// opadajace zbocze na E -> zapis do wyswietlacza
}

//------------------------------------------------
static void lcd_wr_data(unsigned char data) //cykl zapisu danej do LCD
{
	LCD_SET_RS();				//operacja RAM
	lcd_wr( data );		// zapis MSB 
	_delay_us(2);
	lcd_wr(data<<4);	// zapis LSB 
	_delay_us(60);
}

//------------------------------------------------
static void lcd_wr_command(char data) //cykl zapisu komendy do LCD
{
	LCD_CLR_RS();				//komenda
	lcd_wr( data);			// zapis MSB
	_delay_us(2);
	lcd_wr( data<<4);		// zapis LSB
	_delay_us(50);
}


//------------------------------------------------
void lcd_clr(void) 		//czyszczenie LCD 
 {
	lcd_wr_command(0x01); 
	_delay_ms(3);		//wymagane dodatkowe opoznienie
 }

//------------------------------------------------
void lcd_init(void)	//inicjowanie LCD
 {
 	//ustawienie na wyjscie portu dla lini sterujacych 
	LCD_CTR_DDR |= ((1<<LCD_PIN_E)|(1<<LCD_PIN_RS));		//piny sterujace dla ZL10AVR
	//ustawienie na wyjscie portu linii danych 
	LCD_DATA_DDR |= 0xF0;
	LCD_DATA_PORT &= ~0xF0;

	LCD_CLR_RS();			//tryb komend
	LCD_CLR_E();
	
	_delay_ms(30);

	//uwaga wpisywana tylko starsza czesc bajtow
	lcd_wr(0x20);		//tryb 4- bitowy
	lcd_wr(0x20);
	lcd_wr(0xC0);		//2-linie mode, display On
	_delay_us(60);		// !!b.wazne
	lcd_wr(0x00);
	lcd_wr(0xE0);		//Display OnOff control:display ON,cursor on,blink off
	_delay_us(60);		// !!b.wazne	
	
	lcd_clr();			  //Display clear
	lcd_wr_command(0x06); //Entry mode: decrement, entire shift off
 }


//------------------------------------------
void lcd_putchar(char x)	// wyswietlanie znaku z formatowaniem
{
		lcd_wr_data(x);
}

//------------------------------------------
void lcd_puttxt(char*msg)// wy�wietlanie stringu 'msg'
 {
	while( *msg!=0)  lcd_putchar(*msg++);
}


//------------------------------------------
//przesuniecie kursora do 'col','row' (numeracja od zera)
void lcd_gotoxy(char col, char row) 
{
	char add = (col & 0x3F);
	if(row!=0)  add |= 0xC0;
	else		add |= 0x80;	
	lcd_wr_command(add); 
	
}
//------------------------------------------
// nr -numer znaku specjalnego od 0 do 7
// tab[] tablica definicji znaku
// Uwaga: przesuwa kursor, po wykorzystaniu ustawic za pomoca gotoxy()
void lcd_user_char(char tab[], char nr)	//definicja znaku uzytkownika
{
	int i;
	lcd_wr_command(0x40+8*nr); 		//adres znaku o kodzie 0
	for(i=0;i<8;i++)
	lcd_wr_data(tab[i]);
}

//-----------------------------------------------

static int lcd_printchar(char c, FILE *stream) //funkcja wyjsciowa strumienia
{
	if(c=='\r')
		lcd_gotoxy(0, 0) ;
	else
	if(c=='\n')
		lcd_gotoxy(0, 1) ;
	else
		lcd_putchar(c);
	return 0;
}


/*static int lcd_printchar(char c, FILE *stream) //funkcja wyjsciowa strumienia
{
	lcd_putchar(c);
	return 0;
}*/


// definicja standardowego strumienia do zapisu
static FILE lcdStream=
FDEV_SETUP_STREAM(lcd_printchar, NULL,_FDEV_SETUP_WRITE);


void lcdprintf_init(void)
{
	_delay_ms(1000);

	lcd_init();
	// inicjowanie znakow uzytkownika
	// ........
	//lcd_user_char(tab_znak_1,1);	
	lcd_gotoxy(0,0);

	stdout = &lcdStream;
}

