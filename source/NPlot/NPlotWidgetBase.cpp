#include "NPlotWidgetBase.h"


NPlotWidgetBase::NPlotWidgetBase(QWidget *parent, Qt::WindowFlags f)
   : QWidget(parent,f)
   , pixmap(1,1,QImage::Format_RGB32)
{
   setMinimumSize(5,5);
   setMouseTracking(true);             // To follow mouse over widget and send signals
   //setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
   setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.
   sampleoffset=0;
}

NPlotWidgetBase::~NPlotWidgetBase()
{

}
const QImage &NPlotWidgetBase::getImage()
{
   return pixmap;
}

void NPlotWidgetBase::setTitle(const std::string &t)
{
   title = t;
}

/*void NPlotWidgetBase::setSize(unsigned _w,unsigned _h)
{
   resize(_w,_h);
}*/
void NPlotWidgetBase::setSampleOffset(int co)
{
   sampleoffset=co;
}

// Copy the image to the widget
void NPlotWidgetBase::imageToWidget()
{
   QPainter p(this);
   QRect validRect = rect();
   p.setClipRect(validRect);
   p.drawImage(0,0,getImage());
}

void NPlotWidgetBase::plot()
{
   //printf("**********NPlotWidgetBase::plot\n");

}

void NPlotWidgetBase::paintEvent(QPaintEvent *event)
{
   //printf("**********NPlotWidgetBase::paintEvent\n");
   plot();
   imageToWidget();
/*
   // Draw a frame if we have the focus
   printf("NPlotWidgetBase::plot 1 %d\n",painter.isActive());
   QPainter p(this);
   printf("NPlotWidgetBase::plot 1 %d\n",p.isActive());
   p.setPen(Qt::white);
   p.drawRect(0,0,width()-1,height()-1);
   printf("hasfocus: %d\n",hasFocus());
   if(hasFocus())
   {
      QPen pen;
      pen.setColor(Qt::black);
      pen.setStyle(Qt::DotLine);
      p.setPen(pen);
      p.drawRect(0,0,width()-1,height()-1);
   }
*/
}

void NPlotWidgetBase::resizeEvent(QResizeEvent *event)
{
   printf("**********NPlotWidgetBase::resizeEvent to %d %d\n",event->size().width(),event->size().height());
   pixmap = pixmap.scaled(event->size());
}

void NPlotWidgetBase::mousePressEvent ( QMouseEvent * event )
{
   emit mouseMoved(0);
}

void NPlotWidgetBase::mouseMoveEvent ( QMouseEvent * event )
{
   emit mousePressed(event->button(),0);
}
