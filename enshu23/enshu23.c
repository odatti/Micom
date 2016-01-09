
#include<avr/wdt.h>

#define SW ((~PINC>>4)&3)
#define C_MAX 2000UL
#define SW_COUNT 30

unsigned char data;
unsigned char led[8] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
};

void move(int x, int y, int old){
	led[old] = 0x00;
	led[y] = 1 << (7 - x); 
}
int main(void)
{
	unsigned int cnt;
	unsigned char pre_data;
	unsigned char tmp;
	unsigned char n;
	unsigned char sc, scan = 0xff;
	int old = 0;
	int x = 4, y = 4;
	move(x,y,old);

	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xf0;
	PORTC = 0x30;
	PORTD = 0x00;

	
	pre_data = data = SW;
	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		// cnt = (cnt < C_MAX + power) ? cnt + 1 : 0;
		cnt = (cnt < C_MAX) ? cnt + 1 : 0;

		if(cnt == 0){
			tmp = SW;
			n = (data != tmp) ? n + 1 : 0;
			if(SW_COUNT < n){
				data = tmp;
			}
		}

		/* スイッチの状態が変化した時に行う処理 */
		if(pre_data != data){
			pre_data = data;
			switch(data){
				case 0:
					break;
				case 1:
					led[y] = 0x00;
					x = (x-1 >= 0)?x-1:7;
					led[y] = 1 << (7 - x); 
					break;
				case 2:
					led[y] = 0x00;
					y = (y-1 >= 0)?y-1:7;
					led[y] = 1 << (7 - x); 
					break;
				case 3:
					break;
			}
		}
		PORTB = 0;
		scan = (scan + 1) & 7;
		sc = ~(1 << scan);
		PORTC = sc & 0x3f;
		PORTD = sc & 0xf0;
		PORTB = led[7 - scan];
		/*
		if(cnt == 0){
			PORTB = 0;
			scan = (scan + 1) & 7;
			sc = ~(1 << scan);
			PORTC = sc & 0x3f;
			PORTD = sc & 0xf0;
			PORTB = led[7 - scan];
		}
		*/
	}
	return 0;
}
