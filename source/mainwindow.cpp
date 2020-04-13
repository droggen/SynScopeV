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
#include <QSlider>
#include <QProgressDialog>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <assert.h>
#include <math.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "referencesdialog.h"
#include "resampledialog.h"
#include "matlabcodedialog.h"
#include "sourcedialog.h"
#include "savesyncfiledialog.h"
#include "helpdialog.h"
#include "cio.h"
//#include "helper.h"
#include "QVideoEncoder.h"
#include "exportvideodialog.h"
#include "exportmosaicvideodialog.h"
#include "precisetimer.h"
#include "parse.h"
#include "SSViewScope.h"
#include "SSViewVideo.h"
#include "SSViewStickman.h"
#include "SSViewScatter.h"
#include "SSViewLabel.h"

#include "NPlotWidgetScope.h"



/*
   In order to link signal and videos, sources are internally referenced by an ID. This ID does not make distinction between signal or video source.
   All the reference points sources, and relation equations, use this ID.
   ID e [0;NumVideoSource[ -> ID corresponds to a video source
   ID e [NumVideoSource;NumVideoSource+NumSignalSource[ -> ID corresponds to a signal source


*/


// <3;title;1 2 3;xscale=2;yscale = 2 10;color = ff0000 00ff00 0000ff><2;tata;0 1 ;xscale=-12;yscale = auto;color = ff0000 00ff00 0000ff>

//<3;Source new;4 5 6 ;xscale=1;yscale =-3000 3000;color = ff0000 00ff00 0000ff>



void MainWindow::SetUndoable(bool u)
{
   undoable = u;
   ui->actionUndo->setEnabled(undoable);
}

/**
  \brief MainWindow constructor: initialization and co.
**/
MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent), ui(new Ui::MainWindow)
{

#ifdef DEVELMODE
   ConsoleInit();
#endif


   NPLOTWIDGETSCOPECONFIG toto;
   toto.yauto = true;


   source_primary=source_secondary=-1;  // No primary/secondary view set.

   ui->setupUi(this);

   //
   // Complete the UI setup
   //

   // Prepare the signal scroll area
   signalScrollArea=new QScrollArea(this);            // Scroll area for the stack of signals
   signalScrollArea->setWidgetResizable(true);
   signalScrollArea->setFocusPolicy(Qt::NoFocus);     // To ensure that a mouse wheel, when the focus is on a widget within the scroll area, does not change the focus from the widget to the scroll area.

   // Prepare the video scroll area
   videoScrollArea=new QScrollArea(this);
   videoScrollArea->setWidgetResizable(true);
   videoScrollArea->setFocusPolicy(Qt::NoFocus);      // To ensure that a mouse wheel, when the focus is on a widget within the scroll area, does not change the focus from the widget to the scroll area.

   // Prepare the signal/video splitter.
   signalVideoSplitter = new QSplitter(this);
   signalVideoSplitter->addWidget(videoScrollArea);
   signalVideoSplitter->addWidget(signalScrollArea);
   signalVideoSplitter->setOrientation(Qt::Vertical);
   ui->centralWidget->layout()->addWidget(signalVideoSplitter);

   // Create the frame in the signal scroll area
   mainFrame = new QFrame(this);
   mainFrame->setFrameStyle(QFrame::NoFrame);
   mainFrame->setFrameShadow(QFrame::Raised);
   signalScrollArea->setWidget(mainFrame);

   // Create the layout in the scroll area frame
   signalVerticalLayout = new QVBoxLayout();
   mainFrame->setLayout(signalVerticalLayout);

   // Create the video scroll area
   mainVideoFrame = new QFrame();
   mainVideoFrame->setFrameStyle(QFrame::NoFrame);
   mainVideoFrame->setFrameShadow(QFrame::Raised);
   videoScrollArea->setWidget(mainVideoFrame);
   videoLayout = new QHBoxLayout();
   mainVideoFrame->setLayout(videoLayout);





   // Prepare the dialog box showing the reference points.
   RefDialog = new ReferencesDialog(this);
   connect(RefDialog,SIGNAL(AddReference()),this,SLOT(on_AddReference()));
   connect(RefDialog,SIGNAL(RemoveReference(int)),this,SLOT(on_RemoveReference(int)));
   connect(RefDialog,SIGNAL(ChangedActiveRelations()),this,SLOT(on_ChangedActiveRelations()));
   connect(RefDialog,SIGNAL(ReferenceSelected(int)),this,SLOT(on_ReferenceSelected(int)));





   //timer=startTimer(1000);

   //ui->lineEdit_View->setPlainText("<0;Source 0;1 2 3;xscale=1;yscale = -300 300;color = ff0000 00ff00 0000ff><1;Source 1;0 1 ;xscale=1;yscale = -500 500;color = ff0000 00ff00><2;Source 2;0 2;xscale=1;yscale = -300 300;color = FFFFFF FF0000><0;Source 0 bis;0 2 4;xscale=1;yscale = -300 300>");
   //ui->lineEdit_View->setPlainText("<0;Source 0;0 1;xscale=1;yscale = -300 300;color = ff0000 00ff00><1;Source 1;0 1;xscale=1;yscale = -500 500;color = ff0000 00ff00><2;Source 2;0 1;xscale=1;yscale = -300 300;color = FFFFFF FF0000><0;Source 0 bis;2;xscale=1;yscale = -300 300><3;Source 3(file3.dat);0;xscale=1;yscale = -300 300><4;Source 4(file4.dat);0;xscale=1;yscale = -300 300>");
   //ui->lineEdit_Source->setPlainText("<signal;../file1.dat><video;file.avi><signal;../file2.dat><signal;../file3.dat>");
   //ui->lineEdit_Source->setPlainText("<signal;../file0.dat><video;file.avi><signal;../file1.dat><signal;../file2.dat><signal;../file3.dat><signal;../file4.dat>");
   //ui->lineEdit_Source->setPlainText("<signal;../file0.dat><video;../vid1.avi><video;../vid2.avi><video;../vid3.avi><signal;../file1.dat><signal;../file2.dat><signal;../file3.dat><signal;../file4.dat>");

   //ui->lineEdit_View->setPlainText("<Label;0;Label;243 244 245 246 247;xscale=10;color=ff0000 00ff00 0000ff ffff00 ff00ff;title= toto tata tutu><  scope;0;A Title;0 1 2;Xscale=3;yscale=auto;color=ff00aa 000010 1234AA > <  scope;0;A Title;0 ;yscale=auto;Xscale=3;color=ff00aa  > <  scope;0;A Title;0 1 ;color=ff00aa 00ff00; yscAle=AUTo; XScale=3> < stickman;1;title; 118 119 120 121 131 132 133 134 144 145 146 147 157 158 159 160 170 171 172 173> <  scope;2;Another title;4 2  > < stickman;3;another ttl; -1 -1 -1 -1 131 132 133 134 144 145 146 147 157 158 159 160 170 171 172 173>< scatter ;0;A scatter plot;4 2 9 10;scale = -10 -10 20 20 ;color=ff0000 00ff00 ; points = 100 > ");
   ui->lineEdit_View->setPlainText("<Label;0;Label;243 244 245 246 247;xscale=10;color=ff0000 00ff00 0000ff ffff00 ff00ff;title= toto tata tutu>");

   SB_LabelSource=new QLabel(statusBar());
   statusBar()->addWidget(SB_LabelSource);
   SB_LabelView=new QLabel(statusBar());
   statusBar()->addWidget(SB_LabelView);
   SB_LabelPosition=new QLabel(statusBar());
   statusBar()->addWidget(SB_LabelPosition);
   SB_LabelCenter=new QLabel(statusBar());
   statusBar()->addWidget(SB_LabelCenter);
   SB_LabelPriSec=new QLabel(statusBar());
   statusBar()->addWidget(SB_LabelPriSec);

   SetUndoable(false);


   //connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(focusChanged(QWidget*,QWidget*)));




}

/**
    \brief Destructor
**/
MainWindow::~MainWindow()
{
   delete ui;
}
/******************************************************************************
   Add / Delete views
*******************************************************************************
   - SignalView
   - VideoView
   - StickmanView
******************************************************************************/

