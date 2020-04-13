/*
   Scope - Fast digital oscilloscope

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
/*
   Version:
   1.2 - 2009-08-08:
         + Set signal alignment: right or center.
         + Set signal offset
   1.1 - 2008-08-31
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ScopeBase.h"
#include "Scope.h"

#include <QtDebug>
 #include <QThread>
//#define printf qDebug 


/******************************************************************************
******************************************************************************/

void itoa(int val, char *str)
{
	static char num[] = "0123456789";
	int sign=0;
	char aux;
	char *wstr=str;
	if(val<0)
	{
		val=-val;
		sign=1;		
	}
	
	do 
	{
		*wstr++ = num[val%10]; 
	}
	while(val/=10);
	
	if(sign) *wstr++='-';
	*wstr=0;	
	// Reverse string
	wstr--;
	while(wstr>str)
		aux=*wstr, *wstr--=*str, *str++=aux;
}


Scope::Scope(unsigned _x,unsigned _y,unsigned _w,unsigned _h)
   : title(""),
   ScopeBase(_w,_h)
{
   Resize(_x,_y,_w,_h);

   highlight=false;
   EnableDrawHGrid=true;
   EnableDrawVGrid=true;
   EnableFrame=true;
   gridcenter=false;
   RightAlignLast=true;
	

}
Scope::~Scope()
{
	
}
void Scope::Resize(unsigned _x,unsigned _y,unsigned _w,unsigned _h)
{
	basex=_x;
	basey=_y;
	w=_w;
	h=_h;	
   SetSize(_w,_h);
}






void Scope::Highlight()
{
	highlight=true;
}
void Scope::NoHighlight()
{
	highlight=false;
}




void Scope::Plot(vector<int> &data,unsigned color)
{	
	vector<vector<int> *> v;
	v.push_back(&data);
	vector<unsigned> vc;
	vc.push_back(color);
	Plot(v,vc);
}

