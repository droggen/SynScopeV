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

#ifndef __WidgetStickman_H
#define __WidgetStickman_H

#include <QGLWidget>
#include "PlotWidgetBase.h"
#include "PlotStickman.h"




//class WidgetStickman : public PlotGLWidgetBase
class WidgetStickman : public PlotWidgetBase
{
    Q_OBJECT        // must include this if you use Qt signals/slots

public:
    WidgetStickman(QWidget *parent=0);
    virtual ~WidgetStickman();

    virtual PlotStickman *GetPlot();



protected:
    PlotStickman plot;
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );



signals:
    void Repaint();
    void mousePressed(Qt::MouseButton button,int samplex);
    void mouseMoved(int samplex);

};


#endif // __WidgetStickman_H
