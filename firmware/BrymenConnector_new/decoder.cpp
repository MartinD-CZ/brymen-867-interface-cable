#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "decoder.h"
#include "softUART\softUART.h"

uint8_t data[20];
extern volatile bool rawData;

uint8_t getByte(void)
{
	uint8_t temp = 0;

	for (uint8_t i = 0; i < 8; i ++)
	{
		IRLED_HIGH;
		_delay_us(CLOCK_DELAY);
		
		IRLED_LOW;
		if (!IRTRAN_LOW)
			temp |= (1 << i);
		_delay_us(CLOCK_DELAY);
	}

	return temp;
}

uint8_t getData(void)
{
	IRLED_HIGH;
	_delay_ms(10);
	IRLED_LOW;

	uint8_t timeout = 0xFF;
	while (!IRTRAN_LOW)
	{
		timeout--;
		if (!timeout)	return 0;
		_delay_us(2000);
	}

	for (uint8_t i = 0; i < 20; i ++)
		data[i] = getByte();

	return 1;
}

void processData(void)
{
	if (rawData)
	{
		uart_sendString_P(PSTR("RAW DATA: "));
		
		for (uint8_t i = 0; i < 20; i++)
		{
			uart_sendNumber(data[i], 16);
			uart_sendChar(' ');
		}

		uart_sendChar('\n');
		return;
	}

	//decode the output
	if (data[0] & 0x01)
		uart_sendString_P(PSTR("AUTO "));
	if ((data[0] >> 7) & 0x01)
		uart_sendString_P(PSTR("AVG "));
	if ((data[0] >> 6) & 0x01)
		uart_sendString_P(PSTR("MIN "));
	if ((data[0] >> 5) & 0x01)
		uart_sendString_P(PSTR("MAX "));
	if ((data[0] >> 2) & 0x01)
		uart_sendString_P(PSTR("CREST "));
	if ((data[0] >> 1) & 0x01)
		uart_sendString_P(PSTR("REC "));
	if ((data[0] >> 3) & 0x01)
		uart_sendString_P(PSTR("HOLD "));
	if (data[2] & 0x01)
		uart_sendString_P(PSTR("DELTA "));
	if (data[9] & 0x01)
		uart_sendString_P(PSTR("BEEP "));

	//DECODE MAIN DISPLAY
	uart_sendString_P(PSTR("MAIN: "));

	if ((data[1] >> 7) & 0x01)
		uart_sendChar('-');

	for (uint8_t i = 0; i < 6; i++)
	{
		uart_sendChar(48 + decodeDigit(data[2 + i]));
		if ((data[3 + i] & 0x01) & (i < 4))
		uart_sendChar('.');
	}
	uart_sendChar(' ');
	
	//DECODE UNIT PREFIX FOR MAIN DISPLAY
	if ((data[13] >> 6) & 0x01)
		uart_sendChar('n');
	if ((data[14] >> 3) & 0x01)
		uart_sendChar('u');
	if ((data[14] >> 2) & 0x01)
		uart_sendChar('m');
	if ((data[14] >> 6) & 0x01)
		uart_sendChar('k');
	if ((data[14] >> 5) & 0x01)
		uart_sendChar('M');
	
	//DECODE UNIT FOR MAIN DISPLAY
	if (data[7] & 0x01)
		uart_sendChar('V');
	if ((data[13] >> 7) & 0x01)
		uart_sendChar('A');
	if ((data[13] >> 5) & 0x01)
		uart_sendChar('F');
	if ((data[13] >> 4) & 0x01)
		uart_sendChar('S');
	if ((data[14] >> 7) & 0x01)
		uart_sendString_P(PSTR("D%"));
	if ((data[14] >> 4) & 0x01)
		uart_sendString_P(PSTR("Ohm"));
	if ((data[14] >> 1) & 0x01)
		uart_sendString_P(PSTR("dB"));
	if (data[14] & 0x01)
		uart_sendString_P(PSTR("Hz"));
	uart_sendChar(' ');

	//DC OR AC
	if ((data[0] >> 4) & 0x01)
		uart_sendString_P(PSTR("DC "));
	if (data[1] & 0x01)
		uart_sendString_P(PSTR("AC "));

	//DECODE AUXILIARY DISPLAY
		uart_sendString_P(PSTR("AUX: "));

	if ((data[8] >> 4) & 0x01)
		uart_sendChar('-');

	for (uint8_t i = 0; i < 4; i++)
	{
		uart_sendChar(48 + decodeDigit(data[9 + i]));
		if ((data[10 + i] & 0x01) & (i < 3))
		uart_sendChar('.');
	}
	
	uart_sendChar(' ');

	//DECODE UNIT PREFIX FOR AUXILIARY DISPLAY
	if ((data[8] >> 1) & 0x01)
		uart_sendChar('m');
	if (data[8] & 0x01)
		uart_sendChar('u');
	if ((data[13] >> 1) & 0x01)
		uart_sendChar('k');
	if (data[13] & 0x01)
		uart_sendChar('M');

	//DECODE UNIT FOR AUXILIARY DISPLAY
	if ((data[13] >> 2) & 0x01)
		uart_sendString_P(PSTR("Hz"));
	if ((data[13] >> 3) & 0x01)
		uart_sendChar('V');
	if ((data[8] >> 2) & 0x01)
		uart_sendChar('A');
	if ((data[8] >> 3) & 0x01)
		uart_sendString_P(PSTR("%4-20mA"));
		uart_sendChar(' ');
	if ((data[13] >> 2) & 0x01)
		uart_sendString_P(PSTR("AC "));

	//FINISH
	uart_sendChar('\n');

	//detect low battery
	if ((data[8] >> 7) & 0x01)
		uart_sendString_P(PSTR("LOW BAT\n"));
}

int8_t decodeDigit(uint8_t source)
{
	int8_t result = 0;

	switch (source >> 1)
	{
		case 0b1011111: result = 0; break;
		case 0b1010000: result = 1; break;
		case 0b1101101: result = 2; break;
		case 0b1111100: result = 3; break;
		case 0b1110010: result = 4; break;
		case 0b0111110: result = 5; break;
		case 0b0111111: result = 6; break;
		case 0b1010100: result = 7; break;
		case 0b1111111: result = 8; break;
		case 0b1111110: result = 9; break;
		case 0b1111001: result = ('d' - 48); break;
		case 0b0010000: result = ('i' - 48); break;
		case 0b0111001: result = ('o' - 48); break;
		case 0b0001011: result = ('L' - 48); break;
		case 0b0000000: result = (' ' - 48); break;
		default: result = ('?' - 48); break;
	}

	return result;
}
