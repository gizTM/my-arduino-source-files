#include "EC.h"
#include "Util.h"

EC::EC() : graph() {
  _ecvalue                = 0.0;
  _kvalue                 = 1.0;
  _kvalueLow              = 1.0;
  _kvalueHigh             = 1.0;
  _cmdReceivedBufferIndex = 0;
  _voltage                = 0.0;
  _temperature            = 25.0;
  ox = 0; oy = 0;
  title = graphECTitle;
  minRange = minEC;
  maxRange = maxEC;
  minSafe = minSafeEC;
  maxSafe = maxSafeEC;
  step = stepEC;
  displayGraph = true;
  labelX = graphMainECLabelX; 
  labelY = graphMainECLabelY;
  labelFontWidth = graphLabelMainFontWidth; 
  labelFontHeight = graphLabelMainFontHeight; 
  labelColor = graphMainLabelECColor;
  valX = graphMainECValueX; 
  valY = graphMainECValueY; 
  valFontWidth = graphLabelMainFontWidth; 
  valFontHeight = graphLabelMainFontHeight; 
  valColor = graphMainValueECColor;
  minValX = graphMinECValueX; 
  minValY = graphMinECValueY; 
  minLabelX = graphMinECLabelX; 
  minLabelY = graphMinECLabelY; 
  minColor = graphMinECColor;
  maxValX = graphMaxECValueX; 
  maxValY = graphMaxECValueY; 
  maxLabelX = graphMaxECLabelX; 
  maxLabelY = graphMaxECLabelY; 
  maxColor = graphMaxECColor;
  labelMinMaxFontWidth = graphLabelMinMaxFontWidth; 
  labelMinMaxFontHeight = graphLabelMinMaxFontHeight;
  graphLowerLeftX = ECGraphLowerLeftX;
  graphLowerLeftY = ECGraphLowerLeftY;
  graphLineColor = graphLineECColor;
  graphSafeColor = graphSafeECColor;
}
EC::~EC() { delete &graph; }

