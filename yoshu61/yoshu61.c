#include<avr/io.h>
#include<avr/wdt.h>
#include<ctype.h>

int main()
{
	char x;
	DDRD = 0xFA;
	DDRC = 0x0F;
	DDRB = 0xFF;
	PORTD = 0x00;
	PORTC = 0x00;

	UBRR0 = 103;
	UCSR0C = 0b00000110;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);

	for(;;){
		while((UCSR0A & _BV(RXC0)) == 0)
			wdt_reset(); // ウォッチドッグタイマのリセット
		x = UDR0;

		PORTB = x;
		while((UCSR0A & _BV(UDRE0)) == 0)
			wdt_reset(); // ウォッチドッグタイマのリセット
		UDR0 = toupper(x);

	}
	return 0;
}
