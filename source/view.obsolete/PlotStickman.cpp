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

#include <stdio.h>
#include <QMouseEvent>
#include <cassert>
#include "PlotStickman.h"
#include "glhelper.h"





/******************************************************************************
*******************************************************************************
PlotStickman   PlotStickman   PlotStickman   PlotStickman   PlotStickman
*******************************************************************************
******************************************************************************/


PlotStickman::PlotStickman()
    : PlotBase()
{
    sampleoffset=0;

}
PlotStickman::~PlotStickman()
{

}






void PlotStickman::Resize(unsigned _w,unsigned _h)
{
    pixmap = pixmap.scaled(QSize(_w,_h));
    printf("PlotScatter: Image size: %d %d\n",pixmap.size().width(),pixmap.size().height());
    gl.resize(_w,_h);

}



void PlotStickman::SetTitle(std::string &t)
{

}

void PlotStickman::SetData(DataBase *data)
{
   DataStickman *data2 = dynamic_cast<DataStickman *>(data);
   assert(data2);

   PlotStickman::data = *data2;
}

void PlotStickman::Plot()
{
    printf("PlotStickman::Plot\n");

    // Quaternions for the joints.
    std::vector<float> q_torso={1,0,0,0};
    std::vector<float> q_lua={1,0,0,0};
    std::vector<float> q_lla={1,0,0,0};
    std::vector<float> q_rua={1,0,0,0};
    std::vector<float> q_rla={1,0,0,0};

    // Get the data at sampleoffset
    for(unsigned i=0;i<4;i++)
    {
       if(sampleoffset<data.torsorot[i]->size())
          q_torso[i] = data.torsorot[i]->operator[](sampleoffset)/1000.0;
       else
          q_torso[i] = 0;
       if(sampleoffset<data.luarot[i]->size())
          q_lua[i] = data.luarot[i]->operator[](sampleoffset)/1000.0;
       else
          q_lua[i] = 0;
       if(sampleoffset<data.llarot[i]->size())
          q_lla[i] = data.llarot[i]->operator[](sampleoffset)/1000.0;
       else
          q_lla[i] = 0;
       if(sampleoffset<data.ruarot[i]->size())
          q_rua[i] = data.ruarot[i]->operator[](sampleoffset)/1000.0;
       else
          q_rua[i] = 0;
       if(sampleoffset<data.rlarot[i]->size())
          q_rla[i] = data.rlarot[i]->operator[](sampleoffset)/1000.0;
       else
          q_rla[i] = 0;
    }

    // Do the rendering using the private class
    gl.Plot(q_torso,q_lua,q_lla,q_rua,q_rla);

    // Get the image
    pixmap = gl.GetImage();
    printf("gl.GetImage() size: %d %d\n",pixmap.width(),pixmap.height());

}

const QImage &PlotStickman::GetImage()
{
    printf("PlotStickman::GetImage\n");
    printf("Pixmap: %d %d\n",pixmap.width(),pixmap.height());

    // Return some other pixmap
    //pixmap = QImage(pixmap.size(),QImage::Format_RGB32);

    return pixmap;
}
void PlotStickman::SetSampleOffset(int value)
{
   sampleoffset = value;
}




