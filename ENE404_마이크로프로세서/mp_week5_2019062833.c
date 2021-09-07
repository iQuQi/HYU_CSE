#include <avr/io.h>


int main() {
	unsigned char input_data;
	DDRD = 0x00;
	DDRB = 0xFF;

	while (1) {
		input_data = PIND & 0x0f;

		if (input_data < 8)
			PORTB = 0x01 << input_data;
		else
			PORTB = 0x00;
	}

}