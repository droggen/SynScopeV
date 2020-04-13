/*
   SynScopeV

   Copyright (C) 2008,2009,2010:
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


#include "exportvideo.h"
#include "QVideoDecoder.h"




bool GetVideoSize(QString filename,int &w,int &h)
{
   QVideoDecoder decoder;
   decoder.openFile(filename);
   decoder.seekNextFrame();
   QImage img;
   bool ok = decoder.getFrame(img);
   if(!ok)
      return false;
   w = img.width();
   h = img.height();
   return true;
}

/**
   Returns the length of a video in milliseconds
**/
int GetVideoLength(QString filename)
{
   QVideoDecoder decoder;
   decoder.openFile(filename);
   return decoder.getVideoLengthMs();
}