void MainWindow::AddScopeView(SCOPEVIEWDATA &vd)
{
   vd.view = new SSViewScope();

   // Config
   SSViewScopeConfig c;
   signalVerticalLayout->addWidget(vd.view);
   AddScopeViewData(vd);

   //   c.colors = vd.colors;
   c.config = vd.config;
   PrepareDataPlotScope(VIEWID(VIEWTYPESCOPE,GetNumScopeViews()-1),c.data,c.config.colors);

   vd.view->setConfig(c);

   int success;
   success=connect(vd.view,SIGNAL(timeChanged(int)),this,SLOT(on_View_timeChanged(int)));
   success=connect(vd.view,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   success=connect(vd.view,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}
/**
  \brief Delete all the signal views, free the widgets
**/
void MainWindow::DeleteScopeViews()
{
   printf("---- DeleteScopeViews\n");
   int numviews=GetNumScopeViews();
   printf("---- DeleteScopeViews. Numviews: %d\n",numviews);
   for(int i=0;i<numviews;i++)
   {
      printf("Deleting view %d, %p\n",i,GetScopeViewData(GetNumScopeViews()-1).view);
      signalVerticalLayout->removeWidget(GetScopeViewData(GetNumScopeViews()-1).view);
      delete GetScopeViewData(GetNumScopeViews()-1).view;
      RemoveScopeViewData(GetNumScopeViews()-1);
   }
}

/**
  \brief Adds a video.

  If the video file is not found, this function reports it but nevertheless creates the GUI elements.

  \warning: Phonon's ability to render the video isn't tested. File existence is tested outside of Phonon
**/
void MainWindow::AddVideoView(QString file)
{
   // Opens in one existing widged
   VIDEOVIEWDATA vd;

   // Test file existence
   QDir dir;
   if(!dir.exists(file))
   {
      QMessageBox::critical(this,"Load video file",QString("File '%1' does not exist").arg(file));
   }

   // Keep filename
   vd.file = file;
   vd.centersample=0;

   // Create the video view
   vd.view = new SSViewVideo(this);
   vd.view->setConfig(file);

   // Add the frame to the video scroll area
   videoLayout->addWidget(vd.view);

   // Keep data
   AddVideoViewData(vd);


   // Add various video handlers
   connect(vd.view,SIGNAL(timeChanged(int)),this,SLOT(on_View_timeChanged(int)));
   connect(vd.view,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   connect(vd.view,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}
/**
   \brief Remove all the video views
**/
void MainWindow::DeleteVideoViews()
{
   int numviews=GetNumVideoViews();
   for(unsigned i=0;i<numviews;i++)
   {
      printf("Deleting video view %d\n",i);
      delete GetVideoViewData(GetNumScopeViews()-1).view;
      RemoveVideoViewData(GetNumVideoViews()-1);
   }

}

/**
  \brief Adds a stickman

**/
void MainWindow::AddStickmanView(STICKMANVIEWDATA &sd)
{
   printf("In addstickmanview\n");

   // Create a stickman view
   sd.view = new SSViewStickman(this);
   // Keep data
   AddStickmanViewData(sd);
   // Configuration
   SSViewStickmanConfig c;
   PrepareDataPlotStickman(VIEWID(VIEWTYPESTICKMAN,GetNumStickmanViews()-1),c.data_torso,c.data_lua,c.data_lla,c.data_rua,c.data_rla);
   sd.view->setConfig(c);
   // Add the frame to the video scroll area
   videoLayout->addWidget(sd.view);
   // Add various video handlers
   connect(sd.view,SIGNAL(timeChanged(int)),this,SLOT(on_View_timeChanged(int)));
   connect(sd.view,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   connect(sd.view,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}


void MainWindow::DeleteStickmanViews()
{
   printf("---- DeleteStickmanViews");
   int numviews=GetNumStickmanViews();
   printf("---- DeleteStickmanViews. Numviews: %d\n",numviews);
   for(int i=0;i<numviews;i++)
   {
      printf("Deleting view %d, %p\n",i,GetStickmanViewData(GetNumStickmanViews()-1).view);
      videoLayout->removeWidget(GetStickmanViewData(GetNumStickmanViews()-1).view);
      delete GetStickmanViewData(GetNumStickmanViews()-1).view;
      RemoveStickmanViewData(GetNumStickmanViews()-1);
   }
}

void MainWindow::AddScatterView(SCATTERVIEWDATA &sd)
{
   printf("In addscatterview\n");

   // Create a scatter view
   sd.view = new SSViewScatter(this);
   // Keep data
   AddScatterViewData(sd);

   // Configuration
   SSViewScatterConfig sc;
   sc.config = sd.config;

   printf("Calling PrepareDataPlotScatter. svd.s: %d\n",GetNumScatterViews());
   PrepareDataPlotScatter(VIEWID(VIEWTYPESCATTER,GetNumScatterViews()-1),sc.data_x,sc.data_y,sc.config.colors);

   printf("Calling sd.view->setConfig\n");

   sd.view->setConfig(sc);


   // Add the frame to the video scroll area
   videoLayout->addWidget(sd.view);

   // Add various video handlers
   connect(sd.view,SIGNAL(timeChanged(int)),this,SLOT(on_View_timeChanged(int)));
   connect(sd.view,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   connect(sd.view,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}
void MainWindow::DeleteScatterViews()
{
   printf("---- DeleteScatterViews");
   int numviews=GetNumScatterViews();
   printf("---- DeleteScatterViews. Numviews: %d\n",numviews);
   for(int i=0;i<numviews;i++)
   {
      printf("Deleting view %d, %p\n",i,GetScatterViewData(GetNumScatterViews()-1).view);
      videoLayout->removeWidget(GetScatterViewData(GetNumScatterViews()-1).view);
      delete GetScatterViewData(GetNumScatterViews()-1).view;
      RemoveScatterViewData(GetNumScatterViews()-1);
   }

}


void MainWindow::AddLabelView(LABELVIEWDATA &vd)
{
   vd.view = new SSViewLabel();

   // Config
   signalVerticalLayout->addWidget(vd.view);
   AddLabelViewData(vd);


   SSViewLabelConfig sc;
   sc.config = vd.config;

   PrepareDataPlotLabel(VIEWID(VIEWTYPELABEL,GetNumLabelViews()-1),sc.data,sc.config.colors);

   vd.view->setConfig(sc);

   int success;
   success=connect(vd.view,SIGNAL(timeChanged(int)),this,SLOT(on_View_timeChanged(int)));
   success=connect(vd.view,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   success=connect(vd.view,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));
}

void MainWindow::DeleteLabelViews()
{
   printf("---- DeleteLabelViews\n");
   int numviews=GetNumLabelViews();
   printf("---- DeleteLabelViews. Numviews: %d\n",numviews);
   for(int i=0;i<numviews;i++)
   {
      printf("Deleting view %d, %p\n",i,GetLabelViewData(GetNumLabelViews()-1).view);
      signalVerticalLayout->removeWidget(GetLabelViewData(GetNumLabelViews()-1).view);
      delete GetLabelViewData(GetNumLabelViews()-1).view;
      RemoveLabelViewData(GetNumLabelViews()-1);
   }
}

/**
   \brief Set min/max range of signal sliders according to source length
**/

void MainWindow::AdjustSignalViewSlider()
{
   for(unsigned int i=0;i<GetNumScopeViews();i++)
   {
      GetScopeViewData(i).view->lengthChanged();
      //ScopeViewData[i].slider->setRange(0,SignalData[ScopeViewData[i].source].data[0].size());
   }
}









/**
  \brief Prepares the data for a scope view
**/

void MainWindow::PrepareDataPlotScope(VIEWID view,vector<vector<int> *> &vd,vector<unsigned> &vc)
{
   assert(view.viewtype==VIEWTYPESCOPE);
   assert(GetScopeViewData(view.viewnum).source<SignalData.size());

   // Get the traces
   vd = SignalData[GetScopeViewData(view.viewnum).source].getRef(GetScopeViewData(view.viewnum).traces);
   // Must ensure that the colors are properly set - this shouldn't be here anymore actually.
   if(vd.size() != GetScopeViewData(view.viewnum).config.colors.size())
      vc = std::vector<unsigned>(vd.size(),0xffffffff);        // Default to white if the number of specified colors doesn't match
   else
      vc =GetScopeViewData(view.viewnum).config.colors;
}
/**
  \brief Prepares the data for a label view
  TODO: this is too similar to PrepareDataPlotScope - find a way to merge
**/

void MainWindow::PrepareDataPlotLabel(VIEWID view,vector<vector<int> *> &vd,vector<unsigned> &vc)
{
   assert(view.viewtype==VIEWTYPELABEL);
   assert(GetLabelViewData(view.viewnum).source<SignalData.size());

   // Get the traces
   vd = SignalData[GetLabelViewData(view.viewnum).source].getRef(GetLabelViewData(view.viewnum).traces);
   // Must ensure that the colors are properly set - this shouldn't be here anymore actually.
   if(vd.size() != GetLabelViewData(view.viewnum).config.colors.size())
      vc = std::vector<unsigned>(vd.size(),0xffffffff);        // Default to white if the number of specified colors doesn't match
   else
      vc =GetLabelViewData(view.viewnum).config.colors;
}
/**
  \brief Prepares the data for a scatter view
  TODO: this is too similar to PrepareDataPlotScope - find a way to merge

  view: view number
**/
void MainWindow::PrepareDataPlotScatter(VIEWID view,vector<vector<int> *> &datax,vector<vector<int> *> &datay,vector<unsigned> &vc)
{
   assert(view.viewtype==VIEWTYPESCATTER);
   assert(GetScatterViewData(view.viewnum).source<SignalData.size());

   // Get the traces
   datax = SignalData[GetScatterViewData(view.viewnum).source].getRef(GetScatterViewData(view.viewnum).x);
   datay = SignalData[GetScatterViewData(view.viewnum).source].getRef(GetScatterViewData(view.viewnum).y);
   // Must ensure that the colors are properly set - this shouldn't be here anymore actually.
   if(datax.size() != GetScatterViewData(view.viewnum).config.colors.size())
      vc = std::vector<unsigned>(datax.size(),0xffffffff);        // Default to white if the number of specified colors doesn't match
   else
      vc =GetScatterViewData(view.viewnum).config.colors;
}

void MainWindow::PrepareDataPlotStickman(VIEWID view,TRACESRI &_torsorot,TRACESRI &_luarot,TRACESRI &_llarot,TRACESRI &_ruarot,TRACESRI &_rlarot)
{
   printf("in PrepareDataPlotStickman\n");
   printf("view: %d %d\n",static_cast<int>(view.viewtype),view.viewnum);
   printf("num views: %d\n",GetNumStickmanViews());
   assert(view.viewtype==VIEWTYPESTICKMAN);
   assert(GetStickmanViewData(view.viewnum).source<SignalData.size());

   printf("PrepareDataPlotStickman after asserts\n");

   _torsorot = {0,0,0,0};
   _luarot = {0,0,0,0};
   _llarot = {0,0,0,0};
   _ruarot = {0,0,0,0};
   _rlarot = {0,0,0,0};

   // Get the traces
   _torsorot = SignalData[GetStickmanViewData(view.viewnum).source].getRef(GetStickmanViewData(view.viewnum).trace_torso);
   _luarot = SignalData[GetStickmanViewData(view.viewnum).source].getRef(GetStickmanViewData(view.viewnum).trace_lua);
   _llarot = SignalData[GetStickmanViewData(view.viewnum).source].getRef(GetStickmanViewData(view.viewnum).trace_lla);
   _ruarot = SignalData[GetStickmanViewData(view.viewnum).source].getRef(GetStickmanViewData(view.viewnum).trace_rua);
   _rlarot = SignalData[GetStickmanViewData(view.viewnum).source].getRef(GetStickmanViewData(view.viewnum).trace_rla);
}

/**
  \brief Set the scroll bar positions of the signal views to the 'centersample' value of the sources

  Redraws all the signal/video views, or only selected ones specified by vid and sid.
**/
void MainWindow::SetAllSliderPosition(bool all,vector<int> vid,vector<int> sid)
{
   // Iterate all the videos
   for(unsigned i=0;i<GetNumVideoViews();i++)
   {
      if(all || std::find(vid.begin(),vid.end(),i)!=vid.end())
      {
         // Only seek the video if we update all, or if the video id is in vid.
         GetVideoViewData(i).view->setTime(GetVideoViewData(i).centersample);
      }
   }
   // Iterate all the scopes
   for(unsigned i=0;i<GetNumScopeViews();i++)
   {
      if(all || std::find(sid.begin(),sid.end(),GetScopeViewData(i).source)!=sid.end())
      {
         // Only seek the signal if we update all, or if the signal id is in sid.
         GetScopeViewData(i).view->setTime(SignalData[GetScopeViewData(i).source].centersample);
      }
   }
   // Iterate all the stickmans
   for(unsigned i=0;i<GetNumStickmanViews();i++)
   {
      if(all || std::find(sid.begin(),sid.end(),GetStickmanViewData(i).source)!=sid.end())
      {
         // Only seek the signal if we update all, or if the signal id is in sid.
         GetStickmanViewData(i).view->setTime(SignalData[GetStickmanViewData(i).source].centersample);
      }
   }
   // Iterate all the scatters
   for(unsigned i=0;i<GetNumScatterViews();i++)
   {
      if(all || std::find(sid.begin(),sid.end(),GetScatterViewData(i).source)!=sid.end())
      {
         // Only seek the signal if we update all, or if the signal id is in sid.
         GetScatterViewData(i).view->setTime(SignalData[GetScatterViewData(i).source].centersample);
      }
   }
   // Iterate all the labels
   for(unsigned i=0;i<GetNumLabelViews();i++)
   {
      if(all || std::find(sid.begin(),sid.end(),GetLabelViewData(i).source)!=sid.end())
      {
         // Only seek the signal if we update all, or if the signal id is in sid.
         GetLabelViewData(i).view->setTime(SignalData[GetLabelViewData(i).source].centersample);
      }
   }
}

/**
  \brief Handles repaint and synchronization of views upon slider activity
  sourceidx: uid
**/
void MainWindow::on_Source_Slided(int sourceidx,int value)
{
   //printf("On source slided %d -> %d\n",sourceidx,value);
   assert(sourceidx!=-1);

   std::map<int,int> offsetout;
   ComputeAllSourceOffsetFrom(RelationsActive,sourceidx,value,offsetout);

   // Status bar update
   SetSBLabelPointer(sourceidx);

   vector<int> vid,sid;    // Memorize which sliders are affected by the linking

   // Set the center sample value - but do not effect it yet.
   for(std::map<int,int>::iterator it = offsetout.begin();it!=offsetout.end();it++)
   {
      SetCenterSample(it->first,it->second);
      if(IsIdxVideo(it->first))
         vid.push_back(IDX2VID(it->first));
      else
         sid.push_back(IDX2SID(it->first));
   }

   // Effect center sample only on affected signals
   SetAllSliderPosition(false,vid,sid);
}








/**
  \brief Export Matlab resample code
**/
void MainWindow::on_actionExport_Matlab_resample_code_triggered()
{
   Resample(false);
}
/**
  \brief In-place resample
**/
void MainWindow::on_action_Resample_triggered()
{
   Resample(true);
}
/**
  \brief Handles the high-level GUI resample; asking user options, creating the resample logic, and applying it.
**/
void MainWindow::Resample(bool inplace)
{
   bool prepend,postpend;


   // Find disconnected graphs
   vector<RELATIONS> graphs = FindGraphs(RelationsActive);
   // Find the resample logic - only to check if there are signal relations available.
   map<int,RESAMPLEREL> m = ResampleLogic(graphs,prepend);
   //printf("m size: %d\n",m.size());

   if(m.size()==0)
   {
      QMessageBox::information(this,"Resample","No signals to resample: first, link the signals that must be jointly resampled.");
      return;
   }


   ResampleDialog *rdlg = new ResampleDialog(this);
   if(inplace)
      rdlg->SetC();
   else
      rdlg->SetMatlab();
   if(rdlg->exec() == QDialog::Rejected)
      return;
   int strategy = rdlg->getStrategy();
   int method = rdlg->getMethod();
   delete rdlg;

   switch(strategy)
   {
   case 2:
      prepend=true;
      postpend=true;
      break;
   case 1:
      prepend=true;
      postpend=false;
      break;
   default:
      prepend=false;
      postpend=false;
   }

   // Re-run the resample logic, this time with the user defined parameters.
   m = ResampleLogic(graphs,prepend);

   // Apply the logic
   if(inplace)
   {
      // keep undo data
      SignalDataUndo=SignalData;
      RefPointsUndo=RefPoints;
      SetUndoable(true);
      ResampleInPlace(m,prepend,postpend);
      AdjustSignalViewSlider();
   }
   else
      ResampleMatlab(m,method==0 ? true : false,prepend,postpend);
}

/**
  \brief Prepares to merge the files. Ensures the sources to merge are all of identical length.
   If they are of identical length assume they were resampled.

**/
void MainWindow::MergeFiles()
{
   // Find disconnected graphs
   vector<RELATIONS> graphs = FindGraphs(RelationsActive);

   // Find the edges.
   GRAPHSRC GraphSrc,GraphSrcOk,GraphSrcNOk;
   GRAPHSRC GraphToSave;

   // Iterate each disconnected graph, and extracts&sort within that graph the connected edges.
   // Discard video edges, and converts IDX to Signal IDS
   for(unsigned i=0;i<graphs.size();i++)
   {
      vector<int> edges;

      if(IsIdxSignal(graphs[i][0].x1))
         edges.push_back(IDX2SID(graphs[i][0].x1));
      for(unsigned j=0;j<graphs[i].size();j++)
         if(IsIdxSignal(graphs[i][j].x2))
            edges.push_back(IDX2SID(graphs[i][j].x2));
      // Sort the edges
      sort(edges.begin(),edges.end());

      GraphSrc.push_back(edges);
   }

   // Check the graphs, to make sure within a graph all sources have the same length
   for(unsigned i=0;i<GraphSrc.size();i++)
   {
      int ml;
      bool ok=true;
      ml = SignalData[GraphSrc[i][0]].data[0].size();
      for(unsigned  j=0;j<GraphSrc[i].size();j++)
      {
         if(SignalData[GraphSrc[i][j]].data[0].size() != ml)
         {
            ok=false;
            break;
         }
      }
      if(ok)
         GraphSrcOk.push_back(GraphSrc[i]);
      else
         GraphSrcNOk.push_back(GraphSrc[i]);
   }

   //

   // Reports an information box if some selected sources are of different sizes, thus preventing a merge.
   if(GraphSrcNOk.size()!=0)
   {
      QString str="A number of linked signal sources have different lengths. This prevents these sources from being merged.\n\n";
      str+="Resample the linked sources before merging them.\n\n";
      str+="The following linked sources are concerned:\n";
      for(unsigned  i=0;i<GraphSrcNOk.size();i++)
      {
         str += GraphToString(GraphSrcNOk[i]);
         str+="\n";
      }
      str+="\nThese sources are discarded from the merge operation.\n";
      QMessageBox::information(this,"Merge sources",str);
   }
   if(GraphSrcOk.size()==0)
   {
      QMessageBox::information(this,"Merge sources","No signal sources to merge.");
      return;
   }

   SaveSyncFileDialog *dlg = new SaveSyncFileDialog(this);
   //dlg->SetGraphs(graphs);
   dlg->SetGraphs(GraphSrcOk);
   int rv = dlg->exec();
   GraphToSave = dlg->GetSaveOrder();
   delete dlg;

   if(rv)
   {
      // User asked to save one or more graphs.
      // Print them out.

      for(unsigned i=0;i<GraphToSave.size();i++)        // Iterate all the graphs
      {
         //printf("Saving graph %d\n",i);
         //printf("\t %s\n",GraphToString(GraphToSave[i]).toStdString().c_str());

         // Create some big signal stuff
         int nc=0;      // Number of channels in the merged data.
         int length;    // Lenght of the data to merge
         for(unsigned  j=0;j<GraphToSave[i].size();j++)
            nc += SignalData[GraphToSave[i][j]].data.size();
         length = SignalData[GraphToSave[i][0]].data[0].size();

         //printf("Resulting merged data will be %d channels and %d samples\n",nc,length);
         vector<vector<int> > newsignal(nc,vector<int>(length,0));

         // Fill the merged signal
         int cc=0;                                                            // Current channel
         for(unsigned  src=0;src<GraphToSave[i].size();src++)                       // Iterate all the sources within graph i
         {
            for(unsigned  c=0;c<SignalData[GraphToSave[i][src]].data.size();c++)    // Iterate all the channels in source src of graph i
            {
               for(unsigned  s=0;s<length;s++)                                      // Iterate all the samples
                  newsignal[cc][s] = SignalData[GraphToSave[i][src]].data[c][s];
               cc++;
            }
         }
         QString str=QString("Save merged sources ");
         str += GraphToString(GraphToSave[i]);

         SaveSignal(newsignal,str);
      }
   }
}



/**
  \brief Creates the signal views from the view definition string.
**/
void MainWindow::on_pushButton_ApplyView_clicked()
{
   SetupViews(ui->lineEdit_View->toPlainText().toStdString());
}
void MainWindow::SetupViews(string layout)
{
   // Preemptively delete all the views
   DeleteScopeViews();
   DeleteStickmanViews();
   DeleteScatterViews();
   DeleteLabelViews();


   CurrentLayoutString = layout;

   bool iserr;
   QString err;
   SCOPEVIEWDATAS scopevd;
   STICKMANVIEWDATAS stickmanvd;
   SCATTERVIEWDATAS scattervd;
   LABELVIEWDATAS labelvd;
   int rv = ParseLayoutString(layout,scopevd,stickmanvd,scattervd,labelvd);
   printf("ParseLayoutString returned: %d. %X\n",rv,-rv);

   printf("sizes of returned structures: %d %d %d %d\n",scopevd.size(),stickmanvd.size(),scattervd.size(),labelvd.size());

   //
   for(int i=0;i<scopevd.size();i++)
   {
      printf("scoped %d. %d\n",i,scopevd[i].config.colors.size());
   }

   if(rv!=0)
   {
      QString str;
      if(rv==-1)
         QMessageBox::information(this,"Syntax error in layout string","Cannot parse the layout string");
      else
      {
         str.sprintf("Cannot parse the view definition %d",(-rv)&0xff);
         switch(((-rv)>>8)&0xff)
         {
         default:
            str+=". View type unknown.";
            break;
         case 1:
            str+=" of type scope.";
            break;
         case 2:
            str+=" of type stickman.";
            break;
         case 3:
            str+=" of type scatter.";
            break;
         case 4:
            str+=" of type label.";
            break;
         }
         QMessageBox::information(this,"Syntax error in layout string",str);
      }
      return;
   }



   // Check that all the views refer to available signal sources and channels.


   err="The following scope views refer to nonexistant sources or channels:\n\n";

   SCOPEVIEWDATAS scopevdvalid;
   iserr=false;
   for(unsigned i=0;i<scopevd.size();i++)
   {
      if(scopevd[i].source<SignalData.size())
      {
         // The view refers to a valid source. Check if all the traces refer to valid channels
         bool invalid=false;
         for(unsigned t=0;t<scopevd[i].traces.size();t++)
         {
            if(scopevd[i].traces[t]>=SignalData[scopevd[i].source].data.size())
               invalid=true;
         }
         if(!invalid)
         {
            // The view is valid. Store it as valid
            scopevdvalid.push_back(scopevd[i]);
         }
         else
         {
            // Report the error
            iserr=true;
            err+=QString(" Scope view %1: nonexistant traces in source %2\n").arg(i).arg(scopevd[i].source);
         }
      }
      else
      {
         // The view refers to an unavailable source
         iserr=true;
         err+=QString(" Scope view %1: nonexistant source %2\n").arg(i).arg(scopevd[i].source);
      }
   }
   if(iserr)
   {
      err+=QString("\nInvalid views are discarded.\n");
      QMessageBox::information(this,"Error in view definitions",err);
   }

   // Same with the stickman
   err="The following stickman views refer to nonexistant sources or channels:\n\n";
   STICKMANVIEWDATAS stickmanvdvalid;
   iserr=false;
   for(unsigned i=0;i<stickmanvd.size();i++)
   {
      if(stickmanvd[i].source<SignalData.size())
      {
         // The view refers to a valid source. Check if all the traces refer to valid channels
         bool invalid=false;
         for(unsigned t=0;t<4;t++)
         {
            if(stickmanvd[i].trace_torso[t]>=SignalData[stickmanvd[i].source].data.size())
               invalid=true;
            if(stickmanvd[i].trace_rua[t]>=SignalData[stickmanvd[i].source].data.size())
               invalid=true;
            if(stickmanvd[i].trace_rla[t]>=SignalData[stickmanvd[i].source].data.size())
               invalid=true;
            if(stickmanvd[i].trace_lua[t]>=SignalData[stickmanvd[i].source].data.size())
               invalid=true;
            if(stickmanvd[i].trace_lla[t]>=SignalData[stickmanvd[i].source].data.size())
               invalid=true;
         }
         if(!invalid)
         {
            // The view is valid. Store it as valid
            stickmanvdvalid.push_back(stickmanvd[i]);
         }
         else
         {
            // Report the error
            iserr=true;
            err+=QString(" Stickman view %1: nonexistant traces in source %2\n").arg(i).arg(stickmanvd[i].source);
         }
      }
      else
      {
         // The view refers to an unavailable source
         iserr=true;
         err+=QString(" Stickman view %1: nonexistant source %2\n").arg(i).arg(stickmanvd[i].source);
      }
   }
   if(iserr)
   {
      err+=QString("\nInvalid views are discarded.\n");
      QMessageBox::information(this,"Error in view definitions",err);
   }

   // Same with scatter
   err="The following scatter views refer to nonexistant sources or channels:\n\n";
   SCATTERVIEWDATAS scattervdvalid;
   iserr=false;
   for(unsigned i=0;i<scattervd.size();i++)
   {
      if(scattervd[i].source<SignalData.size())
      {
         // The view refers to a valid source. Check if all the traces refer to valid channels
         bool invalid=false;
         for(unsigned t=0;t<scattervd[i].x.size();t++)
         {
            if(scattervd[i].x[t]>=SignalData[scattervd[i].source].data.size())
               invalid=true;
            if(scattervd[i].y[t]>=SignalData[scattervd[i].source].data.size())
               invalid=true;
         }
         if(!invalid)
         {
            // The view is valid. Store it as valid
            scattervdvalid.push_back(scattervd[i]);
         }
         else
         {
            // Report the error
            iserr=true;
            err+=QString(" Scatter view %1: nonexistant traces in source %2\n").arg(i).arg(scattervd[i].source);
         }
      }
      else
      {
         // The view refers to an unavailable source
         iserr=true;
         err+=QString(" Scatter view %1: nonexistant source %2\n").arg(i).arg(scattervd[i].source);
      }
   }
   if(iserr)
   {
      err+=QString("\nInvalid views are discarded.\n");
      QMessageBox::information(this,"Error in view definitions",err);
   }


   LABELVIEWDATAS labelvdvalid;
   iserr=false;
   for(unsigned i=0;i<labelvd.size();i++)
   {
      if(labelvd[i].source<SignalData.size())
      {
         // The view refers to a valid source. Check if all the traces refer to valid channels
         bool invalid=false;
         for(unsigned t=0;t<labelvd[i].traces.size();t++)
         {
            if(labelvd[i].traces[t]>=SignalData[labelvd[i].source].data.size())
               invalid=true;
         }
         if(!invalid)
         {
            // The view is valid. Store it as valid
            labelvdvalid.push_back(labelvd[i]);
         }
         else
         {
            // Report the error
            iserr=true;
            err+=QString(" Label view %1: nonexistant traces in source %2\n").arg(i).arg(labelvd[i].source);
         }
      }
      else
      {
         // The view refers to an unavailable source
         iserr=true;
         err+=QString(" Label view %1: nonexistant source %2\n").arg(i).arg(labelvd[i].source);
      }
   }
   if(iserr)
   {
      err+=QString("\nInvalid views are discarded.\n");
      QMessageBox::information(this,"Error in view definitions",err);
   }


   // Create the desired views

   for(unsigned i=0;i<scopevdvalid.size();i++)
      AddScopeView(scopevdvalid[i]);
   for(unsigned i=0;i<stickmanvdvalid.size();i++)
      AddStickmanView(stickmanvdvalid[i]);
   for(unsigned i=0;i<scattervdvalid.size();i++)
      AddScatterView(scattervdvalid[i]);
   for(unsigned i=0;i<labelvdvalid.size();i++)
      AddLabelView(labelvdvalid[i]);



}




/**
  \brief Ordinary Least Square Regression

  Finds a and b so that: x2 = b*x1 + a

  Returns false if regression is not possible, true otherwise.
**/
bool MainWindow::OLSQRegress(std::vector<double> x1,std::vector<double> x2, double &a, double &b)
{
   // Mean
   double x1m,x2m;
   x1m=x2m=0;
   for(int i=0;i<x1.size();i++)
   {
      x1m+=x1[i];
      x2m+=x2[i];
   }
   x1m/=(double)x1.size();
   x2m/=(double)x2.size();


   // b^
   b=0;
   double bn,bd;
   bn = 0;
   for(unsigned i=0;i<x1.size();i++)
      bn+=(x1[i]-x1m)*(x2[i]-x2m);
   bd = 0;
   for(unsigned i=0;i<x1.size();i++)
      bd+=pow(x1[i]-x1m,2.0);
   if(bd==0)
      return false;
   b = bn/bd;

   // a^
   a = x2m-b*x1m;
   return true;
}
// x2 is y
bool MainWindow::OrthogonalLSQRegression(std::vector<double> x1,std::vector<double> x2, double &a, double &b)
{
   // Mean
   double x1m,x2m;

   int n = x1.size();
   if(n==0)
      return false;

   x1m=x2m=0;
   for(int i=0;i<n;i++)
   {
      x1m+=x1[i];
      x2m+=x2[i];
   }
   x1m/=(double)n;
   x2m/=(double)n;


   double Bn=0;
   for(int i=0;i<n;i++)
   {
      Bn+=pow(x2[i],2.0);
      Bn-=pow(x1[i],2.0);
   }
   Bn -= ((double)n) * pow(x2m,2.0);
   Bn += ((double)n) * pow(x1m,2.0);

   double Bd=0;
   double sx1=0,sx2=0,sx1x2=0;
   for(int i=0;i<n;i++)
   {
      sx1 += x1[i];
      sx2 += x2[i];
      sx1x2 += x1[i]*x2[i];
   }
   Bd = (1.0/(double)n) * sx1 * sx2 - sx1x2;


   if(Bd==0)
      return false;

   double B = 0.5 * Bn / Bd;



   double b1,b2;
   double a1,a2;


   if(1.0+B*B<0)
      return false;

   //   printf("x1m %lf x2m %lf sx1 %lf sx2 %lf sx1x2 %lf\n",x1m,x2m,sx1,sx2,sx1x2);
   //   printf("Bn %lf Bd %lf B %lf\n",Bn,Bd,B);

   b1 = -B + sqrt(1.0+B*B);
   b2 = -B - sqrt(1.0+B*B);

   a1 = x2m - b1 * x1m;
   a2 = x2m - b2 * x1m;



   //printf("Option 1: x2 = x1 * %lf + %lf\n",b1,a1);
   //printf("Option 2: x2 = x1 * %lf + %lf\n",b2,a2);

   // Need to decide between the two linear fit.

   // We return the positive slope. This assumes there is no negative correlation in the input data.
   if(b1<0)
   {
      a=a2;
      b=b2;
   }
   else
   {
      a=a1;
      b=b1;
   }




   return true;
}



/**
  \brief Compute the signal relations from the reference points
**/
void MainWindow::ComputeRelations()
{
   // Do a regression test.
   double a,b;
   std::vector<double> x1,x2;

   RELATIONS RelationsNew;
   RELATIONS RelError;
   RELATIONS RevRelError;
   QString strign="These relation(s) are ignored. Check reference points consistency.\n";
   QString strlinkerror="Link equation(s) error";

   RelationsAvailable.clear();

   // Exhaustive search
   for(int s1=0;s1<GetNumSources();s1++)
   {
      for(int s2=0;s2<GetNumSources();s2++)
      {
         //printf("******************** LSQ %d %d\n",s1,s2);
         x1.clear();
         x2.clear();
         // Collect all the source points in x1,x2
         for(unsigned i=0;i<RefPoints.size();i++)
         {
            if(RefPoints[i][0]==s1 && RefPoints[i][2]==s2)
            {
               x1.push_back(RefPoints[i][1]);
               x2.push_back(RefPoints[i][3]);
            }
            if(RefPoints[i][0]==s2 && RefPoints[i][2]==s1)
            {
               x1.push_back(RefPoints[i][3]);
               x2.push_back(RefPoints[i][1]);
            }
         }
         // Regression via ordinary least squares
         //if(OLSQRegress(x1,x2,a,b))
         if(OrthogonalLSQRegression(x1,x2,a,b))
         {
            //printf("x%d=x%d %.3lf + %.3lf\n",s2,s1,b,a);
            // Store the relation
            RELATION r;
            r.x1 = s1;
            r.x2 = s2;
            r.b = b;
            r.a = a;
            r.s1 = IsIdxSignal(r.x1);
            r.s2 = IsIdxSignal(r.x2);
            if(r.s1)
               r.tx1=IDX2SID(r.x1);
            else
               r.tx1=IDX2VID(r.x1);
            if(r.s2)
               r.tx2=IDX2SID(r.x2);
            else
               r.tx2=IDX2VID(r.x2);

            // Ensure no negative relations
            if(r.b<0)
               RelError.push_back(r);
            else
               RelationsNew.push_back(r);       // Keep erroneous relations - report at the end in a single shot.

            //printf("Ordinary LSQ: x2 = x1 * %lf + %lf\n",r.b,r.a);
            //printf("Orthogonal LSQ: x%d = x%d * %lf + %lf\n",s2,s1,r.b,r.a);
         }
         /*else
         {
            printf("Orthogonal LSQ failed\n");
         }*/
         // Try the ortogonal
         //printf("Orthogonal LSQ\n");
         //bool suc=OrthogonalLSQRegression(x1,x2,a,b);
         //if(suc==false) printf("orthogonal LSQ failed\n");


      }
   }
   // Check irreversible relations
   bool revrelok;
   for(RELATIONS::iterator r1=RelationsNew.begin();r1!=RelationsNew.end();r1++)
   {
      // For relation r1.x1->r1.x2, a reversible relation r2.x2->r2.x1 must exist
      revrelok=false;
      for(RELATIONS::iterator r2=RelationsNew.begin();r2!=RelationsNew.end();r2++)
      {
         if(r1->x1 == r2->x2 && r1->x2 == r2->x1)
         {
            revrelok=true;
            break;
         }
      }
      if(revrelok==false)
      {
         // No reversible relation found, report it.
         RevRelError.push_back(*r1);
      }
   }
   // Remove the non-reversible relations.
   if(RevRelError.size()!=0)
   {
      // Iterator on erroneous relations
      RELATIONS::iterator re = RevRelError.begin();

      // Iterate new relations
      for(RELATIONS::iterator r1=RelationsNew.begin();r1!=RelationsNew.end();r1++)
      {
         if( re!=RevRelError.end() && r1->x1 == re->x1 && r1->x2 == re->x2 )
         {
            // r1 is an erroneous relation; we don't add it to available relations, and we look for the next erroneous relation
            re++;    // This is correct as erroneous relations are stored in parallel to RelationsNew.
         }
         else
         {
            // r1 is a valid relation.
            RelationsAvailable.push_back(*r1);
         }
      }
   }
   else
      RelationsAvailable = RelationsNew;

   // Report errors
   QString strerr;
   if(RelError.size())
   {
      strerr+="The following signal links are invalid (negative correlation):\n";
      for(RELATIONS::iterator r=RelError.begin();r!=RelError.end();r++)
      {
         QString str2;
         str2.sprintf("x%d = x%d * %lf + %lf.\n",r->x2,r->x1,r->b,r->a);
         strerr+=str2;
      }
   }
   if(RevRelError.size())
   {
      strerr+="The following signal links are not reversible:\n";
      for(RELATIONS::iterator r=RevRelError.begin();r!=RevRelError.end();r++)
      {
         QString str2;
         str2.sprintf("x%d - x%d\n",r->x1,r->x2);
         strerr+=str2;
      }
   }

   if(RelError.size()!= 0 || RevRelError.size()!=0)
      QMessageBox::information(this,strlinkerror,strerr+strign);

   /*printf("Checking if relations are okay\n");
   RELATIONS::iterator it;
   for(it=RelationsAvailable.begin();it!=RelationsAvailable.end();it++)
   {
      printf("x%d = x%d * %.1lf + %1lf\n",it->x2,it->x1,it->b,it->a);
   }*/

   // Update the user interface
   RefDialog->InitRelationsUI(SignalData.size(),GetNumVideoViews());
   RefDialog->SetRelationsAvailable(RelationsAvailable);
   UpdateRelationsActive();
}

void MainWindow::ClearActiveRelations()
{
   RefDialog->SetRelationsAvailable(RelationsAvailable);
}




void MainWindow::on_pushButton_LoadSource_clicked()
{
   //<signal;file.dat><video;file.avi><signal;file2.dat>
   //printf("Loading sources: %s\n",ui->lineEdit_Source->toPlainText().toStdString().c_str());

   SOURCEFILES src;
   int rv = ParseSourceString(ui->lineEdit_Source->toPlainText().toStdString(),src);
   if(rv!=0)
   {
      QString str;
      str.sprintf("Error number %d",rv);
      QMessageBox::information(this,"Syntaxt error in layout string",str);
      return;
   }


   /*printf("Signal sources:\n");
   for(int i=0;i<src[1].size();i++)
      printf("- %s\n",src[1][i].c_str());*/

   // Loading sources
   SignalData.clear();

   std::vector<std::vector<int> > v;
   int ok;
   bool anyerr=false;
   QString err;
   for(unsigned i=0;i<src[1].size();i++)
   {
      v = LoadSignalFile(src[1][i],ok);
      // Add a source:
      //printf("Add a source %d %s, ok: %d. Returned vector size: %d\n",i,src[1][i].c_str(),ok,v.size());
      if(ok==0 && v.size()!=0)
      {
         SIGNALDATA sd;
         sd.data = v;
         sd.centersample=0;
         sd.filename=QString(src[1][i].c_str());
         SignalData.push_back(sd);
      }
      else
      {
         if(ok==-1)
            err=QString("Cannot load %1: file not found.").arg(src[1][i].c_str());
         else
            err=QString("Cannot load %1: invalid file format.").arg(src[1][i].c_str());
         anyerr=true;
         break;
      }
   }
   // If there has been any error loading a file, deactivate the views, and return.
   if(anyerr)
   {
      SignalData.clear();
      QMessageBox::critical(this,"Load source",err+QString("\n\nData sources are discarded. Check source definitions and file format."));
      SetupViews("");
      return;
   }



   // Re-interpret the views

   SetupViews(ui->lineEdit_View->toPlainText().toStdString());
   AdjustSignalViewSlider();

   // Clear all the backup.
   SignalDataUndo.clear();
   RefPointsUndo.clear();
   SetUndoable(false);


   // Load the videos
   //printf("Delete video views\n");
   DeleteVideoViews();
   for(unsigned i=0;i<src[0].size();i++)
   {
      //printf("Add video %s\n",src[0][i].c_str());
      AddVideoView(QString(src[0][i].c_str()));
   }






   // Prune the reference points to make sure none refers to unavailable source.
   QString str;
   RefPoints=PruneRefPoints(RefPoints,str);
   SetReferencePoints(RefPoints);
   RefDialog->ClearLinks();


   // Update the splitter
   QList<int> s;
   if(GetNumVideoViews())
      s.push_back(1);
   else
      s.push_back(0);
   if(GetNumScopeViews())
      s.push_back(1);
   else
      s.push_back(0);
   signalVideoSplitter->setSizes(s);



}
int MainWindow::ParseSourceString(string str,SOURCEFILES &src)
{
   src.clear();
   src.resize(2);


   // Regexp for layout string:
   // Characters ;<> are excluded in the match of the expression for syntax checking.
   // Group 0: whole expression
   // Group 1: source type
   // Group 2: source file
   // Validation of the whole string: look for multiple expressions with a '(expression)*' regex string.
   string expression = string("\\s*<\\s*([^;^>^<]*)\\s*;\\s*([^;^>^<]*)\\s*>\\s*");

   QRegExp e(QString(expression.c_str()));
   //printf("e.isValid: %d\n",(int)e.isValid());
   //printf("e.isValid: %s\n",e.errorString().toStdString().c_str());
   string expression2=string("(")+expression+string(")*");
   QRegExp validation(QString(expression2.c_str()));
   //printf("validation.isValid: %d\n",(int)validation.isValid());
   //printf("validation.isValid: %s\n",e.errorString().toStdString().c_str());


   // Validation
   int iin=validation.exactMatch(QString(str.c_str()));
   if(iin==-1)
   {
      //printf("Validation match failed\n");
      return -5;
   }


   // Expression parsing
   e.indexIn(QString(str.c_str()));
   unsigned midx=0;
   int pos = 0;
   while((pos = e.indexIn(QString(str.c_str()), pos)) != -1)
   {
      pos += e.matchedLength();

      //printf("Capture group %d\n",midx);
      //printf("NumCaptures: %d\n",e.numCaptures());
      for(int i=0;i<e.numCaptures()+1;i++)
         //printf("Capture %d: %s\n",i,e.cap(i).toStdString().c_str());


         if(e.numCaptures()!=2)
         {
            //cout << "Invalid match size\n";
            return -9;
         }


      string sourcetype = e.cap(1).toStdString();
      if(sourcetype.compare("video")!=0 && sourcetype.compare("signal")!=0)
      {
         //cout << "Invalid source type\n";
         return -10;
      }
      string sourcefile = e.cap(2).toStdString();

      if(sourcetype.compare("video")==0)
         src[0].push_back(sourcefile);
      else
         src[1].push_back(sourcefile);


      midx++;
   }
   return 0;
}


/**
  \brief Load a data source from a file. Return format: data[channel][samples]

  rv: return value.
      0: success
      -1: file not found
      -2: invalid file content

   Parses signed integers space or tab separated. Handles special entries of the form 'NaN' and replaces them by zeroes.
**/
std::vector<std::vector<int> > MainWindow::LoadSignalFile(string file,int &rv, bool report)
{
   std::vector<std::vector<int> > data;

   rv=0;

   std::vector<int> d;

   QFile filein(QString(file.c_str()));
   if (filein.open(QIODevice::ReadOnly))
   {
      QByteArray filedata = filein.readAll();
      bool ok;
      data = parse(filedata.data(),ok);
      if(!ok)
      {
         rv=-2;
         return data;
      }

   }
   else
   {
      rv=-1;
      return data;
   }

   return data;
}

/**
  \brief Slot handling mouse button press in a scope or video view. Find view and pass to ViewClick
**/
void MainWindow::on_View_mousePressed(Qt::MouseButton button, int samplex)
{
   // Find which view has triggered the message.
   QObject *s = sender();

   // Find the corresponding source, view, etc
   SRCID srcid;
   VIEWID viewid;
   if(!GetSourceOfView((QWidget*)s,srcid,viewid))
   {
      qDebug("Mouse pressed in unknown view %p\n",s);
      return;
   }
   SetSBLabelPointer(srcid);
   SetSBLabelView(viewid);
   SourceClick(srcid.srcuid,button,samplex);                            // Convert signal to absolude ID
}
/**
  \brief Slot handling mouse move press in a scope or video view.
**/
void MainWindow::on_View_mouseMoved(int samplex)
{
   QObject *s = sender();

   // Find the corresponding source, view, etc
   SRCID srcid;
   VIEWID viewid;
   if(!GetSourceOfView((QWidget*)s,srcid,viewid))
   {
      qDebug("Mouse moved in unknown view %p\n",s);
      return;
   }

   QString str;
   //   if(IsIdxSignal(view))
   str.sprintf("Pointer position: %d",samplex);
   //   else
   //    str.sprintf("-");
   SB_LabelPosition->setText(str);
   SetSBLabelPointer(srcid);
   SetSBLabelView(viewid);
}

/**
  \brief Handles mouse button click
  source: IDX of the source.
**/
void MainWindow::SourceClick(int source,Qt::MouseButton button,int samplex)
{
   if(button==Qt::RightButton)
   {
      // Handle primary/secondary selection
      if(GetPrimarySource()==-1 && source!=GetSecondarySource())    // if no primary selected, and we didn't click on a secondary...
         source_primary=source;                             // ... set selected source as primary
      else
      {
         if(source==GetPrimarySource())                         // we might have clicked on a primary - it will become a secondary...
         {
            source_secondary=source;
            source_primary=-1;
         }
         else
         {
            if(source==GetSecondarySource())                    // we might have clicked on a secondary - it will be deselected
            {
               source_secondary=-1;
            }
            else
               source_secondary=source;                     // we have clicked on an unselected source but there is a primary
         }
      }

      VisualizePrimarySecondary();
   }
   if(button==Qt::LeftButton && IsIdxSignal(source))
   {
      // Center source on location
      if(samplex>=0 && samplex<SignalData[IDX2SID(source)].data[0].size())
      {
         on_Source_Slided(source,samplex);
      }
   }
}



/**
  \brief Visualize the primary and secondary sources by highlighting the respective scopes, and update the status bar.

**/
void MainWindow::VisualizePrimarySecondary()
{
   // Highlight the primary and secondary.
   QString s1,s2;
   s1 = QString("Pri: %1").arg(GetPrimarySource()==-1?QString("-"):IsIdxSignal(GetPrimarySource())?QString("S%1").arg(IDX2SID(GetPrimarySource())):QString("V%1").arg(IDX2VID(GetPrimarySource())));
   s2 = QString("Sec: %1").arg(GetSecondarySource()==-1?QString("-"):IsIdxSignal(GetSecondarySource())?QString("S%1").arg(IDX2SID(GetSecondarySource())):QString("V%1").arg(IDX2VID(GetSecondarySource())));
   SB_LabelPriSec->setText(s1+QString(" ")+s2);


   // Iterate all the viewtypes
   for(int vt = VIEWTYPEFIRST;vt<=VIEWTYPELAST;vt++)
   {
      for(unsigned i=0;i<GetNumViews(static_cast<VIEWTYPE>(vt));i++)
      {
         //printf("VT type %d num %d. focus: %d\n",vt,i,GetSSView(static_cast<VIEWTYPE>(vt),i)->hasFocus());
         if(vt == VIEWTYPEVIDEO)
         {
            if(VID2IDX(i) == GetPrimarySource())
            {
                GetVideoViewData(i).view->setBackground(SSVIEWBACKGROUNDPRIMARY);
            }
            else
            {
               if(VID2IDX(i) == GetSecondarySource())
               {
                   GetVideoViewData(i).view->setBackground(SSVIEWBACKGROUNDSECONDARY);
               }
               else
               {
                   GetVideoViewData(i).view->setBackground(SSVIEWBACKGROUNDNONE);
               }
            }
         }
         else
         {
            if(SID2IDX(GetViewData(VIEWID(static_cast<VIEWTYPE>(vt),i)).source) == GetPrimarySource())
            {
               GetSSView(static_cast<VIEWTYPE>(vt),i)->setBackground(SSVIEWBACKGROUNDPRIMARY);
            }
            else
            {
               if(SID2IDX(GetViewData(VIEWID(static_cast<VIEWTYPE>(vt),i)).source) == GetSecondarySource())
               {
                  GetSSView(static_cast<VIEWTYPE>(vt),i)->setBackground(SSVIEWBACKGROUNDSECONDARY);
               }
               else
               {
                  GetSSView(static_cast<VIEWTYPE>(vt),i)->setBackground(SSVIEWBACKGROUNDNONE);
               }
            }
         }
      }
   }
}


// idx: uid
void MainWindow::SetSBLabelPointer(int uid)
{
   //printf("Set sb label pointer: uid %d\n",uid);
   SRCID srcid;
   UID2SRCID(uid,srcid);
   SetSBLabelPointer(srcid);
}
void MainWindow::SetSBLabelPointer(SRCID idx)
{
   QString str1="Source under pointer: ";
   QString str2;

   switch(idx.srctype)
   {
   case SRCTYPESIGNAL:
      str1+=QString("S%1").arg(idx.srcid);
      // It crashes here
      /*
on_Slider_valueChanged 0E7C9440 -> 10
        source 0
idx.srcid: 2
svd[id].src: -1163005939

Seems the problem is idx.srcid
       */
      //printf("idx.srcid: %d\n",idx.srcid);
      //printf("svd[id].src: %d\n",ScopeViewData[idx.srcid].source);
      //printf("centersample: %d\n",SignalData[ScopeViewData[idx.srcid].source].centersample);
      str2=QString("Center sample: %1").arg(SignalData[GetScopeViewData(idx.srcid).source].centersample);
      break;
   case SRCTYPEVIDEO:
      str1+=QString("V%1").arg(idx.srcid);
      str2=QString("Frame time: %1").arg(GetVideoViewData(idx.srcid).view->getTime());
      break;
   default:
      str1+=QString("UNKNOWN%1").arg(idx.srcid);
      str2+=QString("UNKNOWN");
   }
   str1+=QString(" UID%1").arg(idx.srcuid);

   SB_LabelSource->setText(str1);
   SB_LabelCenter->setText(str2);
}
// Displays info about the view under cursor
void MainWindow::SetSBLabelView(VIEWID idx)
{
   QString str1="View under pointer: ";
   QString str2;

   switch(idx.viewtype)
   {
   case VIEWTYPESCOPE:
      str1+=QString("Scope %1").arg(idx.viewnum);
      break;
   case VIEWTYPEVIDEO:
      str1+=QString("Video %1").arg(idx.viewnum);
      break;
   case VIEWTYPESTICKMAN:
      str1+=QString("Stickman %1").arg(idx.viewnum);
      break;
   case VIEWTYPESCATTER:
      str1+=QString("Scatter %1").arg(idx.viewnum);
      break;
   case VIEWTYPELABEL:
      str1+=QString("Label %1").arg(idx.viewnum);
      break;
   default:
      str1+=QString("UNKNOWN%1").arg(idx.viewnum);
   }
   SB_LabelView->setText(str1);
}

/**
  \brief returns the center sample, or the current video timecode, corresponding to source idx

   flag effective only applies to video. When true, returns the current, visible, video position.
   When false, returns 'centersample'. Used when scrolling linked sources to ensure smoothness of scrolling.
**/
unsigned MainWindow::GetCenterSample(int idx,bool effective)
{
   if(idx<GetNumVideoViews())
   {
      // idx is a video index
      if(effective)
         return GetVideoViewData(idx).view->getTime();
      else
         return GetVideoViewData(idx).centersample;
   }
   //return SignalData[idx-VideoViewData.size()].centersample;
   return SignalData[idx-GetNumVideoViews()].centersample;
}
void MainWindow::SetCenterSample(int idx,int centersample)
{
   if(IsIdxVideo(idx))
      // idx is a video index
      GetVideoViewData(IDX2VID(idx)).centersample=centersample;
   else
      SignalData[IDX2SID(idx)].centersample=centersample;
}
/**
  \brief Add a reference point between a primary and a secondary.
**/
void MainWindow::on_AddReference()
{
   if(GetPrimarySource()!=-1 && source_secondary!=-1)
   {
      // Both sources are selected -> we add a reference
      std::vector<int> v(4);
      v[0] = GetPrimarySource();
      v[1] = GetCenterSample(source_primary);
      v[2] = source_secondary;
      v[3] = GetCenterSample(source_secondary);
      RefPoints.push_back(v);
      SetReferencePoints(RefPoints);
   }
}
void MainWindow::on_RemoveReference(int idx)
{
   if(idx>=0)
      RefPoints.erase(RefPoints.begin()+idx);
   else
      RefPoints.clear();
   SetReferencePoints(RefPoints);
}

/**
  \brief Reponds to changes in the signal relations linking

  Mirrors the active relation and global activation flag locally.
**/
void MainWindow::on_ChangedActiveRelations()
{
   UpdateRelationsActive();
}
void MainWindow::UpdateRelationsActive()
{
   RelationsActive = RefDialog->GetRelationsActive();
   // Need to patch RelationsActive to copy the regression equation (i.e. the reference point dialog box only holds the src/dst relations, not the equation
   for(unsigned i=0;i<RelationsActive.size();i++)
   {
      for(RELATIONS::iterator it = RelationsAvailable.begin();it!=RelationsAvailable.end();it++)
      {
         if(it->x1==RelationsActive[i].x1 && it->x2==RelationsActive[i].x2)
         {
            RelationsActive[i].a = it->a;
            RelationsActive[i].b = it->b;
            continue;
         }
      }
   }

   LinkSignals = RefDialog->GetLinkState();
}







void MainWindow::on_ReferenceSelected(int idx)
{
   // Highlight the primary and secondary
   source_primary=RefPoints[idx][0];
   source_secondary=RefPoints[idx][2];

   // Set to center position
   SetCenterSample(source_primary,RefPoints[idx][1]);
   SetCenterSample(source_secondary,RefPoints[idx][3]);

   SetAllSliderPosition();
   VisualizePrimarySecondary();
}


/**
   \brief Load program configuration
**/

void MainWindow::on_actionLoad_configuration_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, "Load Configuration",QString(),"Configuration (*.xml)");
   if(!fileName.isNull())
   {
      QFileInfo fi(fileName);
      //printf("absolute path: %s\n",fi.absolutePath().toStdString().c_str());
      path.setCurrent(fi.absolutePath());
      loadConfiguration(fileName);
   }
}

/**
  \brief Save program configuration
**/
void MainWindow::on_actionSave_configuration_triggered()
{
   QString fileName = QFileDialog::getSaveFileName(this, "Save Configuration",QString(),"Configuration (*.xml)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         QXmlStreamWriter xml(&file);
         xml.setAutoFormatting(true);
         xml.writeStartDocument();
         // Start configuration
         xml.writeStartElement("Configuration");

         // Main-related itms
         xml.writeStartElement("Main");
         xml.writeAttribute("Source",ui->lineEdit_Source->toPlainText());
         xml.writeAttribute("View",ui->lineEdit_View->toPlainText());
         xml.writeEndElement();

         // Reference related items
         xml.writeStartElement("References");
         for(int i=0;i<RefPoints.size();i++)
         {
            QString str;
            str.sprintf("relation%d",i);
            xml.writeStartElement("ReferenceData");
            xml.writeAttribute("source1",QString("%1").arg(RefPoints[i][0]));
            xml.writeAttribute("position1",QString("%1").arg(RefPoints[i][1]));
            xml.writeAttribute("source2",QString("%1").arg(RefPoints[i][2]));
            xml.writeAttribute("position2",QString("%1").arg(RefPoints[i][3]));
            xml.writeEndElement();
         }

         xml.writeEndElement();  // References

         xml.writeEndElement();  // Configuration

         xml.writeEndDocument();
         file.close();
      }
      else
      {
         QMessageBox::critical(this, "Save configuration error", "Cannot write to file");
      }
   }
}


void MainWindow::loadConfiguration(QString fileName)
{
   QFile file(fileName);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      RefPoints.clear();

      QXmlStreamReader xml(&file);
      while (!xml.atEnd())
      {
         QXmlStreamReader::TokenType type = xml.readNext();
         //printf("readNext type: %d\n",type);
         if(type==QXmlStreamReader::StartElement)
         {
            //printf("name: %s\n",xml.name().toString().toStdString().c_str());
            //printf("namespaceUri: %s\n",xml.namespaceUri().toString().toStdString().c_str());
            if(xml.name().toString().compare("Main")==0)
            {
               QXmlStreamAttributes at = xml.attributes();
               QString src = at.value("Source").toString();
               QString vw = at.value("View").toString();
               //printf("src: %s\n",src.toStdString().c_str());
               //printf("view: %s\n",vw.toStdString().c_str());
               if(!src.isNull())
                  ui->lineEdit_Source->setPlainText(src);
               else
                  ui->lineEdit_Source->setPlainText("");
               if(!vw.isNull())
                  ui->lineEdit_View->setPlainText(vw);
               else
                  ui->lineEdit_View->setPlainText("");
            }
            if(xml.name().toString().compare("ReferenceData")==0)
            {


               QXmlStreamAttributes at = xml.attributes();
               QString s1 = at.value("source1").toString();
               QString v1 = at.value("position1").toString();
               QString s2 = at.value("source2").toString();
               QString v2 = at.value("position2").toString();

               vector<int> r(4);

               r[0] = s1.toInt();
               r[1] = v1.toInt();
               r[2] = s2.toInt();
               r[3] = v2.toInt();

               RefPoints.push_back(r);

            }
         }
      }
      file.close();

      // Effect the loaded configuration
      REFERENCEPOINTS emptyref;
      RefDialog->SetReferencePoints(emptyref);
      on_pushButton_LoadSource_clicked();    // Load source automatically applies the view
      //on_pushButton_ApplyView_clicked();
      // Set the reference points
      RefDialog->SetReferencePoints(RefPoints);
   }
   else
   {
      QMessageBox::critical(this, "Load configuration error", "Cannot read from file");
   }
}

/**
  \brief Export the link equation as text
**/
void MainWindow::on_actionText_triggered()
{
   QString t="Save relation ";
   QString fileName = QFileDialog::getSaveFileName(this, t+"as text",QString(),"Text (*.txt)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         // Stream writer...
         QTextStream out(&file);

         for(int i=0;i<RelationsAvailable.size();i++)
         {
            QString eq = RelationToText(RelationsAvailable[i]);
            out << eq << endl;
         }
         file.close();
      }
      else
      {
         QMessageBox::critical(this,t+"error", "Cannot write to file");
      }
   }
}



/**
  \brief Export the link equation as a matrix
**/
void MainWindow::on_actionMatrix_triggered()
{
   QString t="Save relation ";
   QString fileName = QFileDialog::getSaveFileName(this, t+"as matrix",QString(),"Text (*.txt)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         // Stream writer...
         QTextStream out(&file);

         for(int i=0;i<RelationsAvailable.size();i++)
         {
            out << RelationsAvailable[i].x1 << " " << RelationsAvailable[i].x2 << " " << RelationsAvailable[i].b << " " << RelationsAvailable[i].a << endl;
         }
         file.close();
      }
      else
      {
         QMessageBox::critical(this,t+"error", "Cannot write to file");
      }
   }
}


/**
  \brief Resamples sigdataorg with the resample factor. resample < 1.0. FrqOld/FrqNew = resample.
**/
SIGNALDATA MainWindow::ResampleSignalData(SIGNALDATA &sigdataorg,double resample,int padleft,int padright)
{
   printf("MainWindow::ResampleSignalData m: %lf padleft: %d\n",resample,padleft);
   SIGNALDATA sigdat;

   printf("sigdataorg number of channels: %d\n",sigdataorg.data.size());
   printf("sigdataorg channel size: %d\n",sigdataorg.data[0].size());

   sigdat.filename = sigdataorg.filename+"_res";
   sigdat.centersample=sigdataorg.centersample;

   sigdat.data.resize(sigdataorg.data.size());                    // Set number of channels
   for(int c=0;c<sigdataorg.data.size();c++)                      // Iterate channels
   {
      // Resize channel
      int newsize = (int)ceil((((double)sigdataorg.data[0].size()) / resample));             // Round up to be consistent with matlab resample
      printf("Channel %d newsize w/o pad %d newsize w/ pad: %d\n",c,newsize,newsize+padleft);

      sigdat.data[c].resize(newsize+padleft+padright,0);
      for(int i=0;i<newsize;i++)                    // Iterate the samples
      {
         double srcidx;
         srcidx = resample*(double)i;
         sigdat.data[c][padleft+i] =  sigdataorg.data[c][(int)srcidx];
      }
   }
   return sigdat;
}

/**
  \brief Computes the resample actions to execute

   Returns a map linking a source (first) to a resample relation (second, RESAMPLEREL)
   The resample relation includes:
   - the resample ratio (second.m)
   - the (optional) offset to prepend (second.o). If not used, it is set to 0
   - the optional padding at the enf of the signal (second.p). If not used, it is set to 0.

**/
map<int,RESAMPLEREL> MainWindow::ResampleLogic(vector<RELATIONS> Graphs, bool prepend)
{

   map<int,RESAMPLEREL> allm;
   printf("------------------------void ResampleLogic(vector<RELATIONS> Graphs) prepend: %d\n",prepend);
   for(int i=0;i<Graphs.size();i++)
   {
      RELATIONS RelationsFollowed = Graphs[i];
      printf("**** Graph %d ****\n",i);

      // If pure video graph, skip.
      bool purevid=true;
      int numsig=0;
      for(int i=0;i<RelationsFollowed.size();i++)
      {
         if(IsIdxSignal(RelationsFollowed[i].x1) || IsIdxSignal(RelationsFollowed[i].x2))
         {
            purevid=false;
         }
         if(IsIdxSignal(RelationsFollowed[i].x1))
            numsig++;
         if(IsIdxSignal(RelationsFollowed[i].x2))
            numsig++;
      }
      if(purevid==true)
      {
         printf("This is a pure video graph, skip it\n");
         continue;
      }
      // If the graph has only 1 signal edge (eg. if the only link is between a video and a signal) no resampling is possible.
      if(numsig<2)
      {
         printf("This graph has only one signal edge, no resampling possible, skip\n");
         continue;
      }



      // m holds a relative set of relations within the graph, with the first edge of the first relation as the start point
      map<int,RESAMPLEREL> m;

      m.clear();
      m[RelationsFollowed[0].x1].m=1.0;
      m[RelationsFollowed[0].x1].o=0.0;

      //printf("Relations followed: \n");

      for(int i=0;i<RelationsFollowed.size();i++)
      {
         m[RelationsFollowed[i].x2].m = m[RelationsFollowed[i].x1].m * RelationsFollowed[i].b;
         m[RelationsFollowed[i].x2].o = m[RelationsFollowed[i].x1].o * RelationsFollowed[i].b + RelationsFollowed[i].a;
         //printf("x%d->x%d\n",RelationsFollowed[i].x1,RelationsFollowed[i].x2);
      }

      printf("Signal - RelFrq  RelOffset\n");
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         printf("%d - %lf %lf\n",it->first,it->second.m,it->second.o);
      }


      // Here should identify the highest frequency, and relate everything to the highest frequency
      // Find highest value to normalize by it.
      // Exploits relations going through a video edge, but does not take into account the video in determining the highest frequency.
      double hv;
      int hfsrc;
      bool first;
      first=true;
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         if(IsIdxVideo(it->first))
            continue;
         if(first)
         {
            hv = it->second.m;
            hfsrc = it->first;
            first=false;
         }
         else
         {
            if(it->second.m>hv)
            {
               hv=it->second.m;
               hfsrc = it->first;
            }
         }
      }
      //printf("Highest sample rate hv: %lf. hf source: %d\n",hv,hfsrc);
      // Normalize frequency
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         it->second.m/=hv;
         it->second.o/=it->second.m;
      }
      printf("Signal - NormRelFrq  NormRelOffset\n");
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         printf("%d - %lf %lf\n",it->first,it->second.m,it->second.o);
      }

      // Look for the leftmost signal, skipping the video in determining this
      double lm;
      int lmsrc;
      first=true;
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         if(IsIdxVideo(it->first))
            continue;
         if(first)
         {
            lm = it->second.o;
            lmsrc = it->first;
            first=false;
         }
         else
         {
            if(it->second.o>lm)
            {
               lm=it->second.o;
               lmsrc = it->first;
            }
         }
      }
      //printf("Left most sample: %lf. lm source: %d\n",lm,lmsrc);
      // Convert o from leftmost sample to pad left offset (offset=maxo-o). Handles the prepen
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         if(prepend)
            it->second.o = (int)(lm-it->second.o);
         else
            it->second.o = 0;
      }

      // Compute the new length of the signals for this graph, including the offset
      // Find the length of the longest resampled signal with matlab and C resample variants
      // Skips video
      printf("Compute new length\n");
      int maxl_c=0;
      int maxl_matlab=0;
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         if(IsIdxVideo(it->first))
            continue;
         int cl;
         // Matlab
         int n,d;
         DoubleToRatio(1.0/it->second.m,n,d);
         cl = it->second.o + ceil((double)SignalData[IDX2SID(it->first)].data[0].size()*(double)n/(double)d);
         it->second.l_matlab = cl;
         if(cl>maxl_matlab)
            maxl_matlab=cl;
         // C
         cl = it->second.o + ceil((double)SignalData[IDX2SID(it->first)].data[0].size()/it->second.m);
         it->second.l_c = cl;
         if(cl>maxl_c)
            maxl_c=cl;
      }

      // set the padding right size
      printf("set the padding right size\n");
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         it->second.p_matlab = maxl_matlab-it->second.l_matlab;
         it->second.p_c = maxl_c-it->second.l_c;
      }


      // This means
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         printf("Src %d prepad %lf. NL C/Matlab: %d/%d. Postpend C/Matlab: %d/%d\n",it->first,lm-it->second.o,it->second.l_c,it->second.l_matlab,it->second.p_c,it->second.p_matlab);
      }

      // Go through all the relations and erase those that aren't signals
      map<int,RESAMPLEREL> m2;
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         if(IsIdxSignal(it->first))
            m2[it->first]=it->second;
      }

      printf("after pruning\n");
      for(map<int,RESAMPLEREL>::iterator it=m2.begin();it!=m2.end();it++)
      {
         printf("Src %d prepad %lf. NL C/Matlab: %d/%d. Postpend C/Matlab: %d/%d\n",it->first,lm-it->second.o,it->second.l_c,it->second.l_matlab,it->second.p_c,it->second.p_matlab);
      }

      // End of a graph, we keep the relations in allm.
      allm.insert(m2.begin(),m2.end());

   }
   printf("DUMPING all resample relations. Total %d.\n",allm.size());
   for(map<int,RESAMPLEREL>::iterator it = allm.begin();it!=allm.end();it++)
   {
      printf("signal %d. m: %lf. o: %lf\n",it->first,it->second.m,it->second.o);
   }


   return allm;
}

