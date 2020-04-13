#include "NPlotWidgetScope.h"
#include "SDL_gfxPrimitives_font.h"

NPlotWidgetScope::NPlotWidgetScope(QWidget * parent, Qt::WindowFlags f)
   : NPlotWidgetBase(parent,f)
   , Scope(0,0,2,2)
   , alpha(false)
{



}

NPlotWidgetScope::~NPlotWidgetScope()
{

}

void NPlotWidgetScope::setData(const vector<vector<int> *> &_v,const vector<unsigned> &_color)
{
    v=_v;
    color=_color;
}
void NPlotWidgetScope::setAlpha(bool _alpha)
{
    alpha=_alpha;
}
void NPlotWidgetScope::setSampleOffset(int co)
{
    NPlotWidgetBase::setSampleOffset(co);
    offsetsample = co;
}

void NPlotWidgetScope::plot()
{
    painter.begin(&pixmap);
    Scope::Plot(v,color);
    painter.end();


    NPlotWidgetBase::plot();
}


void NPlotWidgetScope::cleararea()
{
    painter.setClipRect(basex,basey,w,h);
    // Optimized version for 32-bit that keeps the alpha channel unchanged.
    if( pixmap.format()==QImage::Format_RGB32 ||
        pixmap.format()==QImage::Format_ARGB32 ||
        pixmap.format()==QImage::Format_ARGB32_Premultiplied
        )
    {
        rgb = pixmap.bits();
        bpl = pixmap.bytesPerLine();
        unsigned char *pixel,*pixelline;
        pixel = rgb+4*basex+bpl*basey;

        if(alpha)
        {
            // Optimized fadeout
            for(unsigned y=0;y<h;y++)
            {
                pixelline=pixel;
                for(unsigned x=0;x<w;x++)
                {
                    // Read pixel
                    unsigned p=*(unsigned *)pixelline;
                    // Write divided by 2 (alpha=128) with corrected shifted bits.
                    *(unsigned *)pixelline = ((p>>1)&0x7f7f7f);
                    pixelline+=4;
                }
                pixel+=bpl;
            }
        }
        else
        {
            // Clear.
            for(unsigned y=0;y<h;y++)
            {
                pixelline=pixel;
                for(unsigned x=0;x<w;x++)
                {
                    // Keep alpha, clear RGB
               *(unsigned *)pixelline = 0;
                    pixelline+=4;
                }
                pixel+=bpl;
            }
        }
    }
    else
    {
        // Unoptimized (non 32-bit)
        if(alpha)
        {
            // QT version of fadeout.
            painter.fillRect(basex,basey,w,h,QColor(0,0,0,208));
        }
        else
        {
            // QT version of clear.
            painter.fillRect(basex,basey,w,h,QColor(0,0,0,255));
        }
    }
}
void NPlotWidgetScope::fastPixelColor(int x,int y,unsigned color)
{
    painter.setPen(color);
    painter.drawPoint(basex+x,basey+y);
}
void NPlotWidgetScope::lineColor(int x1,int y1,int x2,int y2,unsigned color)
{
    painter.setPen(color);
    painter.drawLine(basex+x1,basey+y1,basex+x2,basey+y2);
}
void NPlotWidgetScope::hLineColor(int x1,int y,int x2,unsigned color)
{
    //if(pixmap->format()==QImage::Format_RGB32)
    if( pixmap.format()==QImage::Format_RGB32 ||
        pixmap.format()==QImage::Format_ARGB32 ||
        pixmap.format()==QImage::Format_ARGB32_Premultiplied
        )
    //if(0)
    {
        int xtmp;
        // Sort
        if (x1 > x2) {
            xtmp = x1;
            x1 = x2;
            x2 = xtmp;
        }
        //Check visibility of vline
        if((y<0) || (y>=h))
            return;
        if((x1<0) && (x2<0))
            return;
        if((x1>=w) && (x2>=w))
            return;
        // Clip y
        if(x1<0)
            x1 = 0;
        if(x2>=w)
            x2=w-1;
        // Rebase
        x1+=basex;
        x2+=basex;
        y+=basey;


        unsigned char *pixel,*pixellast;
        pixel = rgb + 4*x1 + bpl*y;
        pixellast = pixel + 4*(x2-x1);
        for(;pixel<=pixellast;pixel+=4)
            *(unsigned *)pixel=color;
    }
    else
    {
        lineColor(x1,y,x2,y,color);
    }
}
void NPlotWidgetScope::fasthLineColor(int x1, int y, int x2, unsigned color)
{
    hLineColor(x1,y,x2,color);
    //lineColor(x1,y,x2,y,color);
}
void NPlotWidgetScope::vLineColor(int x,int y1,int y2,unsigned color)
{

    //if(pixmap->format()==QImage::Format_RGB32)
    if( pixmap.format()==QImage::Format_RGB32 ||
        pixmap.format()==QImage::Format_ARGB32 ||
        pixmap.format()==QImage::Format_ARGB32_Premultiplied
        )
    //if(0)
    {
        int ytmp;
        // Sort
        if (y1 > y2) {
            ytmp = y1;
            y1 = y2;
            y2 = ytmp;
        }
        //Check visibility of vline
        if((x<0) || (x>=w))
            return;
        if((y1<0) && (y2<0))
            return;
        if((y1>=h) && (y2>=h))
            return;
        // Clip y
        if(y1<0)
            y1 = 0;
        if(y2>=h)
            y2=h-1;
        // Rebase
        x+=basex;
        y1+=basey;
        y2+=basey;
        // Height
        int h=y2-y1;


        //int bpl = 1;
        unsigned char *pixel,*pixellast;
        pixel = rgb + 4*x + bpl*y1;
        pixellast = pixel + bpl*h;
        for(;pixel<=pixellast;pixel+=bpl)
            *(unsigned *)pixel=color;
    }
    else
    {
        //painter.fillRect(x1,y1,1,y2-y1+1,QColor(color));
        lineColor(x,y1,x,y2,color);
    }

//	lineColor(x,y1,x1,y2,color);





}
void NPlotWidgetScope::fastvLineColor(int x, int y1, int y2, unsigned color)
{
    vLineColor(x,y1,y2,color);
    //lineColor(x,y1,x,y2,color);
}
void NPlotWidgetScope::fastStringColor(int x,int y,const char *s,unsigned color)
{
/*	painter.setPen(color);
    QFont f("Helvetica");
    f.setPixelSize(11);
    //f.setFixedPitch(true);
    painter.setFont(f);
    painter.drawText(basex+x,basey+y,s);

    return;*/

    //return;

    // Empty string
    if(*s==0)
        return;
    if(x<0 || y<0)
        return;
    if(y+7>=h)
        return;
    if(x+8*strlen(s)>=w)
        return;
    if(pixmap.format()!=QImage::Format_RGB32)	// RGB32 is optimized
        painter.setPen(color);
    do
    {
        icharacterColor(x,y,*s,color);
        x+=8;
    }
    while(*++s);

}
void NPlotWidgetScope::icharacterColor(int x,int y,char c,unsigned color)
{
    unsigned char *data;
    data = gfxPrimitivesFontdata+c*8;
    unsigned char mask;

    //if(pixmap->format()==QImage::Format_RGB32)
    if( pixmap.format()==QImage::Format_RGB32 ||
        pixmap.format()==QImage::Format_ARGB32 ||
        pixmap.format()==QImage::Format_ARGB32_Premultiplied
        )
    //if(0)
    {
        x+=basex;
        y+=basey;
        unsigned pitch = pixmap.bytesPerLine();
        unsigned char *p = pixmap.bits() + y*pitch + x*4;
        for (int iy = 0; iy < 8; iy++)
        {
            mask=0x80;
            for (int ix = 0; ix < 8; ix++)
            {
                if(*data&mask)
                {
                    *(unsigned*)p=color;
                }
                mask>>=1;
                p+=4;
            }
            data++;
            p+=pitch-32;
        }
    }
    else
    {


        for (int iy = 0; iy < 8; iy++)
        {
            mask=0x80;
            for (int ix = 0; ix < 8; ix++)
            {
                if(*data&mask)
                {
                    painter.drawPoint(basex+x+ix,basey+y+iy);
                }
                mask>>=1;
            }
            data++;
        }
    }
}
unsigned NPlotWidgetScope::fastColor(unsigned color)
{
    return color;


}

