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

#include "parse.h"

/******************************************************************************
   Preparse
*******************************************************************************
   Identifies the blocks <...> in the view configuration string and return those as a string list.

   Returns false if the syntax is invalid.
******************************************************************************/
bool Preparse(string str,QStringList &captures)
{
   captures.clear();

   //QString rxexpr("\\s*<[\\f\\n\\r\\t\\s]*([^<^>]*)[\\f\\n\\r\\t\\s]*>\\s*");
   QString rxexpr("\\s*<([^<^>]*)>\\s*");
   QString rxvalid(("(")+rxexpr+QString(")*"));

   QRegExp e(rxexpr);
   QRegExp validation(rxvalid);

   if(!validation.exactMatch(QString(str.c_str())))
      return false;

   int pos = 0;
   while((pos = e.indexIn(QString(str.c_str()), pos)) != -1)
   {
      if(e.numCaptures()!=1)
         return false;

      captures.append(e.cap(1).trimmed());

      pos += e.matchedLength();
   }
   return true;
}

/******************************************************************************
   ParseScope
*******************************************************************************
   Parse the definition of a scope.
   str must not include the <> delimiters.
   str must include one scope definition only.

   The format of the scope definition is:
   scope ; sourcenum ; title ; trace1 trace2 ... [;xscale = xs] [;yscale=<auto|ymin ymax>][;color=rrggbb rrggbb ...]

   Returns:
      true if the definition is of type scope, false if it is something else
      ok: true if the scope definition was successfully parsed, false otherwise

   Note: use the return value to identify if the string is of type scope (although parsing may fail later). Use ok to identify if the parsing is successful

******************************************************************************/

bool ParseScope(QString str,bool &ok, SCOPEVIEWDATA &vd)
{
   //printf("ParseScope '%s'\n",str.toStdString().c_str());

   // Test and capture the scale
   QRegExp rxxscale("xscale\\s*=\\s*(-?\\d+)",Qt::CaseInsensitive);                                // One capture: xscale
   QRegExp rxyscale("yscale\\s*=\\s*(auto|(-?\\d+)\\s+(-?\\d+))",Qt::CaseInsensitive);             // Three captures: auto, ymin, ymax
   QRegExp rxcolor("color\\s*=\\s*(([abcdef\\d]{6,6}\\s*)*)\\s*",Qt::CaseInsensitive);                  // Two captures: all the space separated hex colors, the last hex color

   QString pre("Scope definition parser: ");
   vector<int> d;
   vector<unsigned> du;
   ok = false;



   QStringList elem = str.split(";");

   //for(unsigned i=0;i<elem.size();i++)
   //   printf("Element %d: '%s'\n",i,elem[i].toStdString().c_str());

   if(elem.size() && elem[0].trimmed().compare("scope",Qt::CaseInsensitive))
      return false;

   // It is a scope definition

   if(elem.size()<4 || elem.size()>7)
   {
      qDebug() << pre+"Incorrect number of parameters";
      return true;
   }


   QString source = elem[1].trimmed();
   vd.config.title =  elem[2].trimmed().toStdString();
   QString traces = elem[3].trimmed();
   vd.config.xscale=1;                                          // Default value for optional parameter
   vd.config.yauto = true;                                      // Default value for optional parameter
   vd.config.yscalemin= 0;                                      // Default value for optional parameter
   vd.config.yscalemax= 0;                                      // Default value for optional parameter
   vd.config.colors.clear();                                    // Default value for optional parameter

   // Get the source
   if(!SplitInVector(source.toStdString(),string(" "),du))
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   if(du.size()!=1)
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   vd.source=du[0];

   if(!SplitInVector(traces,QString(" "),vd.traces))
   {
      qDebug() << pre+"Cannot parse traces";
      return true;
   }


   // Iterate the optional parameters
   for(unsigned i=4;i<elem.size();i++)
   {
      //printf("Processing optional element %d: '%s'\n",i,elem[i].toStdString().c_str());

      // xscale= ?
      if(rxxscale.exactMatch(elem[i].trimmed()))
      {
         vd.config.xscale = rxxscale.cap(1).toInt();
      }
      // yscale= ?
      else if(rxyscale.exactMatch(elem[i].trimmed()))
      {
         if(rxyscale.cap(1).compare("auto",Qt::CaseInsensitive) == 0)
         {
            vd.config.yauto=true;
         }
         else
         {
            // Numbers are in cap(2) and cap(3)
            vd.config.yauto=false;
            vd.config.yscalemin = rxyscale.cap(2).toInt();
            vd.config.yscalemax = rxyscale.cap(3).toInt();
            if(vd.config.yscalemin>vd.config.yscalemax)
            {
               int t=vd.config.yscalemin;
               vd.config.yscalemin=vd.config.yscalemax;
               vd.config.yscalemax=t;
            }
            if(vd.config.yscalemin==vd.config.yscalemax)
            {
               vd.config.yscalemin--;
               vd.config.yscalemax++;
            }
         }
      }
      // color= ?
      else if(rxcolor.exactMatch(elem[i].trimmed()))
      {
         // Split cap(1)
         if(!SplitInVector(rxcolor.cap(1)," ",vd.config.colors,hex))
         {
            qDebug() << pre+"Color parse error";
            return true;
         }
         if(vd.config.colors.size()!=0 && (vd.config.colors.size() != vd.traces.size()))
         {
            qDebug() << pre+"Number of specified colors do not match the number of traces";
            return true;
         }
      }
      // Syntax error
      else
      {
         qDebug() << pre+"Invalid optional parameter";
         return true;
      }
   }

   /*
   cout << "Source: " << vd.source << endl;
   cout << "Title: " << vd.title << endl;
   cout << "Traces: " << endl;
   for(unsigned j=0;j<vd.traces.size();j++)
      cout << vd.traces[j] << "; ";
   cout << "\n";
   cout << "xscale: " << vd.xscale << endl;
   if(vd.yauto)
      cout << "yscale: auto" << endl;
   else
      cout << "yscale: " << vd.yscale[0] << " " << vd.yscale[1] << endl;
   cout << "Colors: " << endl;
   for(unsigned j=0;j<vd.colors.size();j++)
      cout << vd.colors[j] << "; ";
   cout << endl << endl;
   */

   ok=true;
   return true;

}

