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

#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QDir>
#include <QThread>
#include <QSplitter>
#include <QProgressDialog>
#include <Phonon/VideoWidget>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <assert.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "referencesdialog.h"
#include "resampledialog.h"
#include "matlabcodedialog.h"
#include "sourcedialog.h"
#include "savesyncfiledialog.h"
#include "videowidgetext.h"
#include "helpdialog.h"
#include "cio.h"
//#include "helper.h"
#include "QVideoEncoder.h"
#include "QVideoDecoder.h"
#include "exportvideodialog.h"
#include "exportmosaicvideodialog.h"
#include "precisetimer.h"
#include "videopreviewdialog.h"

/*************************************************************************************************
**************************************************************************************************
* MainWindow   MainWindow   MainWindow   MainWindow   MainWindow   MainWindow   MainWindow
**************************************************************************************************
*************************************************************************************************/

/*************************************************************************************************
* VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT
*************************************************************************************************/


PlotHelperAbstract *MainWindow::CreatePlotHelper(VIEWID view,const PLOTVIEWPARAM &param)
{
   switch(view.viewtype)
   {
      case VIEWTYPESCOPE:
      {
         std::vector<std::vector<int> *> vd;
         NPLOTWIDGETSCOPECONFIG cfg = GetScopeViewData(view.viewnum).config;
         PrepareDataPlotScope(view,vd,cfg.colors);
         if(!param.drawtitle)
            cfg.title="";
         return new PlotHelperScope(param.videow,param.videoh,param.center,vd,cfg,param.drawhaxis,param.drawvaxis,param.drawframe);
      }
      case VIEWTYPESCATTER:
      {
         vector<vector<int> *> vx,vy;
         NPLOTWIDGETSCATTERCONFIG cfg = GetScatterViewData(view.viewnum).config;
         PrepareDataPlotScatter(view,vx,vy,cfg.colors);
         if(!param.drawtitle)
             cfg.title="";
         return new PlotHelperScatter(param.videow,param.videoh,vx,vy,cfg);
      }
      break;
      case VIEWTYPELABEL:
      {
         std::vector<std::vector<int> *> vd;
         NPLOTWIDGETLABELCONFIG cfg = GetLabelViewData(view.viewnum).config;
         PrepareDataPlotLabel(view,vd,cfg.colors);
         if(!param.drawtitle)
            cfg.title="";
         return new PlotHelperLabel(param.videow,param.videoh,vd,cfg);
      }
      break;
      case VIEWTYPESTICKMAN:
      {
         TRACESRI torso,lua,lla,rua,rla;
         NPLOTWIDGETSTICKMANCONFIG cfg = GetStickmanViewData(view.viewnum).config;
         PrepareDataPlotStickman(view,torso,lua,lla,rua,rla);
         if(!param.drawtitle)
            cfg.title="";
         return new PlotHelperStickman(param.videow,param.videoh,torso,lua,lla,rua,rla,cfg);
      }
      break;
      case VIEWTYPEVIDEO:
      {
         return new PlotHelperVideo(param.videow,param.videoh,GetVideoViewData(view.viewnum).file);
      }
      break;

   }

   return 0;
}



/**
  \brief Generates a video from the primary source
**/
void MainWindow::on_actionSelected_view_triggered()
{
   QString title("Export selected view to video");


   VIEWID vid = GetViewFocus();                          // Get the selected view
   if(vid.viewtype==VIEWTYPEUNDEFINED)
   {
      QMessageBox::information(this,title,"Error: no view is selected");
      return;
   }



   // Set the video parameters
   PLOTVIEWPARAM param;

   // If the view is a video, we should  initialize it's size parameters
   if(vid.viewtype == VIEWTYPEVIDEO)
   {
      QSize s = dynamic_cast<SSViewVideo*>(GetSSView(vid))->frameSize();
      param.videow = s.width();
      param.videoh = s.height();
      printf("Video size: %d %d\n",param.videow,param.videoh);
   }
   else
   {
      param.videow = 640;
      param.videoh = 480;
   }

   if(!GetVideoParameters(param))
      return;

   if(param.videow%8 || param.videoh%8)
   {
      QMessageBox::information(this,title,"Error: the video width and height must be a multiple of 8 pixels.");
      return;
   }
   QString videoFileName = QFileDialog::getSaveFileName(this, title,"video","Movie files (*.avi *.asf *.mpg)");
   if(videoFileName.isNull())
   {
      return;
   }

   int rv=ExportViewToVideo(vid,param,videoFileName,"Generating video...");
   if(rv!=0)
   {
      if(rv==-1)
         QMessageBox::information(this,title,"Error: unable to initialize the video encoder.");

   }


}


