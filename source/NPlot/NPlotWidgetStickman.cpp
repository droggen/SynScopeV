#include "NPlotWidgetStickman.h"

#include <stdio.h>


NPlotWidgetStickman::NPlotWidgetStickman(QWidget * parent, Qt::WindowFlags f )
   : NPlotWidgetBase(parent,f)
{


}

NPlotWidgetStickman::~NPlotWidgetStickman()
{


}


void NPlotWidgetStickman::plot()
{
   // Quaternions for the joints.
   std::vector<float> q_torso={1,0,0,0};
   std::vector<float> q_lua={1,0,0,0};
   std::vector<float> q_lla={1,0,0,0};
   std::vector<float> q_rua={1,0,0,0};
   std::vector<float> q_rla={1,0,0,0};

   // Get the data at sampleoffset
   for(unsigned i=0;i<4;i++)
   {
      if(sampleoffset<(int)torsorot[i]->size())
         q_torso[i] = torsorot[i]->operator[](sampleoffset)/1000.0;
      else
         q_torso[i] = 0;
      if(sampleoffset<(int)luarot[i]->size())
         q_lua[i] = luarot[i]->operator[](sampleoffset)/1000.0;
      else
         q_lua[i] = 0;
      if(sampleoffset<(int)llarot[i]->size())
         q_lla[i] = llarot[i]->operator[](sampleoffset)/1000.0;
      else
         q_lla[i] = 0;
      if(sampleoffset<(int)ruarot[i]->size())
         q_rua[i] = ruarot[i]->operator[](sampleoffset)/1000.0;
      else
         q_rua[i] = 0;
      if(sampleoffset<(int)rlarot[i]->size())
         q_rla[i] = rlarot[i]->operator[](sampleoffset)/1000.0;
      else
         q_rla[i] = 0;
   }

   // Do the rendering using the private class
   gl.Plot(q_torso,q_lua,q_lla,q_rua,q_rla);

   // Get the image
   pixmap = gl.GetImage();

   NPlotWidgetBase::plot();
}



void NPlotWidgetStickman::setData(const std::vector<std::vector<int> *> &_torsorot,const std::vector<std::vector<int> *> &_luarot,const std::vector<std::vector<int> *> &_llarot,const std::vector<std::vector<int> *> &_ruarot,const std::vector<std::vector<int> *> &_rlarot)
{
   torsorot = _torsorot;
   luarot = _luarot;
   llarot = _llarot;
   ruarot = _ruarot;
   rlarot = _rlarot;
}



void NPlotWidgetStickman::resizeEvent(QResizeEvent *event)
{
   //printf("NPlotWidgetStickman::resizeEvent\n");
   gl.resize(event->size().width(),event->size().height());
   NPlotWidgetBase::resizeEvent(event);

   event->accept();

}





/******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void NPlotWidgetStickman::mouseMoveEvent ( QMouseEvent * event )
{
    emit mouseMoved(sampleoffset);
    event->accept();
}

void NPlotWidgetStickman::mousePressEvent ( QMouseEvent * event )
{
   emit mousePressed(event->button(),sampleoffset);
   event->accept();

}
