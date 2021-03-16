#include "SensorGraph.h"
#include "Queue.h"
#include "Util.h"

SensorGraph::SensorGraph() : sensorQueue() {}
SensorGraph::~SensorGraph() { delete &sensorQueue; }

Queue SensorGraph::getQueue() { return sensorQueue; }
double* SensorGraph::queueContent() { return sensorQueue.list(); }
int SensorGraph::queueSize() { return sensorQueue.size(); }
void SensorGraph::pushToQueue(double item) { sensorQueue.enqueue(item); }
void SensorGraph::popQueue() { sensorQueue.dequeue(); }
bool SensorGraph::isQueueFull() { sensorQueue.isFull(); }
bool SensorGraph::isQueueEmpty() { sensorQueue.isEmpty(); }
unsigned int SensorGraph::plotColor(double value, double lo, double hi, unsigned int locolor, unsigned int hicolor, unsigned int normalcolor) {
  if (lo!=-1 && value<=lo) return locolor;
  if (hi!=-1&&value>=hi) return hicolor;
  return normalcolor;
}
void SensorGraph::drawGraph(TFT_22_ILI9225 &d, double x, double y, 
  double &ox, double &oy,
  double gx, double gy,
  double w, double h, 
  double xlo, double xhi, double xinc, 
  double ylo, double yhi, double yinc, 
  double losafe, double hisafe, unsigned int markColor,
  double lozone, unsigned int lozonecolor,
  double hizone, unsigned int hizonecolor,
  String title, String xlabel, String ylabel, 
  unsigned int gcolor, unsigned int acolor, unsigned int pcolor, 
  unsigned int tcolor, unsigned int bcolor, 
  boolean &drawXLabel, boolean &drawYLabel, boolean &drawLabelOnlyOnSafe,
  boolean &redraw, boolean &continuous) {

  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;
  unsigned int linecolor = plotColor(y, lozone, hizone, lozonecolor, hizonecolor, pcolor);
  if (redraw == true) {
    redraw = false;
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
      if (i == 0) {
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else {
        if (i==losafe || i==hisafe) d.drawLine(gx, temp, gx + w, temp, markColor);
        else d.drawLine(gx, temp, gx + w, temp, gcolor);
      }
      if (drawYLabel) {
        if (!drawLabelOnlyOnSafe || (drawLabelOnlyOnSafe && (i==losafe || i==hisafe))) {
          d.setFont(Terminal6x8);
          // d.setGFXFont(&FreeMono9pt7b);
          char iStr[50];
          ftoa(i*1.0, iStr, 0);
          d.setBackgroundColor(bcolor);
          d.drawText(gx - 20, temp, iStr, tcolor);
          // d.drawGFXText(gx - 40, temp, iStr, tcolor);
        }
      }
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {
      // compute the transform
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }
      if (drawXLabel) {
        d.setFont(Terminal6x8);
        // d.setGFXFont(&FreeMono9pt7b);
        d.setBackgroundColor(bcolor);
        char iStr[50];
        ftoa(i*1.0, iStr, 0);
        d.drawText(temp, gy + 10, iStr, tcolor);
        // d.drawGFXText(temp, gy + 10, iStr, tcolor);
      }
    }

    //now draw the labels
    if (title) {
      d.setFont(Terminal11x16);
      d.setBackgroundColor(bcolor);
      d.drawText(gx , gy - h - 30, title, tcolor);
    }

    if (xlabel) {
      d.setFont(Terminal6x8);
      // d.setGFXFont(&FreeMono9pt7b);
      d.setBackgroundColor(bcolor);
      d.drawText(gx , gy + 20, xlabel, acolor);
      // d.drawGFXText(gx , gy + 20, xlabel, acolor);
    }
     if (ylabel) {
      d.setFont(Terminal6x8);
      // d.setGFXFont(&FreeMono9pt7b);
      d.setBackgroundColor(bcolor);
      d.drawText(gx - 10, gy - h - 10, ylabel, acolor);
      // d.drawGFXText(gx - 30, gy - h - 10, ylabel, acolor);
     }
  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  if (continuous) {
    d.drawLine(ox, oy, x, y, linecolor);
    d.drawLine(ox, oy + 1, x, y + 1, linecolor);
    d.drawLine(ox, oy - 1, x, y - 1, linecolor);
  } else {
    d.drawLine(x, y, x, y, linecolor);
    d.drawLine(x, y + 1, x, y + 1, linecolor);
    d.drawLine(x, y - 1, x, y - 1, linecolor);
  }
  ox = x;
  oy = y;
}
void SensorGraph::drawCurrentValue(TFT_22_ILI9225 &d,
  double val, double minVal, double maxVal,
  String title,
  double labelX, double labelY,
  double labelFontWidth, double labelFontHeight, unsigned int labelColor,
  double valX, double valY, 
  double valFontWidth, double valFontHeight, unsigned int valColor,
  double minValX, double minValY, double minLabelX, double minLabelY, unsigned int minColor,
  double maxValX, double maxValY, double maxLabelX, double maxLabelY, unsigned int maxColor,
  double labelMinMaxFontWidth, double labelMinMaxFontHeight
) {
  char valStr[10];
  ftoa(val, valStr, 2);
  char maxStr[10];
  ftoa(maxVal, maxStr, 2);
  char minStr[10];
  ftoa(minVal, minStr, 2);
  if (val<10) {
    d.fillRectangle(valX, valY, valX+labelFontWidth, labelY+labelFontHeight, bgColor);
  }
  d.setFont(graphLabelMainFont);
  d.drawText(labelX, labelY, title, labelColor);
  d.drawText(valX + (val>=10?0:labelFontWidth), valY, valStr, valColor);
  if (maxVal<10) {
    d.fillRectangle(maxValX, maxValY, maxValX+labelMinMaxFontWidth, maxLabelY+labelMinMaxFontHeight, bgColor);
  }
  d.setFont(graphLabelMinMaxFont);
  d.drawText(maxLabelX, maxLabelY, "MAX", maxColor);
  d.drawText(maxValX + (maxVal>=10?0:labelMinMaxFontWidth), maxValY, maxStr, maxColor);
  if (minVal<10) {
    d.fillRectangle(minValX, minValY, minValX+labelMinMaxFontWidth, minLabelY+labelMinMaxFontHeight, bgColor);
  }
  d.drawText(minLabelX, minLabelY, "MIN", minColor);
  d.drawText(minValX + (minVal>=10?0:labelMinMaxFontWidth), minValY, minStr, minColor);
}
void SensorGraph::clearGraph(TFT_22_ILI9225 &d, double graphLowerLeftX, double graphLowerLeftY) {
  d.fillRectangle(graphLowerLeftX, graphLowerLeftY + 1, 
    graphLowerLeftX + graphW, graphLowerLeftY - graphH - 1, bgColor);
}
void SensorGraph::plotGraph(TFT_22_ILI9225 &d,
  double &x, double y, double &ox, double &oy,
  double min, double max, double minSafe, double maxSafe, double step,
  double graphLowerLeftX, double graphLowerLeftY,
  unsigned int graphLineColor, unsigned int graphSafeColor,
  boolean &displayGraph
) {
  boolean drawXLabel = false;
  boolean drawYLabel = true;
  boolean displayContinuous = true;
  boolean drawLabelOnlyOnSafe = true;
  int size = sensorQueue.size();
  double *list = sensorQueue.list();
  if (!continueDrawAfterFull && x == graphXCount) {
    x = 0;
    displayGraph = true;
    clearGraph(d, graphLowerLeftX, graphLowerLeftY);
  }
  if (continueDrawAfterFull && x == graphXCount) {
    x = 0;
    displayGraph = true;
    displayContinuous = false;
    clearGraph(d, graphLowerLeftX, graphLowerLeftY);
    // Serial.print("queue size: ");
    // Serial.println(pHQueue.size());
    // for (int a=0; a<pHQueue.size(); a++) {
    //   char str[20];
    //   ftoa(list[a], str, 2);
    //   Serial.print(str);
    //   Serial.print(", ");
    // }
    // Serial.println();
    int c = 0;
    do {
      drawGraph(d, x, list[c], ox, oy,
        graphLowerLeftX, graphLowerLeftY, /*lower left*/
        graphW, graphH, /*w/h*/
        0, /*xlo bound*/ graphXCount, /*xhi bound*/ 1, /*x-axis division*/
        min, /*ylo bound*/ max, /*yhi bound*/ step, /*y-axis division*/
        minSafe, /*lo safe bound*/ maxSafe, /*hi safe bound*/ graphSafeColor, /*label mark color*/
        -1, /*lo zone*/ 0, /*lo zone color*/
        -1, /*hi zone*/ 0, /*hi zone color*/
        "", "", "", 
        graphScaleColor, graphAxisColor, graphLineColor, 
        COLOR_WHITE, COLOR_BLACK, 
        drawXLabel, drawYLabel, drawLabelOnlyOnSafe,
        displayGraph, displayContinuous);
      x++;
      displayContinuous = true;
      c++;
    } while (c<size);
    x = graphXCount;
  } else {
    displayContinuous = true;
    drawGraph(d, x, y, ox, oy,
      graphLowerLeftX, graphLowerLeftY, /*lower left*/
      graphW, graphH, /*w/h*/
      0, /*xlo bound*/ graphXCount, /*xhi bound*/ 1, /*x-axis division*/
      min, /*ylo bound*/ max, /*yhi bound*/ step, /*y-axis division*/
      minSafe, /*lo safe bound*/ maxSafe, /*hi safe bound*/ graphSafeColor, /*label mark color*/
      -1, /*lo zone*/ 0, /*lo zone color*/
      -1, /*hi zone*/ 0, /*hi zone color*/
      "", "", "", 
      graphScaleColor, graphAxisColor, graphLineColor, 
      graphAxisLabelColor, COLOR_BLACK, 
      drawXLabel, drawYLabel, drawLabelOnlyOnSafe,
      displayGraph, displayContinuous);
    x++;
  }
}
