#ifndef NPLOTWIDGETLABEL_H
#define NPLOTWIDGETLABEL_H


#include <string>
#include "NPlotWidgetBase.h"
#include "DScope/ScopeBase.h"

typedef struct
{
    std::string title;
    QStringList subtitle;
    std::vector<unsigned> colors;
    int xscale;
    std::vector<int> yscale;
    bool yauto;
} NPLOTWIDGETLABELCONFIG;




// New stickman widget....
class NPlotWidgetLabel : public NPlotWidgetBase, public ScopeBase
{
   Q_OBJECT
public:
   NPlotWidgetLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
   virtual ~NPlotWidgetLabel();

   virtual void plot();
   virtual void setData(const std::vector<std::vector<int> *> &_v,const std::vector<unsigned> &_color);
   virtual void setSampleOffset(int co);
   virtual unsigned getIdealHeight(int n);
   virtual void setSubtitle(QStringList &sub);


protected:
   std::vector<std::vector<int> *> v;
   std::vector<unsigned> color;


   int leftsample, rightsample;     // Internally used by Plot and DrawHAxis
   std::string title;
   QStringList subtitle;
   int textheight;

   void DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);

   virtual void resizeEvent(QResizeEvent *event);
   virtual void wheelEvent (QWheelEvent * event);
   virtual void keyPressEvent(QKeyEvent * event);
   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );

signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);
   void panLeft();
   void panRight();









};


#endif // NPLOTWIDGETLABEL_H