/**
  \brief Resample in-place
**/
void MainWindow::ResampleInPlace(map<int,RESAMPLEREL> m,bool prepend,bool postpend)
{
   printf("MainWindow::ResampleInPlace; prepend %d postpend %d\n",prepend,postpend);
   // Iterate all the source in the relation, resample them, and add them to the visualization stuff.
   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
   {
      printf("Resampling source %d (%s)\n",it->first,SignalData[IDX2SID(it->first)].filename.toStdString().c_str());
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
      /*if(prepend)
         o=it->second.o;
      else
         o=0;*/
      int p;
      if(postpend)
         p=it->second.p_c;
      else
         p=0;
      printf("Calling ResampleSignalData; factor %lf offset %lf postpend %d\n",it->second.m,o,p);
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
   }
   RefDialog->SetReferencePoints(RefPoints);             // Update GUI with reference points
   ComputeRelations();                                   // Update GUI with relations
}
void MainWindow::ResampleMatlab(map<int,RESAMPLEREL> m,bool nn,bool prepend,bool postpend)
{
   QString matlabcode;
   QString str;

   matlabcode.sprintf("\% Matlab resample code\n");

   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
   {
      str.sprintf("\% Resampling signal %d (%s). Length: %d. Sample rate ratio: %lf. Prepend offset: %lf\n",it->first,SignalData[it->first].filename.toStdString().c_str(),SignalData[it->first].data[0].size(),it->second.m,it->second.o);
      matlabcode+=str;
   }

   matlabcode+="% input files\n";
   matlabcode+="fin={\n";
   QDir dir;
   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      matlabcode+="\t'"+ dir.cleanPath(dir.absoluteFilePath(SignalData[IDX2SID(it->first)].filename)) +"'\n";
   matlabcode+="};\n";
   matlabcode+="fout={\n";
   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      matlabcode+="\t'"+ dir.cleanPath(dir.absoluteFilePath(SignalData[IDX2SID(it->first)].filename)) +".res"+"'\n";
   matlabcode+="};\n";
   matlabcode+="sr_p=[\n";
   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
   {
      int n,d;
      DoubleToRatio(1.0/it->second.m,n,d);
      matlabcode+=QString("\t%1\n").arg(n);
   }
   matlabcode+="];\n";
   matlabcode+="sr_q=[\n";
   for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
   {
      int n,d;
      DoubleToRatio(1.0/it->second.m,n,d);
      matlabcode+=QString("\t%1\n").arg(d);
   }
   matlabcode+="];\n";

   if(prepend)
   {
      matlabcode+="o=[\n";
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
         matlabcode+=QString("\t%1\n").arg(it->second.o);
      matlabcode+="];\n";
   }

   if(postpend)
   {
      matlabcode+="p=[\n";
      for(map<int,RESAMPLEREL>::iterator it=m.begin();it!=m.end();it++)
      {
         matlabcode+=QString("\t%1\n").arg(it->second.p_matlab);
      }
      matlabcode+="];\n";
   }

   matlabcode+=QString("for i=1:%1\n").arg(m.size());
   matlabcode+=QString("\tx = load(fin{i});\n");
   matlabcode+=QString("\ty = resample(x,sr_p(i),sr_q(i)");       // resample
   if(nn)
      matlabcode+=QString(",0");                                  // nearest neighbor
   matlabcode+=QString(");\n");

   if(prepend)
   {
      matlabcode+=QString("\tz = zeros(o(i),size(x,2));\n");
      matlabcode+=QString("\ty = [z;y];\n");                            // prepend 0
   }
   if(postpend)
   {
      matlabcode+=QString("\tz = zeros(p(i),size(x,2));\n");
      matlabcode+=QString("\ty = [y;z];\n");                            // postpend 0
   }
   matlabcode+=QString("\tsize(x)\n");
   matlabcode+=QString("\tsize(y)\n");
   matlabcode+=QString("\tsave(fout{i},'y','-ascii');\n");
   matlabcode+=QString("end\n");


   MatlabCodeDialog *mdlg = new MatlabCodeDialog(this);
   mdlg->setCode(matlabcode);
   mdlg->exec();
   delete mdlg;
}

