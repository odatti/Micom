#include<avr/io.h>
#include<avr/wdt.h>
int main()
{
	// This is a template source!
	// You can write code here!
	DDRC = 0x20;
	PORTC &= ~0x20;

	while(1){
		wdt_reset(); // ウォッチドッグタイマのリセット
	}
	return 0;
}
