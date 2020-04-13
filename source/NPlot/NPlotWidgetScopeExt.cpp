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

#include "NPlotWidgetScopeExt.h"

#include <stdio.h>

NPlotWidgetScopeExt::NPlotWidgetScopeExt(QWidget *parent, Qt::WindowFlags f)
      : NPlotWidgetScope(parent,f)
{
}

//void NPlotWidgetScopeExt::plot()
void NPlotWidgetScopeExt::paintEvent(QPaintEvent *event)
{
    NPlotWidgetScope::paintEvent(event);
    QPainter painter(this);
    QSize s = size();
    painter.setPen(Qt::white);
    painter.drawLine(s.width()/2-1,0,s.width()/2-1,s.height()-1);
    painter.drawLine(s.width()/2+1,0,s.width()/2+1,s.height()-1);
    event->accept();
}



