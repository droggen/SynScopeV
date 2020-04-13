/*
   SynScopeV

   Copyright (C) 2008,2009:
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

#include "videowidgetext.h"

#include <Qt>
#include <QMouseEvent>
#include <QPainter>

#include <stdio.h>

VideoWidgetExt::VideoWidgetExt(QWidget *parent) : Phonon::VideoWidget(parent)
{
   setAttribute(Qt::WA_NoSystemBackground,false);
   setAttribute(Qt::WA_OpaquePaintEvent, false);
   setAttribute(Qt::WA_PaintOnScreen, false);
   setAutoFillBackground(true);

   setMouseTracking(true);             // To follow mouse over widget and send signals
   //setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
   setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

   focus = false;
}


void VideoWidgetExt::mouseMoveEvent ( QMouseEvent * event )
{
    emit mouseMoved(0);
}

void VideoWidgetExt::mousePressEvent ( QMouseEvent * event )
{
   // signal / slot
   emit mousePressed(event->button(),0);

}
void VideoWidgetExt::paintEvent(QPaintEvent *event)
{
   printf("VideoWidgetExt::paintEvent\n");
   QPainter painter(this);
   QSize s = size();
   painter.setPen(Qt::white);
   //painter.drawLine(s.width()/2,0,s.width()/2,s.height()-1);
   painter.drawLine(s.width()/2-1,0,s.width()/2-1,s.height()-1);
   painter.drawLine(s.width()/2+1,0,s.width()/2+1,s.height()-1);
   event->accept();
}
void VideoWidgetExt::DoClear()
{
   //repaint(0,0,width(),height());
   QPainter painter(this);
   QSize s = size();
   painter.setPen(Qt::white);
   painter.setBrush(Qt::red);
   painter.drawRect(0,0,width(),height());
   //painter.drawLine(s.width()/2,0,s.width()/2,s.height()-1);
   painter.drawLine(s.width()/2-1,0,s.width()/2-1,s.height()-1);
   painter.drawLine(s.width()/2+1,0,s.width()/2+1,s.height()-1);
}
void VideoWidgetExt::focusInEvent ( QFocusEvent * event )
{
   focus=true;
   printf("VideoWidgetExt::focusInEvent\n");
   event->accept();
}

void VideoWidgetExt::focusOutEvent ( QFocusEvent * event )
{
   focus=false;
   printf("VideoWidgetExt::focusOutEvent\n");
   event->accept();

}
bool VideoWidgetExt::hasFocus() const
{
   return focus;
}
