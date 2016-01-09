#include<avr/io.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<string.h>
typedef struct CODE{
	char str[3];
	unsigned char tone;
}CODE;

volatile char c[6];
volatile int onpu;
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
	char o[2];
	int n = -1;
	int i;
	for(i = 0;i < 5;i++){
		c[i]=UDR0;
		while((UCSR0A & (1 << RXC0)) == 0)
			wdt_reset();
	}
	c[5]=UDR0;

	for(i = 0;i <= 25;i++){
		n = (strncmp(c,code[i].str,2) == 0) ? 0 : n;
		n = (strncmp(c,code[i].str,3) == 0) ? 1 : n;
	}
	if(0 <= n){
		o[0] = c[3+n];
		o[1] = c[4+n];
		if(strncmp(o,"2",1) == 0)onpu = 2;
		if(strncmp(o,"4",1) == 0)onpu = 4;
		if(strncmp(o,"8",1) == 0)onpu = 8;
	 	if(strncmp(o,"16",2) == 0)onpu = 16;
		if(play==0){
			play=1;
			PORTB=c;
		}
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
		int i;
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
			for(i = 0;i < (int)(16 / onpu);i++){
			 	_delay_ms(250);
			}
			TCCR2B &= 0xF8;
			play = 0;
		}
	}
	return 0;
}
