#ifndef NPLOTWIDGETSTICKMAN_H
#define NPLOTWIDGETSTICKMAN_H

#include "NPlotWidgetBase.h"
#include "PlotStickman_p.h"



typedef struct
{
   std::string title;
} NPLOTWIDGETSTICKMANCONFIG;



// New stickman widget....
class NPlotWidgetStickman : public NPlotWidgetBase
{
   Q_OBJECT
public:
   NPlotWidgetStickman(QWidget * parent = 0, Qt::WindowFlags f = 0);
   virtual ~NPlotWidgetStickman();

   virtual void plot();
   virtual void setData(const std::vector<std::vector<int> *> &_torsorot,const std::vector<std::vector<int> *> &_luarot,const std::vector<std::vector<int> *> &_llarot,const std::vector<std::vector<int> *> &_ruarot,const std::vector<std::vector<int> *> &_rlarot);

protected:
   std::string title;

   std::vector<std::vector<int> *> torsorot,luarot,llarot,ruarot,rlarot;   // Data as integers (float*1000). Format torsorot[quat][sample]



   PlotStickman_p gl;

   virtual void resizeEvent(QResizeEvent *event);

   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );



signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};



#endif // NPLOTWIDGETSTICKMAN_H
