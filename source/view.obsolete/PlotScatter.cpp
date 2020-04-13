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
#include "PlotScatter.h".h"
#include "glhelper.h"



PlotScatter::PlotScatter()
    : PlotBase()
{
   /*printf("Constructor of PlotScatter\n");
   printf("Do we have a size?\n");
   printf("%d %d\n",size().width(),size().height());
   printf("size of pixmap: %d %d\n",pixmap.size().width(),pixmap.size().height());*/

    //pixmap.scaled(QSize(100,100));

    sampleoffset=0;
    rangepoint=100;

    minspace=64;                        // Minimum pixel spacing between ticks


}
PlotScatter::~PlotScatter()
{

}






void PlotScatter::Resize(unsigned _w,unsigned _h)
{
    pixmap = pixmap.scaled(QSize(_w,_h));
    //printf("PlotScatter: Image size: %d %d\n",pixmap.size().width(),pixmap.size().height());
}



void PlotScatter::SetTitle(std::string &t)
{

}

void PlotScatter::SetData(DataBase *data)
{
   DataScatter *data2 = dynamic_cast<DataScatter *>(data);
   assert(data2);

   PlotScatter::data = *data2;
}

void PlotScatter::Plot()
{
    //printf("PlotScatter::Plot\n");

   //printf("PlotScatter::Plot. x.size: %d. y.size: %d. color.size: %d\n",data.x.size(),data.y.size(),data.color.size());
   //printf("sampleoffset: %d\n",sampleoffset);

   //printf("PlotScatter: Image size: %d %d\n",pixmap.size().width(),pixmap.size().height());
   // Clear the image
   if(!painter.begin(&pixmap))
       printf("Painter begin failed\n");
   painter.fillRect(0,0,pixmap.size().width(),pixmap.size().height(),QColor(0,0,0));

   // Autoscale
   if(autoscale)
   {
      bool first=true;
      // Iterate the traces
      for(unsigned i=0;i<data.x.size();i++)
      {
         for(unsigned j=0;j<data.x[i]->size();j++)
         {
            if(first)
            {
               esxmin=esxmax = data.x[i]->operator[](j);
               esymin=esymax = data.y[i]->operator[](j);

               first=false;
            }
            else
            {
               if(data.x[i]->operator[](j)<esxmin)
                  esxmin = data.x[i]->operator[](j);
               if(data.x[i]->operator[](j)>esxmax)
                  esxmax = data.x[i]->operator[](j);
               if(data.y[i]->operator[](j)<esxmin)
                  esymin = data.y[i]->operator[](j);
               if(data.y[i]->operator[](j)>esymax)
                  esymax = data.y[i]->operator[](j);
            }
         }
      }
   }
   else
   {
      esxmin = sxmin;
      esymin = symin;
      esxmax = sxmax;
      esymax = symax;
   }


   // Plot the axis
   /*
   painter.setPen(QColor(255,255,255));
   int yc,xc;
   xc = s2x(0),
   yc = s2y(0),
   painter.drawLine(0,yc,pixmap.size().width()-1,yc);
   painter.drawLine(xc,0,xc,pixmap.size().height()-1);*/


   // Iterate the traces
   for(unsigned i=0;i<data.x.size();i++)
   {
      //printf("Trace %d. color: %X. Numpoints: %d\n",color[i],x[i]->size());
      painter.setPen(QColor((data.color[i]>>16)&0xff,(data.color[i]>>8)&0xff,data.color[i]&0xff));
      // Here we should clear the bitmap and draw something in it

      // Draw the trace if there's more than one point
      if(data.x[i]->size())
      {
         int jmin,jmax;
         jmin = sampleoffset-rangepoint;
         //jmax = sampleoffset+rangepoint;          // Show future
         jmax = sampleoffset;                       // Don't show future
         if(jmin<1)
            jmin=1;
         if(jmax>data.x[i]->size()-1)
            jmax=data.x[i]->size()-1;

         //for(unsigned j=1;j<x[i]->size();j++)
         for(int j=jmin;j<jmax;j++)
         {
            painter.drawLine(
                  s2x(data.x[i]->operator[](j-1)),
                  s2y(data.y[i]->operator[](j-1)),
                  s2x(data.x[i]->operator[](j)),
                  s2y(data.y[i]->operator[](j)));
         }
      }
      // Show the location of the centerpoint
      if(sampleoffset>=0 && sampleoffset < data.x[i]->size())
      {
         painter.drawEllipse(
               s2x(data.x[i]->operator[](sampleoffset))-1,
               s2y(data.y[i]->operator[](sampleoffset))-1,
               3,3);
      }
   }

   DrawHGrid(0xFFFFFF,0x3f3f3f,0x7f7f7f,0x7f7f7f);
   DrawVGrid(0xFFFFFF,0x3f3f3f,0x7f7f7f,0x7f7f7f);


   painter.end();
}

