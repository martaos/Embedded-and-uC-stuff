#include <avr/io.h>
#define F_CPU 8000000L
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

void lcd_gotoxy(char col, char row);
void lcd_putchar(char x);
void lcd_clr(void);
void lcdprintf_init(void);

char KbScanDecode(void);

void lcd_user_char(char tab[], char nr);

//---------------------------------------------------------------
#define CMP_REGISTER ((F_CPU/1000)/256)				//mamy 1 ms ale ze liczba jest wieksza niz 1 bajt to mamy prescaler 
void InitTimer0(void)
{
	OCR0=CMP_REGISTER;				//aby uzyskac okres 1ms
	TCCR0= (1<<WGM01 | 1<< CS02);	//tryb zerowania po zrownaniu, prescaler=256
	TIMSK=0x01 << OCIE0;			//zezwolenie na przerwania
}

volatile int key;
volatile int msTimer = 0;
//---------------------------------------------------------------
ISR(TIMER0_COMP_vect)	// Obsluga przerwania od TIMER0
{
	/*static int counter=0;			//hermatyzacja danych
	counter++;
	if(counter>100) {
		PORTB =PORTB^0x08;			//dioda miga
		counter=0;
	}*/
	key =KbScanDecode();
	msTimer++;
}

int main(void)
{
	char znak[8]={
		0b00000100,
		0b00000100,
		0b00000100,
		0b00011111,
		0b00000100,
		0b00000100,
		0b00000100,
		0b00000100,
	
	};

	//long counter = 0;
	int min = 0, h = 0, s = 0, ms = 0, tmp = 0;
	DDRB = 0xFF;
	_delay_ms(3000);
	lcdprintf_init();
	InitTimer0();
	sei();
	lcd_user_char(znak, 1);
	//lcd_putchar(1);
	printf("\rCzas: ");
	int on = 0;
//	int key = 0;
    while (1) 
    {
		//key = KbScanDecode();
		if(key == 1) on = 1;
		if(key == 2) on = 0;
		if(on){
			
			cli();				//aby nie gubilo jak mamy 2 bajty w assemblerze do porownania np 0
			ms = (int) (msTimer%1000);
			tmp = msTimer/1000;
			sei();
			s = tmp%60;
			tmp /= 60;
			min = tmp%60;
			tmp /= 60;
			h = (int) tmp;
			
			if(key != 2){
				/*if(s/10 == 60){
					s = 0;
					min++;
				}
				if(min == 60){
					min = 0;
					h++;
				}*/
				
				lcd_gotoxy(6,0);
				printf("%02d:%02d:%02d.%d", h, min, s, ms);
				printf("\nPress 2 to stop ");
				_delay_ms(100);
			
			//	key = KbScanDecode();
			}
				
			else	break;
		}
		else{
			msTimer = 0;
							printf("\nPress 1 to start");
			}
			
		
		
			
		//sprintf(linia_lcd, "HEX: %08lX", counter); 
		//lcd_gotoxy(0,1);
		//lcd_puttxt(linia_lcd);
		//counter++;
    }
}

