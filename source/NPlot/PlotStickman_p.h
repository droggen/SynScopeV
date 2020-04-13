#ifndef PLOTSTICKMAN_P_H
#define PLOTSTICKMAN_P_H

#include <vector>
#include <QGLWidget>


// This is a private class renders a given set of joints to a GLWidget and supports grabbing the widget
class PlotStickman_p : public QGLWidget
{
public:
    PlotStickman_p(QWidget *parent=0);
    virtual ~PlotStickman_p();


    virtual void Plot(std::vector<float> q_torso, std::vector<float> q_lua,std::vector<float> q_lla,std::vector<float> q_rua,std::vector<float> q_rla);
    void setJoints(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot);
    void setJointsQ(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot);
    void convertXsense2Stickman(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot,
                                         std::vector<float> &_torsorot2,std::vector<float> &_luarot2,std::vector<float> &_llarot2,std::vector<float> &_ruarot2,std::vector<float> &_rlarot2);

    virtual const QImage &GetImage();
protected:
    QImage pixmap;


    float SHOULDER_WIDTH;
    float TORSO_HEIGHT;
    float TORSO_RADIUS_TOP;
    float TORSO_RADIUS_BOT;
    float HIP_WIDTH;
    float LEG_LENGTH;
    float UARM_LENGTH;
    float LARM_LENGTH;
    float UARM_RADIUS_TOP;
    float UARM_RADIUS_BOT;
    float LARM_RADIUS_TOP;
    float LARM_RADIUS_BOT;
    float SHOULDER_RADIUS;
    float SKULL_RADIUS;
    float EYE_RADIUS;
    float PUPIL_RADIUS;
    int NUM_POLY;

    void createDisplayList();
    void resizeGL(int w, int h);
    void paintGL();

    void drawStickman();
    void drawFloor();

    GLuint object;
    std::vector<float> base_coord,head_coord,lshoulder_coord,rshoulder_coord,lelbow_coord,relbow_coord,lleg_coord,rleg_coord,hiprot;



   // Objects
   std::vector<float> lightball_vertices;
   std::vector<short> lightball_indices;
   std::vector<float> cone_vertices;
   std::vector<short> cone_indices;
   std::vector<float> torso_vertices,torso_vertices_front;
   std::vector<short> torso_indices,torso_indices_front;
   std::vector<float> shoulder_vertices,hip_vertices,skull_vertices,eye_vertices,pupil_vertices,leg_vertices;
   std::vector<short> shoulder_indices,hip_indices,skull_indices,eye_indices,pupil_indices,leg_indices;

   std::vector<float> uarm_vertices,uarm_vertices_front,larm_vertices,larm_vertices_front,floor_vertices;
   std::vector<short> uarm_indices,uarm_indices_front,larm_indices,larm_indices_front,floor_indices;

   // Rotations are stored in opengl notation aka exponential notation (angle/vector)
   std::vector<float> torsorot,luarot,llarot,ruarot,rlarot;

   void setColor(float r,float g,float b);



};



void drawlist(std::vector<float> vertices,std::vector<short> indices);

#endif // PLOTSTICKMAN_P_H
