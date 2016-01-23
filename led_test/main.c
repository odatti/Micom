#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "user.h"

volatile uchar sw;
volatile uchar led[LED_SZ];

static volatile uchar scan;

static volatile uchar clk;
static volatile uchar period;

static volatile uchar user;
static volatile uchar delay;

static volatile uchar swnow;

static volatile uchar swnew;

static volatile uchar swcnt;

static volatile uchar rnd;

extern void user_init(void);
extern void user_main(void);

ISR(TIMER0_COMPA_vect){
	unsigned char sc;
	PORTB = 0;
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);
	PORTC = 0x30 | (sc & 0x0f);
	PORTD = sc & 0xf0;
	PORTB = led[scan];

	sc = PINC & 0x30;
	if(sc != swnow){
		if(sc == swnew){
			if(++swcnt > 20){
				swnow = swnew;
				sw = (swnow >> 4) ^ 3;
			}
		}
		else{
		swnew = sc;
		swcnt = 0;
		}
		rnd = rnd + scan + sw;
	}
	
	if(++clk >= 50){
		clk = 0;
		if(delay)
			delay--;

		if(period){
			if(--period == 0)
				TCCR2A = 0;
		}
		user = 1;
	}
}

int main(void){
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTC = 0x30;
	
	swnow = 0x30;
	swnew = 0x30;
	sw = 0;

	OCR0A = 249;
	TCCR0A = 2;
	TCCR2B = 3;
	TIMSK0 |= (1 << OCIE0A);

	TCCR2A = 0;
	TCCR2B = 0x44;
	
	user_init();

	sei();
	for(user=0;;){
		wdt_reset();
		if(user){
			user_main();
			user = 0;
		}
	}
	return 0;
}

void _wait(uchar wait){
	for(delay = wait;delay;){
		wdt_reset();
	}
}

uchar _rand(void){
	rnd++;
	return (uchar)(rnd ^ (rnd<<1));
}

void _sound(uchar tone, uchar length){
	OCR2A = tone;
	period = length;
	TCCR2A = 0x12;
}
