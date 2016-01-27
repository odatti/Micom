#ifndef _MY_SOUND_H
#define _MY_SOUND_H_

enum
{
        BEEP_HIGH = 46,
        BEEP_LOW = 168,

        BEEP_C4 = 238,
        BEEP_D4 = 212,
        BEEP_E4 = 189,
        BEEP_F4 = 178,
        BEEP_G4 = 158,
        BEEP_A4 = 141,
        BEEP_B4 = 126,
        BEEP_C5 = 118
};

extern void sound_init();
extern void _sound(unsigned char tone, unsigned char length);
extern void sound_update();

#endif 
