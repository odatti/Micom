#ifndef _MY_LED_H_
#define _MY_LED_H_

#define LED_SIZE 8
/** LEDがオフの時の点滅時間(OFFの場合は光らない) 及び 何もないマス */
#define LED_OFF 6
/** LEDがオンの時の点滅間隔 及び 白の石が置いてあるマス */
#define LED_ON 1
/** LEDが中間色の時の点滅時間 及び 黒の石が置いてあるマス */
#define LED_MIDDLE 5

/** LEDの処理に関するものの初期化処理 */
extern void led_init();

/** LEDの明るさ情報ここに登録されている情報通りにLEDを光らせる */
volatile int ledPower[LED_SIZE][LED_SIZE];

extern void led_target_on();
extern void led_target_off();

#endif 
