#include "GraphVar.h"
#include "Queue.h"

#ifndef sensor_graph_h
#define sensor_graph_h

class SensorGraph {
  private:
    Queue sensorQueue;
    double x;
    double y;
  public:
    SensorGraph();
    ~SensorGraph();

    Queue getQueue();
    double *queueContent();
    int queueSize();
    bool isQueueFull();
    bool isQueueEmpty();
    void pushToQueue(double item);
    void popQueue();
    unsigned int plotColor(double val, double lo, double hi, unsigned int locolor, unsigned int hicolor, unsigned int normalcolor);
    void clearGraph(TFT_22_ILI9225 &d, double graphLowerLeftX, double graphLowerLeftY);
    void drawGraph(TFT_22_ILI9225 &d, double x, double y, 
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
      boolean &redraw, boolean &continuous);
    void drawCurrentValue(TFT_22_ILI9225 &d,
      double val, double minVal, double maxVal,
      String title,
      double labelX, double labelY,
      double labelFontWidth, double labelFontHeight, unsigned int labelColor,
      double valX, double valY, 
      double valFontWidth, double valFontHeight, unsigned int valColor,
      double minValX, double minValY, double minLabelX, double minLabelY, unsigned int minColor,
      double maxValX, double maxValY, double maxLabelX, double maxLabelY, unsigned int maxColor,
      double labelMinMaxFontWidth, double labelMinMaxFontHeight);
    void plotGraph(TFT_22_ILI9225 &d,
      double &x, double val, double &ox, double &oy,
      double min, double max, double minSafe, double maxSafe, double step,
      double graphLowerLeftX, double graphLowerLeftY,
      unsigned int graphLineColor, unsigned int graphSafeColor,
      boolean &displayGraph);
};

#endif
