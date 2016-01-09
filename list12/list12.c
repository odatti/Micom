#include<avr/io.h>
#include<avr/wdt.h>
#define C0_MAX 60000UL
#define C1_MAX 5000UL
#define C2_MAX 100UL
#define C3_MAX 30000UL
int main()
{
	unsigned int cnt0, cnt1, cnt2, cnt3;
	
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;

	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt0 = (cnt0 < C0_MAX) ? cnt0 + 1 : 0;
		cnt1 = (cnt1 < C1_MAX) ? cnt1 + 1 : 0;
		cnt2 = (cnt2 < C2_MAX) ? cnt2 + 1 : 0;
		cnt3 = (cnt3 < C3_MAX) ? cnt3 + 1 : 0;

		if(cnt0 == 0)
			PORTB ^= 3 << 0;
		if(cnt1 == 0)
			PORTB ^= 3 << 2;
		if(cnt1 == 0)
			PORTB ^= 3 << 4;
		if(cnt1 == 0)
			PORTB ^= 3 << 6;
	}
	return 0;
}
