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
   The ScopeBase abstract base class implements the basic scaling/zooming/panning functions:
   - Zoom in/Zoom out/zoom auto
   - Panning up/down
   - sclaing: sample to pixel, pixel to sample, etc

   The drawing code, however, is left for derived classes to implement
*/

#ifndef __SCOPEBASE_H
#define __SCOPEBASE_H

#include <vector>
#include <string>

using namespace std;

class ScopeBase
{
public:
   ScopeBase(unsigned _w,unsigned _h);
   ~ScopeBase();

protected:
   int w,h;                         // Size of the drawing area


   bool VAuto;
   // H zoom factors
   unsigned spp_zf[3],spp_m_i,spp_s,spp_m;
   // H tick spacing
   unsigned minxspace,minyspace;
   // V resolution
   int vpp,vpp_s;
   int vpp_m,vpp_m_i;
   int vmin,vmax;


   bool center;                     // Alignment of scope (center)
   int refsample;
   int offsetsample;
   int offsetyv;                    // Vertical offset [data units]. Used in manual mode: the center of the scope corresponds to sample yvoffset




   void _setvauto(bool);            // Internally called to set the vertical axis in automatic/manual mode


   void SetSize(int _w,int _h);

public:
   virtual int FindSmallestAbove(int target,int &factor,int &power);
   virtual int XsTickSpacing(int &multiplier,int &power);
   virtual int YvTickSpacing(int &multiplier,int &power);

   virtual void SetSampleOffset(int co);
   virtual void SetAlignment(bool _center=false);

   virtual int s2x(int s);
   virtual int v2y(int v);
   virtual int x2s(int x);

   virtual void vpp2vpp_m();
   virtual void vrange2vpp(int vmin,int vmax,int &vpp,int &vpp_s);
   virtual int vpp2vmax(int vpp,int vpp_s);
   virtual void HZoom(int z);
   virtual void HZoomReset();
   virtual void HZoomin();
   virtual void HZoomout();
   virtual void HZoomPrint();
   virtual void VZoomPrint();
   virtual void SetVAuto();
   virtual void SetVRange(int min,int max);
   virtual void VZoomin();
   virtual void VZoomout();
   virtual void PanUp();
   virtual void PanDown();
};


#endif // __SCOPEBASE_H
