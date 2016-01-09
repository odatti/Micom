#include<avr/io.h>
#include<avr/wdt.h>
int main()
{
	unsigned int cnt;
	DDRB = 0xFF;
	DDRC = 0x0F;
	DDRD = 0xFA;

	PORTB = 0x00;
	PORTC = 0x0A;
	PORTD = 0xAF;

	for(;;){
		if (cnt == 0)
			PORTB++;
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt++;
	}
	return 0;
}
