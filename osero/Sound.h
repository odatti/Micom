#ifndef _MY_SOUND_H
#define _MY_SOUND_H_

/** 鳴らすことができる音 */
enum
{
        BEEP_HIGH = 46,
        BEEP_LOW = 168,

	BEEP_C4 = 238,
        BEEP_CS4= 225,
        BEEP_D4 = 212,
        BEEP_DS4= 200,
        BEEP_E4 = 189,
        BEEP_F4 = 178,
        BEEP_FS4= 168,
        BEEP_G4 = 159,
        BEEP_GS4= 150,
        BEEP_A4 = 142,
        BEEP_AS4= 134,
        BEEP_B4 = 126,
        BEEP_C5 = 119,
        BEEP_CS5= 113,
        BEEP_D5 = 106,
        BEEP_DS5= 100,
        BEEP_E5 = 95,
        BEEP_F5 = 89,
        BEEP_FS5= 84,
        BEEP_G5 = 80,
        BEEP_GS5= 75,
        BEEP_A5 = 71,
        BEEP_AS5= 67,
        BEEP_B5 = 63,
        BEEP_END = 0,
	BEEP_FINISH = 1000
};

/** 音の処理に使用するものを初期化する */
extern void sound_init();
/** 指定した音を、指定した時間鳴らす処理 */
extern void _sound(unsigned char tone, unsigned char length);
/** 音の再生状態を更新するための処理、音の長さはupdateが呼ばれる回数である。 */
extern void sound_update();
/** 現在音が再生されているなら1を、それ以外なら0を返す */
extern int sound_isPlaying();

#endif 