/******************************************************************************
   ParseStickman
*******************************************************************************
   Parse the definition of a stickman.
   str must not include the <> delimiters.
   str must include one stickman definition only.

   The format of the stickman  definition is:
   stickman ; sourcenum ; title ; torso_q0 torso_q1 torso_q2 torso_q3 rua_q0 rua_q1 rua_q2 rua_q3 rla_q0 rla_q1 rla_q2 rla_q3 lua_q0 lua_q1 lua_q2 lua_q3 lla_q0 lla_q1 lla_q2 lla_q3

   Returns:
      true if the definition is of type stickman, false if it is something else
      ok: true if the stickman definition was successfully parsed, false otherwise
******************************************************************************/
bool ParseStickman(QString str,bool &ok, STICKMANVIEWDATA &vd)
{
   //printf("ParseStickman '%s'\n",str.toStdString().c_str());

   QString pre("Stickman definition parser: ");
   vector<int> d;
   vector<unsigned> du;

   ok = false;

   QStringList elem = str.split(";");

   //for(unsigned i=0;i<elem.size();i++)
      //printf("Element %d: '%s'\n",i,elem[i].toStdString().c_str());

   if(elem.size() && elem[0].trimmed().compare("stickman",Qt::CaseInsensitive))
      return false;

   // It is a stickman definition

   if(elem.size()!=4)
   {
      qDebug() << pre+"Incorrect number of parameters";
      return true;
   }


   QString source = elem[1].trimmed();
   vd.config.title =  elem[2].trimmed().toStdString();
   QString traces = elem[3].trimmed();


   // Get the source
   if(!SplitInVector(source.toStdString()," ",du))
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   if(du.size()!=1)
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   vd.source=du[0];

   if(!SplitInVector(traces," ",d))
   {
      qDebug() << pre+"Cannot parse traces";
      return true;
   }
   if(d.size()!=20)
   {
      qDebug() << pre+"Invalid trace number";
      return true;
   }

   // Map the traces to the quaternions
   vd.trace_torso={d[0],d[1],d[2],d[3]};
   vd.trace_rua={d[4],d[5],d[6],d[7]};
   vd.trace_rla={d[8],d[9],d[10],d[11]};
   vd.trace_lua={d[12],d[13],d[14],d[15]};
   vd.trace_lla={d[16],d[17],d[18],d[19]};



   /*
   cout << "Source: " << vd.source << endl;
   cout << "Title: " << vd.title << endl;
   cout << "Torso: " ;
   for(unsigned j=0;j<vd.trace_torso.size();j++)
      cout << vd.trace_torso[j] << "; ";
   cout << endl;
   cout << "RUA: " ;
   for(unsigned j=0;j<vd.trace_rua.size();j++)
      cout << vd.trace_rua[j] << "; ";
   cout << endl;
   cout << "RLA: " ;
   for(unsigned j=0;j<vd.trace_rla.size();j++)
      cout << vd.trace_rla[j] << "; ";
   cout << endl;
   cout << "LUA: " ;
   for(unsigned j=0;j<vd.trace_lua.size();j++)
      cout << vd.trace_lua[j] << "; ";
   cout << endl;
   cout << "LLA: " ;
   for(unsigned j=0;j<vd.trace_lla.size();j++)
      cout << vd.trace_lla[j] << "; ";
   cout << endl;
   cout << endl << endl;
   */

   ok=true;
   return true;

}


