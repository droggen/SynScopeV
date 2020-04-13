#include "SeekSlider2.h"

//#include "seekslider.h"

SeekSlider2::SeekSlider2 ( QWidget * parent )
    : Phonon::SeekSlider(parent)
{
    //setFocusProxy(k_ptr->slider);


}

SeekSlider2::SeekSlider2 ( Phonon::MediaObject * media, QWidget * parent  )
    : Phonon::SeekSlider(media,parent)
{

}
