#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/wdt.h>

volatile unsigned char led[3][8] = 
{
{
	0b00011000,
	0b00100100,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b00100100,
	0b00011000,
},
{
	0b00011000,
	0b00111000,
	0b01111000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b01111110,
},
{
	0b00111100,
	0b01101110,
	0b11000110,
	0b00001100,
	0b00011000,
	0b00110000,
	0b01100000,
	0b11111110,
}
};
ISR(TIMER0_COMPA_vect){
	static unsigned int num = 0;
	static unsigned int count = 0;
	static unsigned char scan;
	unsigned char sc;
	
	count = (count < 500) ? count+1: 0;
	if(count == 0){
		num = (num < 2)?num+1:0;
	}


	PORTB = 0;
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = sc & 0x0f;
	PORTD = sc & 0xf0;
	PORTB = led[num][7 - scan];
}

int main()
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	
	OCR0A = 255;
	TCCR0A = 2;
	TCCR0B = 3;
	TIMSK0 |= (1 << OCIE0A);
	
	sei();
	for(;;){
		wdt_reset();
	}
	return 0;
}
