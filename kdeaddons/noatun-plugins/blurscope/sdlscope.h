/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

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

#ifndef SDLSCOPE_H
#define SDLSCOPE_H

#include <noatun/plugin.h>
#include <string.h>

extern "C"
{
#include <SDL.h>
#include <fcntl.h>
#include <unistd.h>
}

class SDLScope : public MonoScope, public Plugin
{
NOATUNPLUGIND

public:
	SDLScope();
	virtual ~SDLScope();

	void init();

protected:
	virtual void scopeEvent(float *data, int bands);

private:
	int mOutFd;
};

struct SDL_Surface;

template<class Pixel> class Bitmap
{
public:
	int width, height, extra;
	Pixel *data;

	Bitmap(int e=0) : extra(e), data(0) { }
	~Bitmap() { delete[] data; }

	inline void addPixel(int x, int y, int bright1, int bright2);
	void addVertLine(int x, int y, int y2, int br1, int br2);
	
	void fadeStar();
	
	void size(int w,int h)
	{
		delete[] data;
		width = w;
		height = h;
		data = new Pixel[w*h+extra];
		clear();
	}

	void clear()
	{
		memset(data,0,sizeof(Pixel)*(width*height+extra));
	}
};


class SDLView
{
public:
	SDLView(int in);
	~SDLView();
	
protected:
	void startVideo();
	void checkInput();
	void setupPalette(double dummy=0.0);

	void repaint();

private:
	int mFd;

	SDL_Surface *surface;
	Bitmap<unsigned short> outputBmp;
	
	bool fullscreen;
	int width;
	int height;
};

#endif
