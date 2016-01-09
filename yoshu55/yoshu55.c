#include<avr/io.h>
#include<avr/wdt.h>
int main()
{
	unsigned char n;
	
	DDRD = 0xfa;
	DDRC = 0x0f;
	DDRB = 0xff;
	
	PORTD = 0;
	PORTC = 0;
	PORTB = 0x0f;
	
	TCCR1A = 0x81;
	TCCR1B = 0x03;
	OCR1A = 40;

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
