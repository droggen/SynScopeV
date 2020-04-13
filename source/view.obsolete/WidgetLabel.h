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


#ifndef __WidgetLabel_H
#define __WidgetLabel_H


#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>
#include <QStringList>
#include "Dscope/ScopeBase.h"
#include "PlotLabel.h"
#include "PlotWidgetBase.h"

class WidgetLabel : public PlotWidgetBase
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    WidgetLabel (QWidget *parent=0);
    virtual ~WidgetLabel ();


    virtual PlotLabel *GetPlot();


protected:
    PlotLabel plot;

   virtual void paintEvent(QPaintEvent *event);
   virtual void wheelEvent (QWheelEvent * event);
   virtual void resizeEvent(QResizeEvent *event);
   virtual void keyPressEvent(QKeyEvent * event);


   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );


signals:
   void Repaint();
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);
   void panLeft();
   void panRight();

};


#endif // __WidgetLabel_H
