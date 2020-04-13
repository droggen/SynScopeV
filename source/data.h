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

#ifndef DATA_H
#define DATA_H

#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/AudioOutput>
#include <Phonon/VideoPlayer>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QtGui/QMainWindow>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointer>
#include <vector>
#include <sstream>
#include <iostream>
#include "videowidgetext.h"
#include "guihelper.h"
#include "SSViewScope.h"
#include "SSViewVideo.h"
#include "SSViewStickman.h"
#include "SSViewScatter.h"
#include "SSViewLabel.h"
#include "NPlotWidgetScope.h"

//-------------------------------------------------------------------------------------------------
// Data structures
//-------------------------------------------------------------------------------------------------
// Trace
typedef std::vector<int> TRACEI;
// Pointer to a trace
typedef std::vector<int> * TRACERI;
// Pointer to a several trace
typedef std::vector<TRACERI> TRACESRI;

//struct SIGNALDATA
class SIGNALDATA
{
public:
   SIGNALDATA();

   // Get reference to data
   TRACERI getRef(unsigned channel);
   TRACESRI getRef(const std::vector<unsigned> &channels);
   TRACESRI getRef(const std::vector<unsigned> &channels,std::vector<unsigned> &channelspruned);

   std::vector<TRACEI> data;     // data[channel][sample]
   int centersample;
   QString filename;

};
/*
  // Parameters for plots
*/
typedef struct
{
   unsigned videow,videoh,bitrate,gop,signalstep;
   bool drawtitle;
   double scale;
   bool center;
   bool drawframe;
   bool drawhaxis,drawvaxis;
} PLOTVIEWPARAM;

enum SRCTYPE {SRCTYPEUNDEFINED=0, SRCTYPESIGNAL=1000, SRCTYPEVIDEO};
enum VIEWTYPE {VIEWTYPEUNDEFINED=0, VIEWTYPEFIRST=1000, VIEWTYPESCOPE=1000, VIEWTYPESTICKMAN, VIEWTYPEVIDEO, VIEWTYPESCATTER, VIEWTYPELABEL, VIEWTYPELAST=VIEWTYPELABEL};
typedef struct
{
   SRCTYPE srctype;
   int srcid;                 // Relative ID within the given source type
   int srcuid;                // Absolute ID within the given source type
} SRCID;

//typedef struct
class VIEWID
{
public:
   VIEWID(VIEWTYPE _t=VIEWTYPEUNDEFINED,int _n=0) {viewtype=_t;viewnum=_n;};
   VIEWTYPE viewtype;
   int viewnum;
};
//} VIEWID;


//typedef unsigned IDSRC;
//typedef unsigned IDVIEW;


// This inheritence may not be needed
class VIEWDATA
{
public:
   virtual const SSViewAbstract *getViewAbstract() const = 0;
   unsigned source;           // signal source (i.e. file)
//private:
  // SSViewAbstract *view;
};


//typedef struct _toto : public VIEWDATA
class SCOPEVIEWDATA : public VIEWDATA
{
public:
   SSViewScope *view;

   NPLOTWIDGETSCOPECONFIG config;

   vector<unsigned> traces;     // Traces id
   virtual const SSViewAbstract *getViewAbstract() const {return view;};
};
//} SCOPEVIEWDATA;

typedef std::vector<SCOPEVIEWDATA> SCOPEVIEWDATAS;

typedef std::vector<std::vector<string> > SOURCEFILES;


//typedef struct
class VIDEOVIEWDATA : public VIEWDATA
{
   public:
   SSViewVideo *view;

   QString file;
   int centersample;
   virtual const SSViewAbstract *getViewAbstract() const {return view;};
};
//} VIDEOVIEWDATA;

typedef std::vector<VIDEOVIEWDATA> VIDEOVIEWDATAS;

//typedef struct
class STICKMANVIEWDATA : public VIEWDATA
{
   public:
   SSViewStickman *view;

