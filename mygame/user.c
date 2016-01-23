#include "user.h"

static uchar px, py;

static void clearLed()
{
	uchar i;
	for(i = 0;i < LED_SZ;i++){
		led[i] = 0;
	}
}

void user_init(void){
	clearLed();
	py = 4;
	px = 2;
}

void user_main(void){
	switch(sw){
		case 1:
			px--;
			px &= 0x7;
			_sound(BEEP_C4, 1);
			break;
		case 2:
			py--;
			py &= 0x7;
			_sound(BEEP_C5, 1);
			break;
		case 3:
			clearLed();
			_sound(BEEP_LOW, 3);
			break;
	}
	led[py] |= 1 << px;
}
