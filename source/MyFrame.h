#ifndef MYFRAME_H
#define MYFRAME_H



#include <QWidget>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>

//class MyFrame : public QFrame
class MyFrame : public QWidget
{
   Q_OBJECT
public:
   MyFrame(QWidget *parent = 0,Qt::WindowFlags f = 0);
   virtual ~MyFrame();


   virtual bool hasFocus() const;         // Redefined as virtual

   virtual void setBg();
   int refresh;

   void myPaint();

protected:

   //QSliderExt *slider;
   QSlider *slider;
   QVBoxLayout *box;
   QPushButton *button;

   virtual bool eventFilter(QObject* obj, QEvent* evt);

   virtual void paintEvent(QPaintEvent *event);
   virtual void focusInEvent ( QFocusEvent * event );
   virtual void focusOutEvent ( QFocusEvent * event );

};

#endif // MYFRAME_H
