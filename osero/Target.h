#ifndef _MY_TARGET_H_
#define _MY_TARGET_H_

/** ターゲットの初期化 */
extern void target_init(int x, int y, int p1, int p2);

/** ターゲットの現在地 */
extern int target_getX();
extern int target_getY();
/** ターゲットの点滅状態 */
extern int target_getState();
/** ターゲットの移動処理 */
extern void target_moveLeft();
extern void target_moveDown();
/** 現在誰の番かを取得する */
extern int target_getTurn();
/** 次の人へ */
extern void target_nextTurn();
/** ターゲットの点滅状態を反転 */
extern void target_reverseState();

#endif 
