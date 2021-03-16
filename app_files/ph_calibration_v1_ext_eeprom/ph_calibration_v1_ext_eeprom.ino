// For ph calibration stored on external EEPROM
/***************************************************
 This example uses software solution to calibration the ph meter,
not the potentiometer. So it is more easy to use and calibrate.
 This is for SEN0161 and SEN0169.

 Created 2016-8-11
 By youyou from DFrobot <youyou.yu@dfrobot.com>

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Troubleshooting***************
 1.Connection and Diagram can be found here http://www.dfrobot.co
m/wiki/index.php/PH_meter%28SKU:_SEN0161%29
 2.This code is tested on Arduino Uno.
 ****************************************************/

#include <Wire.h>
#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include <SD.h>
#include <math.h> 
#include <stdio.h> 

#include "./fonts/FreeMono9pt7b.h"
#include "pH.h"
#include "EC.h"
#include "DO.h"
#include "Temperature.h"
#include "Util.h"

#define TFT_RST 6//A4
#define TFT_RS  7//A3
#define TFT_CS  3//4//A5  // SS
#define TFT_SDI 8//A2  // MOSI
#define TFT_CLK 9//A1  // SCK
#define TFT_LED 0   // 0 if wired to +5V directly

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

#define debug 1

#define EEPROM_ADDR 0x50

#define ReceivedBufferLength 20
char receivedBuffer[ReceivedBufferLength+1]; // store the serialcommand
byte receivedBufferIndex = 0; 

#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; //store the sample voltage
int analogBufferIndex = 0;

#define SlopeValueAddress 0 // (slope of the ph probe)store atthe beginning of the EEPROM. The slope is a float number,occupies4 bytes.
#define InterceptValueAddress (SlopeValueAddress+4)
float slopeValue, interceptValue, averageVoltage;
boolean enterCalibrationFlag = 0;

#define SensorPin A1
#define VREF 5000 //for arduino uno, the ADC reference is the power(AVCC), that is 5000mV

double pHDouble, pHMaxDouble = -1, pHMinDouble = -1;
double ECDouble, ECMaxDouble = -1, ECMinDouble = -1;
double DODouble, DOMaxDouble = -1, DOMinDouble = -1;
double tempDouble, tempMaxDouble = -1, tempMinDouble = -1;

double pHX = 0, ECX = 0, DOX = 0, tempX = 0;

#define SD_CS 4
File file;

pH pHObj;
EC ECObj;
DO DOObj;
Temperature tempObj;

bool enableSD = true;

// Use hardware SPI (faster - on Uno: 13-SCK, 12-MISO, 11-MOSI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

void setup() {
 pinMode(SD_CS, OUTPUT);
 digitalWrite(SD_CS, HIGH);
 Wire.begin();
 tft.begin();
 tft.setOrientation(1);
 Serial.begin(115200);
 delay(1);
 randomSeed(analogRead(0));
 readCharacteristicValues(); //read the slope and intercept of the ph probe
//  EEPROM_write(SlopeValueAddress, 0x05);
//  EEPROM_write(InterceptValueAddress, 0x03);
  if (enableSD) {
    Serial.print("Initializing SD card...");
    tft.setFont(Terminal6x8);
    tft.drawText(10, 50, "Initializing SD card...", COLOR_WHITE);
    if (!SD.begin(SD_CS)) {
      Serial.println("Initialization failed!");
      tft.drawCircle(20, 20, 10, COLOR_RED);
      tft.setFont(Terminal6x8);
      tft.drawText(10, 60, "Initialization failed!", COLOR_WHITE);
      // while (1);
    } else {
      Serial.println("Initialization done.");
      tft.drawCircle(20, 20, 10, COLOR_GREEN);
      tft.setFont(Terminal6x8);
      tft.drawText(10, 60, "Initialization done.", COLOR_WHITE);
      if (SD.exists("test.txt")) {
        Serial.println("test.txt exists.");
        tft.setFont(Terminal6x8);
        tft.drawText(10, 70, "test.txt exists.", COLOR_WHITE);
      }
      else {
        Serial.println("test.txt doesn't exist.");
        tft.setFont(Terminal6x8);
        tft.drawText(10, 70, "test.txt doesn't exist.", COLOR_WHITE);
      }
    }
  }
}

