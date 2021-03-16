#ifndef temp_h
#define temp_h

#include "GraphVar.h"
#include "SensorGraph.h"

#define graphTemperatureTitle "Temp"
#define minTemperature 10
#define maxTemperature 50
#define minSafeTemperature 15
#define maxSafeTemperature 40
#define stepTemperature 5
#define graphMainLabelTemperatureColor COLOR_DARKRED
#define graphMainValueTemperatureColor COLOR_LIGHTGREY
#define graphSafeTemperatureColor COLOR_WHITE
#define graphLineTemperatureColor COLOR_RED
#define TemperatureGraphLowerLeftX 120
#define TemperatureGraphLowerLeftY (initialGraphY + (intergraphOffset * 3))

#define graphMainTemperatureLabelX 5
#define graphMainTemperatureLabelY (initialGraphLabelY + (intergraphOffset * 3))
#define graphMainTemperatureValueX (graphMainTemperatureLabelX + 25)
#define graphMainTemperatureValueY (graphMainTemperatureLabelY)

#define graphMaxTemperatureColor COLOR_DARKRED
#define graphMaxTemperatureLabelX (graphMainTemperatureValueX)
#define graphMaxTemperatureLabelY (graphMainTemperatureValueY - 10)
#define graphMaxTemperatureValueX (graphMaxTemperatureLabelX + 25)
#define graphMaxTemperatureValueY (graphMaxTemperatureLabelY)

#define graphMinTemperatureColor COLOR_TOMATO
#define graphMinTemperatureLabelX (graphMainTemperatureValueX)
#define graphMinTemperatureLabelY (graphMainTemperatureValueY + graphMinLabelYOffset)
#define graphMinTemperatureValueX (graphMinTemperatureLabelX + 25)
#define graphMinTemperatureValueY (graphMinTemperatureLabelY)

class Temperature {
  private:
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
    Temperature();
    ~Temperature();
    bool isQueueFull();
    bool isQueueEmpty();
    void pushData(double item);
    void popData();
    void draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
};

#endif
