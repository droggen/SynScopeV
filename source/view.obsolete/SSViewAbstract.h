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
#ifndef __SSVIEWABSTRACT_H
#define __SSVIEWABSTRACT_H

#include <QWidget>
#include <QFrame>


class SSViewAbstract : public QFrame
{
   Q_OBJECT
public:
    SSViewAbstract(QWidget *parent = 0);
    virtual ~SSViewAbstract();

    virtual void setTime(int t) = 0;
    virtual int getTime() = 0;
    virtual void lengthChanged() = 0;
    virtual void setBackground(QColor color);

    virtual QString name() = 0;


private:

protected:
    bool NoSliderEvents;            // Slider position changes must not emit a timeChanged signal


    //virtual void paintEvent(QPaintEvent *event) = 0;

private slots:
   virtual void on_View_mousePressed(Qt::MouseButton button, int samplex) = 0;
   virtual void on_View_mouseMoved(int samplex) = 0;


signals:
   void timeChanged(int value);
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);

};

#endif // __SSVIEWABSTRACT_H