/**
  \brief Save reference points
**/
void MainWindow::on_actionSave_reference_points_triggered()
{
   QString fileName = QFileDialog::getSaveFileName(this, "Save Reference Points",QString(),"Text (*.txt)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         QTextStream out(&file);

         for(int i=0;i<RefPoints.size();i++)
         {
            //QString eq = RelationToText(RelationsAvailable[i]);
            for(int j=0;j<4;j++)
               out << RefPoints[i][j] << " ";
            out << endl;
         }
         file.close();
      }
      else
      {
         QMessageBox::critical(this, "Save error", "Cannot write to file");
      }
   }
}


/**
  \brief Prune the reference points that refer to unavailable sources. Returns only the valid reference points, and a displayable error message.
**/
REFERENCEPOINTS MainWindow::PruneRefPoints(REFERENCEPOINTS v,QString &str,bool display)
{
   REFERENCEPOINTS vv;
   int err=0;

   for(int i=0;i<v.size();i++)
   {
      if(v[i][0]>=GetNumSources() || v[i][2]>=GetNumSources())
         err++;
      else
      {
         if(v[i][0]==v[i][2])
            err++;
         else
            vv.push_back(v[i]);
      }
   }
   if(err)
      str=QString("%1 reference points relate to nonexistant sources. They will be discarded.").arg(err);

   if(display && err)
      QMessageBox::information(this,"Reference points",str);

   return vv;
}


