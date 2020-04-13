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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDir>
#include <QSplitter>
#include <QScrollArea>

#include "ui_mainwindow.h"

#include "data.h"
#include "guihelper.h"
#include "referencesdialog.h"
#include "PlotHelper.h"



// Source numbering:
// video 0...vn-1
// Signal: vn...vn+sn-1
// i.e. first videos, then signals.



namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
   Ui::MainWindow *ui;
   ReferencesDialog *RefDialog;

   std::vector<SIGNALDATA> SignalData,SignalDataUndo; // SignalData[source]

   // Signal/video splitter & scroll areas
   QSplitter *signalVideoSplitter;
   QScrollArea *signalScrollArea,*videoScrollArea;


   // Scope related
   SCOPEVIEWDATAS p_ScopeViewData;
   QFrame *mainFrame;                 // Frame within the scroll area
   QLayout *signalVerticalLayout;     // Vertical layouting within the frame within the scroll area

   // Video related
   VIDEOVIEWDATAS p_VideoViewData;
   QFrame *mainVideoFrame;
   QHBoxLayout *videoLayout;

   // Stickman
   STICKMANVIEWDATAS p_StickmanViewData;

   // Scatter
   SCATTERVIEWDATAS p_ScatterViewData;

   // Label
   LABELVIEWDATAS p_LabelViewData;


   bool undoable;
   REFERENCEPOINTS RefPoints,RefPointsUndo;          // Reference points between pairs of signals
   RELATIONS RelationsAvailable;       // Relations between pairs of signals (i.e. transfer formula)
   RELATIONS RelationsActive;          // Active relations, i.e. subset of the possible relations. The recursion data are not used in this structure.
   bool LinkSignals;                   // General flag indicating whether signals should be linked.

   QLabel *SB_LabelPriSec,*SB_LabelPosition,*SB_LabelCenter,*SB_LabelSource,*SB_LabelView;
   string CurrentLayoutString;


   int source_primary,source_secondary;    // Primary and secondary sources (-1 for not assigned).

   QDir path;


   int timer;


   void SetAllSliderPosition(bool all=true,vector<int> vid=vector<int>(0),vector<int> sid=vector<int>(0));
   void PrepareDataPlotScope(VIEWID view,vector<vector<int> *> &vd,vector<unsigned> &vc);
   void PrepareDataPlotLabel(VIEWID view,vector<vector<int> *> &vd,vector<unsigned> &vc);
   void PrepareDataPlotScatter(VIEWID view,vector<vector<int> *> &datax,vector<vector<int> *> &datay,vector<unsigned> &vc);
   void PrepareDataPlotStickman(VIEWID view,TRACESRI &_torsorot,TRACESRI &_luarot,TRACESRI &_llarot,TRACESRI &_ruarot,TRACESRI &_rlarot);
   int ParseSourceString(string str,SOURCEFILES &src);

   std::vector<std::vector<int> > LoadSignalFile(string file,int &ok,bool report=true);

   void on_Source_Slided(int sourceidx,int value);
   void AdjustSignalViewSlider();
   void SourceClick(int source,Qt::MouseButton button,int sourcex);
   void VisualizePrimarySecondary();

   bool OLSQRegress(std::vector<double> x1,std::vector<double> x2, double &a, double &b);
   bool OrthogonalLSQRegression(std::vector<double> x1,std::vector<double> x2, double &a, double &b);
   //bool IsRelationConflicting(RELATIONS &Relations, int x1,int x2);

   void loadConfiguration(QString fileName);



   void SaveSignal(vector<vector<int> > &source,QString strtitle);

   // Status bar
   void SetSBLabelPointer(int uid);
   void SetSBLabelPointer(SRCID idx);
   void SetSBLabelView(VIEWID idx);

   void ComputeRelations();
   void UpdateRelationsActive();

   // Data processing
   map<int,RESAMPLEREL> ResampleLogic(vector<RELATIONS> Graphs,bool prepend);
   void ResampleInPlace(map<int,RESAMPLEREL> m,bool prepend,bool postpend);
   void ResampleMatlab(map<int,RESAMPLEREL> m,bool nn,bool prepend,bool postpend);
   SIGNALDATA ResampleSignalData(SIGNALDATA &sigdataorg,double resample,int padleft,int padright);
   void Resample(bool inplace);
   void MergeFiles();

   void SetupViews(string layout);
   void SetUndoable(bool u);
   void ClearActiveRelations();
   REFERENCEPOINTS PruneRefPoints(REFERENCEPOINTS v,QString &str,bool display=true);
   void SetReferencePoints(REFERENCEPOINTS ref);





   // Video

   //bool GetVideoParameters(unsigned &videow,unsigned &videoh,unsigned &bitrate,unsigned &gop,unsigned &signalstep,bool &drawtitle,bool &dbl);
   int ExportViewToVideo(VIEWID view,const PLOTVIEWPARAM &param,QString videoFileName,QString progressTitle);
   PlotHelperAbstract *CreatePlotHelper(VIEWID view,const PLOTVIEWPARAM &param);

   ////////////////////////////////////////////////////////////////
   // SOURCES   SOURCES   SOURCES   SOURCES   SOURCES   SOURCES
   ////////////////////////////////////////////////////////////////
   int GetSourceLengthSignal(int sid);


   ////////////////////////////////////////////////////////////////
   // VIEW   VIEW   VIEW   VIEW   VIEW   VIEW   VIEW   VIEW   VIEW
   ////////////////////////////////////////////////////////////////
   const VIEWDATA &GetViewData(VIEWID view);
   SSViewAbstract *GetSSView(VIEWID view);
   SSViewAbstract *GetSSView(VIEWTYPE vt,int n);   
   VIEWID GetViewFocus();


   // Get the view datas - const ref!
   const SCOPEVIEWDATA &GetScopeViewData(unsigned view);
   const SCATTERVIEWDATA &GetScatterViewData(unsigned view);
   const LABELVIEWDATA &GetLabelViewData(unsigned view);
   const STICKMANVIEWDATA &GetStickmanViewData(unsigned view);
   VIDEOVIEWDATA &GetVideoViewData(unsigned view);
   // Add view data
   void AddScopeViewData(const SCOPEVIEWDATA &view);
   void AddScatterViewData(const SCATTERVIEWDATA &view);
   void AddLabelViewData(const LABELVIEWDATA &view);
   void AddStickmanViewData(const STICKMANVIEWDATA &view);
   void AddVideoViewData(const VIDEOVIEWDATA &view);
   // Remove view data
   void RemoveScopeViewData(unsigned idx);
   void RemoveScatterViewData(unsigned idx);
   void RemoveLabelViewData(unsigned idx);
   void RemoveStickmanViewData(unsigned idx);
   void RemoveVideoViewData(unsigned idx);

   // Add/Delete views
   void AddScopeView(SCOPEVIEWDATA &vd);
   void DeleteScopeViews();
   void AddVideoView(QString file);
   void DeleteVideoViews();
   void AddStickmanView(STICKMANVIEWDATA &vd);
   void DeleteStickmanViews();
   void AddScatterView(SCATTERVIEWDATA &vd);
   void DeleteScatterViews();
   void AddLabelView(LABELVIEWDATA &vd);
   void DeleteLabelViews();

   unsigned GetCenterSample(int idx,bool effective=true);
   void SetCenterSample(int idx,int centersample);
   bool IsIdxVideo(int idx);
   bool IsIdxSignal(int idx);
   int VID2IDX(int vid);
   int SID2IDX(int sid);
   int IDX2SID(int idx);
   int IDX2VID(int idx);
   unsigned GetNumSources();
   unsigned GetNumScopeViews();
   unsigned GetNumScatterViews();
   unsigned GetNumLabelViews();
   unsigned GetNumStickmanViews();
   unsigned GetNumVideoViews();
   unsigned GetNumViews(VIEWTYPE type);
   bool GetSourceOfView(QWidget *w,SRCID &srcid,VIEWID &viewid);
   bool UID2SRCID(int uid,SRCID &srcid);


   int GetPrimarySource();
   int GetSecondarySource();





   void findlinks();



   void image2Pixmap(QImage &img,QPixmap &pixmap);




