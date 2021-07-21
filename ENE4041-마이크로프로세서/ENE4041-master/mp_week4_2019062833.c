#include <avr/io.h>
#define FND_C0 0x01
#define FND_C1 0x02
#define FND_C2 0x04
#define FND_C3 0x08

const char Font[17] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F
,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00 };

void delay(unsigned long x) {
	while (x--);
}


int main(void)
{
	unsigned char digit0, digit1, digit2, digit3;
	DDRB = 0xFF;
	DDRG = 0xFF;
	PORTB = 0x00;
	PORTG = 0xFF;
	for (int i = 0; i < 10; i++)
	{

		for (int j = 0; j < 10; j++)
		{

			for (int k = 0; k < 10; k++)
			{

				for (int t = 0; t < 10; t++)
				{
					PORTG |= 0x0F;
					PORTG &= ~FND_C0;
					PORTB = Font[i];

					PORTG |= 0x0F;
					PORTG &= ~FND_C1;
					PORTB = Font[j];

					PORTG |= 0x0F;
					PORTG &= ~FND_C2;
					PORTB = Font[k];

					PORTG |= 0x0F;
					PORTG &= ~FND_C3;
					PORTB = Font[t];
				}
				delay(10000)
			}
		}
	}


}