/**
  \brief Set the reference points to ref, and handles UI and relation computation update
**/
void MainWindow::SetReferencePoints(REFERENCEPOINTS ref)
{
   // Store the the new reference points
   RefPoints=ref;
   // Set the reference points
   RefDialog->SetReferencePoints(RefPoints);
   // Compute the links
   ComputeRelations();
}

/**
  \brief Load reference points
**/

void MainWindow::on_actionLoad_reference_points_triggered()
{
   QString strtitle("Loading reference points");
   QString fileName = QFileDialog::getOpenFileName(this, "Load Reference Points",QString(),"text (*.txt)");
   if(!fileName.isNull())
   {
      std::vector<std::vector<int> > v;
      int ok;
      v = LoadSignalFile(fileName.toStdString(),ok);
      if(ok<0)
      {
         if(ok==-1)
            QMessageBox::critical(this,QString("Load reference points"),QString("Can't load %1. File not found.").arg(fileName));
         else
            QMessageBox::critical(this,QString("Load reference points"),QString("Can't load %1. Invalid file content.").arg(fileName));
         return;
      }

      if(v.size()!=4)
      {
         QMessageBox::critical(this,strtitle,"Invalid file format; expecting 4 columns");
         return;
      }

      // Convert the loaded format into the referencepoint format
      REFERENCEPOINTS rp,vrp;
      for(int i=0;i<v[0].size();i++)
      {
         vector<int> p(4,0);
         p[0]=v[0][i];
         p[1]=v[1][i];
         p[2]=v[2][i];
         p[3]=v[3][i];
         rp.push_back(p);
      }


      // Check the reference points to make sure they are valid, issuing an error message in case of errors.
      QString str;
      vrp = PruneRefPoints(rp,str);
      SetReferencePoints(vrp);
   }
}

