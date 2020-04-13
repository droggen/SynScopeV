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


#ifndef __DSCOPEQTWIDGET_H
#define __DSCOPEQTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>
#include <valarray>
#include <vector>
#include <map>



#include "DScopeQT.h"




class DScopeQTWidget : public QWidget, public DScopeQT
{
	Q_OBJECT
	

public:
	DScopeQTWidget(unsigned _x,unsigned _y,unsigned _w,unsigned _h,bool _alpha=false,QWidget *parent = 0);
	~DScopeQTWidget();


signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

private:
   //QPixmap pixmap;
   QImage pixmap;


signals:
   void Repaint();
   void panLeft();
   void panRight();

protected:

	// events
   virtual void paintEvent(QPaintEvent *event);
   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );
   virtual void wheelEvent (QWheelEvent * event);
   virtual void resizeEvent(QResizeEvent *event);
   virtual void keyPressEvent(QKeyEvent * event);

   /*virtual void focusInEvent ( QFocusEvent * event );
   virtual void focusOutEvent ( QFocusEvent * event );
   virtual void installEventFilter(QObject *);*/
		

};






#endif
