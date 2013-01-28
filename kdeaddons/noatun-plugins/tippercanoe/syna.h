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

#ifndef SYNA_H
#define SYNA_H

#include "config.h"
#include <qptrlist.h>
#include "polygon.h"

//**************************************
//   For the incurably fiddle prone:   

// log2 of sample size
#define LogSize 9 

// overlap amount between samples. Set to 1 or 2 if you have a fast computer
#define Overlap 1 

// Brightness
#define Brightness 150

// Sample frequency
#define Frequency 22050

//***************************************

#define DefaultWidth  260
#define DefaultHeight 260

#define NumSamples (1<<LogSize)
#define RecSize (1<<LogSize-Overlap)

#ifndef __linux__
#warning This target has not been tested!
#endif

#ifdef __FreeBSD__
#include <machine/endian.h>
typedef unsigned short sampleType;
#else
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_ENDIAN_H
#include <endian.h>
#elif defined(_BIG_ENDIAN) 
#define BIG_ENDIAN 1
#define BYTE_ORDER BIG_ENDIAN
#endif
typedef short sampleType;
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define BIGENDIAN
#else
#define LITTLEENDIAN
#endif

void error(const char *str, bool syscall=false);
void warning(const char *str, bool syscall=false);

enum SymbolID
{
	Speaker, Bulb,
	Play, Pause, Stop, SkipFwd, SkipBack,
	Handle, Pointer, Open, NoCD, Exit,
	Zero, One, Two, Three, Four,
	Five, Six, Seven, Eight, Nine,
	Slider, Selector, Plug, Loop, Box, Bar,
	Flame, Wave, Stars, Star, Diamond, Size, FgColor, BgColor,
	Save, Reset, TrackSelect,
	NotASymbol
};

// wrap 
struct BaseScreen
{
	virtual bool init(int xHint, int yHint, int widthHint, int heightHint, bool fullscreen) = 0;
	virtual void setPalette(unsigned char *palette) = 0;
	virtual void end() = 0;
	virtual int  sizeUpdate() = 0;
	virtual bool inputUpdate(int &mouseX,int &mouseY,int &mouseButtons,char &keyHit) = 0;
	virtual void show() = 0;
};

struct SdlScreen : public BaseScreen
{
	bool init(int xHint, int yHint, int widthHint, int heightHint, bool fullscreen);
	void setPalette(unsigned char *palette);
	void end();
	int  sizeUpdate();
	bool inputUpdate(int &mouseX,int &mouseY,int &mouseButtons,char &keyHit);
	void show();
	int winID();
};

// core
class Combiner
{
public:
	static unsigned short combine(unsigned short a,unsigned short b)
	{
		//Not that i want to give the compiler a hint or anything...
		unsigned char ah = a>>8, al = a&255, bh = b>>8, bl = b&255;
		if (ah < 64) ah *= 4; else ah = 255;
		if (al < 64) al *= 4; else al = 255;
		if (bh > ah) ah = bh;
		if (bl > al) al = bl;
		return ah*256+al;
	}
};

class Interface;

class Core
{
public:
	Core();
	~Core();

	inline unsigned char *output() { return (unsigned char*)outputBmp.data; }
	inline unsigned char *lastOutput() { return (unsigned char*)lastOutputBmp.data; }
	inline unsigned char *lastLastOutput() { return (unsigned char*)lastLastOutputBmp.data; }
	
	void allocOutput(int w,int h);
	void interfaceInit();
	//void coreInit();
	void setStarSize(double size);
	void setupPalette(double);
	
	bool go();
	bool calculate();
	
	void toDefaults();
	void fade();
	void fadeFade();
	inline void fadePixelWave(int x, int y, int where, int step);
	void fadeWave();
	inline void fadePixelHeat(int x,int y,int where,int step);
	void fadeHeat();

	void fft(double*, double*);
	
	inline void addPixel(int x, int y, int br1, int br2);
	inline void addPixelFast(unsigned char *p, int br1, int br2);
	inline unsigned char getPixel(int x, int y, int where);
	
	static int bitReverser(int);


public:
	BaseScreen *screen;
	Interface *interface;
	sampleType *data;
	Bitmap<unsigned short> outputBmp, lastOutputBmp, lastLastOutputBmp;
	PolygonEngine<unsigned short,Combiner,2> polygonEngine;

	double cosTable[NumSamples], negSinTable[NumSamples];
	int bitReverse[NumSamples];
	int scaleDown[256];
	int maxStarRadius;


public:
	int outWidth, outHeight;
	SymbolID fadeMode;
	bool pointsAreDiamonds;

	double brightnessTwiddler; 
	double starSize; 

	double fgRedSlider, fgGreenSlider, bgRedSlider, bgGreenSlider;
	SymbolID state;

	int windX, windY, windWidth, windHeight;
};

extern Core *core;

inline unsigned char Core::getPixel(int x,int y,int where)
{
	if (x < 0 || y < 0 || x >= outWidth || y >= outHeight) return 0;
	return lastOutput()[where];
}


struct Button;

struct UIObject
{
	int visibleMask, activeMask;
	double x,y,width,height;
	bool active;

	virtual int go(bool mouseDown,bool mouseClick,bool mouseOver,
			double x, double y, double scale, char &hotKey, int &action)
		= 0;

	virtual void handleKey(char key, int &action) = 0;
	void changed();
};

class Interface
{
public:
	Interface();
	~Interface();
	bool go();
	void syncToState();
	void setupPalette();

	void putString(char *string,int x,int y,int red,int blue);
	
protected:
	void addUI(UIObject *obj);
	void changeState(int transitionSymbol);
	
private:
	QPtrList<UIObject> uiObjects; 
	Button *stateButton, *starsButton, *waveButton, *flameButton, *starButton, *diamondButton;
	int mouseButtons;

	int visibleMask;
	int mouseX, mouseY, lastX, lastY, countDown;

};


#endif 

