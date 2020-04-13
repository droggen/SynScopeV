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

#include "glhelper.h"



float qa(std::vector<float> q)
{
   return 2.0*acos(q[0]);
}

std::vector<float> quat_mult(std::vector<float> q1,std::vector<float> q2)
{
   std::vector<float> result(4,0);

   result[0] = (q1[0]*q2[0] -q1[1]*q2[1] -q1[2]*q2[2] -q1[3]*q2[3]);
   result[1] = (q1[0]*q2[1] +q1[1]*q2[0] +q1[2]*q2[3] -q1[3]*q2[2]);
   result[2] = (q1[0]*q2[2] -q1[1]*q2[3] +q1[2]*q2[0] +q1[3]*q2[1]);
   result[3] = (q1[0]*q2[3] +q1[1]*q2[2] -q1[2]*q2[1] +q1[3]*q2[0]);
   return result;
}
//Quaternion multiplication without the .a component. Returns a vector
std::vector<float> quat_pointmult(std::vector<float> q1,std::vector<float> q2)
{
   std::vector<float> result(3,0);

   result[0] = (q1[0]*q2[1] +q1[1]*q2[0] +q1[2]*q2[3] -q1[3]*q2[2]);
   result[1] = (q1[0]*q2[2] -q1[1]*q2[3] +q1[2]*q2[0] +q1[3]*q2[1]);
   result[2] = (q1[0]*q2[3] +q1[1]*q2[2] -q1[2]*q2[1] +q1[3]*q2[0]);

   return result;
}
// Rotates vector v by quaternion q=[q0,q1,q2,q3]=[cos(a/2);sin(a/2)x,sin(a/2)y,sin(a/2)z]
std::vector<float> rot_quat(std::vector<float> v,std::vector<float> q)
{
   std::vector<float> qconj(4);     // conjugate of the rotation quaternion
   std::vector<float> qv(4);        // quaternion representation of the vector to rotate


   qv[0] = 0;
   qv[1] = v[0];
   qv[2] = v[1];
   qv[3] = v[2];
   qconj[0] = q[0];
   qconj[1] = -q[1];
   qconj[2] = -q[2];
   qconj[3] = -q[3];
   return quat_pointmult(quat_mult(q,qv),qconj);

}

// Multiplies a vector by a constant
std::vector<float> mul(std::vector<float>a,float m)
{
    std::vector<float> c(3,0);
    for(unsigned i=0;i<3;i++)
        c[i] = a[i]*m;
    return c;
}
// Adds two vectors
std::vector<float> add(std::vector<float>a,std::vector<float> b)
{
    std::vector<float> c(3,0);
    for(unsigned i=0;i<3;i++)
        c[i] = a[i]+b[i];
    return c;
}

/*
  Normalizes the input vector.
*/
std::vector<float> normalize(std::vector<float> in)
{
    //cassert(in.size()==3);
    float norm = 0;
    for(unsigned i=0;i<in.size();i++)
       norm += in[i]*in[i];
    norm = sqrt(norm);
    std::vector<float> out = in;
    for(unsigned i=0;i<in.size();i++)
        out[i]=out[i]/norm;
    return out;
}
void normalize(GLfloat *f,int n)
{
   float norm = 0;
   for(int i=0;i<n;i++)
      norm += f[i]*f[i];
   norm = sqrt(norm);
   for(int i=0;i<n;i++)
      f[i] = f[i]/norm;
}
// Dot product
float dotprod(const std::vector<float> &b,const std::vector<float> &c)
{
    float a=0;
    for(unsigned i=0;i<3;i++)
        a += b[i]*c[i];
    return a;
}
// Cross product
std::vector<float> crossprod(const std::vector<float> &b,const std::vector<float> &c)
{
    std::vector<float> a(3,0);
    a[0] = b[1]*c[2]-b[2]*c[1];
    a[1] = b[2]*c[0]-b[0]*c[2];
    a[2] = b[0]*c[1]-b[1]*c[0];
    return a;
}

