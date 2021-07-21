#include <avr/io.h>

int main(){
	DDRA = 0xFF;
	PORTA = 0xFE;
	return 0;
}
