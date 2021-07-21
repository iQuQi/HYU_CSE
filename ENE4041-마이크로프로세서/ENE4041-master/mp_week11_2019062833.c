#include <avr/io.h>

#define FND_C0 0x01
#define FND_C1 0x02
#define FND_C2 0x04
#define FND_C3 0x08

const char Font[17] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00 };
unsigned char digit0 = 0;
unsigned char digit1 = 0;
unsigned char digit2 = 0;
unsigned char digit3 = 0;
int i = 0,j;

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

SIGNAL(SIG_INTERRUPT0) { i = 0000; }
SIGNAL(SIG_INTERRUPT1) { i = 1111;   }
SIGNAL(SIG_INTERRUPT2) { i = 2222; 
}
SIGNAL(SIG_INTERRUPT3) { i = 3333; 
}
SIGNAL(SIG_INTERRUPT4) { i = 4444; 
}
SIGNAL(SIG_INTERRUPT5) { i = 5555; 
}
SIGNAL(SIG_INTERRUPT6) { i = 6666; 
}
SIGNAL(SIG_INTERRUPT7) { i = 7777; 
}


int main() {
	unsigned char digit0, digit1, digit2, digit3, digit4;
	int c_val;
	DDRB = 0xFF;
	DDRG = 0xFF;
	PORTB = 0x00;
	PORTG = 0xFF;

	EIMSK = 0xFF;
	EICRA = 0xFF;
	EICRB = 0xFF;

	sei();

	while (1) {
		for (i = 0,j=i; i < 9999; i++)
		{
			
			digit0 = i / 1000;
			digit1 = (i% 1000) / 100;
			digit2 = ((i% 1000) % 100) / 10;
			digit3 = (((i % 1000) % 100) % 10);
			for (int n = 0; n <10; n++)
			{
				fnd_display(digit0, digit1, digit2, digit3);
			}
			i = j;
		
		}
		
	}

}
