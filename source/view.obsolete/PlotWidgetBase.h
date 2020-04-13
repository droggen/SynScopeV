#ifndef PLOTWIDGETBASE_H
#define PLOTWIDGETBASE_H

#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>
#include <QGLWidget>
#include "PlotBase.h"


class PlotWidgetBase : public QWidget
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    PlotWidgetBase(QWidget *parent=0);
    virtual ~PlotWidgetBase();
    //virtual QImage *GetImage() = 0;

    //virtual PlotBase *GetPlot();

//protected:
  //  PlotBase *plot;

signals:
   virtual void Repaint() = 0;
   virtual void mousePressed(Qt::MouseButton button,int samplex) = 0;
   virtual void mouseMoved(int samplex) = 0;

};
/*
class PlotGLWidgetBase : public QGLWidget,public PlotBase
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    PlotGLWidgetBase(QWidget *parent=0);
    virtual ~PlotGLWidgetBase();



    virtual QImage *GetImage();

protected:
   QImage pixmap;
   QPainter painter;


signals:
   void Repaint();
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};
*/



#endif // PLOTWIDGETBASE_H
