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

#ifndef __WidgetScatter_H
#define __WidgetScatter_H

#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>

#include "PlotWidgetBase.h"
#include "PlotScatter.h"



class WidgetScatter : public PlotWidgetBase
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    WidgetScatter(QWidget *parent=0);
    virtual ~WidgetScatter();

//    virtual PlotBase *GetPlot();
    virtual PlotScatter *GetPlot();

    //virtual void SetTitle(std::string &t);
    //virtual void SetSampleOffset(int value);
    //virtual void SetRangePoint(int value);
    //virtual void SetData(DataBase *data);       // !Must pass a DataScatter type here
    //virtual void Plot();
    //virtual void SetAutoscale(bool as);
    //virtual void SetScale(int xmin,int ymin,int xmax,int ymax);

    //virtual QImage *GetImage();

protected:
    PlotScatter plot;

   virtual void paintEvent(QPaintEvent *event);

   virtual void resizeEvent(QResizeEvent *event);


   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );




signals:
   void Repaint();
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};



#endif // __WidgetScatter_H
