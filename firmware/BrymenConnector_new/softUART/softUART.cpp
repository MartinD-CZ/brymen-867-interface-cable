/* softUART.cpp
 * 
 * Author : martin@embedblog.eu
 * A simple software-implemented UART with RX/TX
 */ 

#include "..\config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "softUART.h"

volatile uint8_t mode = MODE_DEFAULT;
volatile bool rawData = false;

void uart_init()
{
	//setup TX pin
	DDRB |= (1 << DDB4);
	TX_HIGH;

	//setup RX pin
	DDRB &=~(1 << DDB3);
	PCMSK |= (1 << PCINT3);
	GIMSK |= (1 << PCIE);
	sei();
}

void uart_sendChar(uint8_t ch)
{
	TX_LOW;
	_delay_us(UART_DELAY);
	TX_HIGH;

	for (uint8_t i = 0; i < 8; i++)
	{
		if ((ch >> i) & 0x01)
		TX_HIGH;
		else
		TX_LOW;
		_delay_us(UART_DELAY);
	}

	TX_HIGH;
	_delay_us(UART_DELAY);
}

void uart_sendString(char* s)
{
	while(*s)  uart_sendChar(*s++);
}

void uart_sendString_P(const char* s)
{
	while (pgm_read_byte(s)) uart_sendChar(pgm_read_byte(s++));
}

void uart_sendNumber(uint16_t num, uint8_t base)
{
	char buf[6];
	itoa(num, buf, base);
	uart_sendString(buf);
}

//note: putting delays in interrupts is far from ideal, but since this is the only interrupt vector enabled...
ISR(PCINT0_vect)
{
	if (!RX_LOW)	return;
	
	uint8_t data = 0x0;
	
	_delay_us(UART_DELAY + (UART_DELAY / 2));

	for (uint8_t i = 0; i < 8; i ++)
	{
		if (!RX_LOW)
		data |= (1 << i);
		_delay_us(UART_DELAY);
	}

	/*if (data == 'R')
		rawData = !rawData;
	else*/

	switch (data)
	{
		case 'R': rawData = !rawData; break;
		case 'F': mode = MODE_5X_PER_SEC; break;
		case 'O': mode = MODE_1X_PER_SEC; break;
		case 'S': mode = MODE_STOP; break;
		case 'D': mode = MODE_SEND; break;
		default: break;
	}

	//clear interrupt flag
	GIFR |= (1 << INTF0);
}
