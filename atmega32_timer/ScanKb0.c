/*
 * CFile1.c
 *
 * Created: 02.03.2022 23:03:19
 *  Author: andrzej
 */ 
#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>				

//definicja rejestrów portu do którego podlaczona jest klawiatura
#define KB_PORT	PORTC				//rejestr wyjsciowy 
#define KB_DIR	DDRC				//rejestr kierunku
#define KB_PIN	PINC				//rejestr wejsciowy

#define SCAN_MODE	0		//0,1 - metoda skanowania


#if SCAN_MODE==1
//...............................................................
//Funkcja skanujaca klawiaturê - wersja 1
//Zwraca identyfikacje nacisnietego przycisku lub zero gdy brak detekcji 
char KbScan(void) 
 {
	char result=0;

	KB_DIR =0b00010000;		//tylko kolumna C0 na wyjscie, 
	KB_PORT=0b11101111;		//wymuszenie zera		
	_delay_us(2);
	if( (KB_PIN & 0x01) ==0 )	result=1;
	if( (KB_PIN & 0x02) ==0 )	result=2;
	if( (KB_PIN & 0x04) ==0 )	result=3;
	if( (KB_PIN & 0x08) ==0 )	result=4;

	KB_DIR =0b00100000;		//tylko kolumna C1 na wyjscie, 
	KB_PORT=0b11011111;		//wymuszenie zera
	_delay_us(2);
	if( (KB_PIN & 0x01) ==0 )	result=5;
	if( (KB_PIN & 0x02) ==0 )	result=6;
	if( (KB_PIN & 0x04) ==0 )	result=7;
	if( (KB_PIN & 0x08) ==0 )	result=8;

	KB_DIR =0b01000000;		//tylko kolumna C2 na wyjscie, 
	KB_PORT=0b10111111;		//wymuszenie zera
	_delay_us(2);
	if( (KB_PIN & 0x01) ==0 )	result=9;
	if( (KB_PIN & 0x02) ==0 )	result=10;
	if( (KB_PIN & 0x04) ==0 )	result=11;
	if( (KB_PIN & 0x08) ==0 )	result=12;

	KB_DIR =0b10000000;		//tylko kolumna C3 na wyjscie, 
	KB_PORT=0b01111111;		//wymuszenie zera
	_delay_us(2);
	if( (KB_PIN & 0x01) ==0 )	result=13;
	if( (KB_PIN & 0x02) ==0 )	result=14;
	if( (KB_PIN & 0x04) ==0 )	result=15;
	if( (KB_PIN & 0x08) ==0 )	result=16;
	KB_PORT=0xFF;
	return result;
}

#else
//...............................................................
//Funkcja skanujaca klawiaturê - wersja 2
//Zwraca scankod nacisnietego przycisku lub zero gdy brak detekcji 
char KbScan(void) 
 {
	unsigned char result=0;
	
	KB_DIR =0b11110000;		//wymuszenie zer dla wszystkich kolumn, 
	KB_PORT=0b00001111;	
	_delay_us(5);
	result=	KB_PIN;			//odczyt wierszy
	 
	KB_DIR =0b00001111;		//wymuszenie zer dla wszystkich wierszy, 
	KB_PORT=0b11110000;	
	_delay_us(5);
	result=	result | KB_PIN; //odczyt kolumn i uzyskanie kodu skanowania 
	return result;
}


char KbScanDecode(void){
	switch(KbScan()){
		case 0b01110111: return 1;
		case 0b01111011: return 2;
		case 0b01111101: return 3;
		case 0b01111110: return 4;
		
		case 0b10110111: return 5;
		case 0b10111011: return 6;
		case 0b10111101: return 7;
		case 0b10111110: return 8;
		
		case 0b11010111: return 9;
		case 0b11011011: return 10;
		case 0b11011101: return 11;
		case 0b11011110: return 12;
		
		case 0b11100111: return 13;
		case 0b11101011: return 14;
		case 0b11101101: return 15;
		case 0b11101110: return 16;
	
		
		default:  return 0x00;
		
		
	}
}

#endif
