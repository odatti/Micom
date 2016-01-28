#include "AI.h"
#include "Led.h"
#include "Referee.h"
#include "Random.h"

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
/** 重み付けAIの判断に利用 */
typedef struct AI_TARGET{
        int index;// 座標
        int score;// 重み
}AI_TARGET;
int normal_ai(int turn){
        int x,y;
        int i;
        AI_TARGET putList[64];
        int index = 0;
        int decide = 0;
        for(i = 0;i < 64;i++){
                putList[i].index = 0;
                putList[i].score = 0;
        }
        for(y = 0;y < LED_SIZE;y++){
                for(x = 0;x < LED_SIZE;x++){
                        if(judgePutStone(x,y,turn) == 1){
                                putList[index].index = y*LED_SIZE+x;
                                putList[index].score = ai_normal_map[y][x];
                                index++;
                        }
                }
        }
        // 一番スコアの高い場所を調べる(リニアサーチなので予測しやすい)
        for(i=0;i<index;i++){
                if(putList[decide].score < putList[i].score){
                        decide = i;
                }
        }

        if(index != 0){
                index = putList[decide].index;
                x = index % LED_SIZE;
                y = (int)(index / LED_SIZE);
                putStone(x, y, turn);
		return 1;
	}
	return 0;
}

int random_ai(int turn){
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
                return 0;

        index = putList[random_rand() % index];
        x = index % LED_SIZE;
        y = (int)(index / LED_SIZE);
	putStone(x, y, turn);
	return 1;
}
