#ifndef PLOTSTICKMAN_H
#define PLOTSTICKMAN_H


#include <QGLWidget>
#include "PlotWidgetBase.h"
#include "PlotStickman_p.h"



const float SHOULDER_WIDTH   = 0.4f;
const float TORSO_HEIGHT  = 0.6f;
const float TORSO_RADIUS_TOP = 0.1f;
const float TORSO_RADIUS_BOT = 0.065f;
const float HIP_WIDTH        = 0.20f;
const float LEG_LENGTH = 1.2f;
const float UARM_LENGTH = 0.31f;
const float LARM_LENGTH = 0.31f;
const float UARM_RADIUS_TOP = 0.04f;
const float UARM_RADIUS_BOT = 0.03f;
const float LARM_RADIUS_TOP = 0.03f;
const float LARM_RADIUS_BOT = 0.02f;
const float SHOULDER_RADIUS = 0.065f;
const float SKULL_RADIUS = 0.22f;
const float EYE_RADIUS = 0.05f;
const float PUPIL_RADIUS = 0.02f;
const int NUM_POLY = 12;


class DataStickman : public DataBase
{
public:
   DataStickman() {};
   virtual ~DataStickman() {};


   // Store all the data, here as integers (float*1000). Format torsorot[quat][sample]
   std::vector<std::vector<int> *> torsorot,luarot,llarot,ruarot,rlarot;
};



// Public class owning an internal GL rendering widget
class PlotStickman : public PlotBase
{
public:
    PlotStickman();
    virtual ~PlotStickman();

    virtual void SetTitle(std::string &t);
    virtual void SetSampleOffset(int value);
    virtual void SetData(DataBase *data);       // !Must pass a DataScatter type here
    virtual void Plot();
    virtual void Resize(unsigned _w,unsigned _h);

    virtual const QImage &GetImage();

protected:
    DataStickman data;
    PlotStickman_p gl;
    int sampleoffset;



};


#endif // PLOTSTICKMAN_H
