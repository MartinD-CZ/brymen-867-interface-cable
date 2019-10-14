#ifndef DECODER_H_
#define DECODER_H_

uint8_t getByte(void);
uint8_t getData(void);
void processData(void);
int8_t decodeDigit(uint8_t source);

#endif /* DECODER_H_ */