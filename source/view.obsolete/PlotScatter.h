#ifndef PLOTSCATTER_H
#define PLOTSCATTER_H



#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>

#include "PlotWidgetBase.h"

class DataScatter : public DataBase
{
public:
   DataScatter() {};
   virtual ~DataScatter() {};

   std::vector<std::vector<int> *> x;
   std::vector<std::vector<int> *> y;
   std::vector<unsigned> color;
};


class PlotScatter : public PlotBase
{

public:
    PlotScatter();
    virtual ~PlotScatter();

    virtual void SetTitle(std::string &t);
    virtual void SetSampleOffset(int value);
    virtual void SetRangePoint(int value);
    virtual void SetData(DataBase *data);       // !Must pass a DataScatter type here
    virtual void Plot();
    virtual void Resize(unsigned _w,unsigned _h);
    virtual void SetAutoscale(bool as);
    virtual void SetScale(int xmin,int ymin,int xmax,int ymax);

    virtual const QImage &GetImage();

protected:
    DataScatter data;
    int sampleoffset;
    int rangepoint;
    bool autoscale;
    int sxmin,sxmax,symin,symax;           // User-set scale
    int esxmin,esxmax,esymin,esymax;           // Effective scale
    int minspace;



    //int s2x(int s,int smin,int smax,int pmin,int pmax);
    int s2x(int s);
    //int s2y(int s,int smin,int smax,int pmin,int pmax);
    int s2y(int s);
    int FindSmallestAbove(int target,int &factor,int &power);
    int XsTickSpacing(int &factor,int &power);
    int YvTickSpacing(int &factor,int &power);
    void DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);
    void DrawVGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);






};


#endif // PLOTSCATTER_H
