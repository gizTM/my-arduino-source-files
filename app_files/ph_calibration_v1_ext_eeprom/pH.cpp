#include "pH.h"
#include "Util.h"
#include <Arduino.h>

pH::pH() : graph() {
  _phvalue                = 0.0;
  _kvalue                 = 1.0;
  _kvalueLow              = 1.0;
  _kvalueHigh             = 1.0;
  _cmdReceivedBufferIndex = 0;
  _voltage                = 0.0;
  _temperature            = 25.0;
  ox = 0; oy = 0;
  title = graphpHTitle;
  minRange = minpH;
  maxRange = maxpH;
  minSafe = minSafepH;
  maxSafe = maxSafepH;
  step = steppH;
  displayGraph = true;
  labelX = graphMainpHLabelX; 
  labelY = graphMainpHLabelY;
  labelFontWidth = graphLabelMainFontWidth; 
  labelFontHeight = graphLabelMainFontHeight; 
  labelColor = graphMainLabelpHColor;
  valX = graphMainpHValueX; 
  valY = graphMainpHValueY; 
  valFontWidth = graphLabelMainFontWidth; 
  valFontHeight = graphLabelMainFontHeight; 
  valColor = graphMainValuepHColor;
  minValX = graphMinpHValueX; 
  minValY = graphMinpHValueY; 
  minLabelX = graphMinpHLabelX; 
  minLabelY = graphMinpHLabelY; 
  minColor = graphMinpHColor;
  maxValX = graphMaxpHValueX; 
  maxValY = graphMaxpHValueY; 
  maxLabelX = graphMaxpHLabelX; 
  maxLabelY = graphMaxpHLabelY; 
  maxColor = graphMaxpHColor;
  labelMinMaxFontWidth = graphLabelMinMaxFontWidth; 
  labelMinMaxFontHeight = graphLabelMinMaxFontHeight;
  graphLowerLeftX = pHGraphLowerLeftX;
  graphLowerLeftY = pHGraphLowerLeftY;
  graphLineColor = graphLinepHColor;
  graphSafeColor = graphSafepHColor;
}
pH::~pH() { delete &graph; }

