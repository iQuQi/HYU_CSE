#include <avr/io.h>

void my_delay(unsigned long x)
{
	while (x--);
}

int main() {
	DDRB = 0xFF;
	while (1) {

		PORTB = 0x00000001;
		mydelay(100000);
		PORTB = 0x00000010;
		mydelay(100000);
		PORTB = 0x0000100;
		mydelay(100000);
		PORTB = 0x00001000;
		mydelay(100000);
		PORTB = 0x00010000;
		mydelay(100000);
		PORTB = 0x00100000;
		mydelay(100000);
		PORTB = 0x01000000;
		mydelay(100000);
		PORTB = 0x10000000;
		mydelay(100000);
	
	}
	
}