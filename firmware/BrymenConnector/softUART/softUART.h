/*
 * softUART.h
 *
 * Header for a simple as-small-as-possible software-implemented UART 
 * Author : martin@embedblog.eu
 */ 


#ifndef SOFTUART_H_
#define SOFTUART_H_

class softUART
{
	public:
		void init(void);
		void sendChar(uint8_t ch);
		void sendString(char* s);
};



#endif /* SOFTUART_H_ */