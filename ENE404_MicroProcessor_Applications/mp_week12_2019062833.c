#include <avr/io.h>
#include <avr/interrupt.h>

#define TRUE 1
#define FALSE 0
const char Font[17] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00 };


volatile unsigned int t2_100 = 0;

SIGNAL(SIG_OVERFLLOW 0) {
	TCNT0 = 112;
	t2_100++;
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
	
	unsigned char update = TRUE; 
	unsigned char digit0, digit1, digit2, digit3;
	int val=0;

	DDRB = 0xFF;
	PORTB = 0x00;

	TCCR0 = 0x07;
	TCNTO = 112;
	TIMSK = 0x01;

	sei();

	while (1) {
		if (t2_100 == 100) {
			update = TRUE;
			t2_100 = 0;
		}

		if (update == TRUE) {
			update = FALSE;

			val++;
			digit0 = val / 1000;
			digit1 = (val % 1000) / 100;
			digit2 = ((val % 1000) % 100) / 10;
			digit3 = ((val % 1000) % 100) % 10;


			fnd_display(digit0, digit1, digit2, digit3);



		}
	}

}
