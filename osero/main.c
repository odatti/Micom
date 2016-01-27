// include
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>

#define LED_SIZE 8
/** LEDがオフの時の点滅時間(OFFの場合は光らない) 及び 何もないマス */
#define LED_OFF 6
/** LEDがオンの時の点滅間隔 及び 白の石が置いてあるマス */
#define LED_ON 1
/** LEDが中間色の時の点滅時間 及び 黒の石が置いてあるマス */
#define LED_MIDDLE 5

/** 各機能に利用 */
#define SW_INTERVAL 6000UL
#define SW ((~PINC>>4)&3)
#define EEPADDR 0x000

/** ゲームの状態 */
enum
{
	PLAYING,
	FINISH,
	FINISHED
};
static volatile int gameState;

/** 各種初期化関数 */
void game_init();
void led_init();
void switch_init();
void timer_init();
void sound_init();
void rand_init();

/** ゲームの状態によって何らかの操作をするときはここ */
void game_main();

/** 置けるマスの中からランダムに配置を決定する対戦相手 */
void random_ai(int turn);

/** オセロ機能に関する関数軍 */
int judgePutStone(int x, int y, int turn);
void putStone(int x, int y, int turn);
int countTurnOver(int turn, int x, int y, int d, int e);
/** まだ続くなら0, もう終わるならLED_ONの個数を返す */
int isFinishGame(int turn);
void sortLED();

typedef unsigned char uchar;
static volatile uchar user;

/** プレイヤーの操作に関する構造体 */
typedef struct TARGET{
	int x,y;
	int state;
	int turn;
}TARGET;

/** LEDの明るさ情報 及び ボードの配置情報 */
volatile int ledPower[LED_SIZE][LED_SIZE];
/** LEDの明るさを制御するためのカウント変数 */
volatile int ledCount;
/** LED表示用 */
volatile uchar led[LED_SIZE];
static volatile uchar scan;

/** スイッチとチャタリング対策に使う変数 */
volatile uchar sw;
volatile uchar swnow;
volatile uchar swnew;
volatile unsigned swcnt;
/** ピン変化割り込みとスイッチの処理に使用 */
volatile unsigned char pc = 0;

/** プレイヤーの操作に関する変数 */
TARGET target;
/** 各種機能やタイマカウンタで用いる変数 */
static volatile uchar clk;
static volatile int cursor_clk;
static volatile uchar rnd;

/** 音を鳴らす処理 */
static volatile uchar period;
void _sound(uchar tone, uchar length);
enum
{
	BEEP_HIGH = 46,
	BEEP_LOW = 168,

	BEEP_C4 = 238,
	BEEP_D4 = 212,
	BEEP_E4 = 189,
	BEEP_F4 = 178,
	BEEP_G4 = 158,
	BEEP_A4 = 141,
	BEEP_B4 = 126,
	BEEP_C5 = 118
};

