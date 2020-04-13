/*
   SynScopeV

   Copyright (C) 2008,2009,2010,2011:
         Daniel Roggen, droggen@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "SSViewStickman.h"
#include "WidgetStickman.h"


SSViewStickman::SSViewStickman(QWidget *parent) : SSViewAbstract(parent)
{

   printf("SSViewStickman::SSViewStickman\n");

   plotwidget=0;
   slider=0;
   box=0;

   length=0;



   // Stickman
   //stickman = new WidgetStickman();
   plotwidget = new NPlotWidgetStickman(this);
   plotwidget->setMinimumWidth(160);
   plotwidget->setMinimumHeight(100);


   // Slider
   slider = new QSliderExt(Qt::Horizontal,0);
   slider->setFocusPolicy(Qt::WheelFocus);   // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.
   slider->setRange(0,length);

   // Box
   box = new QVBoxLayout();
   box->addWidget(plotwidget);
   box->addWidget(slider);
   setLayout(box);



   int success;
   success=connect(slider,SIGNAL(valueChanged(int)),this,SLOT(on_Slider_valueChanged(int)));

   success=connect(plotwidget,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   success=connect(plotwidget,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}

SSViewStickman::~SSViewStickman()
{
   printf("SSViewStickman::~SSViewStickman\n");

   if(box) delete box;
   if(slider) delete slider;
   if(plotwidget) delete plotwidget;
}




void SSViewStickman::lengthChanged()
{
   length=0;
   // Find the length of the first non null quaternion vector.
   if(config.data_torso[0])
      length = config.data_torso[0]->size();
   else if(config.data_lua[0])
      length = config.data_lua[0]->size();
   else if(config.data_lla[0])
      length = config.data_lla[0]->size();
   else if(config.data_rua[0])
      length = config.data_rua[0]->size();
   else if(config.data_rla[0])
      length = config.data_rla[0]->size();

   bool ok;
   // Check that all quaterion vectors have the same length between them (and for all joints)
   ok=true;
   if(config.data_torso[0])
   {
      for(unsigned i=0;i<4;i++)
      {
         if(config.data_torso[i]->size()!=length)
            ok=false;
      }
   }
   if(!ok)
      config.data_torso[0] = config.data_torso[1] = config.data_torso[2] = config.data_torso[3] = 0;
   // Check that all quaterion vectors have the same length between them (and for all joints)
   ok=true;
   if(config.data_lua[0])
   {
      for(unsigned i=0;i<4;i++)
      {
         if(config.data_lua[i]->size()!=length)
            ok=false;
      }
   }
   if(!ok)
      config.data_lua[0] = config.data_lua[1] = config.data_lua[2] = config.data_lua[3] = 0;
   // Check that all quaterion vectors have the same length between them (and for all joints)
   ok=true;
   if(config.data_lla[0])
   {
      for(unsigned i=0;i<4;i++)
      {
         if(config.data_lla[i]->size()!=length)
            ok=false;
      }
   }
   if(!ok)
      config.data_lla[0] = config.data_lla[1] = config.data_lla[2] = config.data_lla[3] = 0;
   // Check that all quaterion vectors have the same length between them (and for all joints)
   ok=true;
   if(config.data_rua[0])
   {
      for(unsigned i=0;i<4;i++)
      {
         if(config.data_rua[i]->size()!=length)
            ok=false;
      }
   }
   if(!ok)
      config.data_rua[0] = config.data_rua[1] = config.data_rua[2] = config.data_rua[3] = 0;
   // Check that all quaterion vectors have the same length between them (and for all joints)
   ok=true;
   if(config.data_rla[0])
   {
      for(unsigned i=0;i<4;i++)
      {
         if(config.data_rla[i]->size()!=length)
            ok=false;
      }
   }
   if(!ok)
      config.data_rla[0] = config.data_rla[1] = config.data_rla[2] = config.data_rla[3] = 0;


   printf("Length of data: %d\n",length);

   // Set the slider range
   if(length==0)
      slider->setRange(0,0);
   else
      slider->setRange(0,length-1);
}

void SSViewStickman::setConfig(SSViewStickmanConfig c)
{
   config = c;


   // Check that either all or none of the quaternion vector is a null pointer. Set all to zero in case.
   if(config.data_torso[0]==0 || config.data_torso[1]==0 || config.data_torso[2]==0 || config.data_torso[3]==0)
      config.data_torso[0] = config.data_torso[1] = config.data_torso[2] = config.data_torso[3] = 0;
   if(config.data_lua[0]==0 || config.data_lua[1]==0 || config.data_lua[2]==0 || config.data_lua[3]==0)
      config.data_lua[0] = config.data_lua[1] = config.data_lua[2] = config.data_lua[3] = 0;
   if(config.data_lla[0]==0 || config.data_lla[1]==0 || config.data_lla[2]==0 || config.data_lla[3]==0)
      config.data_lla[0] = config.data_lla[1] = config.data_lla[2] = config.data_lla[3] = 0;
   if(config.data_rua[0]==0 || config.data_rua[1]==0 || config.data_rua[2]==0 || config.data_rua[3]==0)
      config.data_rua[0] = config.data_rua[1] = config.data_rua[2] = config.data_rua[3] = 0;
   if(config.data_rla[0]==0 || config.data_rla[1]==0 || config.data_rla[2]==0 || config.data_rla[3]==0)
      config.data_rla[0] = config.data_rla[1] = config.data_rla[2] = config.data_rla[3] = 0;


   // Set the data for the quaternion.
   /*DataStickman data;
   data.torsorot = config.data_torso;
   data.luarot = config.data_lua;
   data.llarot = config.data_lla;
   data.ruarot = config.data_rua;
   data.rlarot = config.data_rla;
   stickman->GetPlot()->SetData(&data);*/
   plotwidget->setData(config.data_torso,config.data_lua,config.data_lla,config.data_rua,config.data_rla);

   lengthChanged();

}



void SSViewStickman::setTime(int t)
{
   printf("SSViewStickman::setTime\n");
   // When the time is set programmatically we must ensure that no slider signal is emitted

   NoSliderEvents=true;
   slider->setSliderPosition(t);
   NoSliderEvents=false;
}

int SSViewStickman::getTime()
{
   return slider->sliderPosition();
}










void SSViewStickman::on_Slider_valueChanged(int value)
{
   printf("SSViewStickman %p. slider changed to %d\n",this,value);

   plotwidget->setSampleOffset(value);

   repaint();

   if(!NoSliderEvents)
      emit timeChanged(value);
}
/******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void SSViewStickman::on_View_mousePressed(Qt::MouseButton button, int samplex)
{
   emit mousePressed(button,getTime());
}
void SSViewStickman::on_View_mouseMoved(int samplex)
{
   emit mouseMoved(getTime());
}
QString SSViewStickman::name()
{
   return QString("Stickman");
}