void loop() {
  handlepH();
}

void handlepH() {
  if(serialDataAvailable() > 0) {
    byte modeIndex = uartParse();
    phCalibration(modeIndex); // If the correct calibration command is received, the calibration function should be called.
    slopeValue = EEPROM_read(SlopeValueAddress); // After calibration, the new slope and intercept should be read ,to update current value.
    interceptValue = EEPROM_read(InterceptValueAddress);
 }
 
 static unsigned long sampleTimepoint = millis();
 if(millis() - sampleTimepoint > 40U) {
  sampleTimepoint = millis();
  analogBuffer[analogBufferIndex] = analogRead(SensorPin)/1024.0*VREF; //read the voltaage and store into the buffer,every 40ms
  analogBufferIndex++;
  if(analogBufferIndex == SCOUNT) analogBufferIndex = 0;
  averageVoltage = getMedianNum(analogBuffer,SCOUNT); // readthe stable value by the median filtering algorithm
 }

 static unsigned long printTimepoint = millis();
 if(millis()-printTimepoint>3000U) { // display every 5000ms = 5s
  printTimepoint = millis();
  if(enterCalibrationFlag) {// in calibration mode,print the voltage to user, to watch the stability of voltage
    Serial.print("Voltage:");
    Serial.print(averageVoltage);
    Serial.println("mV");
  } else {
    // Serial.println("averateVoltage |  slopeValue | interceptValue");
    // Serial.print(averageVoltage);
    // Serial.print(" | ");
    // Serial.print(slopeValue);
    // Serial.print(" | ");
    // Serial.println(interceptValue);
    // Serial.print("pH:"); // in normal mode, print the ph value to user
    // Serial.println(averageVoltage/1000.0*slopeValue+interceptValue);
    // tft.drawText(5, 30, "pH", COLOR_DARKVIOLET);
    // pHDouble = averageVoltage/1000.0*slopeValue+interceptValue;
    pHDouble = random(15);
    pHMaxDouble = max(pHMaxDouble==-1?pHDouble:pHMaxDouble, pHDouble);
    pHMinDouble = min(pHMinDouble==-1?pHDouble:pHMinDouble, pHDouble);
    if (pHObj.isQueueFull()) pHObj.popData();
    pHObj.pushData(pHDouble);
    pHObj.draw(tft, pHX, pHDouble, pHMinDouble, pHMaxDouble);
  }
 }
 static unsigned long printECTimepoint = millis();
 if(millis()-printECTimepoint>5000U) { // display every 5000ms = 5s
  printECTimepoint = millis();
  ECDouble = random(15);
  ECMaxDouble = max(ECMaxDouble==-1?ECDouble:ECMaxDouble, ECDouble);
  ECMinDouble = min(ECMinDouble==-1?ECDouble:ECMinDouble, ECDouble);
  if (ECObj.isQueueFull()) ECObj.popData();
  ECObj.pushData(ECDouble);
  ECObj.draw(tft, ECX, ECDouble, ECMinDouble, ECMaxDouble);
 }
 static unsigned long printDOTimepoint = millis();
 if(millis()-printDOTimepoint>3000U) { // display every 4000ms = 5s
  printDOTimepoint = millis();
  DODouble = random(15);
  DOMaxDouble = max(DOMaxDouble==-1?DODouble:DOMaxDouble, DODouble);
  DOMinDouble = min(DOMinDouble==-1?DODouble:DOMinDouble, DODouble);
  if (DOObj.isQueueFull()) DOObj.popData();
  DOObj.pushData(DODouble);
  DOObj.draw(tft, DOX, DODouble, DOMinDouble, DOMaxDouble);
 }
 static unsigned long printTemperatureTimepoint = millis();
 if(millis()-printTemperatureTimepoint>4000U) { // display every 4000ms = 5s
  printTemperatureTimepoint = millis();
  tempDouble = random(10, 45);
  tempMaxDouble = max(tempMaxDouble==-1?tempDouble:tempMaxDouble, tempDouble);
  tempMinDouble = min(tempMinDouble==-1?tempDouble:tempMinDouble, tempDouble);
  if (tempObj.isQueueFull()) tempObj.popData();
  tempObj.pushData(tempDouble);
  tempObj.draw(tft, tempX, tempDouble, tempMinDouble, tempMaxDouble);
 }
}

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

