/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>
   Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include "syna.h"
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

void Core::allocOutput(int w,int h)
{
#if 0
	delete[] output;
	delete[] lastOutput;
	delete[] lastLastOutput;
	output = new unsigned char[w*h*2];
	lastOutput = new unsigned char[w*h*2];
	lastLastOutput = new unsigned char[w*h*2];
	memset(output,32,w*h*2);
	memset(lastOutput,32,w*h*2);
	outWidth = w;
	outHeight = h;
#endif
	
	outputBmp.size(w,h);
	lastOutputBmp.size(w,h);
	lastLastOutputBmp.size(w,h);
	polygonEngine.size(w,h);
	outWidth = w;
	outHeight = h;
}

void Core::toDefaults()
{
	fadeMode = Stars;
	pointsAreDiamonds = true;

	brightnessTwiddler = 0.33; //0.125; 
	starSize = 0.125; 

	fgRedSlider=0.0;
	fgGreenSlider=0.5;
	bgRedSlider=1.0;
	bgGreenSlider=0.2;
}

Core::Core()
{
	core=this;
	int i;

	data=new sampleType[NumSamples*2+16];
	windY=windX=1;
	windWidth=320;
	windHeight=240;
	for(i=0;i<NumSamples;i++)
	{
		negSinTable[i] = -sin(3.141592*2.0/NumSamples*i);
		cosTable[i] = cos(3.141592*2.0/NumSamples*i);
		bitReverse[i] = bitReverser(i);
	}

	screen = new SdlScreen;
	if (!screen->init(windX,windY,windWidth,windHeight,false))
	{
		delete screen;
		screen = 0;
		return;
	}

	allocOutput(outWidth,outHeight);

	setStarSize(starSize);

	interface=new Interface;
}

Core::~Core()
{
	delete interface;

	screen->end();
	delete screen;
}

bool Core::go()
{
	time_t timer = time(NULL);

	printf("%u\n", static_cast<SdlScreen *>(screen)->winID());
	fflush(stdout);

	int frames = 0;
	while (1)
	{
		fade();

		if (!calculate())
			break;

		if (!interface->go()) break;

		screen->show();
		frames++;
	} 

	timer = time(0) - timer;
/*	if (timer > 10)
		printf("Frames per second: %f\n", double(frames) / timer);*/
	return true;
}

bool start()
{
return true;
}


void error(const char *, bool)
{ 
	
/*	fprintf(stderr, "synaescope: Error %s\n",str); 
	if (syscall)
		fprintf(stderr,"(reason for error: %s)\n",strerror(errno));*/
	exit(1);
}
void warning(const char *, bool)
{ 
/*	fprintf(stderr, "synaescope: Possible error %s\n",str); 
	if (syscall)
		fprintf(stderr,"(reason for error: %s)\n",strerror(errno));*/
}

#include <kinstance.h>
int main()
{
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
	KInstance in("noatunsynaescope");
	core=new Core;
	core->go();
	delete core;
	exit(0);
}