/******************************************************************************
   PlotScatter::s2x, s2y
*******************************************************************************
   Converts sample value s to pixel coordinate p, assuming that the minum and
   maximum range of visible samples on screen is [smin;smax] that gets mapped
   to pixels [0;width|height-1]
******************************************************************************/
int PlotScatter::s2x(int s)
{
   int pmin=0;
   int pmax = pixmap.width()-1;

   return (int)((float)pmin+(float)(s-esxmin)/(float)(esxmax-esxmin)*(float)(pmax-pmin));
}
int PlotScatter::s2y(int s)
{
   int pmin=0;
   int pmax = pixmap.height()-1;

   return (int)((float)pmax-(float)(s-esymin)/(float)(esymax-esymin)*(float)(pmax-pmin));
}

void PlotScatter::SetSampleOffset(int value)
{
   sampleoffset = value;
}
void PlotScatter::SetRangePoint(int value)
{
   rangepoint = value;
}

void PlotScatter::SetAutoscale(bool as)
{
   autoscale = as;
}
void PlotScatter::SetScale(int xmin,int ymin,int xmax,int ymax)
{
   sxmin=xmin;
   symin=ymin;
   sxmax=xmax;
   symax=ymax;
}
/*
   Find the smallest N,n, N e [1,2,5]*10^n | N*10^n>=target
   Returns: the smallest value and the factor
*/
int PlotScatter::FindSmallestAbove(int target,int &factor,int &power)
{
   // Divide by 10 until equal 0, find minimum n
   int N,n,t;
   t=target;
   n=0;
   while((t/10)!=0)
   {
      t/=10;
      n++;
   }
   t=1;
   for(int i=0;i<n;i++)
      t*=10;
   // smallest N e [1,2,5] | N > target/10^n

   if(t>=target)
      N=1;		// only useful in case target=1
   else
      if(2*t>=target)
         N=2;
      else
         if(5*t>=target)
            N=5;
         else {N=1;n++;t*=10;}
   factor = N;
   power = n;
   return N*t;
}

/*
   Finds the smallest tick spacing which leads to at least minxspace pixel spacing
   Returns samples spacing that matches the requirements
*/
int PlotScatter::XsTickSpacing(int &factor,int &power)
{
   int target;

   // target is the value corresponding to minspace pixels
   target = (int)((float)minspace*(float)(esxmax-esxmin)/(float)(pixmap.width()-1));


   return FindSmallestAbove(target,factor,power);
}
/*
   Finds the smallest vertical tick spacing which leads to at least minyspace pixel spacing
   Return samples spacing that matches the requirements.
*/
int PlotScatter::YvTickSpacing(int &factor,int &power)
{
   int target;
   target = (int)((float)minspace*(float)(esymax-esymin)/(float)(pixmap.height()-1));

   return FindSmallestAbove(target,factor,power);
}
void PlotScatter::DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text)
{
   // Draw
   int factor,power;
   int xs = XsTickSpacing(factor,power);
   //printf("XsTickSpacing. xs: %d. factor: %d. power: %d\n",xs,factor,power);

   // Original (right-align only) version:...
   int i=0;

   for(i=esxmin/xs-1;i<esxmax/xs+1; i++)
   {
      int posx = s2x(i*xs);
      if(posx<0 || posx>=pixmap.size().width())
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
      sprintf(s,"%d",i*xs);
      QRect br = painter.fontMetrics().boundingRect(s);
      //printf("Bounding rect: %d %d %d %d\n",br.x(),br.y(),br.width(),br.height());
      int sx = br.width();
      int xpos = s2x(i*xs)-sx/2;
      if(xpos+sx>=pixmap.width()) xpos=pixmap.width()-sx-1;
      if(xpos<0) xpos=0;
      //int ypos=s2y(0);
      //if(ypos<0) ypos=0;
      //if(ypos>=pixmap.height()) ypos=pixmap.height()-1-br.height();
      //painter.drawText(xpos,ypos,s);
      //int ypos = pixmap.height()-1-br.height();
      int ypos = pixmap.height()-1;
      painter.drawText(xpos,ypos,s);
   }
}

void PlotScatter::DrawVGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text)
{
   // Draw

   int factor,power;
   int ys = YvTickSpacing(factor,power);
   //int yv=50;
   //factor=5; power=1;
   //printf("YvTickSpacing: %u, factor: %u\n",ys,factor);



   int i;
   //printf("vmax/min: %d %d. h: %d. vpp:%d vpp_m:%d vpp_s:%d i:%d yv:%d v2y(i*yv):%d\n",vmax,vmin,h,vpp,vpp_m,vpp_s,i,yv,v2y(i*yv));
   for(i=esymin/ys-1;i<esymax/ys+1; i++)
   {
      // Get the color
      unsigned color = fcolor_minor;
      if(((i*factor)%10)==0)
         color=fcolor_major;
      if(i==0)
         color=fcolor_axis;


      //printf("YsTickSpacing at sample i: %d i*ys: %d. Unit at pos: %d\n",i,i*ys,s2y(i*ys));

      painter.setPen(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
      painter.drawLine(0,s2y(i*ys),pixmap.width()-1,s2y(i*ys));


      // Draw the text here

      char s[256];
      sprintf(s,"%d",i*ys);
      painter.drawText(0,s2y(i*ys),s);




   }



}

const QImage &PlotScatter::GetImage()
{
    printf("PlotScatter::GetImage\n");
    return pixmap;
}







