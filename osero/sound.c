#include <avr/interrupt.h>
#include "Sound.h"

/** 音を鳴らす処理 */
static volatile unsigned period;

void sound_init(){
        TCCR2A = 0;
        TCCR2B = 0x44;
}

void _sound(unsigned char tone, unsigned char length){
	OCR2A = tone;
	period = length;
	TCCR2A = 0x12;
}

void sound_update(){
	if(period){
		if(--period==0){
			TCCR2A = 0;
		}
	}
}

int sound_isPlaying(){
	return (period != 0) ? 1 : 0;
}
