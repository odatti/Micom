#include <avr/io.h>
#include "Led.h"
#include "Target.h"
#include <avr/interrupt.h>

/** LEDの明るさを制御するためのカウント変数 */
volatile int ledCount;
/** LED表示用 */
volatile unsigned char led[LED_SIZE];
static volatile unsigned char scan;

static volatile int desplay_target = 0;
/** ターゲットを表示する */
void led_target_on(){
	desplay_target = 1;
}
/** ターゲットを表示しない */
void led_target_off(){
	desplay_target = 0;
}


void led_reset(){
	int x,y;
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			ledPower[y][x] = LED_OFF;
		}
                led[y] = 0x00;
	}
}
/** LEDの処理に関するものの初期化処理 */
void led_init(){
        int x, y;
        ledCount = 0;
	led_reset();

	// LED捜査用のタイマカウンタ
	TCCR1A = 0;
	TCCR1B = 0b00001010; // CTCモード(OCR1A), PS=8
	OCR1A = 500;
	TIMSK1 |= (1 << OCIE1A);
}


// led走査処理(タイマカウンタ)
ISR(TIMER1_COMPA_vect){
        unsigned char sc;
        int x;
        PORTB = 0; // led表示初期化
        scan = (scan + 1) & 7;
        sc = ~(1 << scan);
        led[scan] = 0; // led表示初期化
        PORTC = 0x30 | (sc & 0x0f);// スイッチ用のプルアップを兼ねる
        PORTD = sc & 0xf0;

        // LED_OFF以外の間隔で転倒するためのカウンタ
        ledCount = (ledCount >= LED_OFF - 1) ? 1:ledCount+1;
        for(x=0;x<LED_SIZE;x++){
                int timing = 1;
                // ターゲットのいる場所をONかOFFにする
                if(desplay_target == 1 && scan == LED_SIZE - target_getY() - 1 && target_getX() == x){
                        timing = target_getState();
                }else{
                        timing = ledPower[LED_SIZE - scan - 1][x];
                }

                if(ledCount % timing == 0){
                        unsigned char temp = 1 << (LED_SIZE - x - 1);
                        led[scan] |= temp;
                }
        }
        PORTB = led[scan];
}