void Scope::Plot(const vector<vector<int> *> &vv,const vector<unsigned> &color)
{
   //printf("::Plot %p - center is %d\n",this,center);

	cleararea();


	


	if(vv.size()==0)		// if no data to plot make sure we plot the scope frames and axis, but not the trace.
		np = 0;
	else		
		np = vv[0]->size();

	
	unsigned nsonscreen;	// number of samples on screen
	if(spp_s==1)
		nsonscreen=w*spp_m;
	else
		nsonscreen=w/spp_m+2;	// round up even if goes out of window
   //printf("w: %d, spp_s: %d spp_m: %d, nsonscreen: %d\n",w,spp_s, spp_m,nsonscreen);

   //if(np<nsonscreen)
//		nsonscreen=np;

   if(center==false)
   {
      //printf("Right align.");
      if(RightAlignLast)
      {
         // In the align-right mode, the rightmost sample corresponds to the last (i.e. most recent) sample.
         rightsample=np-1;    // rightmost sample to display (should be rounded up).
      }
      else
      {
         rightsample = offsetsample;      // rightmost sample to display is the user provided sample.

      }
      leftsample = rightsample-nsonscreen+1; // leftmost sample to display (should be rounded down)

      refsample=rightsample;
   }
   else
   {
      //printf("Center. Refsample: %d - ", refsample);
      // In the center mode, distributes equally the samples on the left and right of nsamples
      refsample=offsetsample;
      leftsample = refsample - nsonscreen/2 - 1;
      rightsample = refsample + nsonscreen/2 + 1;
   }
   if(leftsample<0)
      leftsample=0;
   // After setting the refsample,
   // Make sure the rightsample is not larger than the data we have. We must make this AFTER setting refsample (refsample can be larger than the sample we have)
   if(rightsample>np-1)
      rightsample=np-1;

   /*printf("np: %d - leftsample: %d - rightsample: %d\n\n",np,leftsample,rightsample);
   printf("\n");
   printf("Conversion to pixels: s2x(leftsample): %d - s2x(rightsample): %d\n",s2x(leftsample),s2x(rightsample));*/

	if(VAuto)
	{
		if(np==0)
			vmax=vmin=0;
		else
		{
         //vmax=vmin=vv[0]->operator[](np-1);
         vmax=vmin=vv[0]->operator[](leftsample);
			// Find the max/min and do autoscale.
			for(unsigned iv=0;iv<vv.size();iv++)
			{
            //for(int s=0;s<nsonscreen;s++)
            for(int s=leftsample;s<=rightsample;s++)
				{
               //int value = vv[iv]->operator[](np-1-s);
               int value = vv[iv]->operator[](s);
					if(value<vmin)
						vmin=value;
					if(value>vmax)
						vmax=value;
				}
			}
		}
		if(vmax==vmin)	{ vmax++; vmin--;}	
		vrange2vpp(vmin,vmax,vpp,vpp_s);
	}
	
	
	
	fastStart();
	unsigned fcolor_axis = fastColor(ColorWhite());
	unsigned fcolor_minor = fastColor(ColorLGrey());
	unsigned fcolor_major = fastColor(ColorGrey());
	unsigned fcolor_text = fastColor(ColorGrey());
	unsigned fcolor_border;
	if(highlight)
		fcolor_border=fastColor(ColorRed());
	else
		fcolor_border = fcolor_axis;
	
   if(EnableDrawHGrid)
      DrawHGrid(fcolor_axis,fcolor_minor,fcolor_major,fcolor_text);
   if(EnableDrawVGrid)
      DrawVGrid(fcolor_axis,fcolor_minor,fcolor_major,fcolor_text);
	
	
	
	
	for(unsigned ni=0;ni<vv.size();ni++)
	{
		unsigned fcolor = fastColor(color[ni]);
		int lastx,lasty;
		const vector<int> &v = *vv[ni];
      //for(int s=0;s<nsonscreen;s++)
      int s;
      bool first;
      for(s=rightsample,first=true;s>=leftsample;s--)
		{
         //int value = v[np-1-s];
         //int value=v[rightsample-s];
         int value=v[s];
							
         //if(s==0)
         if(first)
			{
            first=false;
            //lastx=s2x(s);
            //lastx=s2x(rightsample-s);
            lastx=s2x(s);
				lasty=v2y(value);
			}
			else
			{
            //int newx=s2x(s);
            //int newx=s2x(rightsample-s);
            int newx=s2x(s);
				int newy=v2y(value);
				// Comments: Nokia N800
				// single diagonal line (48fps) is faster than 2 lineColor (33fps), and only slightly faster than vLineColor and hLineColor (43fps)
				//lineColor(lastx,lasty,newx,newy,color);
				//lineColor(lastx,lasty,lastx,newy,color);
				//lineColor(lastx,newy,newx,newy,color);
				//vLineColor(lastx,lasty+ni,newy,color);		// beware: bug in SDL_gfx 2.0.16 and earlier
				//hLineColor(lastx,newy+ni,newx,color);		// beware: bug in SDL_gfx 2.0.16 and earlier
				
				
				
				fastvLineColor(lastx,lasty,newy,fcolor);
				if(spp_s==0 )
					fasthLineColor(lastx,newy,newx,fcolor);
					
				
				if(spp_s==0 )//&& spp_m>2)
				{
					fastPixelColor(newx+1,newy+1,fcolor_axis);
					fastPixelColor(newx+1,newy-1,fcolor_axis);
					fastPixelColor(newx-1,newy+1,fcolor_axis);
					fastPixelColor(newx-1,newy-1,fcolor_axis);
				}
				lastx=newx;
				lasty=newy;
			}
		}
	}

	// Draw frame
   if(EnableFrame)
   {
      fasthLineColor(0,0,w-1,fcolor_border);
      fasthLineColor(0,h-1,w-1,fcolor_border);
      fastvLineColor(0,0,h-1,fcolor_border);
      fastvLineColor(w-1,0,h-1,fcolor_border);
   }
		
	// Draw title
	if(title.length())
	{
		unsigned x=w/10;
		unsigned y=4;
		fastStringColor(x,y,title.c_str(),ColorWhite());
	}
	
	
	fastStop();
	
	
}

