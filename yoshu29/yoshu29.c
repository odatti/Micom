#include<avr/io.h>
#include<avr/wdt.h>
#define SW ((~PINC>>4)&3)
int main()
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTB = 0xFF;
	PORTC = 0x30;
	PORTD = 0x00;

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		PORTB = (2 < SW) ? 0xFF : 0x00;
	}
	return 0;
}