void printUint16(uint16_t num) {
  char hexCar[4];

  sprintf(hexCar, "%04X", num);
  Serial.println(hexCar);
}

void printDouble(double value, String label) {
  Serial.print(label);
  char tmpStr[10];
  ftoa(value, tmpStr, 2);
  Serial.println(tmpStr);
}

String Format(double val, int dec, int dig ) {
  int addpad = 0;
  char sbuf[20];
  String condata = (dtostrf(val, dec, dig, sbuf));


  int slen = condata.length();
  for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
    condata = " " + condata;
  }
  return (condata);
}
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
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
  while (!Wire.available()) {Serial.print(".");}
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

boolean serialDataAvailable(void) { 
 char receivedChar;
 static unsigned long receivedTimeOut = millis();
 while (Serial.available()>0) {
  if (millis() - receivedTimeOut > 1000U) {
    receivedBufferIndex = 0;
    memset(receivedBuffer,0,(ReceivedBufferLength+1));
  }
  receivedTimeOut = millis();
  receivedChar = Serial.read();
  if (receivedChar == '\n' || receivedBufferIndex==ReceivedBufferLength) {
    receivedBufferIndex = 0;
    strupr(receivedBuffer);
    return true;
  }
  else {
    receivedBuffer[receivedBufferIndex] = receivedChar;
    receivedBufferIndex++;
  }
 }
 return false;
}