/**
  Save all the views in a video.
**/
void MainWindow::on_actionVideo_of_all_signal_sources_triggered()
{
   QString title("Export all signal sources to video");

   // Set the video parameters
   PLOTVIEWPARAM param;
   if(!GetVideoParameters(param));
      return;

   if(param.videow%8 || param.videoh%8)
   {
      QMessageBox::information(this,title,"Error: the video width and height must be a multiple of 8 pixels.");
      return;
   }
   QString videoFileName = QFileDialog::getSaveFileName(this,"Select the base file name for video export (each view is suffixed with the view number)","video","Movie files (*.avi *.asf *.mpg)");
   printf("video name: %s\n",videoFileName.toStdString().c_str());
   if(videoFileName.isNull())
   {
      return;
   }

   printf("Number of views: %d\n",GetNumScopeViews());

   // Iterate all the views
   for(unsigned v=0;v<GetNumViews(VIEWTYPESCOPE);v++)
   {
      QString fn = QString("%1_%2.mpg").arg(videoFileName).arg(v);
      QString pt = QString("Generating video of view %1").arg(v);
      printf("view %d\n",v);
      printf("in %s\n",fn.toStdString().c_str());
      //int rv=SaveSignalToVideo(v,videow,videoh,bitrate,gop,signalstep,drawtitle,dbl,fn,pt);
      int rv=0;
      if(rv!=0)
      {
         if(rv==-1)
            QMessageBox::information(this,title,"Error: unable to initialize the video encoder.");

      }
   }
}





/**
  view:  view to export (anything but a video)
  Return value:
  0: ok
  -1: video encoder error
  -2: encoding cancelled
  -3: format not supported
**/
int MainWindow::ExportViewToVideo(VIEWID view,const PLOTVIEWPARAM &param,QString videoFileName,QString progressTitle)
{
   QScopedPointer<PlotHelperAbstract> ph(CreatePlotHelper(view,param));
   if(ph.isNull())
   {
      // No plot helper for that kind of view -> error.
      QMessageBox::information(this,progressTitle,"Error: this kind of view cannot be exported to video");
      return -3;
   }

   // Get maximum sample number in view
   unsigned maxs = ph->getMaxOffset();

   // Get the scaled video size
   unsigned videows=param.videow;
   unsigned videohs=param.videoh;
   if(param.scale!=1)
   {
      videows = param.videow*param.scale;
      videohs = param.videoh*param.scale;
   }


   // Create a video encoder
   QVideoEncoder encoder;
   encoder.createFile(videoFileName,videows,videohs,param.bitrate,param.gop);
   if(!encoder.isOk())
   {
      printf("Cannot create encoder\n");
      return -1;
   }


   // Iterate from start to end, generate a file...
   double t1 = PreciseTimer::QueryTimer();

   QProgressDialog pdlg(progressTitle, "Abort", 0, maxs, this);
   pdlg.setWindowModality(Qt::WindowModal);
   pdlg.setValue(0);
   for(unsigned s=0;s<maxs;s+=param.signalstep)
   {
      ph->PlotOffset(s);
      // save the image
      if(param.scale!=1)
      {

         QImage pixmapf=ph->GetImage().scaled(videows,videohs);
         encoder.encodeImage(pixmapf);
      }
      else
      {
         encoder.encodeImage(ph->GetImage());
      }

      // Indicate progress
      pdlg.setValue(s);
      if(pdlg.wasCanceled())
         return -2;
   }

   double t2 = PreciseTimer::QueryTimer();

   printf("Done. Time: %lf\n",t2-t1);

   return 0;

}



void MainWindow::findlinks()
{
   printf("test\n");


   bool prepend;


   prepend=true;
   // Find disconnected graphs
   vector<RELATIONS> graphs = FindGraphs(RelationsActive);
   // Find the resample logic - only to check if there are signal relations available.
   map<int,RESAMPLEREL> m = ResampleLogic(graphs,prepend);

   printf("m size: %d\n",m.size());

   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
   {
      printf("Processing source %d (%s)\n",it->first,SignalData[IDX2SID(it->first)].filename.toStdString().c_str());

      // We dump it.

      RESAMPLEREL r = it->second;
      printf("Relation: m: %lf o: %lf\n",r.m,r.o);
      printf(" \tlc: %d lm: %d pc: %d pm: %d\n",r.l_c,r.l_matlab,r.p_c,r.p_matlab);


   }
   /*
      if(!IsIdxSignal(it->first))
      {
         printf("Source %d is not a signal, skipping\n",it->first);
         continue;
      }

      // In principle magnitude is <= 1 - what happens if not?
      assert(it->second.m<=1.0);



      // This is already done by the resample logic
      double o;
      o=it->second.o;
      int p;
      if(postpend)
         p=it->second.p_c;
      else
         p=0;
      printf("Calling ResampleSignalData; factor %lf offset %d postpend %d\n",it->second.m,o,p);
      SIGNALDATA sigdat = ResampleSignalData(SignalData[IDX2SID(it->first)],it->second.m,o,p);

      // In-place resample
      printf("store\n");
      SignalData[IDX2SID(it->first)] = sigdat;

      // Update the reference points accordingly
      printf("upd ref points\n");
      for(REFERENCEPOINTS::iterator r=RefPoints.begin();r!=RefPoints.end();r++)
      {
         if((*r)[0] == it->first)
            (*r)[1] = (int)(((double)(*r)[1])/it->second.m+o);
         if((*r)[2] == it->first)
            (*r)[3] = (int)(((double)(*r)[3])/it->second.m+o);
      }
   }*/




}






void MainWindow::image2Pixmap(QImage &img,QPixmap &pixmap)
{
   // Convert the QImage to a QPixmap for display
   pixmap = QPixmap(img.size());
   QPainter painter;
   painter.begin(&pixmap);
   painter.drawImage(0,0,img);
   painter.end();
}



