#include <Wire.h>

#define EEPROM_ADDR 0x50

void EEPROM_write(unsigned int addr, byte data) {
  int rdata = data;
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr>>8));     // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  Serial.print("EEPROM write: addr: ");
  Serial.print(addr);
  Serial.print(" ");
  Serial.println(data);
  delay(5);
}

byte EEPROM_read(unsigned int addr) {
  byte data = 0xFF;
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((int)(addr>>8));     // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_ADDR, 1);
  while (!Wire.available()) {Serial.print(".");}
  if (Wire.available()) data = Wire.read();
  Serial.print("EEPROM read: addr: ");
  Serial.print(addr);
  Serial.print(" ");
  Serial.println(data);
  delay(5);
  return data;
}

byte test;
byte lean_max;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  byte flags = EEPROM_read(0);
  byte max_cc = EEPROM_read(1);

  byte lean_min = word(EEPROM_read(3), EEPROM_read(2));

  EEPROM_write(5, 130);
  lean_max = word(EEPROM_read(5), EEPROM_read(4));

  delay(1000);

//  for(intj=0;j<3;j)
}

void loop() {
  // put your main code here, to run repeatedly:
  EEPROM_write(0, 123.456);
  test = EEPROM_read(5);
  Serial.print("this is test: "); Serial.println(test);
  Serial.print("this is lean_max: "); Serial.println(lean_max);
}
