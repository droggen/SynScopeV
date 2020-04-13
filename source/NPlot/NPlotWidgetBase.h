#ifndef NPLOTWIDGETBASE_H
#define NPLOTWIDGETBASE_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>
#include <string>

// New plot widget....
class NPlotWidgetBase : public QWidget
{
   Q_OBJECT
public:
   NPlotWidgetBase(QWidget * parent = 0, Qt::WindowFlags f = 0 );
   virtual ~NPlotWidgetBase();

   virtual void setTitle(const std::string &t);
   virtual const QImage &getImage();
   //virtual void setSize(unsigned _w,unsigned _h);
   virtual void setSampleOffset(int co);
   virtual void imageToWidget();

   virtual void plot();

   //virtual void resize ( int w, int h );

protected:
   std::string title;
   int sampleoffset;

   QImage pixmap;
   QPainter painter;


   virtual void paintEvent(QPaintEvent *event);
   virtual void resizeEvent(QResizeEvent *event);


   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );


signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};

#endif // NPLOTWIDGETBASE_H
