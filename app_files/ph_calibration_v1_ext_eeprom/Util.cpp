#include <math.h>
#include <Wire.h>
#include "Util.h"

#define debug 0

#define EEPROM_ADDR 0x50

void reverse(char* str, int len) {
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}
int intToStr(int x, char* str, int d) { 
  int i = 0;
  if (x == 0) {
    str[i++] = (x % 10) + '0'; 
    x = x / 10; 
  } 
  while (x) { 
    str[i++] = (x % 10) + '0'; 
    x = x / 10; 
  } 
  // If number of digits required is more, then 
  // add 0s at the beginning 
  while (i < d) str[i++] = '0'; 
  reverse(str, i); 
  str[i] = '\0';
  return i; 
} 
void ftoa(float n, char* res, int afterpoint) {
  // Extract integer part 
  int ipart = (int)n; 
  // Extract floating part 
  float fpart = n - (float)ipart; 
  // convert integer part to string 
  int i = intToStr(ipart, res, 0); 
  // check for display option after point 
  if (afterpoint != 0) { 
    res[i] = '.'; // add dot 
    // Get the value of fraction part upto given no. 
    // of points after dot. The third parameter  
    // is needed to handle cases like 233.007 
    fpart = fpart * pow(10, afterpoint); 
    intToStr((int)fpart, res + i + 1, afterpoint); 
  } 
}

void EEPROM_wr(unsigned int addr, byte data) {
  int rdata = (data);
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr>>8));     // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  if (debug) {
    Serial.print("EEPROM write: addr: ");
    Serial.print(addr);
    Serial.print(" ");
    Serial.println(data);
  }
  delay(5);
}
byte EEPROM_rd(unsigned int addr) {
  byte data = 0xFF;
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr>>8));     // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_ADDR, 1);
  while (!Wire.available()) {Serial.println("wire not available");}
  if (Wire.available()) {
    data = Wire.read();
    if (debug) {
      Serial.print("Wire.read()");
      Serial.println(data);
    }
  }
  if (debug) {
    Serial.print("EEPROM read: addr: ");
    Serial.print(addr);
    Serial.print(" ");
    Serial.println(data);
  }
  delay(5);
  return data;
}
void EEPROM_write(int address, float value) {
  byte byteVal[sizeof(float)];
  memcpy(byteVal, &value, sizeof(float));
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM_wr(address + i, byteVal[i]);
  }
}
float EEPROM_read(int address) {
  byte byteVal[sizeof(float)];
  for (int i = 0; i < sizeof(float); i++) {
    byteVal[i] = EEPROM_rd(address + i);
  }
  float f;
  memcpy(&f, byteVal, sizeof(float));
  return f;
}

int getMedianNum(int bArray[], int iFilterLen) {
 int bTab[iFilterLen];
 for (int idx = 0; idx<iFilterLen; idx++) {
  bTab[idx] = bArray[idx];
 }
 int i, j, bTemp;
 for (j = 0; j < iFilterLen - 1; j++) {
  for (i = 0; i < iFilterLen - j - 1; i++) {
    if (bTab[i] > bTab[i + 1]) {
      bTemp = bTab[i];
      bTab[i] = bTab[i + 1];
      bTab[i + 1] = bTemp;
    }
  }
 }
 if ((iFilterLen & 1) > 0) bTemp = bTab[(iFilterLen - 1) / 2];
 else bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) /2;
 return bTemp;
}
