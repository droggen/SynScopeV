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
#ifndef GUIHELPER_H
#define GUIHELPER_H

#include <QSlider>
#include <QScrollArea>
#include <QEvent>
#include <stdio.h>

class QScrollAreaExt : public QScrollArea
{
   Q_OBJECT

public:
   QScrollAreaExt(QWidget * parent = 0) : QScrollArea(parent) {}
protected:
    //virtual void paintEvent(QPaintEvent *event);
   virtual void focusInEvent ( QFocusEvent * event ) {printf("QScrollAreaExt focus in %p\n",this);}
   virtual void focusOutEvent ( QFocusEvent * event ) {printf("QScrollAreaExt focus out %p\n",this);}
   //virtual void wheelEvent(QWheelEvent *evt) {printf("QScrollAreaExt::wheelEvent\n"); QScrollArea::wheelEvent(evt);}

   virtual void wheelEvent(QWheelEvent *evt) {printf("QScrollAreaExt::wheelEvent %p\n",this);}    // This is important: it avoids the wheel event to be dispatched to the scrollbar, and thus the scrollarea to scroll when the mouse is in the scope widget

   virtual bool eventFilter(QObject *o, QEvent *e) {printf("QScrollAreaExt::eventFilter %p. Object %p eventtype %d\n",this,o,e->type());
      //if(e->type()==QEvent::FocusOut) {printf("filtering out focusout\n"); return true;}
      //if(e->type()==QEvent::FocusIn) {printf("filtering out focusin\n"); return true;}
      return QScrollArea::eventFilter(o,e);
   }


};


/******************************************************************************
   QSliderExt
*******************************************************************************

   QSliderExt extends QSlider by providing slots to programmatically control the scrolling.

******************************************************************************/
class QSliderExt : public QSlider
{
   Q_OBJECT

public:
   QSliderExt(QWidget * parent = 0);
   QSliderExt(Qt::Orientation orientation, QWidget * parent = 0);
   ~QSliderExt();

private slots:
   virtual void scrollLeft();
   virtual void scrollRight();


};


#endif // GUIHELPER_H
