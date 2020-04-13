#ifndef VIEWBASEWIDGET_H
#define VIEWBASEWIDGET_H

#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>

// Should find a base class that is baser than scopebase
// Basic stuff of scopebase without scope specific things into PlotBase



// This should be the base class for any widget displaying signals
class ViewbaseWidget : public QWidget
{
   Q_OBJECT        // must include this if you use Qt signals/slots

public:
   ViewbaseWidget(QWidget *parent=0) {};
   ~ViewbaseWidget() {};

   virtual void SetData(const std::vector<std::vector<int> *> &v,const std::vector<unsigned> &color) = 0;
   virtual void Plot(const std::vector<std::vector<int> *> &v,const std::vector<unsigned> &color) = 0;
   virtual void SetTitle(std::string &t) = 0;
   virtual void SetSampleOffset(int co)=0;
   virtual QImage *GetImage()=0;


private:
   QImage pixmap;
   QPainter painter;


};

#endif // VIEWBASEWIDGET_H
