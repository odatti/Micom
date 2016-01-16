#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>

#define N 4
#define EEPADDR 0x000

volatile char n;
volatile unsigned char led[N][8] = {
	{
		0b00001000,	
		0b00011100,	
		0b00111110,	
		0b01111111,	
		0b00111110,	
		0b00011100,	
		0b00001000,	
		0b00000000	
	},
	{
		0b00000000,	
		0b00110110,	
		0b01111111,	
		0b01111111,	
		0b00111110,	
		0b00011100,	
		0b00001000,	
		0b00000000
	},
	{
		0b00001000,	
		0b00111110,	
		0b01111111,	
		0b01111111,	
		0b00111110,	
		0b00010100,	
		0b00001000,	
		0b00011100	
	},
	{
		0b00001000,	
		0b00011100,	
		0b00011100,	
		0b00101010,	
		0b01110111,	
		0b00110110,	
		0b00001000,	
		0b00011100	
	}
};

ISR(TIMER0_COMPA_vect){
	static int cnt;
	static unsigned char scan;
	unsigned char sc;
	PORTB = 0;
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = sc & 0x0f;
	PORTD = sc & 0xf0;
	PORTB = led[n][7 - scan];
	if(cnt < 100){
		n = rand() % N;
		OCR2A = cnt;
	}
	if(cnt == 100){
		TCCR2B = 0x0;
	}
	if(500 <= cnt){
		cnt = 0;
		TCCR2B = 0x04;
	}
	cnt++;
}

void init_rand(){
	srand(eeprom_read_word((uint16_t *) EEPADDR));
	eeprom_write_word((uint16_t *) EEPADDR, rand());
}

int main()
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	
	OCR0A = 249;
	TCCR0A = 2;
	TCCR0B = 3;
	TIMSK0 |= (1 << OCIE0A);
	
	TCCR2A = 0x12;
	TCCR2B = 0x04;
	OCR2A = 238;
	
	init_rand();
	n = rand() % N;
	sei();

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
