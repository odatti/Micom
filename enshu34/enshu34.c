#include<avr/io.h>
#include<avr/wdt.h>
#include <avr/interrupt.h>

#define SW ((~PINC>>4)&1)

ISR(PCINT1_vect){
	if((TCCR2B == 0) && (0 < SW)){
		TCCR2B = 0x04; // 音の再生開始
		TCCR0B = 3; // タイマ割り込み開始
		OCR2A = 30;
	}
}

// 2ms
ISR(TIMER0_COMPA_vect){
	static int n = 0;
	// 0.2s
	if(n==100){
		OCR2A = 62;
	}
	if(n == 200){
		TCCR2B = 0;
		TCCR0B = 0;
		TCNT0 = 0;
		n = 0;
		return;
	}
	n++;
	// OCR2A--;
}

int main()
{
	DDRD = 0x08;
	DDRC = 0x00;
	PORTC = 0x10;

	TCCR2A = 0x12;

	OCR0A = 249;
	TCCR0A = 2;
	TCCR0B = 0;
	TIMSK0 |= (1 << OCIE0A);
	
	PCICR = _BV(PCIE1);
	PCMSK1 = _BV(PCINT12) | _BV(PCINT13);

	sei();

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
