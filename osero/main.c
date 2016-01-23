#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define LED_SIZE 8
#define LED_OFF 1000
#define LED_ON 1
#define LED_MIDDLE 5

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

/*
volatile uchar sw;

static volatile uchar clk;

static volatile uchar delay;

static volatile uchar swnow;
static volatile uchar swnew;
static volatile uchar swcnt;
*/
/*
ISR(TIMER0_COMPA_vect){
	unsigned char sc;
	PORTB = 0;
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = 0x30 | (sc & 0x0f);
	PORTD = sc & 0xf0;
	PORTB = led[scan];

	sc = PINC & 0x30;
	if(sc != swnow){
		if(sc == swnew){
			if(++swcnt > 20){
				swnow = swnew;
				sw = (swnow >> 4) ^ 3;
			}
		}
		else{
		swnew = sc;
		swcnt = 0;
		}
		rnd = rnd + scan + sw;
	}
	
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




ISR(TIMER2_COMPA_vect){
	// LED走査処理
	unsigned char sc;
	int x;
	PORTB = 0; // LED表示初期化
	led[scan] = 0; // LED表示初期化
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = 0x30 | (sc & 0x0f);// スイッチ用のプルアップを兼ねる
	PORTD = sc & 0xf0;
	// PORTB = led[scan];
	
	// 4段階の明るさでLEDを点滅させている
	ledCount = (ledCount < LED_OFF - 1) ? ledCount+1:0;
	for(x=0;x<LED_SIZE;x++){
		if(ledCount % ledPower[scan][x] == 0){
		// if(ledCount % (1+2*x) == 0){
			uchar temp = 1 << (LED_SIZE - x - 1);
			led[scan] |= temp;
		}
	}
	PORTB = led[scan];

	//sc = PINC & 0x30;
}


int main(void){
	int x,y;
	ledCount = 0;
	for(y=0;y<LED_SIZE;y++){
		for(x=0;x<LED_SIZE;x++){
			switch(x%4){
				case 0:
					ledPower[y][x] = LED_ON;
					break;
				case 1:
					ledPower[y][x] = LED_MIDDLE;
					break;
				default:
					ledPower[y][x] = LED_OFF;
					break;
			}
		}
	}


	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTC = 0x30;
	PORTD = 0x00;
	PORTB = 0xff;
	
	// LED捜査用のタイマカウンタ
	TCCR2A = 2; // CTCモード
	TCCR2B = 3; // PS=32
	OCR2A = 4000;
	TIMSK2 |= (1 << OCIE2A);

/*
	swnow = 0x30;
	swnew = 0x30;
	sw = 0;
	OCR0A = 249;
	TCCR0A = 2;
	TCCR2B = 3;
	TIMSK0 |= (1 << OCIE0A);

	TCCR2A = 0;
	TCCR2B = 0x44;
	
*/
	sei();
	for(user=0;;){
		wdt_reset();
/*
		if(user){
			user_main();
			user = 0;
		}
*/
	}
	return 0;
}


