/*
 * BrymenConnector_new.cpp
 * Info: adapter, used to send data from a Brymen 867/9 to a PC using UART
 * Author : martin@embedblog.eu
 * More info: http://embedblog.eu/?p=475
 * IC: ATtiny45
 * Fuses: L0x02 H0xDF E0xFF
 * License:  CC BY-SA 4.0 International
 */ 

#include <avr/io.h>
#include "config.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "softUART/softUART.h"
#include "decoder.h"

extern volatile uint8_t mode;

int main(void)
{
	//setup IO - output for the IR  & activity LEDs
	DDRB |= (1 << DDB0) | (1 << DDB1);

	//input for the phototransistor
	DDRB &=~(1 << DDB2);

	//give the serial terminal a while to react
	_delay_ms(500);		
	
	uart_init();
	uart_sendString_P(PSTR("Brymen 867/869 interface cable\nfor more info, see embedblog.eu/?p=475\n"));
	uart_sendString_P(PSTR("Available commands:\n"));
	uart_sendString_P(PSTR("F - 5 samples per second\n"));
	uart_sendString_P(PSTR("O - 1 sample per second\n"));
	uart_sendString_P(PSTR("S - stop autosend\n"));
	uart_sendString_P(PSTR("D - send a single reading\n"));
	uart_sendString_P(PSTR("R - toggle raw data output\n"));
	
	while (1) 
    {
		if (getData())
		{
			LED_HIGH;
			processData();
			LED_LOW;

			if (mode == MODE_1X_PER_SEC)
				_delay_ms(920);

			if ((mode == MODE_STOP) | (mode == MODE_SEND))
			{
				mode = MODE_STOP;
				while (mode == MODE_STOP);
			}
		}
    }
}

