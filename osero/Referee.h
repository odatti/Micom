#ifndef _MY_REFEREE_H_
#define _MY_REFEREE_H_

/** 指定した場所に、指定した石をおけるか確認する。おけるなら1を返す */
extern int judgePutStone(int x, int y, int turn);
/** 指定した場所に、指定した石を置く処理 */
extern void putStone(int x, int y, int turn);
/** 指定した場所に、指定した方向に向けて、自分の石をいくつおけるか取得する */
extern int countTurnOver(int turn, int x, int y, int d, int e);
/** まだ続くなら0, もう終わるならLED_ONの個数を返す */
extern int isFinishGame(int turn);
/** 石を並び替える処理 */
extern void sortLED();


#endif 
