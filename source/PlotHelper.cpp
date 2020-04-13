#include "PlotHelper.h"




/******************************************************************************
*******************************************************************************
* PlotAbstractHelper   PlotAbstractHelper   PlotAbstractHelper   PlotAbstractHe
*******************************************************************************
******************************************************************************/
PlotHelperAbstract::PlotHelperAbstract(int w,int h)
{
   printf("PlotAbstractHelper::PlotAbstractHelper\n");
   videow=w;
   videoh=h;
}

PlotHelperAbstract::~PlotHelperAbstract()
{

}

/******************************************************************************
*******************************************************************************
* PlotScopeHelper   PlotScopeHelper   PlotScopeHelper   PlotScopeHelper   PlotS
*******************************************************************************
******************************************************************************/
PlotHelperScope::PlotHelperScope(int w,int h,bool center,std::vector<std::vector<int> *> &_vd,NPLOTWIDGETSCOPECONFIG config,bool drawhaxis,bool drawvaxis,bool drawframe)
   :PlotHelperAbstract(w,h)
{
    printf("videow/h: %d %d\n",videow,videoh);
    printf("w/h: %d %d\n",w,h);
    printf("vd.size: %d colors.size: %d\n",_vd.size(),config.colors.size());

    plotwidget.resize(w,h);
    if(_vd.size())
       maxoffset = _vd[0]->size();
    else
       maxoffset = 0;

    plotwidget.setAttribute(Qt::WA_DontShowOnScreen);       // Required to handle the resizing correctly: widget must be visible yet unshown
    plotwidget.show();                                      // Required to handle the resizing correctly: widget must be visible yet unshown

    plotwidget.setData(_vd,config.colors);
    if(config.yauto)
       plotwidget.SetVAuto();
    else
       plotwidget.SetVRange(config.yscalemin,config.yscalemax);
    plotwidget.HZoom(config.xscale);
    plotwidget.SetTitle(config.title);
    plotwidget.SetEnableHGrid(drawhaxis);
    plotwidget.SetEnableVGrid(drawvaxis);
    plotwidget.SetEnableFrame(drawframe);



    if(center)
    {
       // Set alignment and grid position to have the offset sample in the middle (like in the UI).
       // Thus the 'current' sample corresponding to the current video frame is in the center of the scope.
       plotwidget.SetAlignment(true);
       plotwidget.SetGridPosition(true);
    }
    else
    {
       // For videos when not showing the axes, it may be nicer to have the 'current' sample corresponding to the current video frame on the right.
       plotwidget.SetAlignment(false);
       plotwidget.SetGridPosition(true);
       plotwidget.SetRightAlignLast(false);
    }
}

void PlotHelperScope::PlotOffset(int offset)
{
   plotwidget.setSampleOffset(offset);
   plotwidget.plot();
}

const QImage &PlotHelperScope::GetImage()
{
   return plotwidget.getImage();
}

/******************************************************************************
*******************************************************************************
* PlotHelperScatter   PlotHelperScatter   PlotHelperScatter   PlotHelperScatter
*******************************************************************************
******************************************************************************/

PlotHelperScatter::PlotHelperScatter(int w,int h,const TRACESRI &data_x,const TRACESRI &data_y,NPLOTWIDGETSCATTERCONFIG config)
   :PlotHelperAbstract(w,h)
{
   if(data_x.size())
      maxoffset = data_x[0]->size();
   else
      maxoffset = 0;

   plotwidget.setAttribute(Qt::WA_DontShowOnScreen);       // Required to handle the resizing correctly: widget must be visible yet unshown
   plotwidget.show();                                      // Required to handle the resizing correctly: widget must be visible yet unshown
   plotwidget.setData(data_x,data_y,config.colors);
   plotwidget.resize(w,h);
   plotwidget.setAutoscale(config.autoscale);
   plotwidget.setScale(config.xmin,config.ymin,config.xmax,config.ymax);
   plotwidget.setTitle(config.title);
}

