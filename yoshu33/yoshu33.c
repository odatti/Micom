#include<avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>
#define SW ((~PINC>>4)&3)
volatile unsigned char j;
ISR(PCINT1_vect){
	if(0 < SW){
		j++;
		j &= 0x7;
		PORTB = 1 << j;
	}
}
int main(void)
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;
	PORTB = 0xFF;
	PORTC = 0x30;
	PORTD = 0x00;
	PCICR = _BV(PCIE1);
	PCMSK1 = _BV(PCINT12) | _BV(PCINT13);
	sei();
	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
