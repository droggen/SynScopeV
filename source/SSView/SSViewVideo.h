/*
   SynScopeV

   Copyright (C) 2008,2009,2010,2011:
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

#ifndef _SSVIEWVIDEO_H
#define _SSVIEWVIDEO_H

#include <QWidget>
#include <QFrame>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPushButton>
#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/AudioOutput>
#include <Phonon/VideoPlayer>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>
#include <QPointer>
#include <vector>
#include "SSViewAbstract.h"
#include "videowidgetext.h"
#include "guihelper.h"
//#include "SeekSlider2.h"


/******************************************************************************
   SSViewVideoConfig
*******************************************************************************

   Configuration for SSViewVideo

******************************************************************************/
/*
typedef struct
{
} SSViewVideoConfig;
*/

/******************************************************************************
   SSViewVideo
*******************************************************************************

   Widget presenting a video view, with the associated controls (scrollbar, play, etc)

******************************************************************************/
class SSViewVideo : public SSViewAbstract
{
   Q_OBJECT
public:
    SSViewVideo(QWidget *parent = 0);
    virtual ~SSViewVideo();

    virtual void setConfig(QString file);
    virtual void setTime(int t);
    virtual int getTime();
    virtual void lengthChanged() {};
    virtual QString name();
    virtual bool hasFocus () const;
    virtual QSize frameSize() const;

protected:
    // GUI elements
    QPointer<Phonon::MediaObject> mo;
    Phonon::MediaSource *src;
    QPointer<VideoWidgetExt> vw;
    QPointer<Phonon::AudioOutput> audioOutput;
    QPointer<Phonon::SeekSlider> slider;
    //QPointer<SeekSlider2> slider;
    QPointer<QPushButton> button;
    QPointer<Phonon::VolumeSlider> volumeSlider;
    QPointer<QVBoxLayout> vbox;
    QPointer<QHBoxLayout> hboxctrl,hboxvid;
    QPointer<QFrame> frame;
    QString file;

    //virtual void paintEvent(QPaintEvent *event);

    //virtual bool eventFilter(QObject* obj, QEvent* evt);

private slots:
   //void stateChanged(Phonon::State, Phonon::State);
   void on_videoPlayPauseButton(bool);
   void on_Video_tick (qint64 time);
   virtual void on_View_mousePressed(Qt::MouseButton button, int samplex);
   virtual void on_View_mouseMoved(int samplex);


/*signals:
   void timeChanged(int value);
   void mousePressed(Qt::MouseButton button,int samplex);
   void mouseMoved(int samplex);*/


};

#endif // _SSVIEWVIDEO_H
