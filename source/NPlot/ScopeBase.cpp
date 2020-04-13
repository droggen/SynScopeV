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



#include <vector>
#include <string>
#include <stdio.h>
#include "ScopeBase.h"





ScopeBase::ScopeBase(unsigned  _w,unsigned  _h)
{
   SetSize(_w,_h);

   minxspace=32;
   minyspace=24;

   VAuto=true;
   vpp=1;
   vpp_m=1;
   vpp_s=1;
   vpp_m_i=0;

   HZoomReset();



   center=false;
   refsample=0;
   offsetsample=0;


   offsetyv=0;


}

ScopeBase::~ScopeBase()
{

}

void ScopeBase::SetSize(int _w,int _h)
{
   w=_w;
   h=_h;
}
void ScopeBase::SetSampleOffset(int co)
{
   offsetsample=co;
}

void ScopeBase::SetAlignment(bool _center)
{
   center=_center;
}

/*
   Find the smallest N,n, N e [1,2,5]*10^n | N*10^n>=target
   Returns: the smallest value and the factor
*/
int ScopeBase::FindSmallestAbove(int target,int &factor,int &power)
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
int ScopeBase::XsTickSpacing(int &factor,int &power)
{
   // Assumes the sample per pixel is >=1, i.e. spp_s==1

   // There is minxspace*spp samples in the minimum space
   // need to find a number of samples for which the pixel spacing is higher than minxspace....
   // This number of samples must be one of [1,2,5]*10^n
   // If spp>1:
   // 	find the smallest N,n, N e [1,2,5]*10^n | N*10^n>minxspace*spp_m
   // if spp<1:
   // 	find the smallest N,n, N e [1,2,5]*10^n | N*10^n>minxspace/spp_m
   // Note: makes no sense to draw sub-sample grid - therefore with extreme zoom no grid is shown
   int target;
   if(spp_s==1)
      target=minxspace*spp_m;
   else
   {
      target=minxspace/spp_m;
      if(minxspace%spp_m!=0)
         target++;
   }
   return FindSmallestAbove(target,factor,power);
}
/*
   Finds the smallest vertical tick spacing which leads to at least minyspace pixel spacing
   Return samples spacing that matches the requirements.
*/
int ScopeBase::YvTickSpacing(int &factor,int &power)
{
   int target;

   if(vpp_s==1)
      target=minyspace*vpp;
   else
   {
      target=minyspace/vpp;
      if(minyspace%vpp!=0)
         target++;
   }
   return FindSmallestAbove(target,factor,power);
}


/**
   \brief converts a sample number into a pixel coordinate.

   s2x has two operating modes: when center is false, the sample 'refsample' is on the rightmost pixel.
   When center is true, the sample 'refsample' is on the center pixel.
   s larger than refsample means the pixel is on the right side of the reference sample.
**/
int ScopeBase::s2x(int s)
{
   if(center==false)
   {
      if(spp_s==0)
         // multiply
         //return w-1-s*spp_m;
         return w-1-(refsample-s)*(int)spp_m;
      else
         //return w-1-s/spp_m;
         return w-1-(refsample-s)/(int)spp_m;
   }
   else
   {
      if(spp_s==0)
         // multiply
         return (w/2)-(refsample-s)*(int)spp_m;
      else
         return (w/2)-(refsample-s)/(int)spp_m;
   }

}
/******************************************************************************
   Scope::v2y
*******************************************************************************
   Converts a sample value into a y pixel coordinate
******************************************************************************/

int ScopeBase::v2y(int v)
{
   if(vpp_s==1)
      //return (vmax-v)/vpp;                             // The very initial version, no centering
      //return (vmax-v)/vpp + (h-(vmax-vmin)/vpp)/2;		// Center by placing [vmax;vmin] at equal distance from border
      return h/2-(v-offsetyv)/vpp;                       // The value offsetyv is at the center
   else
      //return (vmax-v)*vpp;
      //return (vmax-v)*vpp + (h-(vmax-vmin)*vpp)/2;
      return h/2-(v-offsetyv)*vpp;
}
/**
  \brief Converts a pixel coordinate into a sample number
  Mathematical opposite of s2x.
**/
int ScopeBase::x2s(int x)
{
   if(center==false)
   {
      if(spp_s==0)
         return (x-w+1)/(int)spp_m + refsample;
      else
         return (x-w+1)*(int)spp_m + refsample;
   }
   else
   {
      if(spp_s==0)
         return (x-(w/2))/(int)spp_m+refsample;
      else
         return (x-(w/2))*(int)spp_m+refsample;
   }
}