/******************************************************************************
   ParseScatter
*******************************************************************************
   Parse the definition of a scatter.
   str must not include the <> delimiters.
   str must include one stickman definition only.

   The format of the scatter  definition is:
   scatter ; sourcenum ; title ; tracex1 tracey1 tracex2 tracey2 ... [; scale=xmin ymin xmax ymax] [;color=rrggbb rrggbb ...] [;points=numpoints]

   Returns:
      true if the definition is of type scatter, false if it is something else
      ok: true if the scatter definition was successfully parsed, false otherwise
******************************************************************************/
bool ParseScatter(QString str,bool &ok, SCATTERVIEWDATA &vd)
{
   //printf("ParseScatter '%s'\n",str.toStdString().c_str());

   // Test and capture the scale
   QRegExp rxscale("scale\\s*=\\s*(-?\\d+)\\s+(-?\\d+)\\s+(-?\\d+)\\s+(-?\\d+)",Qt::CaseInsensitive);          // Four capture: xmin, ymin, xmax, ymax
   QRegExp rxcolor("color\\s*=\\s*(([abcdef\\d]{6,6}\\s*)*)\\s*",Qt::CaseInsensitive);                         // Two captures: all the space separated hex colors, the last hex color
   QRegExp rxpoints("points\\s*=\\s*(\\d+)",Qt::CaseInsensitive);                                              // One capture: numpoints

   QString pre("Scatter definition parser: ");
   vector<int> d;
   vector<unsigned> du;

   ok = false;

   QStringList elem = str.split(";");

   //for(unsigned i=0;i<elem.size();i++)
      //printf("Element %d: '%s'\n",i,elem[i].toStdString().c_str());

   if(elem.size() && elem[0].trimmed().compare("scatter",Qt::CaseInsensitive))
      return false;

   // It is a scatter definition

   if(elem.size()<4 || elem.size()>7)
   {
      qDebug() << pre+"Incorrect number of parameters";
      return true;
   }

   QString source = elem[1].trimmed();
   vd.config.title =  elem[2].trimmed().toStdString();
   QString traces = elem[3].trimmed();
   vd.config.autoscale=true;
   vd.config.rangepoint=0;



   // Get the source
   if(!SplitInVector(source.toStdString()," ",du))
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   if(du.size()!=1)
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   vd.source=du[0];

   if(!SplitInVector(traces," ",du))
   {
      qDebug() << pre+"Cannot parse traces";
      return true;
   }
   if(du.size()%2)
   {
      qDebug() << pre+"Invalid trace number";
      return true;
   }

   vd.x.clear();
   vd.y.clear();
   for(unsigned i=0;i<du.size();i+=2)
   {
      vd.x.push_back(du[i]);
      vd.y.push_back(du[i+1]);
   }

   // Iterate the optional parameters
   for(unsigned i=4;i<elem.size();i++)
   {
      //printf("Processing optional element %d: '%s'\n",i,elem[i].toStdString().c_str());

      // scale= ?
      if(rxscale.exactMatch(elem[i].trimmed()))
      {
         vd.config.autoscale = false;
         vd.config.xmin = rxscale.cap(1).toInt();
         vd.config.ymin = rxscale.cap(2).toInt();
         vd.config.xmax = rxscale.cap(3).toInt();
         vd.config.ymax = rxscale.cap(4).toInt();
         // Check the ordering
         if(vd.config.xmax<vd.config.xmin)
             swap(vd.config.xmin,vd.config.xmax);
         if(vd.config.ymax<vd.config.ymin)
             swap(vd.config.ymin,vd.config.ymax);
         // Check if the scale range is zero
         if(vd.config.xmin==vd.config.xmax)
         {
            vd.config.xmin--;
            vd.config.xmax++;
         }
         if(vd.config.ymin==vd.config.ymax)
         {
            vd.config.ymin--;
            vd.config.ymax++;
         }
      }
      // color= ?
      else if(rxcolor.exactMatch(elem[i].trimmed()))
      {
         // Split cap(1)
         if(!SplitInVector(rxcolor.cap(1)," ",vd.config.colors,hex))
         {
            qDebug() << pre+"Color parse error";
            return true;
         }
         if(vd.config.colors.size()!=0 && (vd.config.colors.size() != vd.x.size()))
         {
            qDebug() << pre+"Number of specified colors do not match the number of traces";
            return true;
         }
      }
      // points= ?
      else if(rxpoints.exactMatch(elem[i].trimmed()))
      {
         vd.config.rangepoint = rxpoints.cap(1).toUInt();
      }
      // Syntax error
      else
      {
         qDebug() << pre+"Invalid optional parameter";
         return true;
      }
   }


/*
   cout << "Source: " << vd.source << endl;
   cout << "Title: " << vd.title << endl;
   cout << "Traces: " ;
   for(unsigned j=0;j<vd.x.size();j++)
      cout << "(" << vd.x[j] << " " << vd.y[j] << "); ";
   cout << endl;
   if(vd.autoscale)
      cout << "scale: auto" << endl;
   else
      cout << "scale: " << vd.xmin << " " << vd.ymin << " " << vd.xmax << " " << vd.ymax << endl;
   cout << "Colors: " << endl;
   for(unsigned j=0;j<vd.colors.size();j++)
      cout << vd.colors[j] << "; ";
   cout << endl;
   cout << "Points: " << vd.numpoints << endl;
   cout << endl << endl;*/

   ok=true;
   return true;

}



