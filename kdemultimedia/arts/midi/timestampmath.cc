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

#include "timestampmath.h"
#include <arts/debug.h>
#include <stdio.h>

using namespace std;

namespace Arts {
void timeStampInc(TimeStamp& t, const TimeStamp& delta)
{
	/* expect a normalized t, delta */
	arts_return_if_fail(t.usec >= 0 && t.usec < 1000000);
	arts_return_if_fail(delta.usec >= 0 && delta.usec < 1000000);

	t.sec += delta.sec;
	t.usec += delta.usec;

	if (t.usec >= 1000000)
	{
		t.usec -= 1000000;
		t.sec += 1;
	}

	arts_assert (t.usec >= 0 && t.usec < 1000000);
}

void timeStampDec(TimeStamp& t, const TimeStamp& delta)
{
	/* expect a normalized t, delta */
	arts_return_if_fail(t.usec >= 0 && t.usec < 1000000);
	arts_return_if_fail(delta.usec >= 0 && delta.usec < 1000000);

	t.sec -= delta.sec;
	t.usec -= delta.usec;

	if(t.usec < 0)
	{
		t.usec += 1000000;
		t.sec -= 1;
	}

	arts_assert(t.usec >= 0 && t.usec < 1000000);
}

string timeStampToString(const TimeStamp& t)
{
	arts_return_val_if_fail(t.usec >= 0 && t.usec < 1000000, "");

	char buffer[1024];
	if(t.sec < 0 && t.usec != 0)
	{
		sprintf(buffer, "-%d.%06d", -t.sec-1, 1000000-t.usec);
	}
	else
	{
		sprintf(buffer, "%d.%06d", t.sec, t.usec);
	}
	return buffer;
}

double timeStampToDouble(const TimeStamp& t)
{
	arts_return_val_if_fail(t.usec >= 0 && t.usec < 1000000, 0.0);

	return double(t.sec) + double(t.usec)/1000000.0;
}

TimeStamp timeStampFromDouble(double d)
{
	TimeStamp t;

	arts_return_val_if_fail(d >= 0, t);

	t.sec = int(d);
	d -= t.sec;
	t.usec = int(d * 1000000.0);

	return t;
}

TimeStamp timeStampMax(const TimeStamp& t1, const TimeStamp& t2)
{
	if(t1.sec > t2.sec)
		return t1;
	else if((t1.sec == t2.sec) && (t1.usec > t2.usec))
		return t1;
	else
		return t2;
}

}
