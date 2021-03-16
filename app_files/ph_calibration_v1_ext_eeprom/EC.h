#ifndef ec_h
#define ec_h

#include "GraphVar.h"
#include "SensorGraph.h"

#define graphECTitle "EC"
#define minEC 0
#define maxEC 14
#define minSafeEC 4
#define maxSafeEC 10
#define stepEC 2
#define graphMainLabelECColor COLOR_BROWN
#define graphMainValueECColor COLOR_LIGHTGREY
#define graphSafeECColor COLOR_WHITE
#define graphLineECColor COLOR_ORANGE
#define ECGraphLowerLeftX 120
#define ECGraphLowerLeftY (initialGraphY + intergraphOffset)

#define graphMainECLabelX 5
#define graphMainECLabelY (initialGraphLabelY + intergraphOffset)
#define graphMainECValueX (graphMainECLabelX + 25)
#define graphMainECValueY (graphMainECLabelY)

#define graphMaxECColor COLOR_BROWN
#define graphMaxECLabelX (graphMainECValueX)
#define graphMaxECLabelY (graphMainECValueY - 10)
#define graphMaxECValueX (graphMaxECLabelX + 25)
#define graphMaxECValueY (graphMaxECLabelY)

#define graphMinECColor COLOR_YELLOW
#define graphMinECLabelX (graphMainECValueX)
#define graphMinECLabelY (graphMainECValueY + graphMinLabelYOffset)
#define graphMinECValueX (graphMinECLabelX + 25)
#define graphMinECValueY (graphMinECLabelY)

class EC {
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
    EC();
    ~EC();
    bool isQueueFull();
    bool isQueueEmpty();
    void pushData(double item);
    void popData();
    void draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
};

#endif
