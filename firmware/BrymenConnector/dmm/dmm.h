/*
 * dmm.h
 *
 * Header with all the data-processing functions
 * Author : martin@embedblog.eu
 */ 


#ifndef DMM_H_
#define DMM_H_

uint8_t irPulse(void);
int8_t getData(void);
int8_t decodeDigit(uint8_t source);
void processData(void);



#endif /* DMM_H_ */