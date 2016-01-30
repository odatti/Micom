#ifndef _MY_GLOBAL_H_
#define _MY_GLOBAL_H_

/** BGM再生用の構造体 */
typedef struct CODE{
	volatile unsigned char tone;
	volatile unsigned char length;
};
/** どちらのプレイヤーが勝利したのか判断する変数 */
volatile unsigned char win_player;
#endif 