void NPlotWidgetScope::fastStart()
{
    //printf("S %p. T: %d. A: %d. px: %p f:%d x,y,w,h: %d,%d %d,%d\n",this,transparent,alpha,pixmap,pixmap->format(),basex,basey,w,h);
    rgb = pixmap.bits();
    bpl = pixmap.bytesPerLine();
}
void NPlotWidgetScope::fastStop()
{
    return;
}


unsigned NPlotWidgetScope::ColorBlack()
{
        return 0xff000000;
}
unsigned NPlotWidgetScope::ColorWhite()
{

        return 0xffffffff;
}
unsigned NPlotWidgetScope::ColorGrey()
{

        return 0xff7f7f7f;
}
unsigned NPlotWidgetScope::ColorLGrey()
{
        return 0xff3f3f3f;
}
unsigned NPlotWidgetScope::ColorRed()
{
    return 0xffff0000;
}


void NPlotWidgetScope::resizeEvent(QResizeEvent *event)
{
   printf("NPlotWidgetScope::resizeEvent\n");
   Scope::Resize(0,0,event->size().width(),event->size().height());
   NPlotWidgetBase::resizeEvent(event);

   event->accept();

}

void NPlotWidgetScope::mouseMoveEvent ( QMouseEvent * event )
{
   //printf("mouseMoveEvent\n");
   emit mouseMoved(x2s(event->x()));
   event->accept();
}
void NPlotWidgetScope::mousePressEvent ( QMouseEvent * event )
{

   // signal / slot
   emit mousePressed(event->button(),x2s(event->x()));

   //printf("mouse pressed: %d %d %d\n",event->x(),event->y(),event->button());
   //printf("Converstion of pix -> sample %d -> %d\n",event->x(),x2s(event->x()));
    switch(event->button())
    {
            case Qt::MidButton:
                    switch(event->modifiers())
                    {
                            case Qt::ShiftModifier:
                                    SetVAuto();
                                    break;
                            case Qt::NoModifier:
                                    HZoomReset();
                                    break;
                            default:
                                    event->ignore();
                                    return;
                    }
                    break;
            default:
                    event->ignore();
                    return;
    }
    event->accept();
    repaint();
}
void NPlotWidgetScope::wheelEvent (QWheelEvent * event)
{
   QWidget::wheelEvent(event);
    if(event->delta()>0)
    {
            if(event->modifiers()==Qt::ShiftModifier)
                    VZoomin();
            if(event->modifiers()==Qt::NoModifier)
                    HZoomin();
    }
    else
    {
            if(event->modifiers()==Qt::ShiftModifier)
                    VZoomout();
            if(event->modifiers()==Qt::NoModifier)
                    HZoomout();
    }
    event->accept();
    repaint();
}

void NPlotWidgetScope::keyPressEvent(QKeyEvent * event)
{
   //printf("Key press event %d\n",event->key());
   if(event->key()==Qt::Key_Up)
   {
      PanUp();
   }
   if(event->key()==Qt::Key_Down)
   {
      PanDown();
   }
   if(event->key()==Qt::Key_Left)
   {
      emit panLeft();
   }
   if(event->key()==Qt::Key_Right)
   {
      emit panRight();
   }
   if(event->key()==Qt::Key_PageUp)
   {
      // Zoom
      if(event->modifiers()==Qt::ShiftModifier)
         VZoomin();
      if(event->modifiers()==Qt::NoModifier)
         HZoomin();
   }
   if(event->key()==Qt::Key_PageDown)
   {
      // Zoom
      if(event->modifiers()==Qt::ShiftModifier)
         VZoomout();
      if(event->modifiers()==Qt::NoModifier)
         HZoomout();
   }
   if(event->key()==Qt::Key_Return)
   {
      if(event->modifiers()==Qt::ShiftModifier)
         SetVAuto();
      if(event->modifiers()==Qt::NoModifier)
         HZoomReset();
   }
   event->accept();
   repaint();

}

