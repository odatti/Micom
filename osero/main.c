#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define LED_SIZE 8
#define LED_OFF 1000
#define LED_ON 1
#define LED_MIDDLE 5

#define SW_INTERVAL 3000UL
#define SW ((~PINC>>4)&3)

void game_init();
void led_init();
void switch_init();
void timer_init();

typedef unsigned char uchar;
static volatile uchar user;


// LED表示用の配列
volatile uchar led[LED_SIZE];
// LEDの明るさ情報
volatile int ledPower[LED_SIZE][LED_SIZE];
// LEDの明るさを制御するためのカウント変数
volatile int ledCount;
// ダイナミックスキャン用
static volatile uchar scan;

// スイッチとチャタリング対策に使う変数
volatile uchar sw;
volatile uchar swnow;
volatile uchar swnew;
volatile unsigned swcnt;
// ピン変化割り込みとスイッチの処理に使用
volatile unsigned char pc = 0;

/*
ISR(TIMER0_COMPA_vect){
	if(++clk >= 50){
		clk = 0;
		if(delay)
			delay--;

		if(period){
			if(--period == 0)
				TCCR2A = 0;
		}
		user = 1;
	}
}
*/



// led走査処理(タイマカウンタ)
ISR(TIMER2_COMPA_vect){
	unsigned char sc;
	int x;
	PORTB = 0; // led表示初期化
	led[scan] = 0; // led表示初期化
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = 0x30 | (sc & 0x0f);// スイッチ用のプルアップを兼ねる
	PORTD = sc & 0xf0;
	
	// 4段階の明るさでledを点滅させている
	ledCount = (ledCount < LED_OFF - 1) ? ledCount+1:0;
	for(x=0;x<LED_SIZE;x++){
		if(ledCount % ledPower[scan][x] == 0){
			uchar temp = 1 << (LED_SIZE - x - 1);
			led[scan] |= temp;
		}
	}
	PORTB = led[scan];
}

// スイッチのピン変化割り込み
ISR(PCINT1_vect){
	if(pc == 0){
		pc = 1;
	}
	// ブロックしているピン変化割り込みをキャンセル
	PCIFR |= _BV(PCIF1);
}

int main(void){
	// 初期化
	game_init();
	led_init();
	switch_init();
	timer_init();

	

/*
	OCR0A = 249;
	TCCR0A = 2;
	TCCR0B = 3;
	TIMSK0 |= (1 << OCIE0A);

*/
	// 割り込み処理を実行
	sei();
	for(user=0;;){
		wdt_reset();
		if(pc){
			swcnt = (swcnt < SW_INTERVAL) ? swcnt + 1 : 0;
			if(swcnt == 0){
				swnow = SW;
				pc = 0;
			}
		}

		// スイッチの結果を更新
		if(swnow != sw){
			sw = swnow;
			switch(sw){
				case 0:
					break;	
				case 1:
					break;	
				case 2:
					break;	
				case 3:
					break;	
			}
		}
	}
	return 0;
}


// ゲームに関するものを初期化する関数
void game_init(){
	int x,y;
	ledCount = 0;
	for(y=0;y<LED_SIZE;y++){
		for(x=0;x<LED_SIZE;x++){
			ledPower[y][x] = LED_OFF;
		}
	}
	ledPower[3][3] = LED_ON;
	ledPower[4][4] = LED_ON;
	ledPower[3][4] = LED_MIDDLE;
	ledPower[4][3] = LED_MIDDLE;
	
}

// LEDを表示するために必要な準備
void led_init(){
	// LED表示するための変数
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTC = 0x30;
	PORTD = 0x00;
	PORTB = 0x00;
	// LED捜査用のタイマカウンタ
	TCCR2A = 2; // CTCモード
	TCCR2B = 3; // PS=32
	OCR2A = 4000;
	TIMSK2 |= (1 << OCIE2A);
}

// スイッチを使用するための初期化
void switch_init(){
	// PORTCのピン変化割り込み有効
	PCICR |= _BV(PCIE1);
	// 割り込みを認めるビット位置を指定
	PCMSK1 = _BV(PCINT12) | _BV(PCINT13);
	swnow = 0x30;
	swnew = 0x30;
	sw = 0;
}

// タイマカウンタを使用するための初期化
void timer_init(){

}
