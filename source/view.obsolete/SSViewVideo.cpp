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

#include <QDir>

#include "SSViewVideo.h"



SSViewVideo::SSViewVideo(QWidget *parent) : SSViewAbstract(parent)
{
   mo=0;
   src=0;
   vw=0;
   audioOutput=0;
   slider=0;
   button=0;
   volumeSlider=0;
   vbox=0;
   hboxctrl=0;
   hboxvid=0;
   frame=0;





   // Create the video related widgets
   mo = new Phonon::MediaObject(this);
   //src = new Phonon::MediaSource(file);
   //mo->setCurrentSource(*src);
   mo->setTickInterval(500);
   vw = new VideoWidgetExt(this);
   Phonon::createPath(mo, vw);
   audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
   Phonon::createPath(mo, audioOutput);

   // Seek slider
   slider = new Phonon::SeekSlider(this);
   slider->setMediaObject(mo);
   slider->setSingleStep(20);

   // Volume slider
   volumeSlider = new Phonon::VolumeSlider;
   volumeSlider->setOrientation(Qt::Vertical);
   volumeSlider->setMuteVisible(false);
   volumeSlider->setAudioOutput(audioOutput);

   vw->setVisible(true);
   vw->setScaleMode(Phonon::VideoWidget::FitInView);
   //vd.vw->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);

   // Start the video
   mo->pause();


   // Create the play/pause button
   QIcon icon(":playpause.png");
   button = new QPushButton(icon,"");

   // Set up the horizontal layout: slider and play/pause button
   hboxctrl = new QHBoxLayout();
   hboxctrl->addWidget(slider);
   hboxctrl->addWidget(button);
   hboxctrl->addWidget(volumeSlider);

   // Center the video video
   hboxvid = new QHBoxLayout();
   hboxvid->addWidget(vw);

   // Set-up the vertical layout: video widget, and slider/button
   vbox = new QVBoxLayout();
   vbox->addLayout(hboxvid);
   vbox->addLayout(hboxctrl);

   // Setup the layout in the frame
   setLayout(vbox);


   // Add a button handler
   connect(button, SIGNAL(clicked(bool)), this, SLOT(on_videoPlayPauseButton(bool)));

   // Add various video handlers

   //connect(vd.mo,SIGNAL(stateChanged(Phonon::State, Phonon::State)),this,SLOT(stateChanged(Phonon::State, Phonon::State)));
   connect(mo,SIGNAL(tick(qint64)),this,SLOT(on_Video_tick(qint64)));
   connect(vw,SIGNAL(mousePressed(Qt::MouseButton,int)),this,SLOT(on_View_mousePressed(Qt::MouseButton,int)));
   connect(vw,SIGNAL(mouseMoved(int)),this,SLOT(on_View_mouseMoved(int)));


}

SSViewVideo::~SSViewVideo()
{
   printf("SSViewVideo::~SSViewVideo\n");
   mo->stop();

   // Delete layout first
   //printf("Delete hboxctrl\n");
   if(hboxctrl) delete hboxctrl;
   //printf("Delete hboxvid\n");
   if(hboxvid) delete hboxvid;
   //printf("Delete vbox\n");
   if(vbox) delete vbox;

   if(button) delete button;
   if(mo) delete mo;
   if(vw) delete vw;
   if(audioOutput) delete audioOutput;
   if(slider) delete slider;
   if(volumeSlider) delete volumeSlider;
}

void SSViewVideo::setConfig(QString file)
{
   src = new Phonon::MediaSource(file);
   mo->setCurrentSource(*src);
   // Start the video
   mo->pause();
}

void SSViewVideo::paintEvent(QPaintEvent *event)
{
   QFrame::paintEvent(event);
}

/*void SSViewVideo::setConfig(SSViewVideoConfig c)
{
   config = c;
}*/

// The video time changed
void SSViewVideo::on_Video_tick (qint64 time)
{
   //printf("Video tick: %u. NoSliderEvent: %d\n",(unsigned int)time,NoSliderEvents);
   if(!NoSliderEvents)
      emit timeChanged((unsigned int)time);
   NoSliderEvents=false;
}


void SSViewVideo::setTime(int t)
{

   /*
      When the time is set programmatically we must ensure that no slider signal is emitted

      We employ a counter for DeafToVideoSliderEvents because the video scrolling is asynchronous.
      Thus, upon change of offset, we can't just stop being deaf... as the offset change will only come later.
      We know how many videos should change, so we use the counter to detect when in principle it's again the user scrolling.
   */
   printf("Set time: %d. NoSliderEvent: %d\n",t,NoSliderEvents);

   NoSliderEvents = true;
   mo->seek(t);
}

int SSViewVideo::getTime()
{
   return (int)mo->currentTime();
}






void SSViewVideo::on_videoPlayPauseButton(bool)
{
   //printf("on_videoPlayPauseButton: current state of vid: %d\n",mo->state());
   // Change the state of the play
   if(mo->state() == Phonon::PlayingState)
      mo->pause();
   //if(VideoViewData[i].mo->state() == Phonon::PausedState)
   if(mo->state() == Phonon::PausedState || mo->state() == Phonon::StoppedState)
      mo->play();

}
/******************************************************************************
   Signals & Slots
*******************************************************************************
******************************************************************************/
void SSViewVideo::on_View_mousePressed(Qt::MouseButton button, int samplex)
{
   emit mousePressed(button,samplex);
}
void SSViewVideo::on_View_mouseMoved(int samplex)
{
   emit mouseMoved(samplex);
}


QString SSViewVideo::name()
{
   return QString("Video");
}
