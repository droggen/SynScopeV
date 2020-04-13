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


#ifndef PLOTLABEL_H
#define PLOTLABEL_H




#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPicture>
#include <QPainter>
#include <QStringList>
#include "Dscope/ScopeBase.h"
#include "PlotBase.h"


class DataLabel : public DataBase
{
public:
   DataLabel() {};
   virtual ~DataLabel() {};

   std::vector<std::vector<int> *> v;
   std::vector<unsigned> color;

};

class PlotLabel : public ScopeBase, public PlotBase
{

public:
    PlotLabel ();
    virtual ~PlotLabel ();

    //virtual void Plot(const std::vector<std::vector<int> *> &v,const std::vector<unsigned> &color);
    virtual void SetTitle(std::string &t);
    virtual unsigned getIdealHeight(int n);
    virtual void SetSubtitle(QStringList &sub);
    virtual void SetSampleOffset(int value);
    virtual void SetData(DataBase *data);       // !Must pass a DataScatter type here
    virtual void Plot();
    virtual void Resize(unsigned _w,unsigned _h);



    virtual const QImage &GetImage();



protected:
    DataLabel data;
    //int sampleoffset;

   int leftsample, rightsample;     // Internally used by Plot and DrawHAxis
   string title;
   QStringList subtitle;
   int textheight;

   void DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text);

};



#endif // PLOTLABEL_H