/**
  \brief Shows the loaded signal sources
**/
void MainWindow::on_actionShow_available_sources_triggered()
{
   SourceDialog *dlg = new SourceDialog(this);
   dlg->SetSignalData(SignalData);
   dlg->exec();
   delete dlg;

}



void MainWindow::on_actionAbout_Q_triggered()
{
   QApplication::aboutQt();
}



/**
  \brief If there is a primary source selected prompts the user for a file name in which to save the source.
**/
void MainWindow::on_actionSave_primary_source_triggered()
{
   QString strtitle="Save primary source";

   if(GetPrimarySource()==-1 || !IsIdxSignal(GetPrimarySource()))
   {
      QMessageBox::critical(this,strtitle,"No primary signal source is selected. Select the primary signal source first.");
   }
   else
   {
      SaveSignal(SignalData[IDX2SID(GetPrimarySource())].data,strtitle);
   }
}

/**
  \brief Save a signal in text mode.
**/
void MainWindow::SaveSignal(vector<vector<int> > &source,QString strtitle)
{
   QString fileName = QFileDialog::getSaveFileName(this,strtitle,QString(),"Text (*.txt);;All (*.*)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         // Stream writer...
         QTextStream out(&file);

         for(int i=0;i<source[0].size();i++)
         {
            for(int j=0;j<source.size();j++)
               out << source[j][i] << " ";
            out << endl;
         }
         file.close();
      }
      else
      {
         QMessageBox::critical(this,strtitle, "Cannot write to file");
      }
   }
}


