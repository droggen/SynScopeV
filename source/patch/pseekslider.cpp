// Patched seekslider: missing call to setFocusProxy

/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "pseekslider.h"
#include "pseekslider_p.h"
#include "mediaobject.h"
#include "phonondefs_p.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_SEEKSLIDERP

namespace Phonon
{

SeekSliderP::SeekSliderP(QWidget *parent)
    : QWidget(parent)
    , k_ptr(new SeekSliderPPrivate(this))
{
    K_D(SeekSliderP);
    connect(&d->slider, SIGNAL(valueChanged(int)), SLOT(_k_seek(int)));
}

SeekSliderP::SeekSliderP(MediaObject *mo, QWidget *parent)
    : QWidget(parent)
    , k_ptr(new SeekSliderPPrivate(this))
{
    K_D(SeekSliderP);
    connect(&d->slider, SIGNAL(valueChanged(int)), SLOT(_k_seek(int)));
    setMediaObject(mo);
}

/*SeekSlider::SeekSlider(SeekSliderPrivate &_d, QWidget *parent)
    : QWidget(parent)
    , k_ptr(&_d)
{
} */

SeekSliderP::~SeekSliderP()
{
    delete k_ptr;
}

void SeekSliderP::setMediaObject(MediaObject *media)
{
    K_D(SeekSliderP);
    if (d->media) {
        disconnect(d->media, 0, this, 0);
    }
    d->media = media;

    if (media) {
        connect(media, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
                SLOT(_k_stateChanged(Phonon::State)));
        connect(media, SIGNAL(totalTimeChanged(qint64)), SLOT(_k_length(qint64)));
        connect(media, SIGNAL(tick(qint64)), SLOT(_k_tick(qint64)));
        connect(media, SIGNAL(seekableChanged(bool)), SLOT(_k_seekableChanged(bool)));
        connect(media, SIGNAL(currentSourceChanged(const Phonon::MediaSource&)), SLOT(_k_currentSourceChanged()));
        d->_k_stateChanged(media->state());
        d->_k_seekableChanged(media->isSeekable());
        d->_k_length(media->totalTime());
    } else {
        d->_k_stateChanged(Phonon::StoppedState);
        d->_k_seekableChanged(false);
    }
}

MediaObject *SeekSliderP::mediaObject() const
{
    K_D(const SeekSliderP);
    return d->media;
}

void SeekSliderPPrivate::_k_seek(int msec)
{
    if (!ticking && media) {
        media->seek(msec);
    }
}

void SeekSliderPPrivate::_k_tick(qint64 msec)
{
    ticking = true;
    slider.setValue(msec);
    ticking = false;
}

void SeekSliderPPrivate::_k_length(qint64 msec)
{
    ticking = true;
    slider.setRange(0, msec);
    ticking = false;
}

void SeekSliderPPrivate::_k_seekableChanged(bool isSeekable)
{
    if (!isSeekable || !media) {
        setEnabled(false);
    } else {
        switch (media->state()) {
        case Phonon::PlayingState:
            if (media->tickInterval() == 0) {
                // if the tick signal is not enabled the slider is useless
                // set the tickInterval to some common value
                media->setTickInterval(350);
            }
        case Phonon::BufferingState:
        case Phonon::PausedState:
            setEnabled(true);
            break;
        case Phonon::StoppedState:
        case Phonon::LoadingState:
        case Phonon::ErrorState:
            setEnabled(false);
            ticking = true;
            slider.setValue(0);
            ticking = false;
            break;
        }
    }
}

void SeekSliderPPrivate::_k_currentSourceChanged()
{
    //this releases the mouse and makes the seek slider stop seeking if the current source has changed
    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(), Qt::LeftButton, 0, 0);
    QApplication::sendEvent(&slider, &event);
}

void SeekSliderPPrivate::setEnabled(bool x)
{
    slider.setEnabled(x);
    iconLabel.setPixmap(icon.pixmap(iconSize, x ? QIcon::Normal : QIcon::Disabled));
}

void SeekSliderPPrivate::_k_stateChanged(State newstate)
{
    if (!media || !media->isSeekable()) {
        setEnabled(false);
        return;
    }
    switch (newstate) {
    case Phonon::PlayingState:
        if (media->tickInterval() == 0) {
            // if the tick signal is not enabled the slider is useless
            // set the tickInterval to some common value
            media->setTickInterval(350);
        }
    case Phonon::BufferingState:
    case Phonon::PausedState:
        setEnabled(true);
        break;
    case Phonon::StoppedState:
    case Phonon::LoadingState:
    case Phonon::ErrorState:
        setEnabled(false);
        ticking = true;
        slider.setValue(0);
        ticking = false;
        break;
    }
}

bool SeekSliderP::hasTracking() const
{
    return k_ptr->slider.hasTracking();
}

void SeekSliderP::setTracking(bool tracking)
{
    k_ptr->slider.setTracking(tracking);
}

int SeekSliderP::pageStep() const
{
    return k_ptr->slider.pageStep();
}

void SeekSliderP::setPageStep(int milliseconds)
{
    k_ptr->slider.setPageStep(milliseconds);
}

int SeekSliderP::singleStep() const
{
    return k_ptr->slider.singleStep();
}

void SeekSliderP::setSingleStep(int milliseconds)
{
    k_ptr->slider.setSingleStep(milliseconds);
}

bool SeekSliderP::isIconVisible() const
{
    K_D(const SeekSliderP);
    return d->iconLabel.isVisible();
}

void SeekSliderP::setIconVisible(bool vis)
{
    K_D(SeekSliderP);
    d->iconLabel.setVisible(vis);
}

Qt::Orientation SeekSliderP::orientation() const
{
    return k_ptr->slider.orientation();
}

void SeekSliderP::setOrientation(Qt::Orientation o)
{
    K_D(SeekSliderP);
    Qt::Alignment align = (o == Qt::Horizontal ? Qt::AlignVCenter : Qt::AlignHCenter);
    d->layout.setAlignment(&d->iconLabel, align);
    d->layout.setAlignment(&d->slider, align);
    d->layout.setDirection(o == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    d->slider.setOrientation(o);
}

QSize SeekSliderP::iconSize() const
{
    return k_ptr->iconSize;
}

void SeekSliderP::setIconSize(const QSize &iconSize)
{
    K_D(SeekSliderP);
    d->iconSize = iconSize;
    d->iconLabel.setPixmap(d->icon.pixmap(d->iconSize, d->slider.isEnabled() ? QIcon::Normal : QIcon::Disabled));
}

} // namespace Phonon

#endif //QT_NO_PHONON_SEEKSLIDER

QT_END_NAMESPACE

#include "moc_pseekslider.cpp"

// vim: sw=4 ts=4
