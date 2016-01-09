#include<avr/io.h>
#include<avr/wdt.h>
#define C_MAX 5000UL
int main()
{

	int cnt = 0;
	int h = 0;
	DDRC = 0x20;// LED用
	PORTC |= 0x10;// SW1のプルアップ抵抗
	int dir = 0;

	while(1){
		if(((~PINC>>4)&1) == 0x01)// SW1が押されている時
			cnt = (cnt < C_MAX) ? cnt + 1 : 0;

		if(dir == 0){
			if (cnt == 0){
				PORTC |= 0x20;
				if(h < C_MAX){
					h += 30;
				}else{
					h = C_MAX;
					dir = 1;
				}
			}
			if(cnt == h){
				PORTC &= ~0x20;
			}
		}else{
			if (cnt == 0){
				PORTC |= 0x20;
				if(h > 30){
					h -= 30;
				}else{
					h = 0;
					dir = 0;
				}
			}
			if(cnt == h){
				PORTC &= ~0x20;
			}
		}
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
