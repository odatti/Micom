#include<avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>
volatile char c;
volatile char stat;
typedef struct TONE
{
	char c;
	unsigned char val;
}TONE;

ISR(USART_RX_vect)
{
	c = UDR0;
	PORTB = c;
	stat = 1;
}

unsigned char c2val(unsigned char c)
{
	int n;
	const struct TONE tone[] = {
		{'c', 238},
		{'d', 212},
		{'e', 189},
		{'f', 178},
		{'g', 158},
		{'a', 141},
		{'b', 126},
		{'C', 118},
		{'\0', 0},
	};
	for(n = 0;'\0' != tone[n].c; n++){
		if(c == tone[n].c)
			break;
	}
	return tone[n].val;
}

int main()
{
	DDRB = 0xFF;
	DDRC = 0x0f;
	DDRD = 0xFA;

	PORTB = 0xAA;
	PORTC = 0x00;
	PORTD = 0x00;

	UCSR0C = 3 << UCSZ00;
	UBRR0 = 103;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0);

	TCCR2A = 0x12;

	TCCR1A = 0;
	TCCR1B = 0;
	sei();


	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
		switch(stat){
			case 0:
				break;
			case 1:
				OCR2A = c2val(c);
				TCNT1 = 0;
				TCCR1B |= 0x04;
				TCCR2B |= 0x04;
				stat = 2;
				break;
			case 2:
				if(4 * 3125 < TCNT1){
					TCCR2B &= 0xF8;
					TCCR1B &= 0xF8;
					TCNT1 = 0;
					stat = 0;
				}
				break;
		}
	}
	return 0;
}
