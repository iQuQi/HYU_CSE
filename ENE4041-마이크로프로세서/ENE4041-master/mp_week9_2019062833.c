#include <avr/io.h>

#define LCD_CTRL PORTG
#define LCD_DATA PORTA
#define LCD_RS 0x01
#define LCD_RW 0x02
#define LCD_E 0x04

void enablePulse() {
	LCD_CTRL |= LCD_E;
	asm("NOP");
	LCD_CTRL &= ~LCD_E;
}

void sendLCDcommend(unsigned char command) {
	LCD_CTRL &= ~LCD_RS;
	LCD_DATA = command;
	enablePulse();
}

void initLCD(int line) {
	LCD_CTRL &= ~LCD_RW;
	sendLCDcommend(0x38);
	sendLCDcommend(0x0C);
	sendLCDcommend(0x01);
	locate(line, 1);
	dispString("_");
	locate(line, 1);
	dispString(" ");
	locate(line, 1);
	dispString("_");

}

void sendLDCdata(unsigned char data) {
	LCD_CTRL |= LCD_RS;
	LCD_DATA = data;
	enablePulse();
	
}

void dispString(char *str) {
	while (*str) {
		sendLDCdata(*str);
		str++;
	}
}

void locate(int x, int y) {
	unsigned char ramAddr;
	if (x == 0) ramAddr = 0x80 + y;
	else ramAddr = 0xC0 + y;
	sendLCDcommend(ramAddr);
}

int main() {


	DDRA = 0xFF;
	DDRG = 0xFF;
	PORTA = 0x00;
	PORTG = 0x00;

	initLCD();

	locate(0, 1);
	dispString("1234");

	initLCD(0);
	
	locate(1, 1);
	dispString("_");

	locate(1, 1);



}