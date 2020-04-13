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

#include <stdio.h>
#include <QMouseEvent>
#include <cassert>
#include "PlotStickman.h"
#include "glhelper.h"


/******************************************************************************
*******************************************************************************
PlotStickman_p   PlotStickman_p   PlotStickman_p   PlotStickman_p   PlotStickman_p
*******************************************************************************
******************************************************************************/






PlotStickman_p::PlotStickman_p(QWidget *parent) :
    QGLWidget(parent)
{
   SHOULDER_WIDTH   = 0.4f;
   TORSO_HEIGHT  = 0.6f;
   TORSO_RADIUS_TOP = 0.1f;
   TORSO_RADIUS_BOT = 0.065f;
   HIP_WIDTH        = 0.20f;
   LEG_LENGTH = 1.2f;
   UARM_LENGTH = 0.31f;
   LARM_LENGTH = 0.31f;
   UARM_RADIUS_TOP = 0.04f;
   UARM_RADIUS_BOT = 0.03f;
   LARM_RADIUS_TOP = 0.03f;
   LARM_RADIUS_BOT = 0.02f;
   SHOULDER_RADIUS = 0.065f;
   SKULL_RADIUS = 0.22f;
   EYE_RADIUS = 0.05f;
   PUPIL_RADIUS = 0.02f;
   NUM_POLY = 12;

   torsorot={0,0,0,1};
   luarot={0,0,0,1};
   llarot={0,0,0,1};
   ruarot={0,0,0,1};
   rlarot={0,0,0,1};
   object = -1;

   // Various objects
   Cylinder2(TORSO_RADIUS_TOP,TORSO_RADIUS_BOT,DEG2RAD(80),DEG2RAD(100),TORSO_HEIGHT,NUM_POLY,torso_vertices_front,torso_indices_front,true);
   Cylinder2(TORSO_RADIUS_TOP,TORSO_RADIUS_BOT,DEG2RAD(100),DEG2RAD(440),TORSO_HEIGHT,NUM_POLY,torso_vertices,torso_indices,true);
   Cylinder(SHOULDER_RADIUS,SHOULDER_RADIUS,SHOULDER_WIDTH,NUM_POLY, shoulder_vertices, shoulder_indices,true);
   Cylinder(0.1f,0.1f,HIP_WIDTH,NUM_POLY,hip_vertices,hip_indices,true);
   Cylinder(0.05f,0.02f,LEG_LENGTH,NUM_POLY,leg_vertices,leg_indices,true);
   Cylinder2(UARM_RADIUS_TOP,UARM_RADIUS_BOT,DEG2RAD(80), DEG2RAD(100),UARM_LENGTH,NUM_POLY,uarm_vertices_front,uarm_indices_front,true);
   Cylinder2(UARM_RADIUS_TOP,UARM_RADIUS_BOT,DEG2RAD(100),DEG2RAD(440),UARM_LENGTH,NUM_POLY,uarm_vertices,uarm_indices,true);
   Cylinder2(LARM_RADIUS_TOP,LARM_RADIUS_BOT,DEG2RAD(80), DEG2RAD(100),LARM_LENGTH,NUM_POLY,larm_vertices_front,larm_indices_front,true);
   Cylinder2(LARM_RADIUS_TOP,LARM_RADIUS_BOT,DEG2RAD(100),DEG2RAD(440),LARM_LENGTH,NUM_POLY,larm_vertices,larm_indices,true);
   Cylinder2(UARM_RADIUS_TOP,UARM_RADIUS_TOP,DEG2RAD(0),DEG2RAD(360),LEG_LENGTH,NUM_POLY,cone_vertices,cone_indices,true);
   Sphere(SKULL_RADIUS,NUM_POLY,NUM_POLY,skull_vertices,skull_indices);
   Sphere(EYE_RADIUS,NUM_POLY,NUM_POLY,eye_vertices,eye_indices);
   Sphere(PUPIL_RADIUS,NUM_POLY,NUM_POLY,pupil_vertices,pupil_indices);
   Rectangle2(-3,-3,3,3,NUM_POLY,floor_vertices,floor_indices);
   Sphere(0.2,NUM_POLY,NUM_POLY,lightball_vertices,lightball_indices);




}
PlotStickman_p::~PlotStickman_p()
{
   if(object!=(GLuint)-1)
      glDeleteLists(object,1);
}






