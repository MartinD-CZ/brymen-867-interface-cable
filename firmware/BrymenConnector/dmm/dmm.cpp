/*
 * dmm.cpp
 *
 * Cpp file with all the data processing functions for the Brymen 867/869 protocol
 * Author : martin@embedblog.eu
 */ 

#include "..\settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include "dmm.h"
#include "..\softUART\softUART.h"
extern softUART uart;

extern uint8_t data[16];

uint8_t irPulse(void)
{
	uint8_t temp = 0;

	for (uint8_t i = 0; i < 8; i ++)
	{
		IRLED_LOW;
		_delay_us(CLOCK_DELAY);
		
		if (!IRTRAN_LOW)
		temp |= (1 << i);

		IRLED_HIGH;
		_delay_us(CLOCK_DELAY);
	}

	return temp;
}

int8_t getData(void)
{
	//10 ms pulse to init it
	IRLED_LOW;
	_delay_ms(10);
	IRLED_HIGH;

	int8_t status = 0;
	uint32_t i = 0;
	while (status == 0)
	{
		i++;
		if (i == 0x1FFFF)	//we have an overflow (about 100 ms)
		status = -1;

		if (IRTRAN_LOW)		//the pin is low
		status = 1;
	}

	if (status == 1)	//we had a contact, start clocking
	{
		IRLED_LOW;					//a blank pulse, to init the transfer
		_delay_us(CLOCK_DELAY);
		IRLED_HIGH;
		_delay_us(CLOCK_DELAY);
		
		for (uint8_t j = 0; j < 16; j ++)		//we really care only about the first 16 bytes of data
			data[j] = irPulse();

		for (uint8_t j = 0; j < 5; j ++)		//so lets discard the rest. And also do an extra clock cycle, to avoid errors
			irPulse();
	}

	return status;
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

void processData(void)
{
	//DECODE ATTRIBUTES
	if (data[0] & 0x01)
		uart.sendString((char*)"AUTO ");
	if ((data[0] >> 7) & 0x01)
		uart.sendString((char*)"AVG ");
	if ((data[0] >> 6) & 0x01)
		uart.sendString((char*)"MIN ");
	if ((data[0] >> 5) & 0x01)
		uart.sendString((char*)"MAX ");
	if ((data[0] >> 2) & 0x01)
		uart.sendString((char*)"CREST ");
	if ((data[0] >> 1) & 0x01)
		uart.sendString((char*)"REC ");
	if ((data[0] >> 3) & 0x01)
		uart.sendString((char*)"HOLD ");
	if (data[2] & 0x01)
		uart.sendString((char*)"DELTA ");
	if (data[9] & 0x01)
		uart.sendString((char*)"BEEP ");

	//DECODE MAIN DISPLAY
	uart.sendString((char*)"MAIN: ");

	if ((data[1] >> 7) & 0x01)
	uart.sendChar('-');


	for (uint8_t i = 0; i < 6; i++)
	{
		uart.sendChar(48 + decodeDigit(data[2 + i]));
		if ((data[3 + i] & 0x01) & (i < 4))
			uart.sendChar('.');
	}
	uart.sendChar(' ');
	
	//DECODE UNIT PREFIX FOR MAIN DISPLAY
	if ((data[13] >> 6) & 0x01)
		uart.sendChar('n');
	if ((data[14] >> 3) & 0x01)
		uart.sendChar('u');
	if ((data[14] >> 2) & 0x01)
		uart.sendChar('m');
	if ((data[14] >> 6) & 0x01)
		uart.sendChar('k');
	if ((data[14] >> 5) & 0x01)
		uart.sendChar('M');
	
	//DECODE UNIT FOR MAIN DISPLAY
	if (data[7] & 0x01)
		uart.sendChar('V');
	if ((data[13] >> 7) & 0x01)
		uart.sendChar('A');
	if ((data[13] >> 5) & 0x01)
		uart.sendChar('F');
	if ((data[13] >> 4) & 0x01)
		uart.sendChar('S');
	if ((data[14] >> 7) & 0x01)
		uart.sendString((char*)"D%");
	if ((data[14] >> 4) & 0x01)
		uart.sendString((char*)"Ohm");
	if ((data[14] >> 1) & 0x01)
		uart.sendString((char*)"dB");

	if (data[14] & 0x01)
		uart.sendString((char*)"Hz");
	uart.sendChar(' ');

	//DC OR AC
	if ((data[0] >> 4) & 0x01)
		uart.sendString((char*)"DC ");
	if (data[1] & 0x01)
		uart.sendString((char*)"AC ");

	//DECODE AUXILIARY DISPLAY
	uart.sendString((char*)"AUX: ");

	if ((data[8] >> 4) & 0x01)
		uart.sendChar('-');

	for (uint8_t i = 0; i < 4; i++)
	{
		uart.sendChar(48 + decodeDigit(data[9 + i]));
		if ((data[10 + i] & 0x01) & (i < 3))
			uart.sendChar('.');
	}
	
	uart.sendChar(' ');

	//DECODE UNIT PREFIX FOR AUXILIARY DISPLAY
	if ((data[8] >> 1) & 0x01)
		uart.sendChar('m');
	if (data[8] & 0x01)
		uart.sendChar('u');
	if ((data[13] >> 1) & 0x01)
		uart.sendChar('k');
	if (data[13] & 0x01)
		uart.sendChar('M');

	//DECODE UNIT FOR AUXILIARY DISPLAY
	if ((data[13] >> 2) & 0x01)
		uart.sendString((char*)"Hz");
	if ((data[13] >> 3) & 0x01)
		uart.sendChar('V');
	if ((data[8] >> 2) & 0x01)
		uart.sendChar('A');
	if ((data[8] >> 3) & 0x01)
		uart.sendString((char*)"%4-20mA");
	uart.sendChar(' ');
	if ((data[13] >> 2) & 0x01)
		uart.sendString((char*)"AC");

	//FINISH
	uart.sendChar('\n');

	if ((data[8] >> 7) & 0x01)
		uart.sendString((char*)"LOW BAT\n");
}

