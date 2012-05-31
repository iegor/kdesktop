    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "session.h"
#include <stdio.h>
#include <arts/debug.h>

Session::Session()
{
	session = 0;
}

Session::~Session()
{
	if(session) delete session;
	session = 0;
}

bool Session::startExecute()
{
	arts_debug("TODO:PORT: Session::startExecute()");
#if 0
	assert(session);
	assert(!execID);

	// just in case synthesis has been halted before,
	// restart it and hope we'll have enough computing power now
	Synthesizer->Reset();
	execID = Synthesizer->restoreSession(*session,preferredservers);
#endif
	return true;
}

bool Session::loadSession(const char *filename)
{
#if 0
	if(session) delete session;
	session = 0;

	FILE *infile = fopen(filename,"r");
	if(!infile) return false;

	session = new ArtsCorba::StringSeq;

	char line[1024];
	unsigned long i = 0;

	while(fgets(line,1024,infile))
	{
		// cut eventual CR and/or LFs at the end of the line
		while(strlen(line) && line[strlen(line)-1] < 14)
			line[strlen(line)-1] = 0;

		session->length(i+1);
		(*session)[i++] = CORBA::string_dup(line);
	}
	fclose(infile);
	return true;
#endif
	arts_debug("TODO:PORT:loadSession(%s)",filename);
	return false;
}
