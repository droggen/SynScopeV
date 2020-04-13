#ifndef PLOTBASE_H
#define PLOTBASE_H


#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>

// Should find a base class that is baser than scopebase
// Basic stuff of scopebase without scope specific things into PlotBase



class DataBase
{
public:
   DataBase();
   virtual ~DataBase();
};

// This should be the base class for any widget displaying signals
class PlotBase
{
public:
   PlotBase();
   virtual ~PlotBase();

   virtual void SetData(DataBase *) = 0;
   virtual void Plot()=0;
   virtual void SetTitle(std::string &t) = 0;
   virtual void SetSampleOffset(int co)=0;
   virtual const QImage &GetImage()=0;
   virtual void Resize(unsigned _w,unsigned _h) = 0;



protected:
   QImage pixmap;
   QPainter painter;



};


#endif // PLOTBASE_H
