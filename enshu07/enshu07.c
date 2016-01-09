#include<avr/io.h>
#include<avr/wdt.h>
int main()
{
	DDRB = 0xFF;
	DDRC = 0x0F;
	DDRD = 0xFA;
	
	PORTB = 0x01;
	PORTC = 0x00;
	PORTD = 0x00;

	unsigned int cnt;
	while(1){
		if(cnt == 0){
			PORTB <<= 1;
			if(PORTB == 0x00){
				PORTB = 0x01;
			}
		}
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt++;
	}
	return 0;
}
