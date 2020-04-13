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

#ifndef __PARSE_H
#define __PARSE_H


#include <QString>
#include <QStringList>
#include "data.h"


bool Preparse(string str,QStringList &captures);
bool ParseScope(QString str,bool &ok, SCOPEVIEWDATA &vd);
bool ParseStickman(QString str,bool &ok, STICKMANVIEWDATA &vd);
bool ParseScatter(QString str,bool &ok, SCATTERVIEWDATA &vd);
bool ParseLabel(QString str,bool &ok, LABELVIEWDATA &vd);
int ParseLayoutString(string str,SCOPEVIEWDATAS &vdv,STICKMANVIEWDATAS &smvd,SCATTERVIEWDATAS &scattervds,LABELVIEWDATAS &labelvds);


#endif // __PARSE_H
