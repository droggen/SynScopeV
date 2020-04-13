


#include <QPainter>
#include <QPaintEvent>
#include "MyFrame.h"

//MyFrame::MyFrame(QWidget *parent, Qt::WindowFlags f) : QFrame(parent,f)
MyFrame::MyFrame(QWidget *parent, Qt::WindowFlags f)
    //: QWidget(parent,f)
    : QWidget(parent,Qt::FramelessWindowHint)
{
    printf("----------MyFrame::MyFrame----------\n");

    setMinimumSize(20,20);

    refresh=0;

    // Slider
    //slider = new QSliderExt(Qt::Horizontal,0);
    slider = new QSlider(Qt::Horizontal,0);
    slider->setFocusPolicy(Qt::WheelFocus);   // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

    // Button
    button = new QPushButton();

    // Box
    box = new QVBoxLayout();
    //box->addWidget(plotwidget);
    box->addWidget(slider);
    box->addWidget(button);
    setLayout(box);




   // Frame
   //setFrameStyle(QFrame::Panel);
   //setFrameShadow(QFrame::Raised);
   //setAutoFillBackground(false);

   //setAttribute(Qt::WA_OpaquePaintEvent);
    //setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute();


   setMouseTracking(true);             // To follow mouse over widget and send signals
   //setFocusPolicy(Qt::StrongFocus);  // To accept focus, and thus receive keyboard events
   setFocusPolicy(Qt::WheelFocus);     // To accept focus, and thus receive keyboard events, when: click, tab, mouse wheel.

   installEventFilter(this);
   button->installEventFilter(this);
   slider->installEventFilter(this);
}

MyFrame::~MyFrame()
{

}

void MyFrame::setBg()
{
    if(hasFocus())
    {
        QPalette palette;
        palette.setColor(backgroundRole(), QColor(Qt::red));
        setPalette(palette);
    }
    else
    {
        QPalette palette;
        palette.setColor(backgroundRole(), QColor(Qt::blue));
        setPalette(palette);
    }
}

bool MyFrame::hasFocus() const
{
   return QWidget::hasFocus() || slider->hasFocus();
}
void MyFrame::focusInEvent ( QFocusEvent * event )
{
//    setBg();

  // QWidget::focusInEvent(event);
   //printf("SSViewAbstract::focusInEvent %p\n",this);
   //repaint();
   //repaint(QRect(QPoint(0,0),size()));


   //event->accept();



   //QWidget::repaint(rect());
   //myPaint();
   //QWidget::repaint();
   //update();
    event->ignore();

}

void MyFrame::focusOutEvent ( QFocusEvent * event )
{
 //   setBg();

   //QWidget::focusInEvent(event);
   //printf("SSViewAbstract::focusOutEvent %p\n",this);
   //repaint();


   //event->accept();


   //QWidget::repaint(rect());
   //myPaint();
   //QWidget::repaint();
   //update();
    event->ignore();


}
void MyFrame::myPaint()
{
    QPainter p(this);
    printf("has clipping: %d\n",p.hasClipping());
    p.eraseRect(rect());
    p.setPen(Qt::white);
    p.drawRect(0,0,width()-1,height()-1);

    p.setPen(Qt::white);
    p.drawText(10,10,QString("it %1").arg(refresh++));


    if(hasFocus())
    {
       QPen pen;
       pen.setColor(Qt::black);
       pen.setStyle(Qt::DotLine);
       p.setPen(pen);
       p.drawRect(0,0,width()-1,height()-1);


    }
    else
    {

    }
}

void MyFrame::paintEvent(QPaintEvent *event)
{

    setBg();

    printf("MyFrame::paintEvent %p. hasFocus: %d. rect: %d %d %d %d.\n",this,hasFocus(),event->rect().x(),event->rect().y(),event->rect().width(),event->rect().height());
    QRegion r = mask ();

    printf("region is empty: %d - br %d %d %d %d\n",r.isEmpty(),r.boundingRect().x(),r.boundingRect().y(),r.boundingRect().width(),r.boundingRect().height());

    myPaint();
    //event->rect() = QFrame::size();
   //QFrame::paintEvent(event);

    //QFrame::paintEvent(new QPaintEvent(QRect(QPoint(0,0),QFrame::size())));

   // Draw a frame if we have the focus


}








bool MyFrame::eventFilter(QObject* obj, QEvent* evt)
{
   //printf("MyFrame::eventFilter obj %p type %d\n",obj,evt->type());
   if(evt->type()==QEvent::FocusIn)
   {
       printf("+/- FOCUS IN\n");
       update();
       //return true;
   }
   if(evt->type()==QEvent::FocusOut)
   {
       printf("+/- FOCUS OUT\n");
       update();
       //return true;
   }
   /*if (evt->type() == QEvent::Wheel)
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
   //return true;*/
   return false;
}


