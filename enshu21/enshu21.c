#include<avr/io.h>
#include<avr/wdt.h>

#define SW ((~PINC>>4)&3)
#define C_MAX 2000UL
#define SW_COUNT 30

unsigned char data;

int main(void)
{
	unsigned int cnt;
	unsigned char pre_data;
	unsigned char tmp;
	unsigned char n;
	int power = 0;

	DDRD = 0x0a;
	PORTD = 0x00;

	pre_data = data = SW;
	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		cnt = (cnt < C_MAX + power) ? cnt + 1 : 0;
		// cnt = (cnt < C_MAX) ? cnt + 1 : 0;

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
					power = (power+500<=C_MAX) ? power+500:C_MAX;
					break;
				case 2:
					power = (power-500>=0) ? power-500:0;
					break;
				case 3:
					break;
			}
		}
		/* スイッチの状態に応じて行う処理 */
		switch(data){
			case 0:
			case 1:
			case 2:
				if(cnt == 0){
					PORTD ^= 0x08;
				}
			case 3:
				break;
		}
	}
	return 0;
}
