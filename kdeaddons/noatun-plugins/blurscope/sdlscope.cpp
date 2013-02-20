/*****************************************************************

Copyright (c) 2001 Charles Samuels <charles@kde.org>
              2001 Neil Stevens <multivac@fcmail.com>
              With code stolen from Paul Harrison <pfh@yoyo.cc.monash.edu.au>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************/

#include <sys/wait.h>
#include <sys/types.h>

#include "sdlscope.h"

extern "C"
{
Plugin *create_plugin()
{
	return new SDLScope();
}
}

SDLScope::SDLScope()
	: MonoScope(50)
	, Plugin()
{
	setSamples(320);
}

SDLScope::~SDLScope()
{
	::close(mOutFd);
	wait(0);
}

void SDLScope::init()
{
	int pipes[2];
	::pipe(pipes);
	mOutFd=pipes[1];
	if (!fork())
	{
		::close(pipes[1]);
		new SDLView(pipes[0]);
		exit(0);
	}
	else
	{
		fcntl(mOutFd, F_SETFL, fcntl(mOutFd, F_GETFL) & ~O_NONBLOCK);
		::close(pipes[0]);
		MonoScope::start();
	}	
}

void SDLScope::scopeEvent(float *d, int size)
{
	if (::write(mOutFd, (char *)d, size * sizeof(float))==-1)
	{
		MonoScope::stop();
		unload();
	}	
}