/*
  Performs a rotation of the vector v using the 'axis-angle' or 'exponential coordinates' representation of a rotation.
    v:      vector to rotate
    angle:  rotation angle in radians
    n:      vector around which the rotation is done.

    The rotation is computed using Rodrigues rotation formula.
*/
std::vector<float> rot_axisangle(std::vector<float> v,float angle,std::vector<float> n)
{
    // Normalizes n
    n = normalize(n);

    std::vector<float> t(3,0);

    t = mul(v,cos(angle));

    t = add(t,mul(crossprod(n,v),sin(angle)));

    t = add(t,mul(n,dotprod(n,v)*(1-cos(angle))));

    return t;


}

// Returns the angle in radians of the vector (0,0)-(x,y). [Could also use atan2(y,x)
float angleof(float x,float y)
{
   float h = sqrt(x*x+y*y);
   if(y>0)
   {
      return acos(x/h);
   }
   return -acos(x/h);
}

// Rotates vector
void rot(float x1,float x2,float x3,float a1,float a2, float a3, float&y1,float&y2,float&y3)
{

   y1 = (cos(a2)*cos(a3))*x1 + (-cos(a1)*sin(a3)+sin(a1)*sin(a2)*cos(a3))*x2 + (sin(a1)*sin(a3)+cos(a1)*sin(a2)*cos(a3))*x3;
   y2 = (cos(a2)*sin(a3))*x1 + (cos(a1)*cos(a3)+sin(a1)*sin(a2)*sin(a3))*x2 + (-sin(a1)*cos(a3)+cos(a1)*sin(a2)*sin(a3))*x3;
   y3 = -sin(a2)*x1 + (sin(a1)*cos(a2))*x2 + (cos(a1)*cos(a2))*x3;
}



// Create a rectancle on the plane Z
void Rectangle(float x1, float y1, float x2, float y2,std::vector<float> &vertices,std::vector<short> &indices)
{
    vertices = std::vector<float>(12,0);
    indices = std::vector<short>(6,0);

    // Vertices: bottom left, bottom right, top left, top right
    vertices[0] = x1;
    vertices[1] = y1;
    vertices[2] = 0;
    vertices[3] = x2;
    vertices[4] = y1;
    vertices[5] = 0;
    vertices[6] = x1;
    vertices[7] = y2;
    vertices[8] = 0;
    vertices[9] = x2;
    vertices[10] = y2;
    vertices[11] = 0;

    indices={0,1,3,0,3,2};

}
// Create a rectancle on the plane Z
void Rectangle2(float x1, float y1, float x2, float y2,int slices, std::vector<float> &vertices,std::vector<short> &indices)
{
    vertices = std::vector<float>((slices+1)*(slices+1)*3,0);
    indices = std::vector<short>(slices*slices*2*3,0);


    for(int y=0;y<slices+1;y++)
    {
       for(int x=0;x<slices+1;x++)
       {
          vertices[(y*(slices+1)+x)*3+0] = (x2-x1)/(float)(slices)*x + x1;
          vertices[(y*(slices+1)+x)*3+1] = (y2-y1)/(float)(slices)*y + y1;
          vertices[(y*(slices+1)+x)*3+2] = 0;
       }
    }
    for(int y=0;y<slices;y++)
    {
       for(int x=0;x<slices;x++)
       {
         /*indices[(y*slices+x)*6+0] = y*(slices+1)+x;
         indices[(y*slices+x)*6+1] = y*(slices+1)+x+slices+1;
         indices[(y*slices+x)*6+2] = y*(slices+1)+x+1;*/
         indices[(y*slices+x)*6+0] = y*(slices+1)+x;
         indices[(y*slices+x)*6+1] = y*(slices+1)+x+1;
         indices[(y*slices+x)*6+2] = y*(slices+1)+x+slices+1;

         /*indices[(y*slices+x)*6+0] = 0;
         indices[(y*slices+x)*6+1] = 0+slices+1;
         indices[(y*slices+x)*6+2] = 0+1;*/

         /*indices[(y*slices+x)*6+3] = slices+1;
         indices[(y*slices+x)*6+4] = slices+1+1;
         indices[(y*slices+x)*6+5] = 1;*/

         /*indices[(y*slices+x)*6+3] = y*(slices+1)+x+slices+1;
         indices[(y*slices+x)*6+4] = y*(slices+1)+x+slices+1+1;
         indices[(y*slices+x)*6+5] = y*(slices+1)+x+1;*/
         indices[(y*slices+x)*6+3] = y*(slices+1)+x+slices+1+1;
         indices[(y*slices+x)*6+4] = y*(slices+1)+x+slices+1;
         indices[(y*slices+x)*6+5] = y*(slices+1)+x+1;
       }
    }


    /*printf("Vertices:\n");
    for(unsigned i=0;i<vertices.size();i+=3)
       printf("%02d:  %05f %05f %05f\n",i/3,vertices[i+0],vertices[i+1],vertices[i+2]);
    printf("Triangles:\n");
    for(unsigned i=0;i<indices.size();i+=3)
       printf("  %d:  %02d %02d %02d\n",i,indices[i+0],indices[i+1],indices[i+2]);*/



}

