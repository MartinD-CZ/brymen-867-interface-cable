/*
 * settings.h
 *
 * Header with all the important settings
 * Author : martin@embedblog.eu
 * 
 * DEFAULT PINOUT:
 *										  ________
 *									RST -|°       |- Vcc = +5 V
 *		phototransistor collector = PB3	-|        |- PB2 = software UART RX
 *					 IR LED anode = PB4 -|        |- PB1 = software UART TX
 *									GND	-|________|- PB0 = activity LED
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#define F_CPU 8000000

//SETTINGS FOR SOFTWARE UART
#define SUART_DELAY	104
#define STX_HIGH	PORTB |= (1 << PORTB1)
#define STX_LOW		PORTB &=~(1 << PORTB1)
#define SRX_LOW		!(PINB & (1 << PINB2))

#define IRLED_HIGH	PORTB |= (1 << PORTB4)
#define IRLED_LOW	PORTB &=~(1 << PORTB4)
#define IRTRAN_LOW	!(PINB & (1 << PINB3))
#define CLOCK_DELAY	150

#define LED_HIGH	PORTB |= (1 << PORTB0)
#define LED_LOW		PORTB &=~(1 << PORTB0)

//modes
#define MODE_5X_PER_SEC		0
#define MODE_1X_PER_SEC		1
#define MODE_STOP			2
#define MODE_SEND			3


#endif /* SETTINGS_H_ */