/******************************************************************************
   ParseLabel
*******************************************************************************
   Parse the definition of a label.
   str must not include the <> delimiters.
   str must include one scope definition only.

   The format of the label definition is:
   label ; sourcenum ; title ; trace1 trace2 ... [;xscale = xs][;yscale=<auto|ymin ymax>][;color=rrggbb rrggbb ...][;title=title1 title2 ....]

   Returns:
      true if the definition is of type label, false if it is something else
      ok: true if the label definition was successfully parsed, false otherwise

   Note: use the return value to identify if the string is of type scope (although parsing may fail later). Use ok to identify if the parsing is successful

******************************************************************************/

bool ParseLabel(QString str,bool &ok, LABELVIEWDATA &vd)
{
   //printf("ParseScope '%s'\n",str.toStdString().c_str());

   // Test and capture the scale
   QRegExp rxxscale("xscale\\s*=\\s*(-?\\d+)",Qt::CaseInsensitive);                                // One capture: xscale
   QRegExp rxyscale("yscale\\s*=\\s*(auto|(-?\\d+)\\s+(-?\\d+))",Qt::CaseInsensitive);             // Three captures: auto, ymin, ymax
   QRegExp rxcolor("color\\s*=\\s*(([abcdef\\d]{6,6}\\s*)*)\\s*",Qt::CaseInsensitive);             // Two captures: all the space separated hex colors, the last hex color
   QRegExp rxtitle("title\\s*=\\s*([\\w\\s]+)",Qt::CaseInsensitive);                               // One capture: the titles

   QString pre("Scope definition parser: ");
   vector<int> d;
   vector<unsigned> du;
   ok = false;



   QStringList elem = str.split(";");

   for(unsigned i=0;i<elem.size();i++)
      printf("Element %d: '%s'\n",i,elem[i].toStdString().c_str());

   if(elem.size() && elem[0].trimmed().compare("label",Qt::CaseInsensitive))
      return false;

   // It is a scope definition

   if(elem.size()<4 || elem.size()>8)
   {
      qDebug() << pre+"Incorrect number of parameters";
      return true;
   }


   QString source = elem[1].trimmed();
   vd.config.title =  elem[2].trimmed().toStdString();
   QString traces = elem[3].trimmed();
   vd.config.xscale=1;                                          // Default value for optional parameter
   vd.config.yauto = true;                                      // Default value for optional parameter
   vd.config.yscale.clear();                                    // Default value for optional parameter
   vd.config.colors.clear();                                    // Default value for optional parameter

   // Get the source
   if(!SplitInVector(source.toStdString(),string(" "),du))
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   if(du.size()!=1)
   {
      qDebug() << pre+"Cannot parse source";
      return true;
   }
   vd.source=du[0];

   if(!SplitInVector(traces,QString(" "),vd.traces))
   {
      qDebug() << pre+"Cannot parse traces";
      return true;
   }


   // Iterate the optional parameters
   for(unsigned i=4;i<elem.size();i++)
   {
      //printf("Processing optional element %d: '%s'\n",i,elem[i].toStdString().c_str());

      // xscale= ?
      if(rxxscale.exactMatch(elem[i].trimmed()))
      {
         vd.config.xscale = rxxscale.cap(1).toInt();
      }
      // yscale= ?
      else if(rxyscale.exactMatch(elem[i].trimmed()))
      {
         if(rxyscale.cap(1).compare("auto",Qt::CaseInsensitive) == 0)
         {
            vd.config.yauto=true;
         }
         else
         {
            // Numbers are in cap(2) and cap(3)
            vd.config.yauto=false;
            vd.config.yscale.push_back(rxyscale.cap(2).toInt());
            vd.config.yscale.push_back(rxyscale.cap(3).toInt());
            if(vd.config.yscale[0]>vd.config.yscale[1])
            {
               int t=vd.config.yscale[0];
               vd.config.yscale[0]=vd.config.yscale[1];
               vd.config.yscale[1]=t;
            }
            if(vd.config.yscale[0]==vd.config.yscale[1])
               vd.config.yscale[0]--;
         }
      }
      // color= ?
      else if(rxcolor.exactMatch(elem[i].trimmed()))
      {
         // Split cap(1)
         if(!SplitInVector(rxcolor.cap(1)," ",vd.config.colors,hex))
         {
            qDebug() << pre+"Color parse error";
            return true;
         }
         if(vd.config.colors.size()!=0 && (vd.config.colors.size() != vd.traces.size()))
         {
            qDebug() << pre+"Number of specified colors do not match the number of traces";
            return true;
         }
      }
      // title= ?
      else if(rxtitle.exactMatch(elem[i].trimmed()))
      {
         printf("Title:\n");
         for(int i=0;i<rxtitle.numCaptures()+1;i++)
            printf("%d: %s\n",i,rxtitle.cap(i).toStdString().c_str());

         // Split the capture
         vd.config.subtitle = rxtitle.cap(1).split(' ',QString::SkipEmptyParts);
         for(int i=0;i<vd.config.subtitle.size();i++)
            printf("%d: %s\n",i,vd.config.subtitle[i].toStdString().c_str());

         if(vd.config.subtitle.size()!=0 && (vd.config.subtitle.size() != vd.traces.size()))
         {
            qDebug() << pre+"Number of specified subtitles do not match the number of traces";
            return true;
         }

      }
      // Syntax error
      else
      {
         qDebug() << pre+"Invalid optional parameter";
         return true;
      }
   }

   /*
   cout << "Source: " << vd.source << endl;
   cout << "Title: " << vd.title << endl;
   cout << "Traces: " << endl;
   for(unsigned j=0;j<vd.traces.size();j++)
      cout << vd.traces[j] << "; ";
   cout << "\n";
   cout << "xscale: " << vd.xscale << endl;
   if(vd.yauto)
      cout << "yscale: auto" << endl;
   else
      cout << "yscale: " << vd.yscale[0] << " " << vd.yscale[1] << endl;
   cout << "Colors: " << endl;
   for(unsigned j=0;j<vd.colors.size();j++)
      cout << vd.colors[j] << "; ";
   cout << endl << endl;
   */

   ok=true;
   return true;

}

