#include "Global.h"
#include "Led.h"
#include "Target.h"
#include "sound.h"
#include "Referee.h"

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

int judgePutStone(int x, int y, int turn){
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


int countTurnOver(int turn, int y, int x, int d, int e){
        int i;
        int aite = (turn == LED_ON) ? LED_MIDDLE : LED_ON;

        for (i = 1; ledPower[y+i*d][x+i*e] ==aite; i++) {
                if(y+i*d < 0 || y+i*d > LED_SIZE - 1 || x+i*e < 0 || x+i*e > LED_SIZE - 1){
                        break;
                }
        };
	if(y+i*d < 0 || y+i*d > LED_SIZE - 1 || x+i*e < 0 || x+i*e > LED_SIZE - 1){
		return 0;
	}

        if (ledPower[y+i*d][x+i*e] == turn) {                          
                return i-1;
        } else {
                return 0;
        }
}


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
	int temp = 0;
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
	temp = LED_SIZE * iy + ix;
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
	win_player = (temp > (int)(iy*LED_SIZE+ix-temp)) ? LED_ON : LED_MIDDLE;
}

