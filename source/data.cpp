/*
   SynScopeV

   Copyright (C) 2008,2009:
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

#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <math.h>

#include "data.h"
#include "parsetextmatrix.h"

SIGNALDATA::SIGNALDATA()
{
   centersample=0;
   filename="";
}
/**
  Returns a pointer to the channel. If the channel does not exist, return 0
**/
TRACERI SIGNALDATA::getRef(unsigned channel)
{
   if(channel<data.size())
      return &data[channel];
   return 0;
}

/**
  Returns a vector of pointer to the specified channels. If one channel does not exist, a null pointer is returned for that channel
**/
TRACESRI SIGNALDATA::getRef(const std::vector<unsigned> &channels)
{
   TRACESRI r;
   for(unsigned i=0;i<channels.size();i++)
   {
      r.push_back(getRef(channels[i]));
   }
   return r;
}
/**
  Returns a vector of pointer to the specified channels. If one channel does not exist, it is skipped from the return list.
  The function returns channelspruned, which is the channels effectively returned
**/
TRACESRI SIGNALDATA::getRef(const std::vector<unsigned> &channels,std::vector<unsigned> &channelspruned)
{
   TRACESRI r;
   channelspruned.clear();
   channelspruned.reserve(channels.size());
   for(unsigned i=0;i<channels.size();i++)
   {
      TRACERI tr = getRef(channels[i]);
      if(tr)
      {
         r.push_back(tr);
         channelspruned.push_back(channels[i]);
      }
   }
   return r;
}

/**
  \brief Finds a graph connected to edge x1 following Relations

  Returns a set of relations that, when followed in order, allow to traverse the graph.

   This functions allows to make sure that there are no conflicting sets of relations active.
   A conflicting relation is for instance adding a relation between s1-s2 when the current active relations include s0-s1, s1-s2.
   In other words, this function looks for circular relations within the set.

    This function works even if the existing set of relations is conflicting; circular relations are correctly handled. (how?)

    A recursive implementation is avoided.
**/
RELATIONS FindGraph(RELATIONS &Relations, int x1)
{
   //printf("MainWindow::FindGraph: test for %d\n",x1);


   // Start from the source edge
   // cx1 format: cx1[path][timestep]
   vector<vector<int> > cx1(1,vector<int>(1,x1));     // Kepps the current x1 (last element) and the past x1 (from from to last-1 element)

   RELATIONS RelationsFollowed;

   while(1)
   {
      /*for(int i=0;i<cx1.size();i++)
      {
         printf("cx1 path %d: ",i);
         for(int j=0;j<cx1[i].size();j++)
            printf("%d ",cx1[i][j]);
         printf("\n");
      }*/

      // Look in the relations if current x1 is used in the relation's x1. If not... no conflict. If yes, then follow relations
      vector<RELATION> r;
      r.clear();
      vector<int> ar;      // Alive relations
      ar.clear();

      // Iterate all current start points
      for(unsigned i=0;i<cx1.size();i++)
      {
         // Iterate all relations, to see if one leads further away
         for(RELATIONS::iterator it = Relations.begin();it!=Relations.end();it++)
         {
            if(cx1[i].back() == it->x1)
            {
               // One might lead further away... Check if it was already followed to avoid circular recursion.
               vector<int>::iterator fit;
               fit = std::find(cx1[i].begin(),cx1[i].end()-1,it->x2);
               if(fit == cx1[i].end()-1)
               {
                  // Was not previously followed, hence follow it by keeping the relations to follow.
                  r.push_back(*it);
                  ar.push_back(i);
               }
            }
         }
      }

      /*printf("found relations to follow# %d.\n",r.size());
      if(r.size())
      {
         for(int i=0;i<r.size();i++)
            printf("found %d, for path number %d x1,x2: %d-%d\n",i,ar[i],r[i].x1,r[i].x2);
      }*/

      // No link to cx1 means it isn't conflicting
      if(!r.size())
         break;

      for(unsigned i=0;i<r.size();i++)
         RelationsFollowed.push_back(r[i]);

      // Link may be indirectly conflicting -> follow link
      // Only the alive links are kept.
      // For those alive links, add the new end path
      vector<vector<int> > ncx1(ar.size());
      for(unsigned i=0;i<ar.size();i++)
      {
         ncx1[i] = cx1[ar[i]];
         ncx1[i].push_back(r[i].x2);
      }

      cx1 = ncx1;
   }
   return RelationsFollowed;
}

/**
  \brief Checks whether the relation x1-x2 is in conflict with the existing relations Relations.

**/
bool IsRelationConflicting(RELATIONS &Relations, int x1,int x2)
{
   //printf("MainWindow::IsRelationConflicting: test for %d-%d\n",x1,x2);

   RELATIONS RelationsFollowed = FindGraph(Relations,x1);

   for(unsigned i=0;i<RelationsFollowed.size();i++)
   {
      if(RelationsFollowed[i].x1 == x2 || RelationsFollowed[i].x2 == x2)
         return true;
   }
   return false;
}


// a1,a2: name of variable
QString RelationToText(RELATION &r)
{
   QString eq;
   eq.sprintf("%s%d = %s%d x %lf + %lf",r.s2?"S":"V",r.tx2,r.s1?"S":"V",r.tx1,r.b,r.a);
   return eq;
}