void ScopeBase::vpp2vpp_m()
{
   int factor,power;
   vpp_m=FindSmallestAbove(vpp,factor,power);
   if(vpp_s==1)
   {
      if(factor==1) vpp_m_i=0;
      if(factor==2) vpp_m_i=1;
      if(factor==5) vpp_m_i=2;
   }
   else
   {
      if(factor==1) vpp_m_i=0;
      if(factor==2) vpp_m_i=2;
      if(factor==5) vpp_m_i=1;
   }
   //printf("Closest match to vpp %d: vpp_m: %d vpp_m_i: %d\n",vpp,vpp_m,vpp_m_i);
   vpp=vpp_m;
   //printf("New vpp: %d\n",vpp);
}
void ScopeBase::vrange2vpp(int vmin,int vmax,int &vpp,int &vpp_s)
{
   //printf("%p. ScopeBase::vrange2vpp %d %d %d %d\n",this,vmin,vmax,vpp,vpp_s);
   // Define the vertical values per pixel (vpp)
   if(vmax-vmin>=h-1)
   {
      vpp = (vmax-vmin)/(h-1);
      vpp++;	// fast round up
      vpp_s=1;
   }
   else
   {
      vpp = (h-1)/(vmax-vmin);
      vpp_s=0;
   }
   if(vpp==1)
      vpp_s=1;

   // Compute the vertical offset offsetyv. offsetyv is the value at the center of the scope
   // It is set so that vmax and vmin are equally spaced from the border
   offsetyv = (vmax+vmin)/2;
   //printf("VRange2vpp [%d,%d] -> [%d,%d], offset:%d\n",vmax,vmin,vpp,vpp_s,offsetyv);
}



int ScopeBase::vpp2vmax(int vpp,int vpp_s)
{
   //printf("%p. ScopeBase::vpp2vmax: %d %d\n",this,vpp,vpp_s);
   int vmax;
   if(vpp_s==1)
   {
      vmax = h*vpp/2;
      vmin = -vmax;
   }
   else
   {
      vmax = h/vpp/2;
      vmin = -vmax;
   }
   return vmax;
}


void ScopeBase::HZoomReset()
{
   spp_m=1;
   spp_s=1;		// spp_s=1 -> spp is >=1; spp_s=0 -> spp is <1
   spp_m_i=0;
   spp_zf[0]=4;
   spp_zf[1]=5;
   spp_zf[2]=4;
}
// Zoom in: sample per pixel decreases
void ScopeBase::HZoomin()
{
   if(spp_m==1 && spp_s==1)
      spp_s=0;
   if(spp_m_i==0) spp_m_i=2; else spp_m_i--;
   if(spp_s==0)
      spp_m=(spp_m*spp_zf[spp_m_i])>>1;
   else
      spp_m=(spp_m<<1)/spp_zf[spp_m_i];
   //printf("After zoomin:\n");
   //HZoomPrint();
}
// Zoom out: sample per pixel increases
void ScopeBase::HZoomout()
{
   if(spp_s==0)
      spp_m=(spp_m<<1)/spp_zf[spp_m_i];
   else
      spp_m=(spp_m*spp_zf[spp_m_i])>>1;
   spp_m_i++; if(spp_m_i>2) spp_m_i=0;
   if(spp_m==1 && spp_s==0)
      spp_s=1;
   //printf("After zoomout:\n");
   //HZoomPrint();
}
// Set horizontal zoom factor
void ScopeBase::HZoom(int z)
{
   // Positive z: zoomin. z=10: -> 1/10=0.1 sample per pixel
   // Negative z: zoomout. z=-5: -> 5 sample per pixel
   int factor,power,s;
   //printf("HZoom with z=%d\n",z);
   if(z==0)
      z=-1;
   if(z==1)
      z=-1;
   if(z>0)
   {
      s=FindSmallestAbove(z,factor,power);
      spp_s = 0;
      if(factor==1) spp_m_i = 0;
      if(factor==2) spp_m_i = 2;
      if(factor==5) spp_m_i = 1;
   }
   else
   {
      s=FindSmallestAbove(-z,factor,power);
      spp_s = 1;
      if(factor==1) spp_m_i = 0;
      if(factor==2) spp_m_i = 1;
      if(factor==5) spp_m_i = 2;
   }
   spp_m = s;
   //printf("Smallest above: %d; factor: %d; power: %d\n",s, factor,power);
   //HZoomPrint();
}


