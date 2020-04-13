#ifndef NPLOTWIDGETSCOPE_H
#define NPLOTWIDGETSCOPE_H


#include <vector>
#include <string>
#include "NPlotWidgetBase.h"
#include "DScope/Scope.h"


typedef struct
{
   std::string title;
   std::vector<unsigned> colors;
   int xscale;
   int yscalemin,yscalemax;
   bool yauto;
} NPLOTWIDGETSCOPECONFIG;




// New stickman widget....
class NPlotWidgetScope : public NPlotWidgetBase, public Scope
{
   Q_OBJECT
public:
   NPlotWidgetScope(QWidget * parent = 0, Qt::WindowFlags f = 0);
   virtual ~NPlotWidgetScope();

   virtual void plot();
   virtual void setData(const vector<vector<int> *> &_v,const vector<unsigned> &_color);
   virtual void setSampleOffset(int co);
   //virtual void Resize(QImage *_pixmap,unsigned _x,unsigned _y,unsigned _w,unsigned _h);
   virtual void setAlpha(bool _alpha);

   unsigned getx() {
       return basex;};
   unsigned gety() {
       return basey;
   };

protected:
    vector<vector<int> *> v;
    vector<unsigned> color;

    bool alpha;
    unsigned char *rgb;
    int bpl;

    virtual void cleararea();
    virtual void fastPixelColor(int x,int y,unsigned color);
    virtual void lineColor(int x1,int y1,int x2,int y2,unsigned color=0xffffff);
    virtual void hLineColor(int x1,int y1,int x2,unsigned color=0xffffff);
    virtual void fasthLineColor(int x1, int y, int x2, unsigned color);
    virtual void vLineColor(int x1,int y1,int y2,unsigned color=0xffffff);
    virtual void fastvLineColor(int x, int y1, int y2, unsigned color);
    virtual void fastStringColor(int x,int y,const char *s,unsigned color=0xffffff);
    virtual void fastStart();
    virtual unsigned fastColor(unsigned color);
    virtual void fastStop();
    virtual void icharacterColor(int x,int y,char c,unsigned color);



    virtual unsigned ColorBlack();
    virtual unsigned ColorWhite();
    virtual unsigned ColorGrey();
    virtual unsigned ColorLGrey();
    virtual unsigned ColorRed();

    virtual void resizeEvent(QResizeEvent *event);
    virtual void wheelEvent (QWheelEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );


signals:
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);
   void panLeft();
   void panRight();









};


#endif // NPLOTWIDGETSCOPE_H
