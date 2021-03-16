#include "pH.h"

pH::pH() : graph() {
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
