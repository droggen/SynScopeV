#ifndef PLOTHELPER_H
#define PLOTHELPER_H

#include <Phonon/VideoWidget>
#include <Phonon/MediaSource>
#include <Phonon/MediaObject>
#include "videowidgetext.h"
#include "precisetimer.h"
#include "NPlotWidgetScope.h"
#include "NPlotWidgetScatter.h"
#include "NPlotWidgetLabel.h"
#include "NPlotWidgetStickman.h"
#include "data.h"
#include "QVideoDecoder.h"

/*
  Plot view helper wraps the data required to render a view in a standalone bitmap, e.g. for video rendering
*/

class PlotHelperAbstract
{
protected:
   int videow,videoh;
public:
   PlotHelperAbstract(int w,int h);
   virtual ~PlotHelperAbstract();
   virtual void PlotOffset(int offset) = 0;
   virtual const QImage &GetImage() = 0;
   virtual unsigned getMaxOffset() = 0;
};

/*
  Plot view helper wraps the data required to render a view in a standalone bitmap, e.g. for video rendering
*/
class PlotHelperScope : public PlotHelperAbstract
{
   protected:
      NPlotWidgetScope plotwidget;
      unsigned maxoffset;

   public:
      PlotHelperScope(int w,int h,bool center,std::vector<std::vector<int> *> &_vd,NPLOTWIDGETSCOPECONFIG config,bool drawhaxis,bool drawvaxis,bool drawframe);
      virtual ~PlotHelperScope() {};
      void PlotOffset(int offset);
      virtual const QImage &GetImage();
      virtual unsigned getMaxOffset() {return maxoffset;}
};


class PlotHelperScatter : public PlotHelperAbstract
{
   protected:
      NPlotWidgetScatter plotwidget;
      unsigned maxoffset;

   public:
      PlotHelperScatter(int w,int h,
                        const TRACESRI &data_x,
                        const TRACESRI &data_y,
                        NPLOTWIDGETSCATTERCONFIG config);
      virtual ~PlotHelperScatter() {};
      void PlotOffset(int offset);
      virtual const QImage &GetImage();
      virtual unsigned getMaxOffset() {return maxoffset;}
};

class PlotHelperLabel : public PlotHelperAbstract
{
   protected:
      NPlotWidgetLabel plotwidget;
      unsigned maxoffset;

   public:
      PlotHelperLabel(int w,int h,
                        const TRACESRI data,
                        NPLOTWIDGETLABELCONFIG config);
      virtual ~PlotHelperLabel() {};
      void PlotOffset(int offset);
      virtual const QImage &GetImage();
      virtual unsigned getMaxOffset() {return maxoffset;}
};

class PlotHelperStickman : public PlotHelperAbstract
{
   protected:
      NPlotWidgetStickman plotwidget;
      unsigned maxoffset;

   public:
      PlotHelperStickman(int w,int h,
                           const TRACESRI &_torsorot,
                           const TRACESRI &_luarot,
                           const TRACESRI &_llarot,
                           const TRACESRI &_ruarot,
                           const TRACESRI &_rlarot,
                           NPLOTWIDGETSTICKMANCONFIG config);



      virtual ~PlotHelperStickman() {};
      void PlotOffset(int offset);
      virtual const QImage &GetImage();
      virtual unsigned getMaxOffset() {return maxoffset;}
};

class PlotHelperVideo : public PlotHelperAbstract
{
   protected:
      QVideoDecoder plotwidget;
      unsigned maxoffset;
      QImage pixmap;

   public:
      PlotHelperVideo(int w,int h,QString filename);

      virtual ~PlotHelperVideo() {};
      void PlotOffset(int offset);
      virtual const QImage &GetImage();
      virtual unsigned getMaxOffset() {return maxoffset;}
};

#endif // PLOTHELPER_H
