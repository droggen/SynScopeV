#ifndef SEEKSLIDER2_H
#define SEEKSLIDER2_H

#include <Phonon/SeekSlider>


class SeekSlider2: public Phonon::SeekSlider
{
    Q_OBJECT
public:
    SeekSlider2 ( QWidget * parent = 0 );
    SeekSlider2 ( Phonon::MediaObject * media, QWidget * parent = 0 );

};

#endif // SEEKSLIDER2_H
