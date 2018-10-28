/*
 * BrymenConnector.cpp
 *
 * Firmware for my IR interface cable for Brymen 867/869 multimeters
 * Author : martin@embedblog.eu
 * Info: http://embedblog.eu/2018/10/27/brymen-bm867-869-interface-cable/
 * License: GNU GPL v3
 * IC: ATtiny 25/45/85
 * Fuses: low 0xE2; high 0xDF; ext 0xFF
 */ 


#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include "softUART/softUART.h"
softUART uart;
#include "dmm/dmm.h"

uint8_t data[16];
volatile int8_t mode = MODE_1X_PER_SEC;

int main(void)
{
    //setup IO - output for the IR  & activity LEDs
	DDRB |= (1 << DDB4) | (1 << DDB0);
	IRLED_HIGH;
	//input for the phototransistor, just to be sure
	DDRB &=~(1 << DDB2);
	
	uart.init();

    while (1)
	{
		LED_HIGH;
		if (getData() == 1)
		{
			//succesfull transmission, send data via uart
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