   NPLOTWIDGETSTICKMANCONFIG config;

   vector<unsigned> trace_torso;
   vector<unsigned> trace_lua;
   vector<unsigned> trace_lla;
   vector<unsigned> trace_rua;
   vector<unsigned> trace_rla;
//   int centersample;
   virtual const SSViewAbstract *getViewAbstract() const {return view;};
};
//} STICKMANVIEWDATA;
typedef std::vector<STICKMANVIEWDATA> STICKMANVIEWDATAS;

//typedef struct
class SCATTERVIEWDATA : public VIEWDATA
{
   public:
   SSViewScatter *view;

   NPLOTWIDGETSCATTERCONFIG config;

   std::vector<unsigned> x,y;        // x,y traces in the source
   //int centersample;
   virtual const SSViewAbstract *getViewAbstract() const {return view;};
};
//} SCATTERVIEWDATA;
typedef std::vector<SCATTERVIEWDATA> SCATTERVIEWDATAS;


//typedef struct
class LABELVIEWDATA : public VIEWDATA
{
   public:
   SSViewLabel *view;

   NPLOTWIDGETLABELCONFIG config;

   vector<unsigned> traces;

   virtual const SSViewAbstract *getViewAbstract() const {return view;};
};
//} LABELVIEWDATA;

typedef std::vector<LABELVIEWDATA> LABELVIEWDATAS;

// This should be changed to a nice structure. REFERENCEPOINTS[pointno][0...3
// 0: source 1
// 1: position in source 1
// 2: source 2
// 3: position in source 2
typedef std::vector<std::vector<int> > REFERENCEPOINTS;




/*
  Store a relation between x1 and x2.
  x2 = b*x1 + a;
*/
typedef struct {
   int x1,x2;     // Source and destination
   bool s1,s2;    // indicates whether x1,x2 are signals.
   int tx1,tx2;   // 0-based type-specific source number (for both video and signals)
   double a,b;

} RELATION;
typedef std::vector<RELATION> RELATIONS;

typedef struct { double m,o; int l_c,l_matlab,p_c,p_matlab;} RESAMPLEREL;


RELATIONS FindGraph(RELATIONS &Relations, int x1);
bool IsRelationConflicting(RELATIONS &Relations, int x1,int x2);
QString RelationToText(RELATION &r);
vector<RELATIONS> FindGraphs(RELATIONS Relations);
//void ResampleStuff(vector<RELATIONS> Graphs);
void DoubleToRatio(double v,int &n,int &d,int maxd=10000);
QString GraphToString(vector<int> g);
std::vector<std::vector<int> > parse(char *data,bool &ok);
void ComputeAllSourceOffsetFrom(RELATIONS &RelationsActive,int sourceidx,int value,std::map<int,int> &offsetout);




//-------------------------------------------------------------------------------------------------
// SplitInVector
//-------------------------------------------------------------------------------------------------
// Returns false in case of failure.
// Only one split character is allowed
template<class T> bool SplitInVector(string str,string split,vector<T> &r,ios_base&(&b)(ios_base&)=dec)
{

   r.clear();
   QString qstr(str.c_str());
   QStringList sl = qstr.split(QString(split.c_str()),QString::SkipEmptyParts);
   for(int i=0;i<sl.size();i++)
   {
      std::istringstream iss(sl[i].toStdString());
      T j;
      if( !((iss >> b >> j).fail()))
         r.push_back(j);
      else
         return false;
   }
   return true;
}
template<class T> bool SplitInVector(QString qstr,QString split,vector<T> &r,ios_base&(&b)(ios_base&)=dec)
{
   r.clear();
   QStringList sl = qstr.split(split,QString::SkipEmptyParts);
   for(int i=0;i<sl.size();i++)
   {
      std::istringstream iss(sl[i].toStdString());
      T j;
      if( !((iss >> b >> j).fail()))
         r.push_back(j);
      else
         return false;
   }
   return true;
}


#endif // DATA_H
