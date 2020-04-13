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

#include <QPainter>
#include <QPaintEvent>
#include <assert.h>
#include "SSViewAbstract.h"

//SSViewAbstract::SSViewAbstract(QWidget *parent) : QFrame(parent)
SSViewAbstract::SSViewAbstract(QWidget *parent) : QWidget(parent)
{
    NoSliderEvents = false;

    // Frame
    //setFrameStyle(QFrame::Panel);
    //setFrameShadow(QFrame::Raised);
    setAutoFillBackground(true);

    setMouseTracking(true);             // To follow mouse over widget and send signals
    //setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
    setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.


    bgcolors = {QColor(0,0,0),QColor(64,64,64),
                QColor(0,0,127),QColor(0,0,223),
                QColor(127,0,127),QColor(223,0,223)
               };

    setBackground(SSVIEWBACKGROUNDNONE);

    //installEventFilter(this);

}

SSViewAbstract::~SSViewAbstract()
{

}

//void SSViewAbstract::setBackground(QColor color)
void SSViewAbstract::setBackground(unsigned colorid)
{
    assert(colorid<bgcolors.size());
    backgroundcolortype = colorid;
    setBackgroundPalette();
}
void SSViewAbstract::setBackgroundPalette()
{
    QColor c;
    if(hasFocus())
        c = bgcolors[backgroundcolortype+1];
    else
        c = bgcolors[backgroundcolortype];
    QPalette p = palette();
    p.setColor(backgroundRole(), c);
    setPalette(p);
}


bool SSViewAbstract::hasFocus() const
{
    return QWidget::hasFocus();
}
/*
void SSViewAbstract::focusInEvent ( QFocusEvent * event )
{
   QWidget::focusInEvent(event);
   printf("SSViewAbstract::focusInEvent %p\n",this);
   repaint();

}

void SSViewAbstract::focusOutEvent ( QFocusEvent * event )
{
   QWidget::focusInEvent(event);
   printf("SSViewAbstract::focusOutEvent %p\n",this);
   repaint();

}*/

void SSViewAbstract::paintEvent(QPaintEvent *event)
{
   static int i=0;
   i++;
   printf("SSViewAbstract::paintEvent %p %d\n",this,i);
    QColor b;        // Adapted background color: lighter if focused
    setBackgroundPalette();

    //QWidget::paintEvent(event);

    // Draw a frame if we have the focus
    QPainter p(this);
    p.setPen(Qt::white);
    p.drawRect(0,0,width()-1,height()-1);




    if(hasFocus())
    {
        QPen pen;
        pen.setColor(Qt::black);
        pen.setStyle(Qt::DotLine);
        p.setPen(pen);
        p.drawRect(0,0,width()-1,height()-1);
    }
    else
    {
    }



}

// This event handler must be applied to child widgets to capture their focus change.
// Use this to trigger a repaint of the owner widget
bool SSViewAbstract::eventFilter(QObject* obj, QEvent* evt)
{
    //printf("SSViewAbstract::eventFilter obj %p type %d\n",obj,evt->type());
    if(evt->type()==QEvent::FocusIn)
    {
      //  printf("+++++++  FOCUS IN\n");
        //printf("Focus widget: %p\n",focusWidget());
        update();
        //return true;
    }
    if(evt->type()==QEvent::FocusOut)
    {
        //printf("------- FOCUS OUT\n");
        //printf("Focus widget: %p\n",focusWidget());
        update();
        //return true;
    }
    return false;
}