/**
    * Draws a cylinder along the z-axis centered around the origin
    * @param r1 Radius at the negative z-axis
    * @param r2 Radius at the positive z-axis
    * @param length Length of the cylinder along the z-axis
    * @param slices Number of slices generated around the z-axis
    */
void Cylinder(float r1, float r2, float length, int slices,std::vector<float> &vertices,std::vector<short> &indices,bool close)
{
   vertices = std::vector<float>(2*3*slices+6,0);                       // 2 tri per slice, 2 tri for the center of the circles
   indices = std::vector<short>(2*3*slices + close*(2*3*slices),0);     // Side:[2 tri per slice, 3 vertices per tri]. Extremity:[2 tri per slice (top/bot), 3 vertices per tri]

   length /= 2.0f;


   // Coordinates of the vertices.
   for(int i=0; i<slices; i++ )
   {
      // Bottom coordinate
      vertices[i*3  ] = (float) (r1 * cos(2.0*PI/slices*i));
      vertices[i*3+1] = (float) (r1 * sin(2.0*PI/slices*i));
      vertices[i*3+2] = -length;

      // Top coordinate
      vertices[(slices+i)*3  ] = (float) (r2 * cos(2.0*PI/slices*i));
      vertices[(slices+i)*3+1] = (r2 * sin(2.0*PI/slices*i));
      vertices[(slices+i)*3+2] = length;
   }
   // Coordinate of the origin
   vertices[slices*2*3+0] = 0;
   vertices[slices*2*3+1] = 0;
   vertices[slices*2*3+2] = -length;
   vertices[slices*2*3+3] = 0;
   vertices[slices*2*3+4] = 0;
   vertices[slices*2*3+5] = +length;

   // Create the shape as triangles. Each square is rendered as two triangles
   // Indices contains the indices to link together.
   // With 'slices' there are 2*slices triangles
   for(short i=0; i<slices;i++)
   {
      indices[6*i  ] = i;
      indices[6*i+1] = ((i+1)%slices);
      indices[6*i+2] = (i+slices);

      indices[6*i+3] = i+slices;
      indices[6*i+4] = (i+1)%slices;
      indices[6*i+5] = (i+1)%slices+slices;
   }

   // Close the extremities of the cylinders by creating a triangle between [origin, slice, slice+1]
   if(close)
   {
      for(int i=0;i<slices;i++)
      {
         // Bottom
         indices[2*3*slices+i*6+0] = slices*2;      // Origin
         indices[2*3*slices+i*6+1] = (i+1)%slices;
         indices[2*3*slices+i*6+2] = i;
         // Top
         indices[2*3*slices+i*6+3] = slices*2+1;      // Origin
         indices[2*3*slices+i*6+4] = i+slices;
         indices[2*3*slices+i*6+5] = (i+1)%slices+slices;
      }
   }

   /*printf("Vertices:\n");
   for(unsigned i=0;i<vertices.size();i+=3)
      printf("%02d:  %05f %05f %05f\n",i/3,vertices[i+0],vertices[i+1],vertices[i+2]);
   printf("Triangles:\n");
   for(unsigned i=0;i<indices.size();i+=3)
      printf("  %d:  %02d %02d %02d\n",i,indices[i+0],indices[i+1],indices[i+2]);*/
}
/**
    * Draws a cylinder along the z-axis centered around the origin
    * @param r1 Radius at the negative z-axis
    * @param r2 Radius at the positive z-axis
    * @param length Length of the cylinder along the z-axis
    * @param slices Number of segments generated around the z-axis
    * @param alpha1 Starting angle of the cylinder. Angle is measured from x to y axis.
    * @param alpha2
    */
