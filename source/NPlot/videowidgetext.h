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

#ifndef VIDEOWIDGETEXT_H
#define VIDEOWIDGETEXT_H

#include <Phonon/VideoWidget>
#include <QObject>
#include <QWidget>

class VideoWidgetExt : public Phonon::VideoWidget
{
   Q_OBJECT
public:
    VideoWidgetExt(QWidget *parent=0);

    void DoClear();

    virtual bool hasFocus() const;

signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);




protected:
   bool focus;


   virtual void mousePressEvent ( QMouseEvent * event );
   virtual void mouseMoveEvent ( QMouseEvent * event );

   virtual void paintEvent(QPaintEvent *event);
   virtual void focusInEvent ( QFocusEvent * event );
   virtual void focusOutEvent ( QFocusEvent * event );
};

#endif // VIDEOWIDGETEXT_H
