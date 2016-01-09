
#include<avr/wdt.h>

#define SW ((~PINC>>4)&3)
#define C_MAX 2000UL
#define SW_COUNT 30
#define LED_MAX 1000UL

unsigned char data;
int main(void)
{
	unsigned int cnt;
	unsigned char pre_data;
	unsigned char tmp;
	unsigned char n;
        int dir = 0;

	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xf0;
	PORTC = 0x30;
	PORTD = 0x00;
	PORTB = 0x00;
	int h = 0;

	
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
					h = (h-250 >= 0)?h-250:0;
					break;
				case 2:
					h = (h+250 <= LED_MAX)?h+250:LED_MAX;
					break;
				case 3:
					break;
			}
		}

                if (cnt == 0){
                        PORTB = 0xff;
/*
                        if((dir==0)?h < LED_MAX:h>30){
                                h += (dir==0)?30:-30;
                        }else{
                                h = (dir==0)?LED_MAX:0;
                                dir = (dir==0)?1:0;
                        }
*/
                }
                if(cnt == h){
                        PORTB &= ~0xff;
                }

	}
	return 0;
}