void PlotStickman_p::Plot(std::vector<float> q_torso, std::vector<float> q_lua,std::vector<float> q_lla,std::vector<float> q_rua,std::vector<float> q_rla)
{
    convertXsense2Stickman(q_torso,q_lua,q_lla,q_rua,q_rla, q_torso,q_lua,q_lla,q_rua,q_rla);

    setJointsQ(q_torso,q_lua,q_lla,q_rua,q_rla);

    // Here must render in an image
    pixmap = renderPixmap().toImage();
}

/******************************************************************************
  PlotStickman_p::setJoints
*******************************************************************************
   Set the joint of the stick man.

   Exponential notation (angle vector

   By default, zero rotation corresponds to the stick man in the following position:
   - torso vertical
   - face towards screen outside (positive z)
   - arms parallel to torso
******************************************************************************/
void PlotStickman_p::setJoints(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot)
{
   torsorot=_torsorot;
   luarot=_luarot;
   llarot=_llarot;
   ruarot=_ruarot;
   rlarot=_rlarot;
   createDisplayList();
}
/******************************************************************************
  PlotStickman_p::setJointsQ
*******************************************************************************
   Set the joint of the stick man.

   Same as setJoints, but with quaternions.
******************************************************************************/
void PlotStickman_p::setJointsQ(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot)
{
   float a;

   // Convert the quaternion to exponential notation
   a = acos(_torsorot[0]);
   torsorot={(float)RAD2DEG(a*2.f), (float)(_torsorot[1]/sin(a)), (float)(_torsorot[2]/sin(a)), (float)(_torsorot[3]/sin(a))};
   a = acos(_luarot[0]);
   luarot={(float)RAD2DEG(a*2.f), (float)(_luarot[1]/sin(a)), (float)(_luarot[2]/sin(a)), (float)(_luarot[3]/sin(a))};
   a = acos(_llarot[0]);
   llarot={(float)RAD2DEG(a*2.f), (float)(_llarot[1]/sin(a)), (float)(_llarot[2]/sin(a)), (float)(_llarot[3]/sin(a))};
   a = acos(_ruarot[0]);
   ruarot={(float)RAD2DEG(a*2.f), (float)(_ruarot[1]/sin(a)), (float)(_ruarot[2]/sin(a)), (float)(_ruarot[3]/sin(a))};
   a = acos(_rlarot[0]);
   rlarot={(float)RAD2DEG(a*2.f), (float)(_rlarot[1]/sin(a)), (float)(_rlarot[2]/sin(a)), (float)(_rlarot[3]/sin(a))};


   createDisplayList();
}



/******************************************************************************
  PlotStickman_p::convertXsense2Stickman
*******************************************************************************
   Converts the xsense quaternions, as typically embedded in a motion sensing garment,
   into the quaternions required for visualization.

   By default, zero xsense rotation corresponds to the stick man in the following position:
   - flat torso
   - face down
   - leg to north
   - arms parallel to torso

   Also, the xsense rotation axis do not match the opengl axes

   If that is not the setup of the Xsense then a prior conversion is required
******************************************************************************/
void PlotStickman_p::convertXsense2Stickman(std::vector<float> _torsorot,std::vector<float> _luarot,std::vector<float> _llarot,std::vector<float> _ruarot,std::vector<float> _rlarot,
                                     std::vector<float> &_torsorot2,std::vector<float> &_luarot2,std::vector<float> &_llarot2,std::vector<float> &_ruarot2,std::vector<float> &_rlarot2)
{
   // Convert the Xsense quaternions to the quaternions required by the stickman.

   // Normalize the quaternion prior to any operation
   _torsorot = normalize(_torsorot);
   _ruarot = normalize(_ruarot);
   _rlarot = normalize(_rlarot);
   _luarot = normalize(_luarot);
   _llarot = normalize(_llarot);

   /*
    Without xsens rotation the motion jacket is in this position:
    - Flat model
    - Face down
    - Legs pointing north
    - Arm parallel to body
   */

   // Compose two rotations:
   // First, rotate the rendered model to face inside the screen (negative z)
   // Then, rotate the rendered model to have the torso horizontal (facing downwards, leg facing north
   std::vector<float> prerot = quat_mult({(float)cos(DEG2RAD(-90.0/2.0)),(float)sin(DEG2RAD(-90.0/2.0)),0,0},
                                          {(float)cos(DEG2RAD(180.0/2.0)),0,(float)sin(DEG2RAD(+180.0/2.0)),0});


   // Convert the rotation axes from Xsense to opengl.
   std::vector<float> tmp;
   tmp = _torsorot;
   _torsorot={tmp[0],tmp[2],tmp[3],tmp[1]};

   tmp = _ruarot;
   _ruarot={tmp[0],tmp[2],tmp[3],tmp[1]};

   tmp = _rlarot;
   _rlarot={tmp[0],tmp[2],tmp[3],tmp[1]};

   tmp = _luarot;
   _luarot={tmp[0],tmp[2],tmp[3],tmp[1]};

   tmp = _llarot;
   _llarot={tmp[0],tmp[2],tmp[3],tmp[1]};


   // Effect the pre rotation and return the result.
   _torsorot2=quat_mult(_torsorot,prerot);
   _ruarot2=quat_mult(_ruarot,prerot);
   _rlarot2=quat_mult(_rlarot,prerot);
   _luarot2=quat_mult(_luarot,prerot);
   _llarot2=quat_mult(_llarot,prerot);

}