void Cylinder2(float r1, float r2,float alpha1,float alpha2,float length, int slices,std::vector<float> &vertices,std::vector<short> &indices,bool close)
{
   float dalpha = (alpha2-alpha1)/slices;                            // Angle aperture of a slice
   int nvertices = slices+1;                                         // Number of vertices on the circle

   close=true;

   vertices = std::vector<float>(2*3*nvertices+6,0);                 // Two extremities with slices+1 3D coordinates (2*3*(slices+1)) and the last 6 coordinates for the center of each circle

   indices = std::vector<short>(2*3*slices + close*(2*3*slices),0);  // Side:[2 tri per slice, 3 vertices per tri]. Extremity:[2 tri per slice (top/bot), 3 vertices per tri]

   length /= 2.0f;


   // Coordinates of the vertices. There are slices+1 vertices.
   for(int i=0; i<nvertices; i++ )
   {
      // Bottom coordinate
      vertices[i*3  ] = (float) (r1 * cos(alpha1+dalpha*i));
      vertices[i*3+1] = (float) (r1 * sin(alpha1+dalpha*i));
      vertices[i*3+2] = -length;

      // Top coordinate
      vertices[(nvertices+i)*3  ] = (float) (r2 * cos(alpha1+dalpha*i));
      vertices[(nvertices+i)*3+1] = (r2 * sin(alpha1+dalpha*i));
      vertices[(nvertices+i)*3+2] = length;
   }
   // Coordinate of the origin
   vertices[nvertices*2*3+0] = 0;
   vertices[nvertices*2*3+1] = 0;
   vertices[nvertices*2*3+2] = -length;
   vertices[nvertices*2*3+3] = 0;
   vertices[nvertices*2*3+4] = 0;
   vertices[nvertices*2*3+5] = +length;

   // Create the shape as triangles. Each square is rendered as two triangles
   // Indices contains the indices to link together.
   // With 'slices' there are 2*slices triangles
   for(short i=0; i<slices;i++)
   {
      indices[6*i+5] = (i+1)%slices+slices;
      indices[6*i  ] = i;
      indices[6*i+1] = i+1;
      indices[6*i+2] = i+nvertices;

      indices[6*i+3] = i+nvertices;
      indices[6*i+4] = i+1;
      indices[6*i+5] = i+nvertices+1;
   }

   // Close the extremities of the cylinders by creating a triangle between [origin, slice, slice+1]
   if(close)
   {
      for(int i=0;i<slices;i++)
      {
         // Bottom
         indices[2*3*slices+i*6+0] = nvertices*2;      // Origin
         indices[2*3*slices+i*6+1] = i+1;
         indices[2*3*slices+i*6+2] = i;
         // Top
         indices[2*3*slices+i*6+3] = nvertices*2+1;      // Origin
         indices[2*3*slices+i*6+4] = nvertices+i;
         indices[2*3*slices+i*6+5] = nvertices+i+1;
      }
   }

   /*printf("Vertices:\n");
   for(unsigned i=0;i<vertices.size();i+=3)
      printf("%02d:  %05f %05f %05f\n",i/3,vertices[i+0],vertices[i+1],vertices[i+2]);
   printf("Triangles:\n");
   for(unsigned i=0;i<indices.size();i+=3)
      printf("  %d:  %02d %02d %02d\n",i,indices[i+0],indices[i+1],indices[i+2]);*/

}