bool pH::isQueueFull() { return graph.isQueueFull(); }
bool pH::isQueueEmpty() { return graph.isQueueEmpty(); }
void pH::pushData(double item) { graph.pushToQueue(item); }
void pH::popData() { graph.popQueue(); }
void pH::draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.drawCurrentValue(d, val, minVal, maxVal,
    title,
    labelX, labelY, labelFontWidth, labelFontHeight, labelColor,
    valX, valY,
    valFontWidth, valFontHeight, valColor,
    minValX, minValY, minLabelX, minLabelY, minColor,
    maxValX, maxValY, maxLabelX, maxLabelY, maxColor,
    labelMinMaxFontWidth, labelMinMaxFontHeight);
  graph.plotGraph(d, x, val, ox, oy,
    minRange, maxRange, minSafe, maxSafe, step,
    graphLowerLeftX, graphLowerLeftY,
    graphLineColor, graphSafeColor,
    displayGraph);
}
void pH::drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.drawCurrentValue(d, val, minVal, maxVal,
    title,
    labelX, labelY, labelFontWidth, labelFontHeight, labelColor,
    valX, valY,
    valFontWidth, valFontHeight, valColor,
    minValX, minValY, minLabelX, minLabelY, minColor,
    maxValX, maxValY, maxLabelX, maxLabelY, maxColor,
    labelMinMaxFontWidth, labelMinMaxFontHeight);
}
void pH::drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.plotGraph(d, x, val, ox, oy, 
    minRange, maxRange, minSafe, maxSafe, step,
    graphLowerLeftX, graphLowerLeftY,
    graphLineColor, graphSafeColor,
    displayGraph);
}
void pH::begin(){
  _slopeValue = EEPROM_read(SlopeValueAddress);
  _interceptValue = EEPROM_read(InterceptValueAddress);
  if(EEPROM_rd(SlopeValueAddress)==0xFF && EEPROM_rd(SlopeValueAddress+1)==0xFF && EEPROM_rd(SlopeValueAddress+2)==0xFF && EEPROM_rd(SlopeValueAddress+3)==0xFF) {
    _slopeValue = 3.5; // If the EEPROM is new, the recommendatory slope is 3.5. 
    EEPROM_write(SlopeValueAddress, _slopeValue);
  }
  if(EEPROM_rd(InterceptValueAddress)==0xFF && EEPROM_rd(InterceptValueAddress+1)==0xFF && EEPROM_rd(InterceptValueAddress+2)==0xFF && EEPROM_rd(InterceptValueAddress+3)==0xFF) {
    _interceptValue = 0; // If the EEPROM is new, the recommendatory intercept is 0.
    EEPROM_write(InterceptValueAddress, _interceptValue);
  }
}
float pH::readValue(float voltage, float temperature) {
  float value = 0, valueTemp = 0;
  _rawPH = (voltage/1000.0*_slopeValue)+_interceptValue;
  value = _rawPH;
  return value;
}
void pH::calibration(float voltage, float temperature,char* cmd) {   
  _voltage = voltage;
  _temperature = temperature;
  strupr(cmd);
  phCalibration(cmdParse(cmd));                     //if received Serial CMD from the serial monitor, enter into the calibration mode
  _slopeValue = EEPROM_read(SlopeValueAddress); // After calibration, the new slope and intercept should be read ,to update current value.
  _interceptValue = EEPROM_read(InterceptValueAddress);
}
void pH::calibration(float voltage, float temperature) {   
  _voltage = voltage;
  _temperature = temperature;
  if(cmdSerialDataAvailable() > 0){
    phCalibration(cmdParse());  // if received Serial CMD from the serial monitor, enter into the calibration mode
  }
  _slopeValue = EEPROM_read(SlopeValueAddress); // After calibration, the new slope and intercept should be read ,to update current value.
  _interceptValue = EEPROM_read(InterceptValueAddress);
}
boolean pH::cmdSerialDataAvailable() {
  char cmdReceivedChar;
  static unsigned long cmdReceivedTimeOut = millis();
  while (Serial.available()>0) {
    if(millis() - cmdReceivedTimeOut > 500U){
      _cmdReceivedBufferIndex = 0;
      memset(_cmdReceivedBuffer,0,(CmdReceivedBufferLength));
    }
    cmdReceivedTimeOut = millis();
    cmdReceivedChar = Serial.read();
    if(cmdReceivedChar == '\n' || _cmdReceivedBufferIndex==CmdReceivedBufferLength-1){
      _cmdReceivedBufferIndex = 0;
      strupr(_cmdReceivedBuffer);
      return true;
    }else{
      _cmdReceivedBuffer[_cmdReceivedBufferIndex] = cmdReceivedChar;
      _cmdReceivedBufferIndex++;
    }
  }
  return false;
}
byte pH::cmdParse(const char* cmd) {
  byte modeIndex = 0;
  if(strstr(cmd, "CALIBRATION") != NULL) 
    modeIndex = 1;
  else if(strstr(cmd, "EXIT") != NULL) 
    modeIndex = 4;
  else if(strstr(cmd, "ACID:") != NULL) 
    modeIndex = 2;
  else if(strstr(cmd, "ALKALI:") != NULL) 
    modeIndex = 3;
  else if (strstr(cmd, "EEPROM") != NULL) 
    modeIndex = 5;
  return modeIndex;
}
byte pH::cmdParse() {
  byte modeIndex = 0;
  if(strstr(_cmdReceivedBuffer, "CALIBRATION") != NULL) 
    modeIndex = 1;
  else if(strstr(_cmdReceivedBuffer, "EXIT") != NULL) 
    modeIndex = 4;
  else if(strstr(_cmdReceivedBuffer, "ACID:") != NULL) 
    modeIndex = 2;
  else if(strstr(_cmdReceivedBuffer, "ALKALI:") != NULL) 
    modeIndex = 3;
  else if (strstr(_cmdReceivedBuffer, "EEPROM") != NULL) 
    modeIndex = 5;
  return modeIndex;
}
void pH::phCalibration(byte mode) {
  // static boolean phCalibrationFinish  = 0;
  // static float compECsolution;
  // float KValueTemp;
  char *receivedBufferPtr;
  static byte acidCalibrationFinish = 0, alkaliCalibrationFinish= 0;
  static boolean enterCalibrationFlag = 0;
  static float acidValue,alkaliValue;
  static float acidVoltage,alkaliVoltage;
  float acidValueTemp,alkaliValueTemp,newSlopeValue,newInterceptValue;
  switch(mode){
    case 0:
      if(enterCalibrationFlag){
        Serial.println(F(">>>Command Error<<<"));
      }
      break;
    case 1:
      receivedBufferPtr=strstr(_cmdReceivedBuffer, "CALIBRATION");
      enterCalibrationFlag = 1;
      // phCalibrationFinish  = 0;
      acidCalibrationFinish = 0;
      alkaliCalibrationFinish = 0;
      Serial.println();
      Serial.println(F(">>>Enter pH Calibration Mode<<<"));
      // Serial.println(F(">>>Please put the probe into the 1413us/cm or 12.88ms/cm buffer solution<<<"));
      Serial.println();
      break;
    case 2:
      if(enterCalibrationFlag){
        receivedBufferPtr = strstr(_cmdReceivedBuffer, "ACID:");
        receivedBufferPtr += strlen("ACID:");
        Serial.println();
        acidValueTemp = strtod(receivedBufferPtr, NULL);
        if((acidValueTemp > 3) && (acidValueTemp < 5)) {//typical ph value of acid standand buffer solution should be 4.00
          acidValue = acidValueTemp;
          acidVoltage = _voltage/1000.0; // mV ->V
          acidCalibrationFinish = 1;
          Serial.println(F(">>>Acid Calibration Successful"));
        } else {
          acidCalibrationFinish = 0;
          Serial.println(F(">>>Acid Value Error"));
        }
      }
      break;
    case 3:
      if(enterCalibrationFlag) {
        receivedBufferPtr=strstr(_cmdReceivedBuffer, "ALKALI:");
        receivedBufferPtr+=strlen("ALKALI:");
        Serial.println();
        alkaliValueTemp = strtod(receivedBufferPtr,NULL);
        if((alkaliValueTemp > 8) && (alkaliValueTemp < 11)) {//typical ph value of alkali standand buffer solution should be 9.18or 10.01
          alkaliValue = alkaliValueTemp;
          alkaliVoltage = _voltage / 1000.0;
          alkaliCalibrationFinish = 1;
          Serial.println(F(">>>Alkali Calibration Successful"));
        } else { 
          alkaliCalibrationFinish = 0;
          Serial.println(F(">>>Alkali Value Error"));
        }
      }
      break;
    case 4:
      if(enterCalibrationFlag){
        Serial.println();
        if(acidCalibrationFinish && alkaliCalibrationFinish){   
          newSlopeValue = (acidValue-alkaliValue)/(acidVoltage-alkaliVoltage);
          EEPROM_write(SlopeValueAddress, newSlopeValue);
          newInterceptValue = acidValue - (_slopeValue*acidVoltage);
          EEPROM_write(InterceptValueAddress, newInterceptValue);
          Serial.print(F(">>>Calibration Successful"));
        }else{
          Serial.print(F(">>>Calibration Failed"));
        }
        Serial.println(F(",Exit EC Calibration Mode<<<"));
        Serial.println();
        acidCalibrationFinish  = 0;
        alkaliCalibrationFinish  = 0;
        enterCalibrationFlag = 0;
      }
      break;
    case 5:
      Serial.print("EEPROM read: pH >> ");
      Serial.println(EEPROM_read(SlopeValueAddress));
      Serial.println(EEPROM_read(InterceptValueAddress));
      break;
  }
}