protected:
    //virtual void timerEvent(QTimerEvent *event);

private slots:


    void on_actionMosaic_video_triggered();
    void on_actionVideo_of_all_signal_sources_triggered();
    void on_action_Quit_triggered();
    void on_actionSet_working_directory_triggered();
    void on_actionClear_reference_points_triggered();
    void on_actionAdd_reference_point_triggered();
    void on_actionShow_reference_points_and_links_triggered();
    void on_actionMerge_linked_files_triggered();
    void on_actionHow_to_triggered();
    void on_actionAbout_triggered();
    void on_actionUndo_triggered();
    void on_actionSave_primary_source_triggered();
    void on_actionExport_Matlab_resample_code_triggered();
    void on_action_Resample_triggered();
    void on_actionAbout_Q_triggered();
    void on_actionShow_available_sources_triggered();
    void on_actionLoad_reference_points_triggered();
    void on_actionSave_reference_points_triggered();
    void on_actionMatrix_triggered();
    void on_actionText_triggered();
    void on_actionSave_configuration_triggered();
    void on_actionLoad_configuration_triggered();
    //void on_pushButton_ShowVideo_clicked();
    //void on_pushButton_ComputeRelations_clicked();
    //void on_pushButton_3_clicked();
    void on_pushButton_LoadSource_clicked();
    void on_pushButton_ApplyView_clicked();
    //void on_pushButtonPlay_clicked();

    // Signal-related stuff
    void on_View_timeChanged(int value);
    void on_View_mousePressed(Qt::MouseButton button, int samplex);
    void on_View_mouseMoved(int samplex);


    void on_AddReference();
    void on_RemoveReference(int idx);
    void on_ReferenceSelected(int idx);

    void on_ChangedActiveRelations();


    //virtual bool eventFilter(QObject* /*obj*/, QEvent* evt);
    void on_actionFocus_triggered();
    void on_actionSelected_view_triggered();
};




#endif // MAINWINDOW_H

