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

#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>
#include "SSViewScatter.h"



SSViewScatter::SSViewScatter(QWidget *parent) : SSViewAbstract(parent)
{

   printf("SSViewScatter::SSViewScatter\n");

   slider=0;
   box=0;
   length=0;

   // Scatter
   plotwidget = new NPlotWidgetScatter(this);
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

SSViewScatter::~SSViewScatter()
{
   printf("SSViewScatter::~SSViewScatter\n");

   if(box) delete box;
   if(slider) delete slider;
   if(plotwidget) delete plotwidget;
}






void SSViewScatter::lengthChanged()
{
   length=0;
   // Find the length of the data
   if(config.data_x.size())
      length = config.data_x[0]->size();

   printf("Length of data: %d\n",length);

   // Set the slider range
   if(length==0)
      slider->setRange(0,0);
   else
      slider->setRange(0,length-1);
}

void SSViewScatter::setConfig(SSViewScatterConfig c)
{
   config = c;

   printf("SSViewScatter::setConfig. x,y,c size: %d %d %d\n",c.data_x.size(),c.data_y.size(),c.colors.size());



   plotwidget->setData(config.data_x,config.data_y,config.colors);

   plotwidget->setAutoscale(config.autoscale);
   plotwidget->setScale(config.xmin,config.ymin,config.xmax,config.ymax);


   lengthChanged();

}



void SSViewScatter::setTime(int t)
{
   // When the time is set programmatically we must ensure that no slider signal is emitted

   NoSliderEvents=true;
   slider->setSliderPosition(t);
   NoSliderEvents=false;

}

int SSViewScatter::getTime()
{
   return slider->sliderPosition();
}










/******************************************************************************
*******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void SSViewScatter::on_Slider_valueChanged(int value)
{
   // Here we have to draw something on screen.
   plotwidget->setSampleOffset(value);

   repaint();

   if(!NoSliderEvents)
      emit timeChanged(value);
}
void SSViewScatter::on_View_mousePressed(Qt::MouseButton button, int samplex)
{
   emit mousePressed(button,getTime());
}
void SSViewScatter::on_View_mouseMoved(int samplex)
{
   emit mouseMoved(getTime());
}

QString SSViewScatter::name()
{
   return QString("Scatter");
}