/******************************************************************************
   MainWindow::ParseLayoutString
*******************************************************************************

   Two step parsing approach:
      1. Split according to <> delimiters of each view
      2. Parse the view definition going through the available view parsers.


Example of view definitions:
<scope;0;A title;0 1 2;xscale=3;yscale=auto;color=ff00aa ffffff 8033aa>
<stickman;1;This is a stickman view;118 119 120 121 131 132 133 134 144 145 146 147 157 158 159 160 170 171 172 173>
<scope;2;Another title;4 2>
<stickman;3;Stickman2 without torso; -1 -1 -1 -1 131 132 133 134 144 145 146 147 157 158 159 160 170 171 172 173>
<scatter;0;Scatter plot;0 1 2 3;color=ff0000 00ff00;scale=-10 -5 10 5; points=200>

   Return value:
      0:    Success
      <0:   Error. Negative value of the form: -0xEEVVII with EE the error code, VV the view type (0=unknown,1=scope,2=stickman,3=scatter), II the view number

******************************************************************************/
int ParseLayoutString(string str,SCOPEVIEWDATAS &vdv,STICKMANVIEWDATAS &smvd,SCATTERVIEWDATAS &scattervds,LABELVIEWDATAS &labelvds)
{
   QStringList captures;
   bool ok,ok2;
   ok = Preparse(str,captures);

   if(!ok)
   {
      //printf("Preparse failed\n");
      return -1;
   }

   SCOPEVIEWDATA scopevd;
   STICKMANVIEWDATA stickmanvd;
   SCATTERVIEWDATA scattervd;
   LABELVIEWDATA labelvd;
   for(unsigned i=0;i<captures.size();i++)
   {
      //printf("Capture %d: '%s'\n",i,captures[i].toStdString().c_str());

      ok = ParseScope(captures[i],ok2,scopevd);
      if(ok)
      {
         //printf("Definition is of type scope. Parsing: %s\n",ok2?"success":"fail");
         if(ok2)
            vdv.push_back(scopevd);
         else
            return -(0x010000 + 0x0100 + i);
      }
      else {
      ok = ParseStickman(captures[i],ok2,stickmanvd);
      if(ok)
      {
         //printf("Definition is of type stickman. Parsing: %s\n",ok2?"success":"fail");
         if(ok2)
            smvd.push_back(stickmanvd);
         else
            return -(0x010000 + 0x0200 + i);
      }
      else {
      ok = ParseScatter(captures[i],ok2,scattervd);
      if(ok)
      {
         //printf("Definition is of type scatter. Parsing: %s\n",ok2?"success":"fail");
         if(ok2)
            scattervds.push_back(scattervd);
         else
            return -(0x010000 + 0x0300 + i);
      }
      else {
      ok = ParseLabel(captures[i],ok2,labelvd);
      if(ok)
      {
         //printf("Definition is of type stickman. Parsing: %s\n",ok2?"success":"fail");
         if(ok2)
            labelvds.push_back(labelvd);
         else
            return -(0x010000 + 0x0400 + i);
      }
      else
      {
         //printf("Invalid definition\n");
         return -(0x010000 + 0x0000 + i);
      } } } }
   }
   return 0;
}

