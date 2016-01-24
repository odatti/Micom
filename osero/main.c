// include
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define LED_SIZE 8
#define LED_OFF 1000
#define LED_ON 1
#define LED_MIDDLE 5

#define SW_INTERVAL 6000UL
#define SW ((~PINC>>4)&3)

void game_init();
void led_init();
void switch_init();
void timer_init();

void game_main();

// 石をおけるか否かの判断
int judgePutStone(int x, int y, int turn);
// 石を置く処理
void putStone(int x, int y, int turn);
int countTurnOver(int turn, int x, int y, int d, int e);

typedef unsigned char uchar;
static volatile uchar user;


// プレイヤーの操作に関する構造体
typedef struct TARGET{
	int x,y;
	int state;
	int turn;
}TARGET;

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

// プレイヤーの操作に関する変数
TARGET target;
static volatile uchar clk;
static volatile int cursor_clk;


// 2ms毎に呼ばれる関数（タイマカウンタ）
ISR(TIMER0_COMPA_vect){
	// 100msごとにgame_mainを起動する
	if(++clk >= 50){
		clk = 0;
		user = 1;
	}

	// 0.5sごとにカーソルを点滅させる処理
	if(++cursor_clk >= 250){
		cursor_clk = 0;
		target.state = (target.state == LED_ON) ? LED_OFF : LED_ON;
	} 
}



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
	// ターゲットのいる場所をONかOFFにする
	if(scan == target.y){
		uchar temp = 1 << (LED_SIZE - target.x - 1);
		if(target.state == LED_ON)
			led[scan] |= temp;
		else
			led[scan] &= ~temp;
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
	timer_init();
	switch_init();

	// 割り込み処理を実行
	sei();

	// ゲームを回すforループ
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
					target.x = (target.x > 0) ? target.x - 1 : LED_SIZE - 1;
					break;	
				case 2:
					target.y = (target.y > 0) ? target.y - 1 : LED_SIZE - 1;
					break;	
				case 3:
					putStone(target.x, target.y, target.turn);
					break;	
			}
		}

		if(user){
			game_main();
			user=0;
		}
	}
	return 0;
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

// ゲームの内容を初期化する関数
void game_init(){
	int x, y;
	ledCount = 0;
	for(y = 0;y < LED_SIZE;y++){
		for(x=0;x < LED_SIZE;x++){
			ledPower[y][x] = LED_OFF;
		}
		led[y] = 0x00;
	}
	ledPower[3][3] = LED_ON;
	ledPower[4][4] = LED_ON;
	ledPower[3][4] = LED_MIDDLE;
	ledPower[4][3] = LED_MIDDLE;

	target.x = 2;
	target.y = 4;
	target.state = target.turn = LED_ON;
}

// タイマカウンタを使用するための初期化
void timer_init(){
	OCR0A = 249; // 2ms
	TCCR0A = 2; // CTC MODE
	TCCR0B = 3; // 64PS
	TIMSK0 |= (1 << OCIE0A);
}

// ゲームの本体
void game_main(){

}


// 石をおけるか否かの判断
int judgePutStone(int x, int y, int turn)
{
	if (y < 1 || y > 8 || x < 1 || x > 8) return 0;
	if (ledPower[y][x] != LED_OFF) return 0;
	if (countTurnOver(turn, y, x, -1,  0)) return 1;  // 上 
	if (countTurnOver(turn, y, x,  1,  0)) return 1;  // 下 
	if (countTurnOver(turn, y, x,  0, -1)) return 1;  // 左 
	if (countTurnOver(turn, y, x,  0,  1)) return 1;  // 右 
	if (countTurnOver(turn, y, x, -1, -1)) return 1;  // 左上
	if (countTurnOver(turn, y, x, -1,  1)) return 1;  // 右上
	if (countTurnOver(turn, y, x,  1, -1)) return 1;  // 左下
	if (countTurnOver(turn, y, x,  1,  1)) return 1;  // 右下
	return 0;
}

// 石を置く処理
void putStone(int x, int y, int turn){
	int count, d, e, i;
	if(judgePutStone(x,y,turn) == 0)
		return;

	for (d = -1; d <= 1; d++) {      // 上下方向
		for (e = -1; e <= 1; e++) {  // 左右方向
			if (d == 0 && e == 0) continue; 
			count = countTurnOver(turn, y, x, d, e);
			for (i = 1; i <= count; i++) {
				ledPower[y+i*d][x+i*e] = turn; // 裏返す
			}
		}
	}
	ledPower[y][x] = turn; // 石を置く
	target.turn = (target.turn == LED_ON) ? LED_MIDDLE : LED_ON;
}

// 石を置いたときにどれだけ石を裏返せるかを調べる
int countTurnOver(int turn, int y, int x, int d, int e)
{
	int i;
	int aite = (turn == LED_ON) ? LED_MIDDLE : LED_ON;

	for (i = 1; ledPower[y+i*d][x+i*e] ==aite; i++) {};        

	if (ledPower[y+i*d][x+i*e] == turn) {                             
		return i-1;   
	} else {
		return 0;   
	}
}