void ScopeBase::HZoomPrint()
{
   printf("Zoom: spp_m: %u, spp_m_i: %u, spp_s: %u\n",spp_m,spp_m_i,spp_s);
   printf("Zoom: %lf\n",spp_s==1?((double)spp_m):(1.0/spp_m));
}


void ScopeBase::VZoomPrint()
{
   printf("Zoom:vpp_m: %u, vpp_m_i: %u, vpp_s: %u  vpp: %d\n",vpp_m,vpp_m_i,vpp_s,vpp);
   printf("Zoom: %lf\n",vpp_s==1?((double)vpp_m):(1.0/vpp_m));
}
void ScopeBase::VZoomin()
{
   _setvauto(false);
   vpp2vpp_m();
   vmax=vpp2vmax(vpp,vpp_s);
   if(vpp_m==1 && vpp_s==1)
      vpp_s=0;
   if(vpp_m_i==0) vpp_m_i=2; else vpp_m_i--;
   if(vpp_s==0)
      vpp_m=(vpp_m*spp_zf[vpp_m_i])>>1;
   else
      vpp_m=(vpp_m<<1)/spp_zf[vpp_m_i];
   //printf("After zoomin:\n");
   vpp=vpp_m;
   vmax = vpp2vmax(vpp,vpp_s);
   //VZoomPrint();
}
void ScopeBase::VZoomout()
{
   _setvauto(false);
   vpp2vpp_m();
   vmax=vpp2vmax(vpp,vpp_s);
   if(vpp_s==0)
      vpp_m=(vpp_m<<1)/spp_zf[vpp_m_i];
   else
      vpp_m=(vpp_m*spp_zf[vpp_m_i])>>1;
   vpp_m_i++; if(vpp_m_i>2) vpp_m_i=0;
   if(vpp_m==1 && vpp_s==0)
      vpp_s=1;
   //printf("After zoomout:\n");
   vpp=vpp_m;
   vmax = vpp2vmax(vpp,vpp_s);
   //VZoomPrint();
}






void ScopeBase::SetVAuto()
{
   _setvauto(true);
}

void ScopeBase::SetVRange(int min,int max)
{
   _setvauto(false);
   vmin=min;
   vmax=max;

   //printf("%p. ScopeBase::SetVRange %d %d\n",this,min,max);
   vrange2vpp(vmin,vmax,vpp,vpp_s);
   //printf("%p. ScopeBase::SetVRange. After vrange2vpp\n",this);

   //printf("SetVRange. vmin: %d vmax: %d vpp: %d vpp_s: %d\n",vmin,vmax,vpp,vpp_s);

}



/******************************************************************************
   Scope::_setvauto
*******************************************************************************
   Setter: vertical axis in automatic/manual mode

******************************************************************************/

void ScopeBase::_setvauto(bool a)
{
   VAuto = a;
}


/******************************************************************************
   Scope::PanUp
*******************************************************************************
   Pans the display up (towards the higher Y axis values).

   1. Deactivate the automatic vertical scaling
   2. Pan up

******************************************************************************/
void ScopeBase::PanUp()
{
   _setvauto(false);

   int yvrange;         // Find what is the complete range of values on display

   // Convert h pixels -> value range
   if(vpp_s==1)
      yvrange = h*vpp;
   else
      yvrange = h/vpp;

   // Pan up by a percentage of this
   offsetyv+=yvrange/10;

   //printf("offsetyv: %d\n",offsetyv);
}
/******************************************************************************
   Scope::PanUp
*******************************************************************************
   Pans the display up (towards the lower Y axis values).

   1. Deactivate the automatic vertical scaling
   2. Pan down

******************************************************************************/
void ScopeBase::PanDown()
{
   _setvauto(false);

   int yvrange;         // Find what is the complete range of values on display

   // Convert h pixels -> value range
   if(vpp_s==1)
      yvrange = h*vpp;
   else
      yvrange = h/vpp;

   // Pan up by a percentage of this
   offsetyv-=yvrange/10;

   //printf("offsetyv: %d\n",offsetyv);
}