void PlotHelperScatter::PlotOffset(int offset)
{
   plotwidget.setSampleOffset(offset);
   plotwidget.plot();
}

const QImage &PlotHelperScatter::GetImage()
{
   return plotwidget.getImage();
}
/******************************************************************************
*******************************************************************************
* PlotHelperLabel   PlotHelperLabel   PlotHelperLabel   PlotHelperLabel
*******************************************************************************
******************************************************************************/

PlotHelperLabel::PlotHelperLabel(int w,int h,
                                 const TRACESRI data,
                                 NPLOTWIDGETLABELCONFIG config)
   :PlotHelperAbstract(w,h)
{
   if(data.size())
      maxoffset = data[0]->size();
   else
      maxoffset = 0;

   plotwidget.setAttribute(Qt::WA_DontShowOnScreen);       // Required to handle the resizing correctly: widget must be visible yet unshown
   plotwidget.show();                                      // Required to handle the resizing correctly: widget must be visible yet unshown

   plotwidget.setData(data,config.colors);
   plotwidget.resize(w,h);
   //plotwidget.setAutoscale(config.autoscale);
   //plotwidget.setScale(config.xmin,config.ymin,config.xmax,config.ymax);
   plotwidget.setTitle(config.title);
   plotwidget.setSubtitle(config.subtitle);
}

void PlotHelperLabel::PlotOffset(int offset)
{
   plotwidget.setSampleOffset(offset);
   plotwidget.plot();
}

const QImage &PlotHelperLabel::GetImage()
{
   return plotwidget.getImage();
}

/******************************************************************************
*******************************************************************************
* PlotHelperStickman   PlotHelperStickman   PlotHelperStickman   PlotHelperStickman
*******************************************************************************
******************************************************************************/

PlotHelperStickman::PlotHelperStickman(int w,int h,
                                       const TRACESRI &_torsorot,
                                       const TRACESRI &_luarot,
                                       const TRACESRI &_llarot,
                                       const TRACESRI &_ruarot,
                                       const TRACESRI &_rlarot,
                                     NPLOTWIDGETSTICKMANCONFIG config)
   :PlotHelperAbstract(w,h)
{

   if(_torsorot.size())
      maxoffset = _torsorot[0]->size();
   else
      maxoffset = 0;

   plotwidget.setAttribute(Qt::WA_DontShowOnScreen);       // Required to handle the resizing correctly: widget must be visible yet unshown
   plotwidget.show();                                      // Required to handle the resizing correctly: widget must be visible yet unshown

   plotwidget.setData(_torsorot,_luarot,_llarot,_ruarot,_rlarot);
   plotwidget.resize(w,h);

   plotwidget.setTitle(config.title);
}

void PlotHelperStickman::PlotOffset(int offset)
{
   plotwidget.setSampleOffset(offset);
   plotwidget.plot();
}

const QImage &PlotHelperStickman::GetImage()
{
   return plotwidget.getImage();
}


/******************************************************************************
*******************************************************************************
* PlotHelperVideo   PlotHelperVideo   PlotHelperVideo   PlotHelperVideo
*******************************************************************************
******************************************************************************/

PlotHelperVideo::PlotHelperVideo(int w,int h,QString filename)
   :PlotHelperAbstract(w,h)
{
   pixmap = QImage(w,h,QImage::Format_RGB32);
   bool ok = plotwidget.openFile(filename);
   maxoffset = plotwidget.getVideoLengthMs();
}

void PlotHelperVideo::PlotOffset(int offset)
{
   bool ok = plotwidget.seekMs(offset);
}

const QImage &PlotHelperVideo::GetImage()
{
   QImage tmp;
   bool ok = plotwidget.getFrame(tmp);
   if(tmp.width()==videow && tmp.height()==videoh)
   {
      // Fast path, no scaling
      pixmap = tmp.convertToFormat(QImage::Format_RGB32);
      return pixmap;
   }
   // Slow path, rescaling
   tmp = tmp.convertToFormat(QImage::Format_RGB32);
   pixmap = tmp.scaled(pixmap.size());
   return pixmap;
}