int ai_clk;
/** 2ms毎に呼ばれる関数（タイマカウンタ）*/
ISR(TIMER0_COMPA_vect){
	// 100msごとにgame_mainを起動する
	if(++clk >= 50){
		clk = 0;
		if(period){
			if(--period==0){
				TCCR2A = 0;
			}
		}
		user = 1;
	}
	if(++ai_clk >= 500){
		ai_clk = 0;
		if(target.turn == LED_MIDDLE)
			random_ai(target.turn);
	}

	// 0.5sごとにカーソルを点滅させる処理
	if(++cursor_clk >= 250){
		cursor_clk = 0;
		target.state = (target.state == target.turn) ? LED_OFF : target.turn;
	} 
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
		if(scan == LED_SIZE - target.y - 1 && target.x == x){
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
	sound_init();
	rand_init();

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
					target.y = (target.y < LED_SIZE - 1) ? target.y + 1 : 0;
					break;	
				case 3:
					if(target.turn != LED_ON)
						break;
					if(gameState == PLAYING)
					{
						putStone(target.x, target.y, target.turn);
						if(isFinishGame(target.turn) > 0) gameState = FINISH;
						ai_clk = 0;
					}
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
	PORTB = 0xff;
	// LED捜査用のタイマカウンタ
	TCCR1A = 0; 
	TCCR1B = 0b00001010; // CTCモード(OCR1A), PS=8
	OCR1A = 500;
	TIMSK1 |= (1 << OCIE1A);
	
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
	gameState = 0;
}

// タイマカウンタを使用するための初期化
void timer_init(){
	OCR0A = 249; // 2ms
	TCCR0A = 2; // CTC MODE
	TCCR0B = 3; // 64PS
	TIMSK0 |= (1 << OCIE0A);
}
void rand_init(){
	srand(eeprom_read_word((uint16_t *) EEPADDR));
	eeprom_write_word((uint16_t *) EEPADDR, rand());
}
void sound_init(){
	TCCR2A = 0;
	TCCR2B = 0x44;
}

void _sound(uchar tone, uchar length){
	OCR2A = tone;
	period = length;
	TCCR2A = 0x12;
}
// ゲームの本体
void game_main(){
	switch(gameState){
		case FINISH:
			sortLED();
			gameState = FINISHED;
			break;
		case PLAYING:
			break;
		case FINISHED:
		default:
			break;
	}
}


// 石をおけるか否かの判断
int judgePutStone(int x, int y, int turn)
{
	// if (y < 0 || y > 7 || x < 0 || x > 7) return 0;
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
	_sound(BEEP_C5, 3);
	target.turn = (target.turn == LED_ON) ? LED_MIDDLE : LED_ON;
}

// 石を置いたときにどれだけ石を裏返せるかを調べる
int countTurnOver(int turn, int y, int x, int d, int e)
{
	int i;
	int aite = (turn == LED_ON) ? LED_MIDDLE : LED_ON;

	for (i = 1; ledPower[y+i*d][x+i*e] ==aite; i++) {
		if(y+i*d < 0 || y+i*d > LED_SIZE - 1 || x+i*e < 0 || x+i*e > LED_SIZE - 1){
			break;
		}
	};        

	if (ledPower[y+i*d][x+i*e] == turn) {                             
		return i-1;   
	} else {
		return 0;   
	}
}


// まだ続くなら0, もう終わるならLED_ONの個数を返す
int isFinishGame(int turn){
	int x,y;
	int on = 0;
	
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			// 置く場所があるなら続けられる
			if(judgePutStone(x,y,turn) == 1)
				return 0;
		}
	}

	// ONの個数を調べる
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			switch(ledPower[y][x]){
				case LED_ON: on++; break;
				case LED_MIDDLE:
				case LED_OFF:
				default: break;;
			}
		}
	}
	return on;
}

void replace(int x, int y, int ix, int iy){
	int temp = ledPower[iy][ix];
	ledPower[iy][ix] = ledPower[y][x];
	ledPower[y][x] = temp;
}

void sortLED(){
	int ix=0,iy=0;
	int x, y;
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			if(ledPower[y][x] == LED_ON){
				replace(x,y,ix,iy);
				ix++;
				if(ix == LED_SIZE){
					ix = 0;
					iy++;
				}
			}
		}
	}
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			if(ledPower[y][x] == LED_MIDDLE){
				replace(x,y,ix,iy);
				ix++;
				if(ix == LED_SIZE){
					ix = 0;
					iy++;
				}
			}
		}
	}
}


void random_ai(int turn){
	int x,y;
	int putList[64];
	int index = 0;
	for(x = 0;x < 64;x++){
		putList[64] = 0;
	}
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			if(judgePutStone(x,y,turn) == 1){
				putList[index] = y*LED_SIZE+x;
				index++;
			}
		}
	}
	if(index==0)
		return;
	
	index = putList[rand() % index];	
	x = index % LED_SIZE;
	y = (int)(index / LED_SIZE);
	putStone(x, y, turn);
	if(isFinishGame(target.turn) > 0) gameState = FINISH;
}

