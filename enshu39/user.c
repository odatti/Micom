#include "user.h"

static void MoveFort(void);
static void MoveEnemy(void);
static void MoveBullet(void);
static void UpdateLED(void);

static uchar enemy[LED_SZ];
static uchar bullet[LED_SZ];
static uchar fort;

void user_init()
{
	uchar i;
	for(i = 0;i < LED_SZ;i++){
		enemy[i] = 0;
		bullet[i] = 0;
	}
	fort = 3;
}

void user_main(void){
	MoveFort();
	MoveEnemy();
	MoveBullet();
	UpdateLED();
}

static void MoveEnemy(void)
{
	static uchar tick = 0;
	static uchar dir = 0;
	uchar i;
	
	if(++tick<10){
		return;
	}
	
	tick = 0;
	
	switch(dir){
	case 0:
		for(i=0;i<LED_SZ;i++)
			enemy[i] >>= 1;
		break;
		
	case 2:
		for(i=0;i<LED_SZ;i++)
			enemy[i] <<= 1;
		break;
		
	default:
		for(i=0;i<(LED_SZ - 1);i++)
			enemy[i] = enemy[i+1];
		enemy[LED_SZ-1] = dir == 3 ? 0xaa : 0;
		break;
	}
	dir = (dir + 1) & 3;
}

static void MoveBullet(void)
{
	uchar i;
	for(i=LED_SZ-1;i;i--)
	bullet[i] = bullet[i-1];
	bullet[0] = 0;
}

static void MoveFort(void)
{
	int fshot = 0;
	int i;
	switch(sw){
		case 1:
			if(fort > 1)
				fort--;
			break;

		case 2:
			if(fort < (LED_SZ - 2))
				fort++;
			break;

		case 3:
			for(i = 0;i < (LED_SZ-1);i++){
				if(bullet[i] > 0){
					fshot = 1;
				}
			}
			if(fshot == 0)
				bullet[0] |= 0x80 >> fort;
			break;
	}
}

static void UpdateLED(void)
{
	uchar i,j;
	
	for(i=0;i<LED_SZ;i++){
		j=enemy[i] & bullet[i];
		if(j){
			enemy[i] ^= j;
			bullet[i] ^= j;
		}
		led[i] = enemy[i] | bullet[i];
	}
	led[0] |= (uchar)(0x1c0 >> fort);
	led[1] |= (uchar)(0x80 >> fort);
}
