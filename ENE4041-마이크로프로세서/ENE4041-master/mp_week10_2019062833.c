#include <avr/io.h>

#define FND_C0 0x01
#define FND_C1 0x02
#define FND_C2 0x04
#define FND_C3 0x08

const char Font[17] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00 };

void delay(unsigned long x) {
	while (x--);
}

void fnd_display(unsigned char digit0, unsigned char digit1, unsigned char digit2, unsigned char digit3) {


	PORTG |= 0x0F;
	PORTG &= ~FND_C0;
	PORTB = Font[digit0];
	delay(100);

	PORTG |= 0x0F;
	PORTG &= ~FND_C1;
	PORTB = Font[digit1];
	delay(100);

	PORTG |= 0x0F;
	PORTG &= ~FND_C2;
	PORTB = Font[digit2];
	delay(100);

	PORTG |= 0x0F;
	PORTG &= ~FND_C3;
	PORTB = Font[digit3];
	delay(100);
}

int main() {
	unsigned char digit0, digit1, digit2, digit3,digit4;
	int c_val;
	DDRB = 0xFF;
	DDRG = 0xFF;
	PORTB = 0x00;
	PORTG = 0xFF;

	TCCR1A = 0x00;
	TCCR1B = 0x07;

	while (1) {
		c_val= (TCNT1H<<8)+TCNT1L;//8비트+8비트

		digit0 = c_val/ 10000;
		digit1 = (c_val % 10000)/1000;
		digit2 = ((c_val % 10000)%1000)/100;
		digit3 = (((c_val % 10000) % 1000) % 100)/10;
		digit4 = (((c_val % 10000) % 1000) % 100)%10;

		fnd_display(digit0, digit1, digit2, digit3);
	}

}