/**
   \brief Handles undo operation
**/
void MainWindow::on_actionUndo_triggered()
{
   if(!undoable)
      return;

   // Swaps the current and undo buffers.
   REFERENCEPOINTS r = RefPoints;
   std::vector<SIGNALDATA> s=SignalData;
   SignalData=SignalDataUndo;
   RefPoints=RefPointsUndo;
   SignalDataUndo = s;
   RefPointsUndo = r;

   // Recompute the relations, update the active relations, etc.
   RefDialog->SetReferencePoints(RefPoints);             // Update GUI with reference points
   ComputeRelations();                                   // Update GUI with relations
}

/**
  \brief Show about program information
**/
void MainWindow::on_actionAbout_triggered()
{
   QMessageBox::about(this, "About",
                      "<p><b>SynScopeV</b> - Offline visualization and alignment tool for multiple source signals and videos</p>\n"
                      "<p>Version 2.1</p>"
                      "<p>(c) 2009-2011 Daniel Roggen</p>"
                      "<p>This software is licensed under the terms of the GPL v.2 license.</p>"
                      );
}
/**
  \brief Show a short program usage howto
**/
void MainWindow::on_actionHow_to_triggered()
{
   HelpDialog *dlg = new HelpDialog(this);
   dlg->exec();
   delete dlg;
}

void MainWindow::on_actionMerge_linked_files_triggered()
{
   MergeFiles();
}




/**
  \brief Shows/hides the dialog with the relations
**/
void MainWindow::on_actionShow_reference_points_and_links_triggered()
{
   if(!RefDialog->isVisible())
      RefDialog->show();
   else
      RefDialog->hide();

}


void MainWindow::on_actionAdd_reference_point_triggered()
{
   on_AddReference();
}


void MainWindow::on_actionClear_reference_points_triggered()
{
   on_RemoveReference(-1);
}


/******************************************************************************
   MainWindow::on_View_timeChanged
*******************************************************************************
   Called when the time in a view (scope) has been changed by the user.
******************************************************************************/
void MainWindow::on_View_timeChanged(int value)
{
   QObject *s = sender();
   //printf("on_Slider_valueChanged %p -> %d\n",s,value);

   // Find the corresponding source, view, etc
   SRCID srcid;
   VIEWID viewid;
   if(!GetSourceOfView((QWidget*)s,srcid,viewid))
   {
      qDebug("Mouse moved in unknown view %p\n",s);
      return;
   }
   //printf("Time changed on: srctype: %d. srcuid: %d. srcid: %d\n",srcid.srctype,srcid.srcuid,srcid.srcid);
   on_Source_Slided(srcid.srcuid,value);

}