/**
   \brief Finds a set of disconnected graphs within Relations.

   Returns a vector<RELATIONS>......
   Each RELATIONS is a standalone graph.
**/
vector<RELATIONS> FindGraphs(RELATIONS Relations)
{
   /*printf("Relations:\n");
   for(int i=0;i<Relations.size();i++)
   {
      printf("%d\tx%d = x%d * %lf + %lf\n",i,Relations[i].x2,Relations[i].x1,Relations[i].b,Relations[i].a);
   }*/

   vector<RELATIONS> AllRelationsFollowed;
   RELATIONS RelationsFollowed;
   vector<int> edgeexplored;

   // Iterate through all the relation starting points.
   // For each, follow the relations until all edges are explored
   // Report and repeat
   for(unsigned tg = 0; tg<Relations.size();tg++)
   {
      //printf("Starting to follow tg %d, %d\n",tg,Relations[tg].x1);


      // Check if the new target graph is already belonging to a relation graph
      vector<int>::iterator fit;
      fit = std::find(edgeexplored.begin(),edgeexplored.end(),Relations[tg].x1);
      if(fit != edgeexplored.end())
      {
         //printf("Already followed %d\n",Relations[tg].x1);
         continue;
      }


      RelationsFollowed = FindGraph(Relations, Relations[tg].x1);
      AllRelationsFollowed.push_back(RelationsFollowed);

      // Store the edge explored.
      for(unsigned i=0;i<RelationsFollowed.size();i++)
      {
         edgeexplored.push_back(RelationsFollowed[i].x1);
         edgeexplored.push_back(RelationsFollowed[i].x2);
      }
      /*printf("For tg %d, edges explored: ",tg);
      for(int i=0;i<edgeexplored.size();i++)
         printf("%d ",edgeexplored[i]);
      printf("\n");*/


   }  // tg
   return AllRelationsFollowed;


}
/**
  \brief Converts double v to ratio of integers n/d, where d is at most maxd.
**/
void DoubleToRatio(double v,int &n,int &d,int maxd)
{
   n=1;
   d=1;
   while((v*d)-floor(v*d)!=0 && d<maxd)
      d*=10;
   n=floor(v*d);
}

/**
  \brief Converts vector<int> g to a string representation of the form "g[0]-g[1]-g[2]...-g[n]".
**/
QString GraphToString(vector<int> g)
{
   QString str;
   for(unsigned j=0;j<g.size();j++)
   {
      str+=QString("S%1").arg(g[j]);
      if(j!=g.size()-1)
         str+="-";
   }
   return str;
}

/**
   \brief Parses a null-terminated block of data for space/tab delimited values.

   Parses signed integers. Handles specially 'NaN': replace the value by zero.

   Return: vector[column][line]
**/
std::vector<std::vector<int> > parse(char *data,bool &ok)
{
    unsigned pok;
    bool *outnan;
    double *outdata;
    int sx,sy;

    std::vector<std::vector<int> > alldata;

    // Call the low-level fast parse
    pok = parsetextmatrix(data,&sx,&sy,&outdata,&outnan);

    // Return immediately in case of error
    if(pok!=0)
    {
        // Split the error code and the offset
        int erroffset = pok>>8;
        int errtype = pok&0xff;
        printf("Matrix load error: can't parse file. Error type %d at offset %d\n",errtype,erroffset);
        ok = false;
        return alldata;
    }
    printf("Matrix load ok: Size %d x %d\n",sx,sy);

    // Success: convert the plain matrix into std::vector and handle NaN
    alldata.resize(sx);
    for(unsigned x=0;x<sx;x++)
    {
        for(unsigned y=0;y<sy;y++)
        {
            if(outnan[y*sx+x])
                alldata[x].push_back(0);
            else
                alldata[x].push_back(outdata[y*sx+x]);
        }
    }
    ok = true;


    return alldata;
}


/**
   \brief Finds the offset of linked signal/video sources, from the the current position of the master source.

   RelationsActive: all the relations that are currently enabled
   sourceidx/value: offset of the source gainst which the others must be linked
   offsetout: pairs of <source,offset>

   The ID are absolute (no distinction between video or signal)

**/
void ComputeAllSourceOffsetFrom(RELATIONS &RelationsActive,int sourceidx,int value,std::map<int,int> &offsetout)
{
   assert(sourceidx!=-1);

   std::map<int,int>::iterator offiterator;

   //printf("sourceidx: %d\n",sourceidx);

   // Store the first offset in our output structure
   offsetout.insert(std::pair<int,int>(sourceidx,value));

   // Find the relations that must be followed from the start edge to cover all connected edges.
   RELATIONS RelationsFollowed = FindGraph(RelationsActive,sourceidx);

   /*printf("Dump relations followed\n");
   for(unsigned i=0;i<RelationsFollowed.size();i++)
   {
      printf("Relation %d\n",i);
      printf("\t%d(%d) %d(%d)\n",RelationsFollowed[i].x1,(int)RelationsFollowed[i].s1,RelationsFollowed[i].x2,(int)RelationsFollowed[i].s2);
      printf("\t%d %d --> %lf %lf\n",RelationsFollowed[i].tx1,RelationsFollowed[i].tx2,RelationsFollowed[i].a,RelationsFollowed[i].b);
   }*/
   for(unsigned i=0;i<RelationsFollowed.size();i++)
   {
      // We search the element in the map with key value RelationsFollowed.x1
      // We should always find this value, because FindGraph returns a graph that is constructible/traversable if interpreted sequentially.

      offiterator = offsetout.find(RelationsFollowed[i].x1);
      assert(offiterator!=offsetout.end());

      // Get the offset
      double o1 = offiterator->second;
      // Compute the offset of the second node
      double o2 = o1 * RelationsFollowed[i].b + RelationsFollowed[i].a;
      // Put that in the map
      offsetout.insert(std::pair<int,int>(RelationsFollowed[i].x2,(int)o2));
   }
}


