#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define LED_SIZE 8
#define LED_OFF 100
#define LED_ON 0
#define LED_MIDDLE 50

typedef struct MY_LED{
	unsigned int state;
	unsigned int count;
}MY_LED;


typedef unsigned char uchar;
volatile uchar led[LED_SIZE];
static volatile uchar user;
static volatile MY_LED myLed[LED_SIZE][LED_SIZE];


/*
volatile uchar sw;
volatile uchar led[LED_SZ];

static volatile uchar scan;
static volatile uchar clk;

static volatile uchar delay;

static volatile uchar swnow;
static volatile uchar swnew;
static volatile uchar swcnt;
*/
/*
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
*/

int cnt;
ISR(TIMER2_COMPA_vect){
	if(++cnt == 500){
		cnt = 0;
		PORTB = ~PORTB;
	}
}


int main(void){
	cnt = 0;
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTC = 0x30;
	PORTD = 0x00;
	PORTB = 0xff;
	
	TCCR2A = 2;
	TCCR2B = 4;
	OCR2A = 249;
	TIMSK2 |= (1 << OCIE2A);

/*
	swnow = 0x30;
	swnew = 0x30;
	sw = 0;
	OCR0A = 249;
	TCCR0A = 2;
	TCCR2B = 3;
	TIMSK0 |= (1 << OCIE0A);

	TCCR2A = 0;
	TCCR2B = 0x44;
	
*/
	sei();
	for(user=0;;){
		wdt_reset();
/*
		if(user){
			user_main();
			user = 0;
		}
*/
	}
	return 0;
}