bool EC::isQueueFull() { return graph.isQueueFull(); }
bool EC::isQueueEmpty() { return graph.isQueueEmpty(); }
void EC::pushData(double item) { graph.pushToQueue(item); }
void EC::popData() { graph.popQueue(); }
void EC::draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
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
void EC::drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.drawCurrentValue(d, val, minVal, maxVal,
    title,
    labelX, labelY, labelFontWidth, labelFontHeight, labelColor,
    valX, valY,
    valFontWidth, valFontHeight, valColor,
    minValX, minValY, minLabelX, minLabelY, minColor,
    maxValX, maxValY, maxLabelX, maxLabelY, maxColor,
    labelMinMaxFontWidth, labelMinMaxFontHeight);
}
void EC::drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.plotGraph(d, x, val, ox, oy,
    minRange, maxRange, minSafe, maxSafe, step,
    graphLowerLeftX, graphLowerLeftY,
    graphLineColor, graphSafeColor,
    displayGraph);
}
void EC::begin(){
  _kvalueLow = EEPROM_read(KVALUEADDR);        //read the calibrated K value from EEPROM
  if(EEPROM_rd(KVALUEADDR)==0xFF && EEPROM_rd(KVALUEADDR+1)==0xFF && EEPROM_rd(KVALUEADDR+2)==0xFF && EEPROM_rd(KVALUEADDR+3)==0xFF){
    _kvalueLow = 1.0;                       // For new EEPROM, write default value( K = 1.0) to EEPROM
    EEPROM_write(KVALUEADDR, _kvalueLow);
  }
  _kvalueHigh = EEPROM_read(KVALUEADDR+4);     //read the calibrated K value from EEPRM
  if(EEPROM_rd(KVALUEADDR+4)==0xFF && EEPROM_rd(KVALUEADDR+5)==0xFF && EEPROM_rd(KVALUEADDR+6)==0xFF && EEPROM_rd(KVALUEADDR+7)==0xFF){
    _kvalueHigh = 1.0;                      // For new EEPROM, write default value( K = 1.0) to EEPROM
    EEPROM_write(KVALUEADDR+4, _kvalueHigh);
  }
  _kvalue =  _kvalueLow;                // set default K value: K = kvalueLow
}
float EC::readValue(float voltage, float temperature) {
  float value = 0, valueTemp = 0;
  _rawEC = 1000.0*voltage/RES2/ECREF;
  valueTemp = _rawEC * _kvalue;
  //automatic shift process
  //First Range:(0,2); Second Range:(2,20)
  if(valueTemp > 2.5){
    _kvalue = _kvalueHigh;
  } else if(valueTemp < 2.0){
    _kvalue = _kvalueLow;
  }

  value = _rawEC * _kvalue;             //calculate the EC value after automatic shift
  value = value / (1.0+0.0185*(temperature-25.0));  //temperature compensation
  _ecvalue = value;                           //store the EC value for Serial CMD calibration
  Serial.print("readValue: voltage: ");
  Serial.print(voltage);
  Serial.print(", _rawEC: ");
  Serial.print(_rawEC);
  Serial.print(", valueTemp: ");
  Serial.print(valueTemp);
  Serial.print(", _kvalue: ");
  Serial.print(_kvalue);
  Serial.print(", value: ");
  Serial.println(value);
  return value;
}
void EC::calibration(float voltage, float temperature,char* cmd) {   
  _voltage = voltage;
  _temperature = temperature;
  strupr(cmd);
  ecCalibration(cmdParse(cmd));                     //if received Serial CMD from the serial monitor, enter into the calibration mode
}
void EC::calibration(float voltage, float temperature) {   
  _voltage = voltage;
  _temperature = temperature;
  if(cmdSerialDataAvailable() > 0){
    ecCalibration(cmdParse());  // if received Serial CMD from the serial monitor, enter into the calibration mode
  }
}
boolean EC::cmdSerialDataAvailable() {
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
byte EC::cmdParse(const char* cmd) {
  byte modeIndex = 0;
  if(strstr(cmd, "ENTEREC") != NULL){
    modeIndex = 1;
  } else if(strstr(cmd, "EXITEC") != NULL){
    modeIndex = 3;
  } else if(strstr(cmd, "CALEC")  != NULL){
    modeIndex = 2;
  }
  return modeIndex;
}
byte EC::cmdParse() {
  byte modeIndex = 0;
  if(strstr(_cmdReceivedBuffer, "ENTEREC")     != NULL)
    modeIndex = 1;
  else if(strstr(_cmdReceivedBuffer, "EXITEC") != NULL)
    modeIndex = 3;
  else if(strstr(_cmdReceivedBuffer, "CALEC")  != NULL)
    modeIndex = 2;
  return modeIndex;
}
void EC::ecCalibration(byte mode) {
  char *receivedBufferPtr;
  static boolean ecCalibrationFinish  = 0;
  static boolean enterCalibrationFlag = 0;
  static float compECsolution;
  float KValueTemp;
  switch(mode){
    case 0:
      if(enterCalibrationFlag){
        Serial.println(F(">>>Command Error<<<"));
      }
      break;
    case 1:
      enterCalibrationFlag = 1;
      ecCalibrationFinish  = 0;
      Serial.println();
      Serial.println(F(">>>Enter EC Calibration Mode<<<"));
      Serial.println(F(">>>Please put the probe into the 1413us/cm or 12.88ms/cm buffer solution<<<"));
      Serial.println();
      break;
    case 2:
      if(enterCalibrationFlag){
        Serial.println(_rawEC);
        // if((_rawEC>0.9)&&(_rawEC<1.9)){                         //recognize 1.413us/cm buffer solution
        if((_rawEC>1.9)&&(_rawEC<2.9)){                         //recognize 1.413us/cm buffer solution
          compECsolution = 1.413*(1.0+0.0185*(_temperature-25.0));  //temperature compensation
        // }else if((_rawEC>9)&&(_rawEC<16.8)){                    //recognize 12.88ms/cm buffer solution
        }else if((_rawEC>10)&&(_rawEC<22)){                    //recognize 12.88ms/cm buffer solution
          compECsolution = 12.88*(1.0+0.0185*(_temperature-25.0));  //temperature compensation
        }else{
          Serial.print(F(">>>Buffer Solution Error Try Again<<<   "));
          ecCalibrationFinish = 0;
        }
        KValueTemp = RES2*ECREF*compECsolution/1000.0/_voltage;       //calibrate the k value
        if((KValueTemp>0.5) && (KValueTemp<1.5)){
          Serial.println();
          Serial.print(F(">>>Successful,K:"));
          Serial.print(KValueTemp);
          Serial.println(F(", Send EXITEC to Save and Exit<<<"));
          // if((_rawEC>0.9)&&(_rawEC<1.9)){
          if((_rawEC>1.9)&&(_rawEC<2.9)){ 
            _kvalueLow =  KValueTemp;
          // }else if((_rawEC>9)&&(_rawEC<16.8)){
          }else if((_rawEC>10)&&(_rawEC<22)){  
            _kvalueHigh =  KValueTemp;
          }
          ecCalibrationFinish = 1;
        }
        else{
          Serial.println();
          Serial.println(F(">>>Failed,Try Again<<<"));
          Serial.println();
          ecCalibrationFinish = 0;
        }
      }
      break;
    case 3:
      if(enterCalibrationFlag){
        Serial.println();
        if(ecCalibrationFinish){   
          // if((_rawEC>0.9)&&(_rawEC<1.9)){
          if((_rawEC>1.9)&&(_rawEC<2.9)){
            EEPROM_write(KVALUEADDR, _kvalueLow);
          // }else if((_rawEC>9)&&(_rawEC<16.8)){
          }else if((_rawEC>10)&&(_rawEC<22)){
            EEPROM_write(KVALUEADDR+4, _kvalueHigh);
          }
          Serial.print(F(">>>Calibration Successful"));
        }else{
          Serial.print(F(">>>Calibration Failed"));
        }
        Serial.println(F(",Exit EC Calibration Mode<<<"));
        Serial.println();
        ecCalibrationFinish  = 0;
        enterCalibrationFlag = 0;
      }
      break;
  }
}
