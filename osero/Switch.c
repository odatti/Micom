#include <avr/interrupt.h>
#include "Switch.h"

/** チャタリング待ち時間 */
#define SW_INTERVAL 4000UL
/** スイッチの状態 */
#define SW ((~PINC>>4)&3)

/** スイッチとチャタリング対策に使う変数 */
volatile unsigned char sw;
volatile unsigned char swnow;
volatile unsigned char swnew;
volatile unsigned swcnt;
/** ピン変化割り込みとスイッチの処理に使用 */
volatile unsigned char pc = 0;


/** スイッチのピン変化割り込み */
ISR(PCINT1_vect){
        if(pc == 0){
                pc = 1;
        }
        // ブロックしているピン変化割り込みをキャンセル
        PCIFR |= _BV(PCIF1);
}

/** スイッチ読み取りに使うものを初期化する */
void switch_init(){
	// PORTCのピン変化割り込み有効
	PCICR |= _BV(PCIE1);
	// 割り込みを認めるビット位置を指定
	PCMSK1 = _BV(PCINT12) | _BV(PCINT13);
	swnow = 0x30;
	swnew = 0x30;
	sw = 0;
}

/** スイッチの入力を更新する */
void switch_update(){
	if(pc){
		swcnt = (swcnt < SW_INTERVAL) ? swcnt + 1 : 0;
		if(swcnt == 0){
			swnow = SW;
			pc = 0;
		}
	}
}

/** スイッチの状態が変化して初めて呼ばれたら1を、それ以外なら0を返す */
int switch_isChanged(){
	if(swnow != sw){
		sw = swnow;
		return 1;
	}
	return 0;
}

/** 現在のスイッチの状態を取得する */
int switch_getState(){
	return sw;
}

