#include<avr/io.h>
#include<avr/wdt.h>

#define SW ((~PINC>>4)&3)
#define C_MAX 2000UL
#define SW_COUNT 30

unsigned char data;

int main()
{

	unsigned int cnt;
	unsigned char pre_data;
	unsigned char tmp;
	unsigned char n;

	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTB = 0x01;
	PORTC = 0x30;
	PORTD = 0x00;

	pre_data = data = SW;
	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt = (cnt < C_MAX) ? cnt+1 : 0;
		
		if(cnt == 0){
			tmp = SW;
			n = (data != tmp) ? n + 1 : 0;
			if(SW_COUNT < n){
				data = tmp;
			}
		}
		if(pre_data != data){
			pre_data = data;
			switch(data){
				case 0:
					break;
				case 1:
					PORTB=(PORTB<0x80)?PORTB<<1:0x80;
					break;
				case 2:
					PORTB=(PORTB>0x01)?PORTB>>1:0x01;
					break;
				case 3:
					break;
			}
		}
		switch(data){
			case 0:
			case 1:
			case 2:
			case 3:
				break;
		}
	}
	return 0;
}
