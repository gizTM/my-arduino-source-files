#ifndef do_h
#define do_h

#include "GraphVar.h"
#include "SensorGraph.h"

#define graphDOTitle "DO"
#define minDO 0
#define maxDO 14
#define minSafeDO 4
#define maxSafeDO 8
#define stepDO 4
#define graphMainLabelDOColor COLOR_DARKGREEN
#define graphMainValueDOColor COLOR_LIGHTGREY
#define graphSafeDOColor COLOR_WHITE
#define graphLineDOColor COLOR_GREEN
#define DOGraphLowerLeftX 120
#define DOGraphLowerLeftY (initialGraphY + (intergraphOffset * 2))

#define graphMainDOLabelX 5
#define graphMainDOLabelY (initialGraphLabelY + (intergraphOffset * 2))
#define graphMainDOValueX (graphMainDOLabelX + 25)
#define graphMainDOValueY (graphMainDOLabelY)

#define graphMaxDOColor COLOR_DARKGREEN
#define graphMaxDOLabelX (graphMainDOValueX)
#define graphMaxDOLabelY (graphMainDOValueY - 10)
#define graphMaxDOValueX (graphMaxDOLabelX + 25)
#define graphMaxDOValueY (graphMaxDOLabelY)

#define graphMinDOColor COLOR_YELLOWGREEN
#define graphMinDOLabelX (graphMainDOValueX)
#define graphMinDOLabelY (graphMainDOValueY + graphMinLabelYOffset)
#define graphMinDOValueX (graphMinDOLabelX + 25)
#define graphMinDOValueY (graphMinDOLabelY)

class DO {
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
    DO();
    ~DO();
    bool isQueueFull();
    bool isQueueEmpty();
    void pushData(double item);
    void popData();
    void draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
    void drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal);
};

#endif
