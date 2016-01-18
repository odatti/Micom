#include<avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>

volatile unsigned char led[8] = {
	0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

ISR(ADC_vect){
	int n;
	char val;

	static char comp[9][2] = {
		{90,0xFF},
		{80,0x7F},
		{70,0x3F},
		{60,0x1F},
		{50,0x0F},
		{40,0x07},
		{30,0x03},
		{20,0x01},
		{10,0x00},
	};
	val = ADCH;
	for(n = 0;n < 7;n++){
		led[n] = led[n + 1];
	}
	for(n = 0;n < 9;n++){
		if(comp[n][0] < val){
			led[7] = comp[n][1];			
			break;
		}
	}
	DDRC |= 0x20;
	DDRC &= ~0x20;
}

ISR(TIMER1_COMPB_vect){
	TCNT1 = 0;
}

ISR(TIMER0_COMPA_vect){
	static unsigned scan;
	unsigned char sc;

	PORTB = 0;
	scan = (scan + 1) & 7;
	sc = ~(1 << scan);

	PORTC = sc & 0x0f;
	PORTD = sc & 0xf0;
	PORTB = led[7 - scan];
}

int main()
{
	DDRB = 0xff;
	DDRC = 0x0f;
	DDRD = 0xfa;

	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;

	DIDR0 = _BV(ADC5D);

	ADMUX = 0x45 | _BV(ADLAR);

	ADCSRA = _BV(ADIE) | _BV(ADATE) | _BV(ADEN) | 0b110;
	ADCSRB = 5;

	ADCSRA |= _BV(ADSC) | _BV(ADIF);

	TIMSK1 |= _BV(OCIE1B);

	OCR1B = 2000;
	TCCR1A = 0;
	TCCR1B = 0x0c;

	OCR0A = 249;
	TCCR0A = 2;
	TCCR0B = 3;
	TIMSK0 |= (1 << OCIE0A);
	sei();
	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
