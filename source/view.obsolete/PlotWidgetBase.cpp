#include "PlotWidgetBase.h"


PlotWidgetBase::PlotWidgetBase(QWidget *parent)
   :QWidget(parent)
{
}

PlotWidgetBase::~PlotWidgetBase()
{
}

/*
PlotBase *PlotWidgetBase::GetPlot()
{
    return plot;
}
*/



/*
PlotGLWidgetBase::PlotGLWidgetBase(QWidget *parent)
   :QGLWidget(parent)
   ,pixmap(1,1,QImage::Format_RGB32)
{

}

PlotGLWidgetBase::~PlotGLWidgetBase()
{
}



QImage *PlotGLWidgetBase::GetImage()
{
   return &pixmap;
}
*/
