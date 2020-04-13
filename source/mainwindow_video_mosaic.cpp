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
#include "PlotHelper.h"
#include "exportvideo.h"

/*************************************************************************************************
**************************************************************************************************
* MainWindow   MainWindow   MainWindow   MainWindow   MainWindow   MainWindow   MainWindow
**************************************************************************************************
*************************************************************************************************/

/*************************************************************************************************
* VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT   VIDEO EXPORT
*************************************************************************************************/


/**
  \brief Generate a mosaic video, very configurable
  This is called from the menu
**/
void MainWindow::on_actionMosaic_video_triggered()
{
    printf("on_actionMosaic_video_triggered\n");

    std::vector<MOSAICWINDOWPARAM> mvp,mvptmp,msp,msptmp;

    QString title("Export signal mosaic to video");
    if(GetPrimarySource()==-1)
    {
        QMessageBox::information(this,title,"Error: no primary source selected");
        return;
    }


    ExportMosaicVideoDialog *dlg = new ExportMosaicVideoDialog(this);


    // Generate default parameters moderately cleverly

    // Generate the video parameters
    int vidwidth=0;
    for(unsigned v=0,y=0;v<GetNumVideoViews();v++)
    {
        int w,h;
        bool ok;
        ok = GetVideoSize(GetVideoViewData(v).file,w,h);
        MOSAICWINDOWPARAM p;
        p.name = QString("V%1").arg(v);
        if(ok)
        {
            p.w=w;
            p.h=h;
            vidwidth = p.w;
        }
        else
        {
            p.w=0;
            p.h=0;
        }
        p.px=0;
        p.py=y;
        y+=p.h+20;
        p.render=true;
        p.view=v;               // Video index (VID)
        p.scale=1.0;
        mvp.push_back(p);

    }

    // Generate the parameters for the scope, stickman, scatter
    std::vector<VIEWTYPE> vt;
    vt.push_back(VIEWTYPESCOPE);
    vt.push_back(VIEWTYPESCATTER);
    vt.push_back(VIEWTYPESTICKMAN);
    for(unsigned vti=0;vti<vt.size();vti++)
    {
        int offy=0;
        for(unsigned v=0;v<GetNumViews(vt[vti]);v++)
        {
            MOSAICWINDOWPARAM p;
            p.vt=vt[vti];
            p.name = QString("%1%2").arg(GetSSView(vt[vti],v)->name()).arg(v);
            //p.name=QString("S%1").arg(v);
            p.px = vidwidth;
            p.py=offy;
            offy+=100;
            p.w = 128;
            p.h = 80;
            p.view=v;               // Signal index (SID)
            p.render=true;
            p.scale=1.0;
            msp.push_back(p);
        }
    }


    dlg->SetParam(mvp,msp);
    if(dlg->exec() == QDialog::Rejected)
        return;

    // Get the params
    int bitrate,gop,signalstep;
    bool drawtitle,drawhaxis,drawvaxis,drawframe,transparentscope,globaldouble;
    bool centercurrentsample;
    dlg->GetParam(mvptmp,msptmp,bitrate,gop,signalstep,drawtitle,drawhaxis,drawvaxis,drawframe,transparentscope,centercurrentsample,globaldouble);
    delete dlg;


    /*
   printf("MSP:\n");
   for(unsigned  i=0;i<msptmp.size();i++)
   {
      printf("%d. %d %d, %d %d, dbl: %d, ID: %d\n",i,msptmp[i].px,msptmp[i].py,msptmp[i].w,msptmp[i].h,msptmp[i].dbl, msptmp[i].view);
   }
   printf("MVP:\n");
   for(unsigned i=0;i<mvptmp.size();i++)
   {
      printf("%d. %d %d, %d %d, scale: %lf, ID: %d\n",i,mvptmp[i].px,mvptmp[i].py,mvptmp[i].w,mvptmp[i].h,mvptmp[i].scale,mvptmp[i].view);
   }*/



    // Prune the views not flagged for rendering
    msp.clear();
    for(unsigned i=0;i<msptmp.size();i++)
    {
        if(!msptmp[i].render)
            continue;
        msp.push_back(msptmp[i]);
    }
    // Prune the videos not flagged for rendering
    mvp.clear();
    for(unsigned i=0;i<mvptmp.size();i++)
    {
        if(!mvptmp[i].render)
            continue;
        mvp.push_back(mvptmp[i]);
    }

    /*
   printf("MSP:\n");
   for(unsigned  i=0;i<msp.size();i++)
   {
      printf("%d. %d %d, %d %d, dbl: %d, ID: %d\n",i,msp[i].px,msp[i].py,msp[i].w,msp[i].h,msp[i].dbl, msp[i].view);
   }
   printf("MVP:\n");
   for(unsigned i=0;i<mvp.size();i++)
   {
      printf("%d. %d %d, %d %d, scale: %lf ID: %d\n",i,mvp[i].px,mvp[i].py,mvp[i].w,mvp[i].h,mvp[i].scale,mvp[i].view);
   }*/

    // Find the size of the overall frame
    int maxw=0,maxh=0;
    // MSP
    for(unsigned i=0;i<msp.size();i++)
    {
        //printf("%d %d - %d %d (%d)\n",msp[i].px,msp[i].py,msp[i].w,msp[i].h,(int)msp[i].dbl);
        maxw = max((int)maxw,(int)(msp[i].px + msp[i].w*msp[i].scale));
        maxh = max((int)maxh,(int)(msp[i].py + msp[i].h*msp[i].scale));
    }
    // MVP
    for(unsigned i=0;i<mvp.size();i++)
    {
        //printf("%d %d - %d %d (x %lf)\n",mvp[i].px,mvp[i].py,mvp[i].w,mvp[i].h,mvp[i].scale);
        maxw = max((int)maxw,(int)(mvp[i].px + mvp[i].w*mvp[i].scale));
        maxh = max((int)maxh,(int)(mvp[i].py + mvp[i].h*mvp[i].scale));
    }

    //
    printf("maxw: %d maxh: %d\n",maxw,maxh);
    // Round to a multiple of 8
    if(maxw%8)
        maxw=((maxw>>3)+1)<<3;
    if(maxh%8)
        maxh=((maxh>>3)+1)<<3;

    printf("maxw: %d maxh: %d\n",maxw,maxh);


    // Prepare the render frame
    QImage mosaic(maxw,maxh,QImage::Format_RGB32);
    QPainter painter(&mosaic);


    // Prepare the signal plot helpers
    std::vector<PlotHelperAbstract*>      pvh;        // Plot helpers
    for(unsigned i=0;i<msp.size();i++)
    {
        printf("Prepare plot view helper %d\n",i);
        switch(msp[i].vt)
        {
        case VIEWTYPESCOPE:
        {
            printf("Prepare of type scope\n");
            std::vector<std::vector<int> *> vd;		// data for the traces in this scope
            SCOPEVIEWDATA svd = GetScopeViewData(msp[i].view);
            printf("Before calling PrepareDataPlotScope\n");
            printf("view: %d -> %d\n",i,msp[i].view);
            PrepareDataPlotScope(VIEWID(VIEWTYPESCOPE,msp[i].view),vd,svd.config.colors);       // Prepare signal -> convert to signal IDX
            printf("Newing and pushbacking\n");
            printf("msp[i].view: %d %d\n",i,msp[i].view);
            printf("ScopeViewData.size: %d\n",GetNumScopeViews());


            printf("Got the scope view data\n");

            // dumping data!
            printf("w,h: %d %d\n",msp[i].w,msp[i].h);
            printf("centercurrentsample: %d\n",centercurrentsample);
            printf("svd.yauto: %d\n",svd.config.yauto);
            printf("svd.xscale: %d\n",svd.config.xscale);
            printf("svd.yscalemin: %d\n",svd.config.yscalemin);
            printf("svd.yscalemax: %d\n",svd.config.yscalemax);
            printf("drawtitle: %d\n",drawtitle);
            printf("svd.title: %s\n",svd.config.title.c_str());
            printf("axis: %d %d %d\n",drawhaxis,drawvaxis,drawframe);
            printf("vd.size: %d\n",vd.size());
            if(vd.size())
                printf("vd[0].size: %d\n",vd[0]->size());
            // Optimize the params!

            if(!drawtitle)
                svd.config.title="";
            PlotHelperScope *psh = new PlotHelperScope(
                        msp[i].w,msp[i].h,
                        centercurrentsample,
                        vd,
                        svd.config,
                        drawhaxis,drawvaxis,drawframe);
            printf("push backing\n");
            pvh.push_back(psh);
            printf("After newing and pushbacking\n");
        }
            break;
        case VIEWTYPESCATTER:
        {
            printf("Prepare of type scatter\n");
            vector<vector<int> *> vx,vy;
            SCATTERVIEWDATA svd = GetScatterViewData(msp[i].view);
            PrepareDataPlotScatter(VIEWID(VIEWTYPESCATTER,msp[i].view),vx,vy,svd.config.colors);       // Prepare signal -> convert to signal IDX
            if(!drawtitle)
                svd.config.title="";
            pvh.push_back(new PlotHelperScatter(msp[i].w,msp[i].h,vx,vy,svd.config));


        }
            break;
        case VIEWTYPESTICKMAN:
        {
            printf("Prepare of type stickman\n");
        }
            break;
        default:
            printf("Prepare of type other\n");
            break;
        }

    }


    printf("Prepare the video decoders\n");
    // Prepare the video decoders
    std::vector<QVideoDecoder> pvideoh;
    pvideoh.resize(mvp.size());
    for(unsigned i=0;i<mvp.size();i++)
    {
        bool ok = pvideoh[i].openFile(GetVideoViewData(mvp[i].view).file );
        printf("Opening file %d: %d\n",i,(int)ok);
    }



    // find the max sample that is displayed in the primary view
    unsigned maxs;
    printf("GetPrimarySource: %d\n",GetPrimarySource());
    printf("is idxsignal?: %d\n",IsIdxSignal(GetPrimarySource()));
    if(!IsIdxSignal(GetPrimarySource()))
    {
        printf("Source is a video - find the video length\n");
        // Source is a video - find the video length
        maxs = GetVideoLength(GetVideoViewData(IDX2VID(GetPrimarySource())).file);
    }
    else
    {
        printf("***************Source is a signal - find the signal length****************\n");

        // Source is a signal - find the signal length
        printf("Primary: %d\n",GetPrimarySource());
        printf("IDX2SID: %d\n",IDX2SID(GetPrimarySource()));
        maxs = GetSourceLengthSignal(IDX2SID(GetPrimarySource()));
        printf("maxs: %d\n",maxs);

    }



    // Get the file name

    QString videoFileName = QFileDialog::getSaveFileName(this,"Video file name","video","MPEG movie (*.*)");
    printf("video name: %s\n",videoFileName.toStdString().c_str());
    if(videoFileName.isNull())
    {
        return;
    }

    QVideoEncoder encoder;
    encoder.createFile(videoFileName,maxw*(globaldouble?2:1),maxh*(globaldouble?2:1),bitrate,gop);
    if(!encoder.isOk())
    {
        QMessageBox::information(this,title,"Can't initialize video encoder");
        return;
    }
    printf("file created\n");


    VideoPreviewDialog *VideoPreview = new VideoPreviewDialog(this);
    VideoPreview->show();
    QEventLoop evt;      // we use an event loop to allow for paint events to show on-screen the generated video


    // Iterate the samples
    unsigned totalrenderbytes=0;
    for(unsigned s=0;s<maxs;s+=signalstep)
    {
        printf("Iteration %d of %d\n",s,maxs);
        double t0,t1,t2,t3,t4,t5,t6;

        t0 = PreciseTimer::QueryTimer();

        // Clear the mosaic
        mosaic.fill(0);

        t1 = PreciseTimer::QueryTimer();

        // Compute the offsets for all views
        std::map<int,int> offsetout;

        ComputeAllSourceOffsetFrom(RelationsActive,GetPrimarySource(),s,offsetout);
        /*printf("Map computed for offset %d\n",s);
      for(std::map<int,int>::iterator it=offsetout.begin();it!=offsetout.end();it++)
      {
         printf("%d -> %d\n",it->first,it->second);
      }*/

        //
        for(unsigned i=0;i<mvp.size();i++)
        {
            std::map<int,int>::iterator it;
            it = offsetout.find(VID2IDX(mvp[i].view));         // ComputeAllSourceOffsetFrom returns a map with absolute ID
            int offset;
            // We don't necessarily always find an offset. If a view is selected for video export, but is not linked, it won't appear in offsetout
            if(it==offsetout.end())
                offset = 0;
            else
                offset = it->second;


            // Seek and get frame
            bool ok;
            ok = pvideoh[i].seekMs(offset);
            //printf("Video %d seek to %d. ok: %d\n",i,offset,(int)ok);
            QImage img;
            ok = pvideoh[i].getFrame(img);
            if(ok)
            {
                // Rescale the frame and render it in the mosaic
                QImage imgs = img.scaled(img.width()*mvp[i].scale,img.height()*mvp[i].scale);
                painter.drawImage(mvp[i].px,mvp[i].py,imgs);
            }


        }

        t2 = PreciseTimer::QueryTimer();


        // Render all the views in the helper
        for(unsigned i=0;i<msp.size();i++)
        {
            //printf("Plotting scope %d, view %d\n",i,msp[i].view);
            // Find the offset of the view msp[i].view
            std::map<int,int>::iterator it;
            it = offsetout.find(SID2IDX(GetScopeViewData(msp[i].view).source));         // ComputeAllSourceOffsetFrom returns a map with absolute ID
            int offset;
            // We don't necessarily always find an offset. If a view is selected for video export, but is not linked, it won't appear in offsetout
            if(it==offsetout.end())
            {
                offset = 0;
                //printf(" no offset\n");
            }
            else
            {
                offset = it->second;
                //printf(" offset: %d\n",offset);
            }
            //printf("I %d. V %d. Offset: %d\n",i,msp[i].view,offset);
            pvh[i]->PlotOffset(offset);
        }

        t3 = PreciseTimer::QueryTimer();

        // Compose a mosaic with all the views
        for(unsigned i=0;i<msp.size();i++)
        {
            // Rescale
            //QImage img = pvh[i]->GetImage()->scaled(msp[i].w*(msp[i].scale),msp[i].h*(msp[i].scale));
           QImage img = pvh[i]->GetImage().scaled(msp[i].w*(msp[i].scale),msp[i].h*(msp[i].scale));


            if(transparentscope)
            {

                // Copy with transparent black.
                // In principle no clipping needed for target, as we computed the size of the target according to the sources

                //for(int y=0;y<img.height() && msp[i].py+y<mosaic.height();y++)       // do clipping for target
                for(int y=0;y<img.height();y++)
                {
                    unsigned char *psrc,*pdst;
                    psrc = img.scanLine(y);
                    pdst = mosaic.scanLine(msp[i].py+y) + 4*msp[i].px;
                    //for(int x=0;x<img.width() && msp[i].px+x<mosaic.width();x++)      // do clipping for target
                    for(int x=0;x<img.width();x++)
                    {
                        unsigned pixel = *(unsigned*)psrc;
                        if(pixel!=0)
                            *(unsigned*)pdst = pixel;

                        psrc+=4;
                        pdst+=4;
                    }

                }
            }
            else
                painter.drawImage(msp[i].px,msp[i].py,img);
        }

        t4 = PreciseTimer::QueryTimer();

        int ret;
        if(globaldouble)
        {
            ret = encoder.encodeImage(mosaic.scaled(mosaic.width()*2,mosaic.height()*2));
        }
        else
            ret = encoder.encodeImage(mosaic);
        totalrenderbytes+=ret;

        t5 = PreciseTimer::QueryTimer();


        // Indicate progress
        VideoPreview->setImage(mosaic);
        VideoPreview->setInfoText(QString("Rendered %1 out of %2 (%3\%). Frame: %4 bytes. Total (w/o containter): %5 bytes (%6 MB)").arg(s).arg(maxs).arg((double)s*100.0/(double)maxs).arg(ret).arg(totalrenderbytes).arg(totalrenderbytes/1024/1024));
        if(VideoPreview->wasCanceled())
            break;
        evt.processEvents();

        t6 = PreciseTimer::QueryTimer();

        printf("fill: %lf vdecrdr: %lf plot: %lf\n",t1-t0,t2-t1,t3-t2);
        printf("   plotcomp: %lf enc: %lf view: %lf\n",t4-t3,t5-t4,t6-t5);


    }

    delete VideoPreview;

    encoder.close();

    // Delete the plot helpers
    for(unsigned i=0;i<pvh.size();i++)
        delete pvh[i];
}



