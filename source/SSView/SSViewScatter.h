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


#ifndef __SSVIEWSCATTER_H
#define __SSVIEWSCATTER_H



#include <QWidget>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <vector>
#include <string>
#include "guihelper.h"
#include "SSViewAbstract.h"
//#include "WidgetScatter.h"
#include "NPlotWidgetScatter.h"


/******************************************************************************
   SSViewScatterConfig
*******************************************************************************
   Configuration for SSViewScatter

   Each data_xxx should contain 4 entries for the quaternions of the joints.
   If the quaternions are not available, it should contain 4 null pointers.

   Besides this, all the data entries must have the same length and be synchronized (this is the typical case when the data has been acquired with a mocap system).

******************************************************************************/
typedef struct
{
    // Scope config
    NPLOTWIDGETSCATTERCONFIG config;

   // Pointers to the vectors containing the x and y traces
   std::vector<std::vector<int> *> data_x;
   std::vector<std::vector<int> *> data_y;


} SSViewScatterConfig;


/******************************************************************************
   SSViewScatter
*******************************************************************************

   Widget presenting a signal view, with the associated controls (scrollbar, scopes, etc)

******************************************************************************/
class SSViewScatter : public SSViewAbstract
{
   Q_OBJECT
public:
    SSViewScatter(QWidget *parent = 0);
    virtual ~SSViewScatter();

    void setConfig(SSViewScatterConfig c);
    virtual void lengthChanged();
    virtual void setTime(int t);
    virtual int getTime();
    virtual QString name();
    virtual bool hasFocus () const;

protected:
    // GUI elements
   //WidgetScatter *scatter;
    NPlotWidgetScatter *plotwidget;
   QSliderExt *slider;
   QVBoxLayout *box;

   SSViewScatterConfig config;
   unsigned length;


private slots:
   void on_Slider_valueChanged(int value);
   virtual void on_View_mousePressed(Qt::MouseButton button, int samplex);
   virtual void on_View_mouseMoved(int samplex);


/*signals:
   void timeChanged(int value);
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);*/


};




#endif // __SSVIEWSCATTER_H
