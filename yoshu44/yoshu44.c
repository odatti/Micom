#include<avr/io.h>
#include<avr/wdt.h>
int main()
{
	int n;
	
	DDRD = 0b11111010;
	DDRC = 0b00001111;
	DDRB = 0b11111111;
	
	PORTD = 0x00;
	PORTC = 0x00;
	PORTB = 0x01;

	//OCR1A = 15625;
	TCCR1A = 0x00;
	TCCR1B = 0x04;

	for(n = 0;n < 7;n++){
		TCNT1 = 0;
		TIFR1 |= _BV(TOV1);
		// TIFR1 |= _BV(OCF1A);
		// while(TCNT1 < 15625){
		while((TIFR1&_BV(TOV1))==0){
		// while((TIFR1&_BV(OCF1A))==0){
			wdt_reset();
		}
		PORTB |= PORTB << 1;
	}
	TCCR1B = 0x00;

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
