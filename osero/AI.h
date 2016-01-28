#ifndef _MY_AI_H_
#define _MY_AI_H_

/** プレイする人の情報 */
#define PLAYER_TYPE_MAX 3
enum{
        PLAYER = 0,
        EASY = 1,
        NORMAL = 2
};

/** 置けるマスの中からランダムに配置を決定する対戦相手 */
extern int random_ai(int turn);
/** マスの重みを考慮して戦うちょっと間抜けなAI */
extern int normal_ai(int turn);

#endif 
