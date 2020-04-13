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



#include <stdio.h>
#include <QMouseEvent>
#include "WidgetLabel.h"
#include "glhelper.h"




WidgetLabel::WidgetLabel(QWidget *parent) :
    PlotWidgetBase(parent)
{

    setMinimumSize(50,50);

    setMinimumHeight(GetPlot()->getIdealHeight(1));

    setMouseTracking(true);             // To follow mouse over widget and send signals
    //setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
    setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.


}
WidgetLabel::~WidgetLabel()
{

}

PlotLabel *WidgetLabel::GetPlot()
{
    return &plot;
}


void WidgetLabel::resizeEvent(QResizeEvent *event)
{
    printf("**********WidgetLabel::resizeEvent to %d %d\n",event->size().width(),event->size().height());

    plot.Resize(event->size().width(),event->size().height());
    emit Repaint();
}
void WidgetLabel::paintEvent(QPaintEvent *event)
{
    printf("WidgetLabel::paintEvent\n");
    printf("calling plot\n");
    plot.Plot();
    QPainter p(this);
    QRect validRect = rect() & event->rect();
    p.setClipRect(validRect);
    printf("drawing image\n");
    p.drawImage(0,0,plot.GetImage());

    // Draw a frame if we have the focus
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.drawRect(0,0,width()-1,height()-1);
    if(hasFocus())
    {
       QPen pen;
       pen.setColor(Qt::black);
       pen.setStyle(Qt::DotLine);
       painter.setPen(pen);
       painter.drawRect(0,0,width()-1,height()-1);
    }


    printf("accepting\n");
    event->accept();
}



void WidgetLabel::wheelEvent (QWheelEvent * event)
{
   QWidget::wheelEvent(event);
    if(event->delta()>0)
    {
            //if(event->modifiers()==Qt::ShiftModifier)
              //      VZoomin();
            //if(event->modifiers()==Qt::NoModifier)
                    GetPlot()->HZoomin();
    }
    else
    {
            //if(event->modifiers()==Qt::ShiftModifier)
              //      VZoomout();
            if(event->modifiers()==Qt::NoModifier)
                    GetPlot()->HZoomout();
    }
    event->accept();
    emit Repaint();
}

void WidgetLabel::keyPressEvent(QKeyEvent * event)
{
   /*if(event->key()==Qt::Key_Up)
   {
      PanUp();
   }
   if(event->key()==Qt::Key_Down)
   {
      PanDown();
   }*/
   if(event->key()==Qt::Key_Left)
   {
      emit panLeft();
   }
   if(event->key()==Qt::Key_Right)
   {
      emit panRight();
   }
   if(event->key()==Qt::Key_PageUp)
   {
      // Zoom
      //if(event->modifiers()==Qt::NoModifier)
         //VZoomin();
      //if(event->modifiers()==Qt::ShiftModifier)
         GetPlot()->HZoomin();
   }
   if(event->key()==Qt::Key_PageDown)
   {
      // Zoom
      //if(event->modifiers()==Qt::NoModifier)
         //VZoomout();
      //if(event->modifiers()==Qt::ShiftModifier)
         GetPlot()->HZoomout();
   }
   if(event->key()==Qt::Key_Return)
   {
      //if(event->modifiers()==Qt::NoModifier)
        // SetVAuto();
      //if(event->modifiers()==Qt::ShiftModifier)
         GetPlot()->HZoomReset();
   }
   event->accept();
   emit Repaint();

}


/******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void WidgetLabel::mouseMoveEvent ( QMouseEvent * event )
{
   emit mouseMoved(GetPlot()->x2s(event->x()));
   event->accept();
}


void WidgetLabel::mousePressEvent ( QMouseEvent * event )
{
   // signal / slot
   emit mousePressed(event->button(),GetPlot()->x2s(event->x()));

   //printf("mouse pressed: %d %d %d\n",event->x(),event->y(),event->button());
   //printf("Converstion of pix -> sample %d -> %d\n",event->x(),x2s(event->x()));
    switch(event->button())
    {
            case Qt::MidButton:
                    switch(event->modifiers())
                    {
                            case Qt::ShiftModifier:
                                    GetPlot()->SetVAuto();
                                    break;
                            case Qt::NoModifier:
                                    GetPlot()->HZoomReset();
                                    break;
                            default:
                                    event->ignore();
                                    return;
                    }
                    break;
            default:
                    event->ignore();
                    return;
    }
    event->accept();
    emit Repaint();
}







