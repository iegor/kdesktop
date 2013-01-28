/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>

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
#include <math.h>

#include "syna.h"

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

Core::Core() : StereoScope(40)
{
	setSamples(NumSamples);
	core = this;
	int i;

	data = new sampleType[NumSamples*2+16];

	for(i=0;i<NumSamples;i++)
	{
		negSinTable[i] = -sin(3.141592*2.0/NumSamples*i);
		cosTable[i] = cos(3.141592*2.0/NumSamples*i);
		bitReverse[i] = bitReverser(i);
	}
}

bool Core::init ( void )
{
	windX = 1;
	windY = 1;

	screen = new SdlScreen;
	if (!screen->init(windX,windY,windWidth,windHeight,false))
	{
		delete screen;
		screen = 0;
		return false;
	}

	allocOutput(outWidth,outHeight);

	setStarSize(starSize);

	interface = new Interface;
	return true;
}

Core::~Core()
{
	delete interface;

	screen->end();
	delete screen;
}

bool Core::go()
{
	time_t startTime = time(0);
	time_t currentTime = time(0);
	time_t lastTime = 0;
	char fpsString[32];
	int frames = 0;
	double fps=0;

//	printf("%u\n", static_cast<SdlScreen *>(screen)->winID());
//	fflush(stdout);

	while (1)
	{
		fade();

		if (!calculate())
			break;

		if (!interface->go()) break;

		screen->show();

		frames++;
		currentTime = time(0) - startTime;

		if ((currentTime - lastTime) > 1) // update fps display every 2 seconds
		{
			fps = double(frames) / currentTime;
			lastTime = currentTime;
			snprintf(fpsString, 32, "Noatun - Synaescope: %.2f fps", fps);
			screen->updateWindowCaption(&fpsString[0]);
		}
		if (fps>25.0)
			usleep(5);
	}

	return true;
}

bool start()
{
	return true;
}

void error(const char *str, bool)
{
	fprintf(stderr, "synaescope: Error %s\n",str);
//	if (syscall)
//		fprintf(stderr,"(reason for error: %s)\n",strerror(errno));
	exit(1);
}

void warning(const char *str, bool)
{
	fprintf(stderr, "synaescope: Possible error %s\n",str);
//	if (syscall)
//		fprintf(stderr,"(reason for error: %s)\n",strerror(errno));
}

#include <kinstance.h>
#include <kglobal.h>
#include <kconfig.h>

int main(void)
{
	KInstance in("noatunsynaescope");

	core = new Core;

	KConfig mConfig("noatunrc");
	mConfig.setGroup("Synaescope");

	core->windWidth = mConfig.readNumEntry("xResolution", 320);
	core->windHeight = mConfig.readNumEntry("yResolution", 240);

	if ( core->init() )
		core->go();

	delete core;
	exit(0);
}
