#include "led.h"

/** LEDの処理に関するものの初期化処理 */
void led_init(){
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
                if(gameState == PLAYING && scan == LED_SIZE - target.y - 1 && target.x == x){
                        timing = target.state;
                }else{
                        timing = ledPower[LED_SIZE - scan - 1][x];
                }

                if(ledCount % timing == 0){
                        uchar temp = 1 << (LED_SIZE - x - 1);
                        led[scan] |= temp;
                }
        }
        PORTB = led[scan];
}
