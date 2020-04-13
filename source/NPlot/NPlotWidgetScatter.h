#ifndef NPLOTWIDGETSCATTER_H
#define NPLOTWIDGETSCATTER_H

#include "NPlotWidgetBase.h"

typedef struct
{
    std::string title;
    std::vector<unsigned> colors;            // Colors of the trace
    int rangepoint;
    bool autoscale;
    int xmin,ymin,xmax,ymax;
} NPLOTWIDGETSCATTERCONFIG;

// New stickman widget....
class NPlotWidgetScatter : public NPlotWidgetBase
{
   Q_OBJECT
public:
   NPlotWidgetScatter(QWidget * parent = 0, Qt::WindowFlags f = 0);
   virtual ~NPlotWidgetScatter();

   virtual void plot();
   virtual void setData(const std::vector<std::vector<int> *> &_x,const std::vector<std::vector<int> *> &_y,const std::vector<unsigned> &_color);

   virtual void setRangePoint(int value);
   virtual void setAutoscale(bool as);
   virtual void setScale(int xmin,int ymin,int xmax,int ymax);


protected:
   std::string title;

   std::vector<std::vector<int> *> x;
   std::vector<std::vector<int> *> y;
   std::vector<unsigned> color;

   int rangepoint;
   bool autoscale;
   int sxmin,sxmax,symin,symax;           // User-set scale
   int esxmin,esxmax,esymin,esymax;           // Effective scale
   int minspace;

   int s2x(int s);
   //int s2y(int s,int smin,int smax,int pmin,int pmax);
   int s2y(int s);
   int FindSmallestAbove(int target,int &factor,int &power);
   int XsTickSpacing(int &factor,int &power);
   int YvTickSpacing(int &factor,int &power);
   void DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);
   void DrawVGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);


   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );

signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};



#endif // NPLOTWIDGETSCATTER_H