void PlotStickman_p::resizeGL(int w, int h)
{


   glViewport( 0, 0, (GLint)w, (GLint)h );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glFrustum( -1.5, 1.5, -1.5, 1.5, 5.0, 15.0 );
   glMatrixMode( GL_MODELVIEW );

   createDisplayList();

/*
   // define frustrum
   glViewport(0, 0, (GLint)w, (GLint)h );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float aspect = (float)w/(float)h;
   gluPerspective(45.0f, aspect, 5.0f, 15.0f);
   glMatrixMode(GL_MODELVIEW);*/


}

void PlotStickman_p::setColor(float r,float g,float b)
{
   //glColor4f(r,g,b,1.0);

   float mat[4]={r,g,b,1.0};
   //float mat_s[4]={r,g,b,1};
   //float low_sh[]={5.0};

   //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,mat);
   //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,mat_s);
   //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,low_sh);

   /*GLfloat mat_specular[]={0.0, 0.0, 0.0, 1.0};
   GLfloat mat_diffuse[]={r,g,b,1.0};
   GLfloat mat_ambient[]={r,g,b,1.0};
   GLfloat mat_shininess={20.0};
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);*/
}

void PlotStickman_p::paintGL()
{
   //printf("paint gl\n");
   glCallList( object );
}



/******************************************************************************
   PlotStickman_p::createDisplayList
*******************************************************************************
   Draws a stick figure with:
   - Eyes looking towards outside the screen - left body side is on the screen right side
   - w/o rotation: vertical torso, vertical upper/lower arm


   Known issues:
   -  A torso rotation of 90°<0,0,1> will lead to the leg 'changing colors', i.e. left/right swap.
      Due to the projection of the vector alongside the hip on the ZX plane. Should appears extremely rarely (if ever) with real data.

******************************************************************************/
void PlotStickman_p::createDisplayList()
{
   //printf("PlotStickman_p::createDisplayList\n");

   std::vector<float> tmp;
   std::vector<float> v,w,t;

   // Coordinate of the lower back
   base_coord={0,0,0};

   // Head coordinate
   v = {0,TORSO_HEIGHT,0};
   w = rot_axisangle(v,DEG2RAD(torsorot[0]),{torsorot[1],torsorot[2],torsorot[3]});
   head_coord = add(base_coord,w);

   // Left shoulder
   v = {+SHOULDER_WIDTH/2,0,0};
   w = rot_axisangle(v,torsorot[0]/180.0*PI,{torsorot[1],torsorot[2],torsorot[3]});
   lshoulder_coord = add(head_coord,w);

   // Right shoulder
   v = {-SHOULDER_WIDTH/2,0,0};
   w = rot_axisangle(v,torsorot[0]/180.0*PI,{torsorot[1],torsorot[2],torsorot[3]});
   rshoulder_coord = add(head_coord,w);

   // Left elbow coordinate at shoulder
   v = {0,-UARM_LENGTH,0};
   w = rot_axisangle(v,luarot[0]/180.0*PI,{luarot[1],luarot[2],luarot[3]});

   //printf("LUA rotation representation: %f; %f %f %f\n",luarot[0],luarot[1],luarot[2],luarot[3]);
   //printf("LUA %f,%f,%f -> %f %f %f\n",v[0],v[1],v[2],w[0],w[1],w[2]);


   lelbow_coord = add(lshoulder_coord,w);

   // Right elbow coordinate at shoulder
   v = {0,-UARM_LENGTH,0};
   w = rot_axisangle(v,ruarot[0]/180.0*PI,{ruarot[1],ruarot[2],ruarot[3]});
   relbow_coord = add(rshoulder_coord,w);

   lleg_coord = add(base_coord,{-HIP_WIDTH/2,0,0});
   rleg_coord = add(base_coord,{+HIP_WIDTH/2,0,0});


   //printf("Hip\n");
   // The hip stays parallel to the xz plane. The hip rotation is the projection of the torso rotation on that plane.
   // We find this projection by rotating a unit vector along the hip according to the torso rotation, taking the x,z projection, and normalizing
   v={1,0,0};                                                                              // Unit vector parallel to the hip, pointing towards the left hip
   tmp=rot_axisangle(v,DEG2RAD(torsorot[0]),{torsorot[1],torsorot[2],torsorot[3]});        // Torso rotation
   //printf("<1,0,0> rotated by torso rot: %f %f %f\n",tmp[0],tmp[1],tmp[2]);
   //tmp[1]=0;                                                                                // Projection of the rotated unit vector on the xz plane
   //tmp=normalize(tmp);                                                                      // Normalization
   float hipalpha = -angleof(tmp[0],tmp[2]);                                                // Rotation of the hip. Negate due to the way the angle is computed v.s. the way it is effected by a rotation around the axis

   //printf("Alpha of hip: %f\n",RAD2DEG(hipalpha));
   hiprot = {(float)RAD2DEG(hipalpha),0,1,0};                                              // Rotation representation of the hip: rotation of hipalpha around (0,1,0)
   w = rot_axisangle({1,0,0},hiprot[0]/180.0*PI,{hiprot[1],hiprot[2],hiprot[3]});
   //printf("Torso rot: %f %f %f %f\n",torsorot[0],torsorot[1],torsorot[2],torsorot[3]);
   //printf("Hip rot: %f %f %f %f\n",hiprot[0],hiprot[1],hiprot[2],hiprot[3]);
   rleg_coord = add(base_coord,mul(w,-HIP_WIDTH/2.0));
   lleg_coord = add(base_coord,mul(w,+HIP_WIDTH/2.0));


   /*    printf("Coordinates of:\n");
   printf("base: %f %f %f\n",base_coord[0],base_coord[1],base_coord[2]);
   printf("head: %f %f %f\n",head_coord[0],head_coord[1],head_coord[2]);
   printf("lshoulder: %f %f %f\n",lshoulder_coord[0],lshoulder_coord[1],lshoulder_coord[2]);
   printf("rshoulder: %f %f %f\n",rshoulder_coord[0],rshoulder_coord[1],rshoulder_coord[2]);
   printf("lelbow: %f %f %f\n",lelbow_coord[0],lelbow_coord[1],lelbow_coord[2]);
   printf("relbow: %f %f %f\n",relbow_coord[0],relbow_coord[1],relbow_coord[2]);
   printf("lleg: %f %f %f\n",lleg_coord[0],lleg_coord[1],lleg_coord[2]);
   printf("rleg: %f %f %f\n",rleg_coord[0],rleg_coord[1],rleg_coord[2]);
   */





   ///////////////////////////////////////////////////////////////////////////
   // Initialization
   ///////////////////////////////////////////////////////////////////////////
   // Test the list exists
   if(object==(GLuint)-1)
   {
      object = glGenLists( 1 );
      //printf("glGenLists: %d\n",object);
   }

   glNewList( object, GL_COMPILE );

   glEnable(GL_NORMALIZE);                   // For the normals: allows to pass non normalized normals
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glEnableClientState(GL_VERTEX_ARRAY);

   glLoadIdentity();

   // Clear the screen, z buffer, and stencil
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




   ///////////////////////////////////////////////////////////////////////////
   // Set lighting
   ///////////////////////////////////////////////////////////////////////////
   float lightAngle = DEG2RAD(120);
   float lightHeight = 3.0*LEG_LENGTH;
   float lightDistance = 2*LEG_LENGTH;
   GLfloat lightPosition[4];
   bool directionalLight=true;

   // Light position
   lightPosition[0] = lightDistance*cos(lightAngle);
   lightPosition[1] = lightHeight;
   lightPosition[2] = lightDistance*sin(lightAngle);

   if (directionalLight)
      lightPosition[3] = 0.0;
   else
      lightPosition[3] = 1.0;

   // Light color
   GLfloat lightColor[] = {1.0, 1.0, 1.0, 1.0};
   GLfloat lightAmbient[] = { .8f, .8f, .8f, 1 };

   // Light setup
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
   glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
   glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHTING);



