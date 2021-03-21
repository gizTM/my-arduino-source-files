#ifndef ph_h
#define ph_h

#include <Arduino.h>
#include "GraphVar.h"
#include "SensorGraph.h"

#define CmdReceivedBufferLength 20
#define SlopeValueAddress 0 // (slope of the ph probe)store atthe beginning of the EEPROM. The slope is a float number,occupies4 bytes.
#define InterceptValueAddress (SlopeValueAddress+4)

#define graphpHTitle "pH"
#define minpH 0
#define maxpH 14
#define minSafepH 4
#define maxSafepH 10
#define steppH 2
#define graphMainLabelpHColor COLOR_DARKVIOLET
#define graphMainValuepHColor COLOR_LIGHTGREY
#define graphSafepHColor COLOR_WHITE
#define graphLinepHColor COLOR_DARKVIOLET
#define pHGraphLowerLeftX 120
#define pHGraphLowerLeftY initialGraphY

#define graphMainpHLabelX 5
#define graphMainpHLabelY initialGraphLabelY
#define graphMainpHValueX (graphMainpHLabelX + 25)
#define graphMainpHValueY (graphMainpHLabelY)

#define graphMaxpHColor COLOR_VIOLET
#define graphMaxpHLabelX (graphMainpHValueX)
#define graphMaxpHLabelY (graphMainpHValueY - 10)
#define graphMaxpHValueX (graphMaxpHLabelX + 25)
#define graphMaxpHValueY (graphMaxpHLabelY)

#define graphMinpHColor COLOR_RED
#define graphMinpHLabelX (graphMainpHValueX)
#define graphMinpHLabelY (graphMainpHValueY + graphMinLabelYOffset)
#define graphMinpHValueX (graphMinpHLabelX + 25)
#define graphMinpHValueY (graphMinpHLabelY)

class pH {
  private:
    double _phvalue;
    double _slopeValue;
    double _interceptValue;
    double _kvalue;
    double _kvalueLow;
    double _kvalueHigh;
    byte _cmdReceivedBufferIndex;
    double _voltage;
    double _temperature;
    double _rawPH;
    char _cmdReceivedBuffer[CmdReceivedBufferLength+1]; // store the serialcommand
    double averageVoltage;
    boolean enterCalibrationFlag;
    SensorGraph graph;
    String title;
    double minRange; double maxRange;
    double minSafe; double maxSafe;
    double step;
    boolean displayGraph;
    double ox; double oy;
    double val; double minVal; double maxVal;
    double labelX; double labelY;
    double labelFontWidth; double labelFontHeight; unsigned int labelColor;
    double valX; double valY; 
    double valFontWidth; double valFontHeight; unsigned int valColor;
    double minValX; double minValY; double minLabelX; double minLabelY; unsigned int minColor;
    double maxValX; double maxValY; double maxLabelX; double maxLabelY; unsigned int maxColor;
    double labelMinMaxFontWidth; double labelMinMaxFontHeight;
    double graphLowerLeftX; double graphLowerLeftY;
    unsigned int graphLineColor; unsigned int graphSafeColor;
  
  public:
    pH();
    ~pH();
    bool isQueueFull();
    bool isQueueEmpty();
    void pushData(double item);
    void popData();
    void draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void begin();
    float readValue(float voltage, float temperature);
    void calibration(float voltage, float temperature, char* cmd);
    void calibration(float voltage, float temperature);
    boolean cmdSerialDataAvailable();
    byte cmdParse(const char* cmd);
    byte cmdParse();
    void phCalibration(byte mode);
};

#endif