void Sphere(float radius, int slices, int stacks,std::vector<float> &vertices,std::vector<short> &indices)
{

   vertices = std::vector<float>(3*(slices*stacks+2));

   for(int i=0; i<stacks; i++ )
   {
      float stackoff = -radius + (i+1)*2*radius/(stacks+1);
      float r = (float) sqrt(radius*radius - stackoff*stackoff);
      for(int j=0; j<slices; j++)
      {
         vertices[(i*slices + j)*3  ] = (r * cos(2.0*PI/slices*j));
         vertices[(i*slices + j)*3+1] = (r * sin(2.0*PI/slices*j));
         vertices[(i*slices + j)*3+2] = stackoff;
      }
   }

   // top and bottom
   vertices[3*slices*stacks  ] = 0;
   vertices[3*slices*stacks+1] = 0;
   vertices[3*slices*stacks+2] = -radius;

   vertices[3*slices*stacks+3] = 0;
   vertices[3*slices*stacks+4] = 0;
   vertices[3*slices*stacks+5] = radius;

   // create top and bottom caps
   indices = std::vector<short>(3*(2*slices + 2*stacks*slices));
   for(short i=0; i<slices;i++)
   {
      indices[6*i  ] = slices*stacks;
      indices[6*i+1] = (i+1)%slices;
      indices[6*i+2] = i;

      indices[6*i+3 ] = slices*stacks+1;
      indices[6*i+4]  = slices*(stacks-1) + i;
      indices[6*i+5]  = slices*(stacks-1) + (i+1)%slices;
   }
   int offset = 6*slices;

   // sides
   for (int i=0; i<stacks-1; i++)
   {
      for(int j=0; j<slices; j++)
      {
         indices[offset + 6*(i*slices+j)  ] = i*slices + j;
         indices[offset + 6*(i*slices+j)+1] = i*slices + (j+1)%slices;
         indices[offset + 6*(i*slices+j)+2] = (i+1)*slices + j;

         indices[offset + 6*(i*slices+j)+3] = (i+1)*slices + j;
         indices[offset + 6*(i*slices+j)+4] = i*slices + (j+1)%slices;
         indices[offset + 6*(i*slices+j)+5] = (i+1)*slices + (j+1)%slices;
      }
   }
}




/* Find the plane equation given 3 points. */
void findPlane(GLfloat plane[4],GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
  GLfloat vec0[3], vec1[3];

  /* Need 2 vectors to find cross product. */
  vec0[0] = v1[0] - v0[0];
  vec0[1] = v1[1] - v0[1];
  vec0[2] = v1[2] - v0[2];

  vec1[0] = v2[0] - v0[0];
  vec1[1] = v2[1] - v0[1];
  vec1[2] = v2[2] - v0[2];

  /* find cross product to get A, B, and C of plane equation */
  plane[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
  plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
  plane[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];

  plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

/*
 groundplane[X,Y,Z,W]
 lightpos[X,Y,Z,W]
*/
void shadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])
{
  GLfloat dot;

  /* Find dot product between light position vector and ground plane normal. */
  dot = groundplane[0] * lightpos[0] +
    groundplane[1] * lightpos[1] +
    groundplane[2] * lightpos[2] +
    groundplane[3] * lightpos[3];

  shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
  shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
  shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
  shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];

  shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
  shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
  shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
  shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];

  shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
  shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
  shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
  shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];

  shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
  shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
  shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
  shadowMat[3][3] = dot - lightpos[3] * groundplane[3];

}
