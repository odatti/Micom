#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "Random.h"

#define EEPADDR 0x000

/** 擬似乱数の初期化 */
void random_init(){
        srand(eeprom_read_word((uint16_t *) EEPADDR));
        eeprom_write_word((uint16_t *) EEPADDR, rand());
}
/** 擬似乱数の生成 */
int random_rand(){
	return (int)rand();
}