//   glDisable(GL_CULL_FACE);



   // Starting point of the rendering
   glTranslatef(0.0f,0.0f,-10.0f);

   drawStickman();

   ///////////////////////////////////////////////////////////////////////////
   // Draw floor into stencil
   ///////////////////////////////////////////////////////////////////////////

   // Don't update color or depth
   //glDisable(GL_DEPTH_TEST);
   //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

   // Set 3 into the stencil
   glEnable(GL_STENCIL_TEST);
   glStencilFunc(GL_ALWAYS, 3, 0xffffffff);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

   // Draw floor into. Floor pixels just get their stencil set to 3.
   drawFloor();

   // Re-enable update of color and depth.
   //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   //glEnable(GL_DEPTH_TEST);


   ///////////////////////////////////////////////////////////////////////////
   // Reflection
   ///////////////////////////////////////////////////////////////////////////

   // Draw the reflected scene
   /*glPushMatrix();
      glTranslatef(0,-2*LEG_LENGTH,0);
      glScalef(1.0, -1.0, 1.0);
      drawStickman();
   glPopMatrix();*/

   ///////////////////////////////////////////////////////////////////////////
   // Draw shadow
   ///////////////////////////////////////////////////////////////////////////

   // Shadow projection matrix
   GLfloat floorShadow[4][4];
   // Plane defined by normal vector and length (x,y,z,w)
   GLfloat floorPlane[4];


   // Setup floor plane for projected shadow calculations.  p0,p1,p2: 3 points on the floor.
   // The plane must be specified by counterclockwise points...
   GLfloat p0[]={0,-LEG_LENGTH+0.01f,1};
   GLfloat p1[]={1,-LEG_LENGTH+0.01f,0};
   GLfloat p2[]={0,-LEG_LENGTH+0.01f,0};

   findPlane(floorPlane, p0,p1,p2);
   //printf("floorPlane: %f %f %f %f\n",floorPlane[0],floorPlane[1],floorPlane[2],floorPlane[3]);

   shadowMatrix(floorShadow, floorPlane, lightPosition);
   //for(unsigned i=0;i<4;i++)
     // printf("%f %f %f %f\n",floorShadow[i][0],floorShadow[i][1],floorShadow[i][2],floorShadow[i][3]);

   // Setup tp draw only on pixels with stencil=3 (i.e. on the floor, not outside of the floor.

   glStencilFunc(GL_LESS, 2, 0xffffffff);  // draw if ==3
   glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);


   glEnable(GL_POLYGON_OFFSET_FILL);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_LIGHTING);  // Force the 50% black.
   glColor4f(0.0, 0.0, 0.0, 0.5);
   glPushMatrix();
      glMultMatrixf((GLfloat *) floorShadow);
      drawStickman();
   glPopMatrix();
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glDisable(GL_STENCIL_TEST);


   // Draw the light position
   glPushMatrix();
   glDisable(GL_LIGHTING);
   glColor3f(1.0, 1.0, 0.0);
   if (directionalLight)
   {
      // Draw an arrowhead.
      glDisable(GL_CULL_FACE);
      glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
      glRotatef(lightAngle * -180.0 / M_PI, 0, 1, 0);
      glRotatef(atan(lightHeight/lightDistance) * 180.0 / M_PI, 0, 0, 1);
      glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0, 0, 0);
      glVertex3f(.2, .1, .1);
      glVertex3f(.2, -.1, .1);
      glVertex3f(.2, -.1, -.1);
      glVertex3f(.2, .1, -.1);
      glVertex3f(.2, .1, .1);
      glEnd();
      // Draw a white line from light direction.
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINES);
      glVertex3f(0, 0, 0);
      glVertex3f(.5, 0, 0);
      glEnd();
      glEnable(GL_CULL_FACE);
   }
   else
   {
      // Draw a yellow ball at the light source.
      glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
      drawlist(lightball_vertices,lightball_indices);
   }
   glEnable(GL_LIGHTING);
   glPopMatrix();



   glEndList();
}
/******************************************************************************
   PlotStickman_p::drawStickman
*******************************************************************************
   Draws the stick figure at the 'current' location, with the precomputed rotations and helper variables, and:

   - Eyes looking towards outside the screen - left body side is on the screen right side
   - w/o rotation: vertical torso, vertical upper/lower arm
   - (0,0,0) location corresponds to the bottom of the torso

   Known issues:
   -  A torso rotation of 90°<0,0,1> will lead to the leg 'changing colors', i.e. left/right swap.
      Due to the projection of the vector alongside the hip on the ZX plane. Should appears extremely rarely (if ever) with real data.

   Assumes a list is created, etc.

******************************************************************************/
void PlotStickman_p::drawStickman()
{
   // Reference cones. Puts a red cone on the right and towards screen. Puts a green cone on the left and inside screen.
   glPushMatrix();
      // Red cone
      setColor(1,0,0);
      glTranslatef(2,-LEG_LENGTH/2,+2);
      glPushMatrix();
          glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
          drawlist(cone_vertices,cone_indices);
      glPopMatrix();
      // Green
      setColor(0,1,0);
      glTranslatef(-4,0,-4);
      glPushMatrix();
          glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
          drawlist(cone_vertices,cone_indices);
      glPopMatrix();
   glPopMatrix();


   // Torso
   glPushMatrix();
      glRotatef(torsorot[0],torsorot[1],torsorot[2],torsorot[3]);
      glPushMatrix();
         glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
         glTranslatef(0.0f, 0.0f,-TORSO_HEIGHT/2.0f);
         setColor(1,1,1);
         drawlist(torso_vertices,torso_indices);
         setColor(1,0.647,0);
         drawlist(torso_vertices_front,torso_indices_front);
      glPopMatrix();
   glPopMatrix();

    // Shoulders
    setColor(.4,.4,1);
    glPushMatrix();
        glTranslatef(head_coord[0], head_coord[1], head_coord[2]);
        glRotatef(torsorot[0],torsorot[1],torsorot[2],torsorot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 0.0F, 1.0F, 0.0F );
            drawlist(shoulder_vertices,shoulder_indices);
        glPopMatrix();

        // Head
        glTranslatef(0, SKULL_RADIUS+SHOULDER_RADIUS, 0);
        //setColor(.6,1,.1);
        setColor(239.0/255.0,208.0/255.0,207.0/255.0);
        drawlist(skull_vertices,skull_indices);

        // Eyes
        glPushMatrix();
            glTranslatef(-0.1f, 0.075f, 0.19f);
            setColor(1.0f, 1.0f, 1.0f);
            drawlist(eye_vertices,eye_indices);

            glTranslatef(0.005f, 0.00f, 0.05f);
            setColor(0.0f, 0.0f, 0.0f);
            drawlist(pupil_vertices,pupil_indices);

            glTranslatef(0.190f, 0.00f, 0.00f);
            drawlist(pupil_vertices,pupil_indices);

            glTranslatef(0.005f, 0.0f, -0.05f);
            setColor(1.0f, 1.0f, 1.0f);
            drawlist(eye_vertices,eye_indices);

        glPopMatrix();


    glPopMatrix();


    // Hip - should only rotate along the y axis -> must find the projection of torsorot
    setColor(0,0,1);
    glPushMatrix();
        glRotatef(hiprot[0],hiprot[1],hiprot[2],hiprot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 0.0F, 1.0F, 0.0F );
            drawlist(hip_vertices,hip_indices);
        glPopMatrix();
    glPopMatrix();



    // Left leg (Red)
    setColor(1,0,0);
    glPushMatrix();
        glTranslatef(lleg_coord[0], lleg_coord[1], lleg_coord[2]);
        glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
        glTranslatef(0,0,+LEG_LENGTH/2);
        drawlist(leg_vertices,leg_indices);
    glPopMatrix();

    // Right leg (Green)
    setColor(0,1,0);
    glPushMatrix();
        glTranslatef(rleg_coord[0], rleg_coord[1], rleg_coord[2]);
        glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
        glTranslatef(0,0,+LEG_LENGTH/2);
        drawlist(leg_vertices,leg_indices);
    glPopMatrix();


    // Left upper arm
    glPushMatrix();
        glTranslatef(lshoulder_coord[0], lshoulder_coord[1], lshoulder_coord[2]);
        glRotatef(luarot[0],luarot[1],luarot[2],luarot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
            glTranslatef(0,0,+UARM_LENGTH/2);
            setColor(1,0,0);
            drawlist(uarm_vertices,uarm_indices);
            setColor(1,0.647,0);
            drawlist(uarm_vertices_front,uarm_indices_front);
        glPopMatrix();
    glPopMatrix();

    // Right upper arm
    glPushMatrix();
        glTranslatef(rshoulder_coord[0], rshoulder_coord[1], rshoulder_coord[2]);
        glRotatef(ruarot[0],ruarot[1],ruarot[2],ruarot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
            glTranslatef(0,0,+UARM_LENGTH/2);
            setColor(0,1,0);
            drawlist(uarm_vertices,uarm_indices);
            setColor(1,0.647,0);
            drawlist(uarm_vertices_front,uarm_indices_front);
        glPopMatrix();
    glPopMatrix();

    // Left lower arm
    glPushMatrix();
        glTranslatef(lelbow_coord[0], lelbow_coord[1], lelbow_coord[2]);
        glRotatef( llarot[0],llarot[1],llarot[2],llarot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
            glTranslatef(0,0,+LARM_LENGTH/2);
            setColor(1,.5,.5);
            drawlist(larm_vertices,larm_indices);
            setColor(1,0.647,0);
            drawlist(larm_vertices_front,larm_indices_front);
        glPopMatrix();
    glPopMatrix();

    // Right lower arm
    glPushMatrix();
        glTranslatef(relbow_coord[0], relbow_coord[1], relbow_coord[2]);
        glRotatef( rlarot[0],rlarot[1],rlarot[2],rlarot[3]);
        glPushMatrix();
            glRotatef( 90.0F, 1.0F, 0.0F, 0.0F );
            glTranslatef(0,0,+LARM_LENGTH/2);
            setColor(0.5,1,0.5);
            drawlist(larm_vertices,larm_indices);
            setColor(1,0.647,0);
            drawlist(larm_vertices_front,larm_indices_front);
        glPopMatrix();
    glPopMatrix();



}

void PlotStickman_p::drawFloor()
{
   // Floor
   setColor(1,1,1);
   glPushMatrix();
       glTranslatef(0,-LEG_LENGTH, 0);
       glRotatef( -90.0F, 1.0F, 0.0F, 0.0F );
       drawlist(floor_vertices,floor_indices);
   glPopMatrix();
}

const QImage &PlotStickman_p::GetImage()
{
    return pixmap;
    //pixmap = grabFrameBuffer();
}



void drawlist(std::vector<float> vertices,std::vector<short> indices)
{
   for(unsigned i=0;i<indices.size();i+=3)
   {
      glBegin(GL_TRIANGLES);
      // Here must compute the normal of the plane
      std::vector<float> v1={vertices[indices[i+1]*3+0],vertices[indices[i+1]*3+1],vertices[indices[i+1]*3+2]};
      std::vector<float> v2={vertices[indices[i+2]*3+0],vertices[indices[i+2]*3+1],vertices[indices[i+2]*3+2]};
      std::vector<float> vo={-vertices[indices[i+0]*3+0],-vertices[indices[i+0]*3+1],-vertices[indices[i+0]*3+2]};
      // v1 is the vector from the vertice 0->1. v2 is the vector from the vertice 0->2
      v1 = add(v1,vo);
      v2 = add(v2,vo);
      std::vector<float> nv = crossprod(v1,v2);
      glNormal3f(nv[0],nv[1],nv[2]);
      glVertex3f(vertices[indices[i+0]*3+0],vertices[indices[i+0]*3+1],vertices[indices[i+0]*3+2]);
      glVertex3f(vertices[indices[i+1]*3+0],vertices[indices[i+1]*3+1],vertices[indices[i+1]*3+2]);
      glVertex3f(vertices[indices[i+2]*3+0],vertices[indices[i+2]*3+1],vertices[indices[i+2]*3+2]);
      glEnd();
   }
}

