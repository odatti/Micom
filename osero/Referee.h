#ifndef _MY_REFEREE_H_
#define _MY_REFEREE_H_

/** オセロ機能に関する関数軍 */
extern int judgePutStone(int x, int y, int turn);
extern void putStone(int x, int y, int turn);
extern int countTurnOver(int turn, int x, int y, int d, int e);
/** まだ続くなら0, もう終わるならLED_ONの個数を返す */
extern int isFinishGame(int turn);
extern void sortLED();


#endif 
