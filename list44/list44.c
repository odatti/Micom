#include<avr/io.h>
#include<avr/wdt.h>
static void brightness(unsigned char level){
	unsigned short i;
	OCR1AL = level;
	
	for(i = 0;i < 0x2000;i++){
		wdt_reset();
	}
}
int main()
{
	unsigned char n;
	
	DDRD = 0xfa;
	DDRC = 0x0f;
	DDRB = 0xff;

	PORTD = 0;
	PORTC = 0;
	PORTB = 0x0f;

	TCCR1A = 0x83;
	TCCR1B = 0x03;
	OCR1AH = 0;
	
	while(1){
		n = 0;
		while(--n)
			brightness(n);
		while(++n)
			brightness(n);
	}
	return 0;
}
