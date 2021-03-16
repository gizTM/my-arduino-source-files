#include "DO.h"

DO::DO() : graph() {
  ox = 0; oy = 0;
  title = graphDOTitle;
  minRange = minDO;
  maxRange = maxDO;
  minSafe = minSafeDO;
  maxSafe = maxSafeDO;
  step = stepDO;
  displayGraph = true;
  labelX = graphMainDOLabelX; 
  labelY = graphMainDOLabelY;
  labelFontWidth = graphLabelMainFontWidth; 
  labelFontHeight = graphLabelMainFontHeight; 
  labelColor = graphMainLabelDOColor;
  valX = graphMainDOValueX; 
  valY = graphMainDOValueY; 
  valFontWidth = graphLabelMainFontWidth; 
  valFontHeight = graphLabelMainFontHeight; 
  valColor = graphMainValueDOColor;
  minValX = graphMinDOValueX; 
  minValY = graphMinDOValueY; 
  minLabelX = graphMinDOLabelX; 
  minLabelY = graphMinDOLabelY; 
  minColor = graphMinDOColor;
  maxValX = graphMaxDOValueX; 
  maxValY = graphMaxDOValueY; 
  maxLabelX = graphMaxDOLabelX; 
  maxLabelY = graphMaxDOLabelY; 
  maxColor = graphMaxDOColor;
  labelMinMaxFontWidth = graphLabelMinMaxFontWidth; 
  labelMinMaxFontHeight = graphLabelMinMaxFontHeight;
  graphLowerLeftX = DOGraphLowerLeftX;
  graphLowerLeftY = DOGraphLowerLeftY;
  graphLineColor = graphLineDOColor;
  graphSafeColor = graphSafeDOColor;
}
DO::~DO() { delete &graph; }

bool DO::isQueueFull() { return graph.isQueueFull(); }
bool DO::isQueueEmpty() { return graph.isQueueEmpty(); }
void DO::pushData(double item) { graph.pushToQueue(item); }
void DO::popData() { graph.popQueue(); }
void DO::draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
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
void DO::drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.drawCurrentValue(d, val, minVal, maxVal,
    title,
    labelX, labelY, labelFontWidth, labelFontHeight, labelColor,
    valX, valY,
    valFontWidth, valFontHeight, valColor,
    minValX, minValY, minLabelX, minLabelY, minColor,
    maxValX, maxValY, maxLabelX, maxLabelY, maxColor,
    labelMinMaxFontWidth, labelMinMaxFontHeight);
}
void DO::drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.plotGraph(d, x, val, ox, oy,
    minRange, maxRange, minSafe, maxSafe, step,
    graphLowerLeftX, graphLowerLeftY,
    graphLineColor, graphSafeColor,
    displayGraph);
}
