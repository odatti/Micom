#ifndef _MY_SWITCH_H_
#define _MY_SWITCH_H_


/** スイッチの処理に使うものを初期化する */
extern void switch_init();
/** スイッチ入力を更新する */
extern void switch_update();
/** 入力値が変化した時に、初めて呼ばれたら1を、それ以外は0を返す */
extern int switch_isChanged();
/** 現在のスイッチの状態を返す */
extern int switch_getState();

#endif 
