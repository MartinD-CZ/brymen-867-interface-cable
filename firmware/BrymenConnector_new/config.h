/*
 * config.h
 *
 * Created: 10.10.2019 22:48:01
 *  Author: marti
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU				8000000UL

#define UART_BAUD			9600
#define TX_HIGH				PORTB |= (1 << PB4)
#define TX_LOW				PORTB &=~(1 << PB4)
#define RX_LOW				!(PINB & (1 << PB3))
#define UART_DELAY			1000000UL/UART_BAUD

#define IRLED_HIGH	PORTB |= (1 << PB0)
#define IRLED_LOW	PORTB &=~(1 << PB0)
#define IRTRAN_LOW	!(PINB & (1 << PINB2))
#define CLOCK_DELAY	100

#define LED_HIGH	PORTB |= (1 << PB1)
#define LED_LOW		PORTB &=~(1 << PB1)

//modes
#define MODE_5X_PER_SEC		0
#define MODE_1X_PER_SEC		1
#define MODE_STOP			2
#define MODE_SEND			3

#define MODE_DEFAULT		MODE_1X_PER_SEC;

#endif /* CONFIG_H_ */