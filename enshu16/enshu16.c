#include<avr/io.h>
#include<avr/wdt.h>
#define SW ((~PINC>>4)&1)
int main()
{

	DDRC = 0x20;// LED用
	PORTC = 0x10;// SW1のプルアップ抵抗
	
	while(1){
		if(SW == 0x01){// SW1が押されている時
			PORTC |= 0x20;
		}else{
			PORTC &= ~0x20; 
		}
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
