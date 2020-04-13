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

#ifndef __SSVIEWSTICKMAN_H
#define __SSVIEWSTICKMAN_H


#include <QWidget>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <vector>
#include "guihelper.h"
#include "SSViewAbstract.h"
//#include "WidgetStickman.h"
#include "NPlotWidgetStickman.h"


/******************************************************************************
   SSViewStickmanConfig
*******************************************************************************
   Configuration for SSViewStickman

   Each data_xxx should contain 4 entries for the quaternions of the joints.
   If the quaternions are not available, it should contain 4 null pointers.

   Besides this, all the data entries must have the same length and be synchronized (this is the typical case when the data has been acquired with a mocap system).

******************************************************************************/
typedef struct
{
   std::vector<std::vector<int> *> data_torso;
   std::vector<std::vector<int> *> data_lua;
   std::vector<std::vector<int> *> data_lla;
   std::vector<std::vector<int> *> data_rua;
   std::vector<std::vector<int> *> data_rla;

} SSViewStickmanConfig;


/******************************************************************************
   SSViewStickman
*******************************************************************************

   Widget presenting a signal view, with the associated controls (scrollbar, scopes, etc)

******************************************************************************/
class SSViewStickman : public SSViewAbstract
{
   Q_OBJECT
public:
    SSViewStickman(QWidget *parent = 0);
    virtual ~SSViewStickman();

    void setConfig(SSViewStickmanConfig c);
    virtual void lengthChanged();
    virtual void setTime(int t);
    virtual int getTime();
    virtual QString name();

protected:
    // GUI elements
    //WidgetStickman *stickman;
    NPlotWidgetStickman *plotwidget;
    QSliderExt *slider;
    QVBoxLayout *box;

    QString file;


    SSViewStickmanConfig config;
    unsigned length;

    //virtual void paintEvent(QPaintEvent *event);

private slots:
   void on_Slider_valueChanged(int value);
   virtual void on_View_mousePressed(Qt::MouseButton button, int samplex);
   virtual void on_View_mouseMoved(int samplex);


/*signals:
   void timeChanged(int value);
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);*/


};


#endif // __SSVIEWSTICKMAN_H
