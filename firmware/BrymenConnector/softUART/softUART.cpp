/*
 * softUART.cpp
 *
 * A simple as-small-as-possible software-implemented UART
 * Author : martin@embedblog.eu
 */ 

#include "..\settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "softUART.h"

extern volatile uint8_t mode;

void softUART::init()
{
	//setup TX pin
	DDRB |= (1 << DDB1);
	STX_HIGH;

	//setup RX pin - PB2 as input
	DDRB &=~(1 << DDB2);
	MCUCR |= (1 << ISC01);		//INT on falling edge
	GIMSK |= (1 << INT0);		//enable INT0
	sei();
}

void softUART::sendChar(uint8_t ch)
{
	STX_LOW;
	_delay_us(SUART_DELAY);
	STX_HIGH;

	for (uint8_t i = 0; i < 8; i++)
	{
		if ((ch >> i) & 0x01)
		STX_HIGH;
		else
		STX_LOW;
		_delay_us(SUART_DELAY);
	}

	STX_HIGH;
	_delay_us(SUART_DELAY);
}

void softUART::sendString(char* s)
{
	while(*s)  sendChar(*s++);
}

ISR (INT0_vect)
{
	uint8_t data = 0x0;
	
	_delay_us(SUART_DELAY + (SUART_DELAY / 2));

	for (uint8_t i = 0; i < 8; i ++)
	{
		if (!SRX_LOW)
			data |= (1 << i);
		_delay_us(SUART_DELAY);
	}

	switch (data)
	{
		case 0xC1: mode = MODE_5X_PER_SEC; break;
		case 0xC2: mode = MODE_1X_PER_SEC; break;
		case 0xC3: mode = MODE_STOP; break;
		case 0xC4: mode = MODE_SEND; break;
		default: break;
	}

	//clear interrupt flag
	GIFR |= (1 << INTF0);
}
