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

#ifndef __SSVIEWSCOPE_H
#define __SSVIEWSCOPE_H

#include <QWidget>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <vector>
#include "SSViewAbstract.h"
#include "guihelper.h"
#include "NPlotWidgetScopeExt.h"


/******************************************************************************
   SSViewScopeConfig
*******************************************************************************

   Configuration for SSViewScope

******************************************************************************/
typedef struct
{
   string title;
   vector<unsigned> colors;
   int xscale;
   vector<int> yscale;
   bool yauto;

   std::vector<std::vector<int> *> data;

} SSViewScopeConfig;


/******************************************************************************
   SSViewScope
*******************************************************************************

   Widget presenting a signal view, with the associated controls (scrollbar, scopes, etc)

******************************************************************************/
class SSViewScope : public SSViewAbstract
{
   Q_OBJECT
public:
    SSViewScope(QWidget *parent = 0);
    virtual ~SSViewScope();

    virtual void setConfig(SSViewScopeConfig c);
    virtual void setTime(int t);
    virtual int getTime();
    virtual void lengthChanged();
    virtual QString name();

protected:
    // GUI elements
    NPlotWidgetScopeExt *plotwidget;
    QSliderExt *slider;
    QVBoxLayout *box;
    SSViewScopeConfig config;

private slots:
   virtual void on_Repaint();
   virtual void on_Slider_valueChanged(int value);
   virtual void on_View_mousePressed(Qt::MouseButton button, int samplex);
   virtual void on_View_mouseMoved(int samplex);


signals:
   void timeChanged(int value);
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);


};

#endif // __SSViewScope_H
