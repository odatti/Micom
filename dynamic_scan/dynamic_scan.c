#include<avr/io.h>
#include<avr/wdt.h>
/*
unsigned char led[8] = {
	0b00011000,
	0b00100100,
	0b01000010,
	0b10000001,
	0b10000001,
	0b01000010,
	0b00100100,
	0b00011000,
};
*/
unsigned char led[16] = {
	0b11111000,
	0b00000000,
	0b00111100,
	0b00000000,
	0b01110000,
	0b00000000,
	0b00001111,
	0b00010000,
	0b01111100,
	0b01000000,
	0b11111000,
	0b00000100,
	0b00011111,
	0b01000000,
	0b00010000,
	0b00000100,
	0b00001000,
};
int main()
{
	unsigned int cnt;
	unsigned int cnt2;
	int i = 0;
	unsigned char sc, scan = 0xff;
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;

	for(;;){
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt = (cnt < 2000) ? cnt + 1 : 0;
		cnt2++;
		if(cnt2==0)
			i = (i < 8) ? i+1 : 0 ;
		if(cnt == 0){
			PORTB = 0;
			scan = (scan+1) & 7;
			sc = ~(1 << scan);
			PORTC = sc & 0x0f;
			PORTD = sc & 0xf0;
			PORTB = led[7-scan+i];
		}
	}
	return 0;
}
