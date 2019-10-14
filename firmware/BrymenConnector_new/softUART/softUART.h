#ifndef SOFTUART_H_
#define SOFTUART_H_

void uart_init(void);
void uart_sendChar(uint8_t ch);
void uart_sendString(char* s);
void uart_sendString_P(const char* s);
void uart_sendNumber(uint16_t num, uint8_t base);

#endif