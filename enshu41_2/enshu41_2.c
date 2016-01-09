#include<avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
typedef struct CODE{
	char str[4];
	unsigned char tone;
}CODE;

volatile char c[3];
volatile char play;
struct CODE code[] = {
	"C4",238,
	"C#4",225,
	"D4",212,
	"D#4",200,
	"E4",189,
	"F4",178,
	"F#4",168,
	"G4",159,
	"G#4",150,
	"A4",142,
	"A#4",134,
	"B4",126,
	"C5",119,
	"C#5",113,
	"D5",106,
	"D#5",100,
	"E5",95,
	"F5",89,
	"F#5",84,
	"G5",80,
	"G#5",75,
	"A5",71,
	"A#5",67,
	"B5",63,
	"END",0
};

ISR(USART_RX_vect)
{
	c[0]=UDR0;
	while((UCSR0A & (1 << RXC0)) == 0)
		wdt_reset();
	c[1]=UDR0;
	while((UCSR0A & (1 << RXC0)) == 0)
		wdt_reset();
	c[2]=UDR0;
	if(play==0){
		play=1;
		PORTB=c;
	}
}
int main(void)
{
	// int i = 2;
	DDRD = 0xF8;
	DDRC = 0x0F;
	DDRB = 0xFF;

	PORTD = 0x0F;
	PORTC = 0x00;
	PORTB = 0x00;

	UCSR0C = 3 << UCSZ00;
	UBRR0 = 103;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0);

	TCCR2A = 0x12;

	sei();
	wdt_enable(WDTO_2S);

	while(1){
		int i = 0;
		wdt_reset();
		if(play){
			for(i = 0;i <= 25;i++){
				if(strncmp(c,code[i].str,2) == 0){
					OCR2A = code[i].tone;
				}
				if(strncmp(c,code[i].str,3) == 0){
					OCR2A = code[i].tone;
				}
			}
			TCNT2 = 0;
			TCCR2B |= 0x04;
			_delay_ms(1000);
			TCCR2B &= 0xF8;
			play = 0;
		}
		_delay_ms(100);
	}
	return 0;
}
