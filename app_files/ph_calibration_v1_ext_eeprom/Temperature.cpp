#include "Temperature.h"

Temperature::Temperature() : graph() {
  ox = 0; oy = 0;
  title = graphTemperatureTitle;
  minRange = minTemperature;
  maxRange = maxTemperature;
  minSafe = minSafeTemperature;
  maxSafe = maxSafeTemperature;
  step = stepTemperature;
  displayGraph = true;
  labelX = graphMainTemperatureLabelX; 
  labelY = graphMainTemperatureLabelY;
  labelFontWidth = graphLabelMainFontWidth; 
  labelFontHeight = graphLabelMainFontHeight; 
  labelColor = graphMainLabelTemperatureColor;
  valX = graphMainTemperatureValueX; 
  valY = graphMainTemperatureValueY; 
  valFontWidth = graphLabelMainFontWidth; 
  valFontHeight = graphLabelMainFontHeight; 
  valColor = graphMainValueTemperatureColor;
  minValX = graphMinTemperatureValueX; 
  minValY = graphMinTemperatureValueY; 
  minLabelX = graphMinTemperatureLabelX; 
  minLabelY = graphMinTemperatureLabelY; 
  minColor = graphMinTemperatureColor;
  maxValX = graphMaxTemperatureValueX; 
  maxValY = graphMaxTemperatureValueY; 
  maxLabelX = graphMaxTemperatureLabelX; 
  maxLabelY = graphMaxTemperatureLabelY; 
  maxColor = graphMaxTemperatureColor;
  labelMinMaxFontWidth = graphLabelMinMaxFontWidth; 
  labelMinMaxFontHeight = graphLabelMinMaxFontHeight;
  graphLowerLeftX = TemperatureGraphLowerLeftX;
  graphLowerLeftY = TemperatureGraphLowerLeftY;
  graphLineColor = graphLineTemperatureColor;
  graphSafeColor = graphSafeTemperatureColor;
}
Temperature::~Temperature() { delete &graph; }

bool Temperature::isQueueFull() { return graph.isQueueFull(); }
bool Temperature::isQueueEmpty() { return graph.isQueueEmpty(); }
void Temperature::pushData(double item) { graph.pushToQueue(item); }
void Temperature::popData() { graph.popQueue(); }
void Temperature::draw(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
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
void Temperature::drawLabel(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.drawCurrentValue(d, val, minVal, maxVal,
    title,
    labelX, labelY, labelFontWidth, labelFontHeight, labelColor,
    valX, valY,
    valFontWidth, valFontHeight, valColor,
    minValX, minValY, minLabelX, minLabelY, minColor,
    maxValX, maxValY, maxLabelX, maxLabelY, maxColor,
    labelMinMaxFontWidth, labelMinMaxFontHeight);
}
void Temperature::drawGraph(TFT_22_ILI9225 &d, double &x, double val, double minVal, double maxVal) {
  graph.plotGraph(d, x, val, ox, oy,
    minRange, maxRange, minSafe, maxSafe, step,
    graphLowerLeftX, graphLowerLeftY,
    graphLineColor, graphSafeColor,
    displayGraph);
}
