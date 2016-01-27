// include
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Sound.h"
#include "Switch.h"
#include "Random.h"
#include "Target.h"
#include "Led.h"

typedef unsigned char uchar;
/** ゲームの状態 */
enum
{
	PLAYING,
	FINISH,
	FINISHED,
	MENU
};
static volatile int gameState;

/** 各種初期化関数 */
void game_init();
void timer_init();

/** ゲームの状態によって何らかの操作をするときはここ */
void game_play();
void game_finish();
void game_finished();
void game_menu();

/** 置けるマスの中からランダムに配置を決定する対戦相手 */
void random_ai(int turn);
static volatile int ai_clk;
static volatile uchar aite;
enum{
	PLAYER,
	EASY,
	NORMAL
};
/** 石の位置による重み付け情報 */
static volatile int ai_normal_map[LED_SIZE][LED_SIZE] = {
	{ 30,-12,  0, -1, -1,  0,-12, 30},
	{-12,-15, -3, -3, -3, -3,-15,-12},
	{  0, -3,  0, -1, -1,  0, -3,  0},
	{ -1, -3, -1, -1, -1, -1, -3, -1},
	{ -1, -3, -1, -1, -1, -1, -3, -1},
	{  0, -3,  0, -1, -1,  0, -3,  0},
	{-12,-15, -3, -3, -3, -3,-15,-12},
	{ 30,-12,  0, -1, -1,  0,-12, 30}
};

/** オセロ機能に関する関数軍 */
int judgePutStone(int x, int y, int turn);
void putStone(int x, int y, int turn);
int countTurnOver(int turn, int x, int y, int d, int e);
/** まだ続くなら0, もう終わるならLED_ONの個数を返す */
int isFinishGame(int turn);
void sortLED();

/** 各種機能やタイマカウンタで用いる変数 */
static volatile uchar clk;
static volatile int cursor_clk;
static volatile uchar rnd;


/** 2ms毎に呼ばれる関数（タイマカウンタ）*/
ISR(TIMER0_COMPA_vect){
	// 100msごとにgame_mainを起動する
	if(++clk >= 50){
		clk = 0;
		// 指定した時間になると停止させる
		sound_update();
	}
	if(++ai_clk >= 500){
		ai_clk = 0;
		if(target_getTurn() == LED_MIDDLE)
			random_ai(target_getTurn());
	}

	// 0.5sごとにカーソルを点滅させる処理
	if(++cursor_clk >= 250){
		cursor_clk = 0;
		target_reverseState();
	} 
}

int main(void){
	// LED表示するための変数
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTC = 0x30;
	PORTD = 0x00;
	PORTB = 0x00;
	// 初期化
	game_init();
	timer_init();
	switch_init();
	sound_init();
	random_init();

	// 割り込み処理を実行
	sei();

	// ゲームを回すforループ
	while(1){
		wdt_reset();
		switch_update();
		switch(gameState){
			case FINISH:
				game_finish();
				break;
			case PLAYING:
				game_play();
				break;
			case FINISHED:
			default:
				break;
		}
	}
	return 0;
}

// ゲームの内容を初期化する関数
void game_init(){
	led_init();
	ledPower[3][3] = LED_ON;
	ledPower[4][4] = LED_ON;
	ledPower[3][4] = LED_MIDDLE;
	ledPower[4][3] = LED_MIDDLE;

	target_init(2,4,LED_ON, LED_MIDDLE);
	gameState = PLAYING;
}

// タイマカウンタを使用するための初期化
void timer_init(){
	OCR0A = 249; // 2ms
	TCCR0A = 2; // CTC MODE
	TCCR0B = 3; // 64PS
	TIMSK0 |= (1 << OCIE0A);
}

// ゲームの本体
void game_play(){
	led_target_on();
	// スイッチの結果を更新
	if(switch_isChanged()){
		switch(switch_getState()){
			case 0:
				break;	
			case 1:
				target_moveLeft();
				break;	
			case 2:
				target_moveDown();
				break;	
			case 3:
				if(target_getTurn() != LED_ON)
					break;
				putStone(target_getX(), target_getY(), target_getTurn());
				if(isFinishGame(target_getTurn()) > 0) gameState = FINISH;
				ai_clk = 0;
				break;	
		}
	}
}
void game_finish(){
	led_target_on();
	sortLED();
	gameState = FINISHED;
}
void game_finished(){
}
void game_menu(){

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
	target_nextTurn();
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
	
	index = putList[random_rand() % index];	
	x = index % LED_SIZE;
	y = (int)(index / LED_SIZE);
	putStone(x, y, turn);
	if(isFinishGame(target_getTurn()) > 0) gameState = FINISH;
}

