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
#include "PlotLabel.h"
#include "glhelper.h"
#include <cassert>



PlotLabel::PlotLabel()
   : PlotBase()
   , ScopeBase(2,2)
{

   //pixmap.scaled(size());



   SetVRange(0,10);                    // This isn't used, just to initialize the vertical range
   SetAlignment(true);                 // Align to the center

   //setMinimumSize(2,2);
   //setMouseTracking(true);             // To follow mouse over widget and send signals
   ////setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
   //setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

   painter.begin(&pixmap);
   textheight = painter.fontMetrics().height();
   painter.end();
}
PlotLabel::~PlotLabel()
{

}


void PlotLabel::SetTitle(std::string &t)
{

}
void PlotLabel::SetSubtitle(QStringList &sub)
{
   subtitle=sub;
}

unsigned PlotLabel::getIdealHeight(int n)
{
   // The desired height is
   return textheight+textheight+(textheight+5)*n;
}

void PlotLabel::SetData(DataBase *data)
{
   DataLabel *data2 = dynamic_cast<DataLabel *>(data);
   assert(data2);

   PlotLabel::data = *data2;
}


void PlotLabel::Plot()
{
    printf("PlotLabeL::Plot\n");
    unsigned np;

    //printf("PlotLabeL::Plot. sampleoffset: %d\n",sampleoffset);

    printf("data.v.size: %d\n",data.v.size());
    if(data.v.size())
        printf("data.v[0].size: %d\n",data.v[0]->size());

   if(data.v.size()==0)		// if no data to plot make sure we plot the scope frames and axis, but not the trace.
      np = 0;
   else
      np = data.v[0]->size();


   unsigned nsonscreen;	// number of samples on screen
   if(spp_s==1)
      nsonscreen=w*spp_m;
   else
      nsonscreen=w/spp_m+2;	// round up even if goes out of window




   refsample = offsetsample;
   leftsample = refsample - nsonscreen/2 - 1;
   rightsample = refsample + nsonscreen/2 + 1;

   if(leftsample<0)
      leftsample=0;
   // After setting the refsample,
   // Make sure the rightsample is not larger than the data we have. We must make this AFTER setting refsample (refsample can be larger than the sample we have)
   if(rightsample>np-1)
      rightsample=np-1;

   //printf("leftsample: %d. rightsample: %d. refsample: %d\n",leftsample,rightsample,refsample);

   // Clear the image
   painter.begin(&pixmap);
   painter.fillRect(0,0,pixmap.size().width(),pixmap.size().height(),QColor(0,0,0));

   // Draw the axis
   DrawHGrid(0xFFFFFF,0x3f3f3f,0x7f7f7f,0x7f7f7f);
   // Draw title
   if(title.length())
   {
      painter.setPen(Qt::white);
      painter.drawText(5,textheight,QString(title.c_str()));
   }

   // Draw vertical lines
   QSize s = pixmap.size();
   painter.setPen(Qt::white);
   painter.drawLine(s.width()/2-1,0,s.width()/2-1,s.height()-1);
   painter.drawLine(s.width()/2+1,0,s.width()/2+1,s.height()-1);




   // Plot the labels
   int lastvalue,lastsample;
   // Iterate all the samples
   for(unsigned j=0;j<data.v.size();j++)
   {
      int y1,y2,yt;
      y1 = 1+textheight+(textheight+5)*j;
      y2 = y1+textheight+2;
      yt = y2-1;
      painter.setPen(QColor((data.color[j]>>16)&0xff,(data.color[j]>>8)&0xff,data.color[j]&0xff));


      // Set the label title.
      if(j<subtitle.size())
         painter.drawText(5,yt,subtitle[j]);

      for(int sample = leftsample;sample<=rightsample;sample++)
      {
         int x1,x2;

         x1 = s2x(lastsample);
         x2 = s2x(sample);



         if(sample==leftsample)           // Initialization upon first call
         {
            lastvalue = data.v[j]->operator[](sample);
            lastsample=sample;
            painter.drawLine(s2x(sample),y1,s2x(sample),y2);

         }
         else
         {
            // If the current sample is different from the past one OR we're at the last sample, then plot a line
            if(data.v[j]->operator[](sample) != lastvalue || sample==rightsample)
            {

               char s[256];
               //sprintf(s,"%d: %d",lastsample,v[j]->operator[](lastsample));
               sprintf(s,"%d",data.v[j]->operator[](lastsample));

               QRect br = painter.fontMetrics().boundingRect(s);
               //printf("br: %d,%d. %dx%d. %dx%d\n",br.x(),br.y(),br.width(),br.height(),painter.fontMetrics().width(s),painter.fontMetrics().height());



               painter.drawLine(x2,y1,x2,y2);
               painter.drawLine(x1,y1,x2,y1);
               painter.drawLine(x1,y2,x2,y2);

               int tw = painter.fontMetrics().width(s);

               // Draw text only if enough space (10% margin)
               if(tw<(x2-x1)*9/10)
                  painter.drawText((x1+x2-tw)/2,yt,s);

               // Keep the last vertical boundary
               lastvalue = data.v[j]->operator[](sample);
               lastsample = sample;
            }
         }
      }
   }

   painter.end();

}

void PlotLabel::Resize(unsigned _w,unsigned _h)
{
    pixmap = pixmap.scaled(QSize(_w,_h));
    ScopeBase::SetSize(_w,_h);
    printf("PlotLabel: Image size: %d %d\n",pixmap.size().width(),pixmap.size().height());
}


void PlotLabel::SetSampleOffset(int value)
{
    ScopeBase::SetSampleOffset(value);
   //sampleoffset = value;
}

const QImage &PlotLabel::GetImage()
{
    printf("PlotLabel::GetImage: Image size: %d %d\n",pixmap.size().width(),pixmap.size().height());
    return pixmap;
}


void PlotLabel::DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text)
{
   // Draw
   int factor,power;
   int xs = XsTickSpacing(factor,power);

   // Original (right-align only) version:...
   int i=0;

   for(i=leftsample/xs-1;i<rightsample/xs+1; i++)
   {
      int posx = s2x(i*xs);
      if(posx<0 || posx>=w)
         continue;

      //printf("XsTickSpacing at sample i: %d i*xs: %d. Unit at pos: %d\n",i,i*xs,s2x(i*xs));
      // Get the color
      unsigned color = fcolor_minor;
      if(((i*factor)%10)==0)
         color=fcolor_major;
      if(i==0)
         color=fcolor_axis;

      painter.setPen(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
      painter.drawLine(s2x(i*xs),0,s2x(i*xs),pixmap.height()-1);




      char s[256];
      //sprintf(s,"%d",-i*xs);
      //if(gridcenter==false)
//         itoa(-i*xs,s,10);
  //    else
         itoa(i*xs,s,10);
      //if(strlen(s)>5)
         //sprintf(s,"%dE%d",-factor*i,power);
      int sx = painter.fontMetrics().width(s);
      int xpos = s2x(i*xs)-sx/2;
      if(xpos+sx>=w) xpos=w-sx-1;
      if(xpos<0) xpos=0;
      int ypos=pixmap.height()-2;


      painter.drawText(xpos,ypos,s);

   }
}


