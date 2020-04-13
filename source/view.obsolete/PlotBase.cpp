#include "PlotBase.h"


DataBase::DataBase()
{

}

DataBase::~DataBase()
{

}


// This should be the base class for any widget displaying signals
PlotBase::PlotBase()
    : pixmap(1,1,QImage::Format_RGB32)
{

}

PlotBase::~PlotBase()
{

}
