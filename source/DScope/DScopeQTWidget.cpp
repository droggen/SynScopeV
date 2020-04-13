/*
   DScopeQT: Digital Scope QT widget
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


#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QSizePolicy>
#include <QMenu>
#include <QPixmap>
#include <QDateTime>
#include <QApplication>
#include <QLineEdit>
#include <QStringList>
#include <QSettings>
#include <QtDebug>
#include <QtGlobal>
#include <QFont>
#include "DScopeQTWidget.h"


////////////////////////////////////
DScopeQTWidget::DScopeQTWidget(unsigned _x,unsigned _y,unsigned _w,unsigned _h,bool _alpha,QWidget *parent) :
	QWidget(parent),
	DScopeQT(0,0,0,_w,_h,_alpha)
{
	// create the pixmap
	//pixmap = QPixmap(_w,_h);
	pixmap = QImage(_w,_h,QImage::Format_RGB32);
	DScopeQT::Resize(&pixmap,0,0,_w,_h);
   setMinimumSize(2,2);

   setMouseTracking(true);             // To follow mouse over widget and send signals
   //setFocusPolicy(Qt::StrongFocus); // To accept focus, and thus receive keyboard events
   setFocusPolicy(Qt::WheelFocus);  // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.
}
DScopeQTWidget::~DScopeQTWidget()
{
   printf("DScopeQTWidget::~DScopeQTWidget()\n");
}
	
//! Implementation of painting. event is the zone to redraw
void DScopeQTWidget::paintEvent(QPaintEvent *event)
{
   //printf("DScopeQTWidget::paintEvent\n");
   QPainter p(this);
	QRect validRect = rect() & event->rect();
	p.setClipRect(validRect);
	//p.drawPixmap(basex,basey,pixmap);
	p.drawImage(basex,basey,pixmap);
   event->accept();
}

void DScopeQTWidget::mouseMoveEvent ( QMouseEvent * event )
{
   //printf("mouseMoveEvent\n");
   emit mouseMoved(x2s(event->x()));
   event->accept();
}
void DScopeQTWidget::mousePressEvent ( QMouseEvent * event )
{

   // signal / slot
   emit mousePressed(event->button(),x2s(event->x()));

   //printf("mouse pressed: %d %d %d\n",event->x(),event->y(),event->button());
   //printf("Converstion of pix -> sample %d -> %d\n",event->x(),x2s(event->x()));
    switch(event->button())
    {
            case Qt::MidButton:
                    switch(event->modifiers())
                    {
                            case Qt::ShiftModifier:
                                    SetVAuto();
                                    break;
                            case Qt::NoModifier:
                                    HZoomReset();
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
void DScopeQTWidget::wheelEvent (QWheelEvent * event)
{
   QWidget::wheelEvent(event);
    if(event->delta()>0)
    {
            if(event->modifiers()==Qt::ShiftModifier)
                    VZoomin();
            if(event->modifiers()==Qt::NoModifier)
                    HZoomin();
    }
    else
    {
            if(event->modifiers()==Qt::ShiftModifier)
                    VZoomout();
            if(event->modifiers()==Qt::NoModifier)
                    HZoomout();
    }
    event->accept();
    emit Repaint();
}
void DScopeQTWidget::resizeEvent(QResizeEvent *event)
{
   //printf("resizeEvent from %d %d to %d %d\n",pixmap.width(),pixmap.height(),event->size().width(),event->size().height());

   //printf("isNull: %d\n",pixmap.isNull());
   if(pixmap.isNull())
      printf("Problem: pixmap is null!\n");
   pixmap = pixmap.scaled(event->size());
   DScopeQT::Resize(&pixmap,0,0,event->size().width(),event->size().height());
   emit Repaint();
}

/*
16777235 up
16777237 dn
16777238 pup
16777239 pdn
 */
void DScopeQTWidget::keyPressEvent(QKeyEvent * event)
{
   //printf("Key press event %d\n",event->key());
   if(event->key()==Qt::Key_Up)
   {
      PanUp();
   }
   if(event->key()==Qt::Key_Down)
   {
      PanDown();
   }
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
      if(event->modifiers()==Qt::ShiftModifier)
         VZoomin();
      if(event->modifiers()==Qt::NoModifier)
         HZoomin();
   }
   if(event->key()==Qt::Key_PageDown)
   {
      // Zoom
      if(event->modifiers()==Qt::ShiftModifier)
         VZoomout();
      if(event->modifiers()==Qt::NoModifier)
         HZoomout();
   }
   if(event->key()==Qt::Key_Return)
   {
      if(event->modifiers()==Qt::ShiftModifier)
         SetVAuto();
      if(event->modifiers()==Qt::NoModifier)
         HZoomReset();
   }
   event->accept();
   emit Repaint();

}



/*
void DScopeQTWidget::focusInEvent ( QFocusEvent * event )
{
   printf("focus in event\n");
   emit Repaint();
}

void DScopeQTWidget::focusOutEvent ( QFocusEvent * event )
{
   printf("focus out event. Reason: %d\n",event->reason());
   emit Repaint();
}



void DScopeQTWidget::installEventFilter(QObject *)
{
   printf("DScopeQTWidget::installEventFilter\n");
}

*/
