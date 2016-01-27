#include "Target.h"
#include "Led.h"

/** プレイヤーの操作に関する構造体 */
typedef struct TARGET{
        int x,y;
        int state;
        int turn;
}TARGET;

/** プレイヤーの操作に関する変数 */
static TARGET target;
static volatile int player1;
static volatile int player2;

/** ターゲットの初期化 */
void target_init(int x, int y, int p1, int p2){
	target.x = x;
	target.y = y;
	player1 = p1;
	player2 = p2;
	target.state = target.turn = p1;
}
 
/** ターゲットの現在地 */
int target_getX(){
	return target.x;
}
int target_getY(){
	return target.y;
}
/** ターゲットの点滅状態 */
int target_getState(){
	return target.state;
}

/* ターゲットを左に移動させる処理 */
void target_moveLeft(){
	target.x = (target.x>0)?target.x-1:7;
}

/** ターゲットを下に移動させる処理 */
void target_moveDown(){
	target.y = (target.y<7)?target.y+1:0;
}

/** 現在誰の番なのかを取得 */
int target_getTurn(){
	return target.turn;
}

/** 次の人へ */
void target_nextTurn(){
	target.turn = (target.turn == player1) ? player2 : player1;
}

/** ターゲットの点滅状態を反転 */
void target_reverseState(){
	target.state = (target.state == target.turn) ? LED_OFF : target.turn;
}

