#ifndef util_h
#define util_h

#include <Arduino.h>

void reverse(char* str, int len);
int intToStr(int n, char* res, int afterpoint);
void ftoa(float n, char* res, int afterpoint);
void EEPROM_wr(unsigned int addr, byte data);
byte EEPROM_rd(unsigned int addr);
void EEPROM_write(int addr, float data);
float EEPROM_read(int addr);
int getMedianNum(int bArray[], int iFilterLen);

#endif