bool MainWindow::IsIdxVideo(int idx)
{
   if(idx<GetNumVideoViews())
      return true;
   return false;
}
bool MainWindow::IsIdxSignal(int idx)
{
   if(idx>=GetNumVideoViews())
      return true;
   return false;
}
/**
  \brief Converts a video ID to absolute ID
**/
int MainWindow::VID2IDX(int vid)
{
   return vid + 0x0000;
}
/**
  \brief Converts a signal ID to absolute ID
**/
int MainWindow::SID2IDX(int sid)
{
   return sid+GetNumVideoViews();
   //return sid + 0x0100;
}
int MainWindow::IDX2SID(int idx)
{
   assert(idx>=GetNumVideoViews() && idx<GetNumSources());
   //assert(idx>=0x0100 && idx<0x0100+SignalData.size());
   return idx-GetNumVideoViews();
}
int MainWindow::IDX2VID(int idx)
{
   assert(idx>=0 && idx<GetNumVideoViews());
   return idx;
}
/**
  \brief Converts a unique signal ID to a SRCID
**/
bool MainWindow::UID2SRCID(int uid,SRCID &srcid)
{
   srcid.srcuid = uid;
   if(IsIdxVideo(uid))
   {
      srcid.srctype = SRCTYPEVIDEO;
      srcid.srcid = IDX2VID(uid);
   }
   if(IsIdxSignal(uid))
   {
      srcid.srctype = SRCTYPESIGNAL;
      srcid.srcid = IDX2SID(uid);
   }
   return true;
}



/**
  \brief Returns the total number of sources (video + signals)
**/
unsigned MainWindow::GetNumSources()
{
   return GetNumVideoViews()+SignalData.size();
}
/**
  \brief Finds the view widget corresponding to the parameter and returns to which source it is associated
**/
bool MainWindow::GetSourceOfView(QWidget *w,SRCID &srcid,VIEWID &viewid)
{
   int view=-1;

   // Iterate all the viewtypes
   for(int vt = VIEWTYPEFIRST;vt<=VIEWTYPELAST;vt++)
   {
      for(unsigned i=0;i<GetNumViews(static_cast<VIEWTYPE>(vt));i++)
      {
         if(GetSSView(static_cast<VIEWTYPE>(vt),i)==w)
         {
            viewid.viewtype=static_cast<VIEWTYPE>(vt);
            viewid.viewnum=i;
            if(vt==VIEWTYPEVIDEO)
            {
               srcid.srctype=SRCTYPEVIDEO;
               srcid.srcid=i;
               srcid.srcuid=VID2IDX(i);
            }
            else
            {
               srcid.srctype=SRCTYPESIGNAL;
               srcid.srcid=GetViewData(viewid).source;
               srcid.srcuid=SID2IDX(GetViewData(viewid).source);
            }
            return true;
         }
      }
   }
   // Not found
   return false;
}




void MainWindow::on_actionSet_working_directory_triggered()
{
   QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
   printf("dir: %s\n",dir.toStdString().c_str());
   path.setCurrent(dir);
}

/**
  \brief Return the view that has the focus, or returns viewtype=VIEWTYPEUNDEFINED
**/
VIEWID MainWindow::GetViewFocus()
{
   VIEWID id;

   id.viewtype = VIEWTYPEUNDEFINED;

   // Iterate all the viewtypes
   for(int vt = VIEWTYPEFIRST;vt<=VIEWTYPELABEL;vt++)
   {
      for(unsigned i=0;i<GetNumViews(static_cast<VIEWTYPE>(vt));i++)
      {
         //printf("VT type %d num %d. focus: %d\n",vt,i,GetSSView(static_cast<VIEWTYPE>(vt),i)->hasFocus());
         if(GetSSView(static_cast<VIEWTYPE>(vt),i)->hasFocus())
         {
            id.viewnum=i;
            id.viewtype=static_cast<VIEWTYPE>(vt);
            return id;
         }
      }
   }



   return id;
}

/**
  \brief Returns the length of the source signal sid
**/
int MainWindow::GetSourceLengthSignal(int sid)
{
   assert(sid<SignalData.size());      // Check the source is available
   assert(SignalData[sid].data.size());     // Check the source has a non-null number of channels
   return SignalData[sid].data[0].size();   // Returns the length of the first channel as the length of the source
}

/**
  \brief Terminates the program
**/
void MainWindow::on_action_Quit_triggered()
{
   close();
}

/**
  \brief Returns the primary source UID
**/
int MainWindow::GetPrimarySource()
{
   return source_primary;
}
/**
  \brief Returns the secondary source UID
**/
int MainWindow::GetSecondarySource()
{
   return source_secondary;
}

/**
  \brief Accessor: number of views of type
**/

unsigned MainWindow::GetNumViews(VIEWTYPE type)
{
   switch(type)
   {
   case VIEWTYPESCOPE:
      return GetNumScopeViews();
   case VIEWTYPESCATTER:
      return GetNumScatterViews();
   case VIEWTYPESTICKMAN:
      return GetNumStickmanViews();
   case VIEWTYPEVIDEO:
      return GetNumVideoViews();
   case VIEWTYPELABEL:
      return GetNumLabelViews();
   default:
      return 0;
   }
}

/**
  \brief Accessor: number of video views
**/
unsigned MainWindow::GetNumVideoViews()
{
   return p_VideoViewData.size();
}
/**
  \brief Accessor: number of scope views
**/
unsigned MainWindow::GetNumScopeViews()
{
   return p_ScopeViewData.size();
}
/**
  \brief Accessor: number of scatter views
**/
unsigned MainWindow::GetNumScatterViews()
{
   return p_ScatterViewData.size();
}
/**
  \brief Accessor: number of label views
**/
unsigned MainWindow::GetNumLabelViews()
{
   return p_LabelViewData.size();
}
/**
  \brief Accessor: number of stickman views
**/
unsigned MainWindow::GetNumStickmanViews()
{
   return p_StickmanViewData.size();
}
/**
  \brief Accessor: get view type vt number n
**/
SSViewAbstract *MainWindow::GetSSView(VIEWID view)
{
   return GetSSView(view.viewtype,view.viewnum);
}

/**
  \brief Accessor: get view type vt number n
**/
SSViewAbstract *MainWindow::GetSSView(VIEWTYPE vt,int n)
{
   assert(vt>=VIEWTYPEFIRST);
   assert(vt<=VIEWTYPELAST);
   assert(n<GetNumViews(vt));
   switch(vt)
   {
   case VIEWTYPESCOPE:
      return GetScopeViewData(n).view;
   case VIEWTYPESCATTER:
      return GetScatterViewData(n).view;
   case VIEWTYPESTICKMAN:
      return GetStickmanViewData(n).view;
   case VIEWTYPEVIDEO:
      return GetVideoViewData(n).view;
   case VIEWTYPELABEL:
      return GetLabelViewData(n).view;
   default:
      return 0;
   }
}

/**
  \brief Accessor: returns the base class ViewData of view
**/
const VIEWDATA &MainWindow::GetViewData(VIEWID view)
{
   assert(view.viewtype!=VIEWTYPEUNDEFINED);
   switch(view.viewtype)
   {
   case VIEWTYPESCOPE:
      return GetScopeViewData(view.viewnum);
   case VIEWTYPESCATTER:
      return GetScatterViewData(view.viewnum);
   case VIEWTYPELABEL:
      return GetLabelViewData(view.viewnum);
   case VIEWTYPESTICKMAN:
      return GetStickmanViewData(view.viewnum);
   case VIEWTYPEVIDEO:
      return GetVideoViewData(view.viewnum);
   }
   assert(0);
   // This should not happen so we don't care about returning anything
}

/**
  \brief Accessor: get the ScopeViewData view
**/
const SCOPEVIEWDATA &MainWindow::GetScopeViewData(unsigned view)
{
   assert(view<GetNumScopeViews());
   return p_ScopeViewData[view];
}
/**
  \brief Accessor: get the ScatterViewData view
**/
const SCATTERVIEWDATA &MainWindow::GetScatterViewData(unsigned view)
{
   assert(view<GetNumScatterViews());
   return p_ScatterViewData[view];
}
/**
  \brief Accessor: get the LabelViewData view
**/
const LABELVIEWDATA &MainWindow::GetLabelViewData(unsigned view)
{
   assert(view<GetNumLabelViews());
   return p_LabelViewData[view];
}
/**
  \brief Accessor: get the StickmanViewData view
**/
const STICKMANVIEWDATA &MainWindow::GetStickmanViewData(unsigned view)
{
   assert(view<GetNumStickmanViews());
   return p_StickmanViewData[view];
}
/**
  \brief Accessor: get the VideoViewData view
**/
VIDEOVIEWDATA &MainWindow::GetVideoViewData(unsigned view)
{
   assert(view<GetNumVideoViews());
   return p_VideoViewData[view];
}


/**
  \brief Add a new entry to ScopeViewData
**/
void MainWindow::AddScopeViewData(const SCOPEVIEWDATA &view)
{
   // Do quick sanity check - view musn't be null
   assert(view.view);
   p_ScopeViewData.push_back(view);
}
/**
  \brief Add a new entry to ScatterViewData
**/
void MainWindow::AddScatterViewData(const SCATTERVIEWDATA &view)
{
   // Do quick sanity check - view musn't be null
   assert(view.view);
   p_ScatterViewData.push_back(view);
}
/**
  \brief Add a new entry to LabelViewData
**/
void MainWindow::AddLabelViewData(const LABELVIEWDATA &view)
{
   // Do quick sanity check - view musn't be null
   assert(view.view);
   p_LabelViewData.push_back(view);
}
/**
  \brief Add a new entry to StickmanViewData
**/
void MainWindow::AddStickmanViewData(const STICKMANVIEWDATA &view)
{
   // Do quick sanity check - view musn't be null
   assert(view.view);
   p_StickmanViewData.push_back(view);
}
/**
  \brief Add a new entry to VideoViewData
**/
void MainWindow::AddVideoViewData(const VIDEOVIEWDATA &view)
{
   // Do quick sanity check - view musn't be null
   assert(view.view);
   p_VideoViewData.push_back(view);
}

/**
  \brief Remove a ScopeViewData
**/
void MainWindow::RemoveScopeViewData(unsigned idx)
{
   assert(idx<GetNumScopeViews());
   p_ScopeViewData.erase(p_ScopeViewData.begin()+idx);
}
/**
  \brief Remove a ScatterViewData
**/
void MainWindow::RemoveScatterViewData(unsigned idx)
{
   assert(idx<GetNumScatterViews());
   p_ScatterViewData.erase(p_ScatterViewData.begin()+idx);
}
/**
  \brief Remove a LabelViewData
**/
void MainWindow::RemoveLabelViewData(unsigned idx)
{
   assert(idx<GetNumLabelViews());
   p_LabelViewData.erase(p_LabelViewData.begin()+idx);
}
/**
  \brief Remove a StickmanViewData
**/
void MainWindow::RemoveStickmanViewData(unsigned idx)
{
   assert(idx<GetNumStickmanViews());
   p_StickmanViewData.erase(p_StickmanViewData.begin()+idx);
}
/**
  \brief Remove a VideoViewData
**/
void MainWindow::RemoveVideoViewData(unsigned idx)
{
   assert(idx<GetNumVideoViews());
   p_VideoViewData.erase(p_VideoViewData.begin()+idx);
}

/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/* DEV/INTERNAL FUNCTIONS   DEV/INTERNAL FUNCTIONS   DEV/INTERNAL FUNCTIONS   DEV/INTERNAL FUNCT*/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/








/******************************************************************************
   MainWindow::eventFilterForScrollArea
*******************************************************************************
   event filter to associate to the scrollArea that contains the signals/videos
   Filters out the wheel event.

******************************************************************************/
/*bool MainWindow::eventFilter(QObject* obj, QEvent* evt)
{
   printf("mainWindow:: Event filter obj %p type %d\n",obj,evt->type());
   if (evt->type() == QEvent::Wheel)
   {
      printf("mainWindow:: Event filter - wheel\n");
      // ignore the event (this effectively
      // makes it "skip" one object)
      evt->ignore();
      return true;
   }
   // return false to continue event propagation
   // for all events
   return false;
   //return true;
}*/





void MainWindow::on_actionFocus_triggered()
{
   GetViewFocus();
}

