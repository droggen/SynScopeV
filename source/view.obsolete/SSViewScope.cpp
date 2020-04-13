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


#include "SSViewScope.h"


SSViewScope::SSViewScope(QWidget *parent) : SSViewAbstract(parent)
{
   plotwidget=0;
   slider=0;
   box=0;

    // Scope
   plotwidget = new NPlotWidgetScopeExt(this);
   plotwidget->setMinimumHeight(50);

   // Slider
   slider = new QSliderExt(Qt::Horizontal,0);
   slider->setFocusPolicy(Qt::WheelFocus);   // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

   // Box
   box = new QVBoxLayout();
   box->addWidget(plotwidget);
   box->addWidget(slider);
   setLayout(box);



   // Add a handler
   int success;
   success=connect(slider,SIGNAL(valueChanged(int)),this,SLOT(on_Slider_valueChanged(int)));

   success=connect(plotwidget,SIGNAL(panLeft()),slider,SLOT(scrollLeft()));
   success=connect(plotwidget,SIGNAL(panRight()),slider,SLOT(scrollRight()));
   success=connect(plotwidget,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   success=connect(plotwidget,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));

}

SSViewScope::~SSViewScope()
{
   printf("SSViewScope::~SSViewScope");
   if(box) delete box;
   if(slider) delete slider;
   if(plotwidget) delete plotwidget;
}


void SSViewScope::on_Repaint()
{
   update();
}




void SSViewScope::setConfig(SSViewScopeConfig c)
{
   config = c;

   plotwidget->setData(c.data,c.colors);
   plotwidget->setTitle(config.title);
   plotwidget->SetAlignment(true);
   plotwidget->SetGridPosition(true);


   if(config.yauto)
      plotwidget->SetVAuto();
   else
      plotwidget->SetVRange(config.yscale[0],config.yscale[1]);
   plotwidget->HZoom(config.xscale);

   lengthChanged();
}

void SSViewScope::setTime(int t)
{
   // When the time is set programmatically we must ensure that no slider signal is emitted

   NoSliderEvents=true;
   slider->setSliderPosition(t);
   NoSliderEvents=false;

}

int SSViewScope::getTime()
{
   return slider->sliderPosition();
}

// Call this to indicate that the data length has changed, for instance due to resampling.
// It adapts the slided length accordingly
void SSViewScope::lengthChanged()
{
   slider->setRange(0,config.data[0]->size());
}


void SSViewScope::on_Slider_valueChanged(int value)
{
   plotwidget->setSampleOffset(value);
   update();

   if(!NoSliderEvents)
      emit timeChanged(value);
}

void SSViewScope::on_View_mousePressed(Qt::MouseButton button, int samplex)
{
   emit mousePressed(button,samplex);
}

void SSViewScope::on_View_mouseMoved(int samplex)
{
   emit mouseMoved(samplex);
}
QString SSViewScope::name()
{
   return QString("Scope");
}

