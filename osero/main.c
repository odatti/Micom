// include
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Sound.h"
#include "Switch.h"
#include "Random.h"
#include "Target.h"
#include "Led.h"
#include "Referee.h"

typedef unsigned char uchar;

static volatile uchar menuLed[3][LED_SIZE] = {
{
	0b00000000,
	0b11000110,
	0b10100101,
	0b11000110,
	0b10000100,
	0b00101000,
	0b00010000,
	0b00000000
},{
	0b01001110,
	0b10100100,
	0b11100100,
	0b10101110,
	0b00000000,
	0b10000110,
	0b10101010,
	0b11010111
},{
	0b01001110,
	0b10100100,
	0b11100100,
	0b10101110,
	0b00000010,
	0b10000101,
	0b10101010,
	0b11010111
}
};

/** ゲームの状態 */
enum
{
	MENU,
	GAME_INIT,
	PLAYING,
	FINISH,
	FINISHED,
};
static volatile int gameState;

/** 各種初期化関数 */
void timer_init();

/** ゲームの状態によって何らかの操作をするときはここ */
void game_play();
void game_menu();
void nextMenu();

/** プレイする人の情報 */
#define PLAYER_TYPE_MAX 3
enum{
	PLAYER = 0,
	EASY = 1,
	NORMAL = 2
};
static volatile int player2 = NORMAL;
/** 置けるマスの中からランダムに配置を決定する対戦相手 */
void random_ai(int turn);
static volatile int ai_clk;

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
	led_init();
	timer_init();
	switch_init();
	sound_init();
	random_init();
	gameState = MENU;
	led_target_off();
	nextMenu();

	// 割り込み処理を実行
	sei();

	// ゲームを回すforループ
	while(1){
		wdt_reset();
		switch_update();
		switch(gameState){
			case MENU:
				game_menu();
				break;
			case GAME_INIT:
				led_reset();
				// 最初の石とターゲットを配置
				ledPower[3][3] = LED_ON;
				ledPower[4][4] = LED_ON;
				ledPower[3][4] = LED_MIDDLE;
				ledPower[4][3] = LED_MIDDLE;
				target_init(2,4,LED_ON, LED_MIDDLE);
				led_target_on();
				gameState = PLAYING;
				break;
			case PLAYING:
				game_play();
				break;
			case FINISH:
				led_target_off();
				sortLED();
				gameState = FINISHED;
				break;
			case FINISHED:
			default:
				break;
		}
	}
	return 0;
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

void game_menu(){
	// スイッチの結果を更新
	if(switch_isChanged()){
		switch(switch_getState()){
			case 1:
				nextMenu();
				break;
			case 2:
				gameState = GAME_INIT;
				break;	
			default: break;	
		}
	}
}
void nextMenu(){
	int x,y;
	player2 = (player2 < PLAYER_TYPE_MAX - 1) ? player2 + 1 : 0;
	led_reset();
	for(y = 0;y < LED_SIZE;y++){
		for(x = 0;x < LED_SIZE;x++){
			ledPower[y][x] = (menuLed[player2][y] & (0x01 << (LED_SIZE - x -1))) ? LED_ON : LED_OFF;
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