byte uartParse() {
 byte modeIndex = 0;
 if(strstr(receivedBuffer, "CALIBRATION") != NULL) modeIndex = 1;
 else if(strstr(receivedBuffer, "EXIT") != NULL) modeIndex = 4;
 else if(strstr(receivedBuffer, "ACID:") != NULL) modeIndex = 2;
 else if(strstr(receivedBuffer, "ALKALI:") != NULL) modeIndex = 3;
 else if (strstr(receivedBuffer, "EEPROM") != NULL) modeIndex = 5;
 return modeIndex;
}
void phCalibration(byte mode)
{
 char *receivedBufferPtr;
 static byte acidCalibrationFinish = 0, alkaliCalibrationFinish= 0;
 static float acidValue,alkaliValue;
 static float acidVoltage,alkaliVoltage;
 float acidValueTemp,alkaliValueTemp,newSlopeValue,newInterceptValue;
 switch(mode) {
  case 0:
    if(enterCalibrationFlag)
    Serial.println(F("Command Error"));
  break;

  case 1:
    receivedBufferPtr=strstr(receivedBuffer, "CALIBRATION");
    enterCalibrationFlag = 1;
    acidCalibrationFinish = 0;
    alkaliCalibrationFinish = 0;
    Serial.println(F("Enter Calibration Mode"));
  break;

  case 2:
    if(enterCalibrationFlag) { 
      receivedBufferPtr = strstr(receivedBuffer, "ACID:");
      receivedBufferPtr += strlen("ACID:");
      acidValueTemp = strtod(receivedBufferPtr, NULL);
      if((acidValueTemp > 3) && (acidValueTemp < 5)) {//typical ph value of acid standand buffer solution should be 4.00
        acidValue = acidValueTemp;
        acidVoltage = averageVoltage/1000.0; // mV ->V
        acidCalibrationFinish = 1;
        Serial.println(F("Acid Calibration Successful"));
      } else {
        acidCalibrationFinish = 0;
        Serial.println(F("Acid Value Error"));
      }
    }
    break;

  case 3:
    if(enterCalibrationFlag) {
      receivedBufferPtr=strstr(receivedBuffer, "ALKALI:");
      receivedBufferPtr+=strlen("ALKALI:");
      alkaliValueTemp = strtod(receivedBufferPtr,NULL);
    if((alkaliValueTemp > 8) && (alkaliValueTemp < 11)) {//typical ph value of alkali standand buffer solution should be 9.18or 10.01
      alkaliValue = alkaliValueTemp;
      alkaliVoltage = averageVoltage / 1000.0;
      alkaliCalibrationFinish = 1;
      Serial.println(F("Alkali Calibration Successful"));
    } else { 
      alkaliCalibrationFinish = 0;
      Serial.println(F("Alkali Value Error"));
    }
  }
  break;
  case 4:
    if(enterCalibrationFlag) {
      if(acidCalibrationFinish && alkaliCalibrationFinish) {
        newSlopeValue = (acidValue-alkaliValue)/(acidVoltage- alkaliVoltage);
        Serial.print("acidValue: ");
        Serial.println(acidValue);
        Serial.print("alkaliValue: ");
        Serial.println(alkaliValue);
        Serial.print("acidVoltage: ");
        Serial.println(acidVoltage);
        Serial.print("alkaliVoltage: ");
        Serial.println(alkaliVoltage);
        Serial.print("newSlopeValue");
        Serial.println(newSlopeValue);
        Serial.print("newInterceptValue");
        Serial.println(newInterceptValue);
        EEPROM_write(SlopeValueAddress, newSlopeValue);
        newInterceptValue = acidValue - (slopeValue*acidVoltage);
        EEPROM_write(InterceptValueAddress, newInterceptValue);
        Serial.print(F("Calibration Successful"));
      }
      else Serial.print(F("Calibration Failed"));
      Serial.println(F(",Exit Calibration Mode"));
      acidCalibrationFinish = 0;
      alkaliCalibrationFinish = 0;
      enterCalibrationFlag = 0;
    }
    break;
    case 5:
    Serial.print("EEPROM read: 0,4 >> ");
    Serial.println(EEPROM_read(0));
    Serial.println(EEPROM_read(4));
    break;
  }
}

int getMedianNum(int bArray[], int iFilterLen) {
 int bTab[iFilterLen];
 for (byte i = 0; i<iFilterLen; i++) {
  bTab[i] = bArray[i];
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

void readCharacteristicValues() {
 slopeValue = EEPROM_read(SlopeValueAddress);
 interceptValue = EEPROM_read(InterceptValueAddress);
 if(EEPROM_rd(SlopeValueAddress)==0xFF && EEPROM_rd(SlopeValueAddress+1)==0xFF && EEPROM_rd(SlopeValueAddress+2)==0xFF && EEPROM_rd(SlopeValueAddress+3)==0xFF) {
  slopeValue = 3.5; // If the EEPROM is new, the recommendatory slope is 3.5. 
  EEPROM_write(SlopeValueAddress, slopeValue);
 }
 if(EEPROM_rd(InterceptValueAddress)==0xFF && EEPROM_rd(InterceptValueAddress+1)==0xFF && EEPROM_rd(InterceptValueAddress+2)==0xFF && EEPROM_rd(InterceptValueAddress+3)==0xFF) {
  interceptValue = 0; // If the EEPROM is new, the recommendatory intercept is 0.
  EEPROM_write(InterceptValueAddress, interceptValue);
 }
} 
/*
  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn
  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  gcolor = graph line colors
  acolor = axi ine colors
  pcolor = color of your plotted data
  tcolor = text color
  bcolor = background color
  &redraw = flag to redraw graph on fist call only
*/

