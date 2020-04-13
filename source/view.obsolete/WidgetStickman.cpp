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
#include <cassert>
#include "WidgetStickman.h"
#include "glhelper.h"
#include "PlotStickman.h"
#include "PlotStickman_p.h"




WidgetStickman::WidgetStickman(QWidget *parent)
    : PlotWidgetBase(parent)
{

    setMinimumSize(10,10);


    setMouseTracking(true);             // To follow mouse over widget and send signals
    //setFocusPolicy(Qt::StrongFocus); // To accept focus, and thus receive keyboard events
    //setFocusPolicy(Qt::WheelFocus);  // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

}
WidgetStickman::~WidgetStickman()
{

}




PlotStickman *WidgetStickman::GetPlot()
{
    return &plot;
}


void WidgetStickman::resizeEvent(QResizeEvent *event)
{
    plot.Resize(event->size().width(),event->size().height());
    emit Repaint();
}
void WidgetStickman::paintEvent(QPaintEvent *event)
{
    plot.Plot();
    QPainter p(this);
    QRect validRect = rect() & event->rect();
    p.setClipRect(validRect);
    p.drawImage(0,0,plot.GetImage());
    event->accept();
}





/******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void WidgetStickman::mouseMoveEvent ( QMouseEvent * event )
{
    emit mouseMoved(0);
    event->accept();
}

void WidgetStickman::mousePressEvent ( QMouseEvent * event )
{
   emit mousePressed(event->button(),0);
   event->accept();

}
