    /*

    Copyright (C) 2002 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef ARTS_TIMESTAMPMATH_H
#define ARTS_TIMESTAMPMATH_H

#include "artsmidi.h"
#include <kdelibs_export.h>
namespace Arts {

/**
 * increments the timestamp by delta
 */
KDE_EXPORT void timeStampInc(TimeStamp& t, const TimeStamp& delta);

/**
 * decrements the timestamp by delta
 */
void timeStampDec(TimeStamp& t, const TimeStamp& delta);

/**
 * stringifies a timestamp
 */
std::string timeStampToString(const TimeStamp& t);

/**
 * converts a timestamp to a double of seconds
 */
double timeStampToDouble(const TimeStamp& t);

/**
 * converts a double of seconds to a timestamp
 */
TimeStamp timeStampFromDouble(double d);

/**
 * returns the maximum of two timestamps
 */
TimeStamp timeStampMax(const TimeStamp& t1, const TimeStamp& t2);

}

#endif  /* ARTS_TIMESTAMPMATH_H */
