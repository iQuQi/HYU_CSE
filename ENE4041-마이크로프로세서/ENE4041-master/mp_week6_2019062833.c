#include <avr/io.h>
#define KEY_CTRL PORTE
#define R0 0x10

const char Font[17] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F
,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00 };


void delay(unsigned long x) {
	while (x--);
}

unsigned char key_scan() {
	unsigned char input_data = 0;
	unsigned char scan = 0;
	unsigned char key_control = 0;
	unsigned char key = 0;

	key_control = R0;

	for (scan = 0; scan < 4; scan++)
	{
		KEY_CTRL &= 0x0f;
		KEY_CTRL |= key_control;
		delay(1);
		input_data = PINF & 0x07;

		if (input_data!=0) {
			key = (input_data >> 1) + 1 + scan * 3;

			//여기채워주기
		}
		
		key_control <<= 1; // R0->R1->R2->R3로 순환
	}

	//input으로 key를 판별해서 반환 
		return key;
}


int main() {
	
	unsigned char led = 0;
	unsigned char key = 0;

	unsigned char digit1, digit0;



	DDRF = 0x00;
	DDRE = 0xFF;
	DDRA = 0xFF;

	DDRB = 0xFF;
	DDRC = 0xFF;



	while (1) {
		//키를 받아옴
		key = key_scan();

		led = 0x01;


		//받아온 키에따라 led를 작동
		if (key == 0) {
			digit0 = 0;
			digit1 = 0;
			led = 0x00;
		}
		else if (key < 9) {
			digit0 = key;
			digit1 = 0;
			led = led << (key - 1);
		}
		else if (key == 9) {
			digit0 = 9;
			digit1 = 0;
			led = 0x81;
		}
		else if (k == 10) {
			digit0 = 0;
			digit1 = 1;
			led = 0x0F;
		}

		else if (key == 11) {
			digit0 = 1;
			digit1 = 1;
			led = 0xFF;
		}
		else if (k == 12) {
			digit0 = 2;
			digit1 = 1;
			led = 0xF0;
		}

		PORTA = led;
		PORTB = Font[digit1];
		PORTC = Font[digit0];



	}


}