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

#ifndef __GLHELPER_H
#define __GLHELPER_H

#include <QGLWidget>
#include <vector>
#include <math.h>

#define PI (3.14159265358979323846)
#define RAD2DEG(x) (x*180.0/PI)
#define DEG2RAD(x) (x*PI/180.0)

float qa(std::vector<float> q);
std::vector<float> quat_mult(std::vector<float> q1,std::vector<float> q2);
std::vector<float> quat_pointmult(std::vector<float> q1,std::vector<float> q2);
std::vector<float> rot_quat(std::vector<float> v,std::vector<float> q);
std::vector<float> mul(std::vector<float>a,float m);
std::vector<float> add(std::vector<float>a,std::vector<float> b);
std::vector<float> normalize(std::vector<float> in);
void normalize(GLfloat *f,int n);
float dotprod(const std::vector<float> &b,const std::vector<float> &c);
std::vector<float> crossprod(const std::vector<float> &b,const std::vector<float> &c);
std::vector<float> rot_axisangle(std::vector<float> v,float angle,std::vector<float> n);
float angleof(float x,float y);
void rot(float x1,float x2,float x3,float a1,float a2, float a3, float&y1,float&y2,float&y3);
void Rectangle(float x1, float y1, float x2, float y2,std::vector<float> &vertices,std::vector<short> &indices);
void Rectangle2(float x1, float y1, float x2, float y2,int slices, std::vector<float> &vertices,std::vector<short> &indices);
void Cylinder(float r1, float r2, float length, int slices,std::vector<float> &vertices,std::vector<short> &indices,bool close=false);
void Cylinder2(float r1, float r2,float alpha1,float alpha2,float length, int slices,std::vector<float> &vertices,std::vector<short> &indices,bool close=false);
void Sphere(float radius, int slices, int stacks,std::vector<float> &vertices,std::vector<short> &indices);

void findPlane(GLfloat plane[4],GLfloat v0[3], GLfloat v1[3], GLfloat v2[3]);
void shadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4]);



#endif // __GLHELPER_H