void Scope::DrawHGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text)
{
	// Draw
	int factor,power;
	int xs = XsTickSpacing(factor,power);
   //printf("XsTickSpacing. xs: %d. factor: %d. power: %d\n",xs,factor,power);
	
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
		
		//lineColor(s2x(i*xs),0,s2x(i*xs),h,color);
      //fastvLineColor(s2x(i*xs),0,h-1,color);
      //fastvLineColor(s2x(rightsample-i*xs),0,h-1,color);
      fastvLineColor(s2x(i*xs),0,h-1,color);
		//fastvLineColor(s2x(i*xs),0,h,color);
		

		char s[256];
		//sprintf(s,"%d",-i*xs);
      if(gridcenter==false)
         //itoa(i*xs-refsample,s);
         itoa(-i*xs,s);
      else
         itoa(i*xs,s);
		//if(strlen(s)>5)
			//sprintf(s,"%dE%d",-factor*i,power);
		int sx = strlen(s)*8;
		int xpos = s2x(i*xs)-strlen(s)*4;
      if(xpos+sx>=w) xpos=w-sx-1;
		if(xpos<0) xpos=0;
		int ypos=v2y(0)-8;
		if(ypos<0) ypos=0;
		if(ypos>=h) ypos=h-1-8;
		fastStringColor(xpos,ypos,s,fcolor_text);

	}	
}

void Scope::DrawVGrid(unsigned fcolor_axis,unsigned fcolor_minor,unsigned fcolor_major,unsigned fcolor_text)
{
	// Draw
	
	int factor,power;
	int yv = YvTickSpacing(factor,power);
	//int yv=50;
	//factor=5; power=1;
	//printf("YvTickSpacing: %u, factor: %u\n",yv,factor);
	
	int i;
	// Start from bottom
	if(vpp_s==1)
		i = (vmax-h*vpp)/yv;
	else
		i = (vmax-h/vpp)/yv;
	//printf("vmax/min: %d %d. h: %d. vpp:%d vpp_m:%d vpp_s:%d i:%d yv:%d v2y(i*yv):%d\n",vmax,vmin,h,vpp,vpp_m,vpp_s,i,yv,v2y(i*yv));
	while(v2y(i*yv)>=0)
	{
		// Get the color
		unsigned color = fcolor_minor;
		if(((i*factor)%10)==0)
			color=fcolor_major;
		if(i==0)
			color=fcolor_axis;
		//lineColor(0,v2y(i*yv),w,v2y(i*yv),color);
		fasthLineColor(0,v2y(i*yv),w-1,color);
		//fasthLineColor(0,v2y(i*yv),w,color);
		
		// Draw the text here
		
		char s[256];
		//sprintf(s,"%d",i*yv);
		itoa(i*yv,s);		// slightly faster
		fastStringColor(0,v2y(i*yv)-4,s,fcolor_text);
		
		
		
		i++;
	}
	


}


void Scope::SetTitle(const string &t)
{
	title=t;
}




/**
  \brief Indicates whether the grid origin must be on the right of the scope, or centered.

**/
void Scope::SetGridPosition(bool center)
{
   gridcenter=center;
}

/**
  \brief In the non-center mode, indicates whether the rightmost sample is the last one of the display vector (default for a scope)
      or the user provided offset (e.g. to make a scrolling scope).

**/
void Scope::SetRightAlignLast(bool _ralign)
{
   RightAlignLast = _ralign;
}

void Scope::SetEnableVGrid(bool e)
{
   EnableDrawVGrid = e;
}
void Scope::SetEnableHGrid(bool e)
{
   EnableDrawHGrid = e;
}
void Scope::SetEnableFrame(bool e)
{
   EnableFrame = e;
}



