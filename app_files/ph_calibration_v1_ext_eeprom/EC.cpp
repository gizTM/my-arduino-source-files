#include "EC.h"

EC::EC() : graph() {
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
