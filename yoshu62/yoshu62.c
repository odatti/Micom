#include<avr/io.h>
#include<avr/wdt.h>
#include<ctype.h>

static unsigned char _getchar(void){
	unsigned char c;
	while((UCSR0A & (1 << RXC0)) == 0)
		wdt_reset();
	c = UDR0;
	return (c == '\r') ? '/n' : c;
}
static void _putchar(unsigned char c){
	if(c == '\n')
		_putchar('\r');
	while((UCSR0A & (1 << UDRE0)) == 0)
		wdt_reset();
	UDR0 = c;
}

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
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	for(;;){
		x = _getchar();
		PORTB = x;
		_putchar(toupper(x));
	}
	return 0;
}
