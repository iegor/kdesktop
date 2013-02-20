#include "font.h"
#include "icons.h"
#include "syna.h"
#include <math.h>
#include <stdlib.h>
#include <kconfig.h>

#define output core->output()

static void putChar(unsigned char character,int x,int y,int red,int blue)
{
	unsigned short *ptr = ((unsigned short *)output) + x + y*core->outWidth;
	unsigned short  put = (blue<<8)+red;
	int i,j;
	for(i=0;i<8;i++,ptr += core->outWidth-8)
		for(j=0;j<8;j++,ptr++)
			if (font[character*8+i] & (128>>j))
				*ptr = put;
}

void Interface::putString(char *string,int x,int y,int red,int blue)
{
	if (x < 0 || y < 0 || y >= core->outHeight-8)
		return;
	for(;*string && x <= core->outWidth-8;string++,x+=8)
		putChar((unsigned char)(*string),x,y,red,blue);
}

void UIObject::changed()
{
}

struct Button : public UIObject
{
	int icon;
	char hotKey;

	bool passive, bright;

	Button(int _am,int _vm,double _x,double _y,
			double _size,int _icon,char _key = 0, 
			bool _passive = false, bool _bright = false)
	{
		activeMask = _am; visibleMask = _vm;
		x = _x; y = _y; width = height = _size;
		icon = _icon; hotKey = _key; passive = _passive; bright = _bright;
	}

	int go(bool mouseDown,bool mouseClick,bool mouseOver,
			double _x, double _y, double scale, char &_hotKey, int &action)
	{
		(void)mouseDown;
		(void)_x;
		(void)_y;
		core->polygonEngine.icon(
				Icons[icon],
				(bright ? 0x0202 : 0x0100),
				x*scale,y*scale,width*scale,height*scale);

		if (mouseOver && !passive)
			core->polygonEngine.icon(
					Icons[icon],
					0x0002,
					(x-IconWidths[icon]*width/2)*scale,
					(y-height/2)*scale,width*scale*2,height*scale*2);

		if (mouseOver && mouseClick && !passive)
			action = icon;

		if (mouseOver && !passive && hotKey)
			_hotKey = hotKey;

		return 0;
	}

	void handleKey(char key, int &action)
	{
		if (key == hotKey && !passive)
			action = icon;
	}
};
#define BarWidth 0.1
struct SliderBar : public UIObject
{
	double *value;
	char leftKey, rightKey;

	typedef void (Core::*Callback)(double v);
	Callback callback;

	SliderBar(int _am,int _vm,double _x,double _y,
			double _width,double _height, double *_value,
			Callback _callback, char _leftKey, char _rightKey)
	{
		activeMask = _am; visibleMask = _vm;
		x = _x; y = _y; width = _width; height = _height;
		value = _value; callback = _callback;
		leftKey = _leftKey; rightKey = _rightKey;
	}

	int go(bool mouseDown,bool mouseClick,bool mouseOver,
			double _x, double _y, double scale, char &_hotKey, int &action)
	{
		(void)mouseClick;
		(void)_y;
		(void)action;
		core->polygonEngine.icon(
			Icons[Bar],
			0x0100,
			x*scale,y*scale,width*scale,height*scale);
		core->polygonEngine.icon(
			Icons[Slider],
			0x0200,
			(x+*value*width-IconWidths[Slider]*height/2)*scale,
			y*scale,height*scale,height*scale);

		if (mouseOver)
		{
			double newValue = (_x)/(width);
			if (newValue < 0.0) newValue = 0.0;
			if (newValue > 1.0) newValue = 1.0;

			core->polygonEngine.icon(
					Icons[Selector],
					0x0002,
					(x+newValue*width-IconWidths[Selector]*height/2)*scale,
					y*scale,height*scale,height*scale);

			if (mouseDown)
			{
				*value = newValue;

				if (callback)
					(core->*callback)(*value);
				changed();
			}

			if (mouseOver)
				_hotKey = (newValue < *value ? leftKey : rightKey);
		}

		return 0;
	}
	void handleKey(char key, int &action)
	{
		(void)action;
		if (key == leftKey || key == rightKey)
		{
			if (key == leftKey)
			{
				if (*value == 0.0) return;
				*value -= 0.05;
				if (*value < 0.0) *value = 0.0;
			}
			else
			{
				if (*value == 1.0) return;
				*value += 0.05;
				if (*value > 1.0) *value = 1.0;
			}

			if (callback)
				(core->*callback)(*value);
			changed();
		}
	}
};
#undef BarWidth


struct PopperUpper : public UIObject
{
	int maskAdd;

	PopperUpper(int _am,int _vm,double _x,double _y,
			double _width,double _height, int _maskAdd)
	{
		activeMask = _am; visibleMask = _vm; 
		x = _x; y = _y; width = _width; height = _height;
		maskAdd = _maskAdd;
	}

	int go(bool mouseDown,bool mouseClick, bool mouseOver,
			double _x, double _y, double scale, char &_hotKey, int &action)
	{
		(void)mouseDown;
		(void)mouseClick;
		(void)_x;
		(void)_y;
		(void)_hotKey;
		(void)action;
		
		core->polygonEngine.icon(
				Icons[Box],
				0x0200,
				x*scale,y*scale,width*scale,height*scale);

		return mouseOver ? maskAdd : 0;
	}

	void handleKey(char key, int &action)
	{
		(void)key;
		(void)action;
	}
};

void Interface::setupPalette()
{
#define BOUND(x) ((x) > 255 ? 255 : (x))
#define PEAKIFY(x) int(BOUND((x) - (x)*(255-(x))/255/2))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
	int i;
	unsigned char palette[768];

	double scale, fgRed, fgGreen, fgBlue, bgRed, bgGreen, bgBlue;
	fgRed = core->fgRedSlider;
	fgGreen = core->fgGreenSlider;
	fgBlue = 1.0 - MAX(core->fgRedSlider,core->fgGreenSlider);
	scale = MAX(MAX(fgRed,fgGreen),fgBlue);
	fgRed /= scale;
	fgGreen /= scale;
	fgBlue /= scale;

	bgRed = core->bgRedSlider;
	bgGreen = core->bgGreenSlider;
	bgBlue = 1.0 - MAX(core->bgRedSlider, core->bgGreenSlider);
	scale = MAX(MAX(bgRed, bgGreen), bgBlue);
	bgRed /= scale;
	bgGreen /= scale;
	bgBlue /= scale;

	for(i=0;i<256;i++)
	{
		int f = i&15, b = i/16;
		palette[i*3+0] = PEAKIFY(b*bgRed*16+f*fgRed*16);
		palette[i*3+1] = PEAKIFY(b*bgGreen*16+f*fgGreen*16);
		palette[i*3+2] = PEAKIFY(b*bgBlue*16+f*fgBlue*16);
	}
	core->screen->setPalette(palette);
#undef BOUND
#undef PEAKIFY
#undef MAX
}

//Visible mask
#define ALL 1
#define BUTTONBAR 2
#define TRACKBAR 4
#define DIALBAR 8
#define VOLUMEBAR 16

//Active mask
//#define ALL 1
#define PLAYING 2
#define PAUSED 4
#define STOPPED 8
#define NOCD 32 
#define OPEN 64 


// TODO Lay things out with parents and a stack, like QT
Interface::Interface()
{
	static const float IconSize=0.2;
	static const float SliderSize=0.125;

	{
		KConfig config("noatun/synaescope", false, false, "data");
		core->fadeMode=(SymbolID)config.readNumEntry("mode", (int)Stars);
		core->pointsAreDiamonds=config.readBoolEntry("diamonds", false);
		core->brightnessTwiddler=config.readDoubleNumEntry("brightness", .4);
		core->starSize=config.readDoubleNumEntry("starsize", .1);
		core->fgRedSlider=config.readDoubleNumEntry("FGRed", 0.0);
		core->fgGreenSlider=config.readDoubleNumEntry("FGgreen", 1.0);
		core->bgRedSlider=config.readDoubleNumEntry("BGRed", 0.0);
		core->bgGreenSlider=config.readDoubleNumEntry("BGGreen", 0.0);
	}
	
	uiObjects.setAutoDelete(true);

	double x,y;
	
	//addUI(new Button(ALL,0.025,0.525,IconSize, 0, 'x'));
//	addUI(new PopperUpper(ALL,ALL,0,0,0.25,0.25, BUTTONBAR));
//	addUI(stateButton = new Button(ALL,ALL,0.05,0.025,IconSize, 0, 0, true, false));

	addUI(new PopperUpper(ALL,BUTTONBAR,x=0.25,y=0,1.375,0.25, BUTTONBAR));
//	x += 0.1; y += 0.025;

//	addUI(new PopperUpper(PLAYING|PAUSED|STOPPED, ALL,0,0.25,0.25,0.25, TRACKBAR));
//	addUI(new PopperUpper(PLAYING|PAUSED|STOPPED, TRACKBAR,x=0.25,y=0.25,1.0,0.625, TRACKBAR));
//	x += 0.1; y += 0.1;

	addUI(new PopperUpper(ALL,ALL,0,0.0,0.25,0.25, DIALBAR));
	addUI(new Button(ALL,ALL,0.075,0.05,IconSize, Bulb, 0, true, false));

	addUI(new PopperUpper(ALL,DIALBAR,x=0.25,y=0.0,1.25,1.0, DIALBAR));
	x += 0.05; y += 0.025;

	addUI(starsButton = new Button(ALL,DIALBAR,x,y,IconSize, Stars, 'd'));
	addUI(waveButton = new Button(ALL,DIALBAR,x+IconSize,y,IconSize, Wave, 'f'));
	addUI(flameButton = new Button(ALL,DIALBAR,x+IconSize*2.5,y,IconSize, Flame, 'g'));

	addUI(starButton = new Button(ALL,DIALBAR,x+IconSize*3.5,y,IconSize, Star, 'h'));
	addUI(diamondButton = new Button(ALL,DIALBAR,x+IconSize*4.5,y,IconSize, Diamond, 'j'));

	y += IconSize*1.3;

	addUI(new Button(ALL,DIALBAR,x,y-0.05,IconSize, Bulb, 0, true));
	addUI(new SliderBar(ALL,DIALBAR,
		x+IconSize,y, 0.75, SliderSize, &core->brightnessTwiddler, /*&Core::setBrightness,*/0, 'z', 'x'));

	addUI(new Button(ALL,DIALBAR,x,y+SliderSize*1,IconSize, Size, 'x', true));
	addUI(new SliderBar(ALL,DIALBAR,
		x+IconSize,y+SliderSize, 0.75, SliderSize, &core->starSize, &Core::setStarSize, 'c','v'));

	addUI(new Button(ALL,DIALBAR,x+0.5,y+SliderSize*2-0.025,IconSize, FgColor, 0, true));
	addUI(new SliderBar(ALL,DIALBAR,
		x,y+SliderSize*2, 0.45, SliderSize, &(core->fgRedSlider), &Core::setupPalette, 'b','n'));
	addUI(new SliderBar(ALL,DIALBAR,
				x+0.5+SliderSize,y+SliderSize*2, 0.45, SliderSize, &core->fgGreenSlider, &Core::setupPalette, 'm',','));

	addUI(new Button(ALL,DIALBAR,x+0.5,y+SliderSize*3,IconSize, BgColor, 0, true));
	addUI(new SliderBar(ALL,DIALBAR,
				x,y+SliderSize*3, 0.45, SliderSize, &core->bgRedSlider, &Core::setupPalette, 'B','N'));
	addUI(new SliderBar(ALL,DIALBAR,
				x+0.5+SliderSize,y+SliderSize*3, 0.45, SliderSize, &core->bgGreenSlider, &Core::setupPalette, 'M','<'));

	x += 0.1;// y += 0.0625;
	//static double value = 0.5;
	//addUI(new SliderBar(ALL,0,0.75,1.0,0.25,&value)); 

	//addUI(new Button(BUTTONBAR,x,y,IconSize, 1, 'x'));
	//addUI(new Button(BUTTONBAR,x += IconSize,y,IconSize, 2, 'x'));
	//addUI(new Button(BUTTONBAR,x += IconSize,y,IconSize, 3, 'x'));

	visibleMask = ALL;
	mouseX = -1;
	mouseY = -1;
	lastY = -1;
	lastY = -1;
	countDown = 0;
	mouseButtons = 0;

	syncToState();
	
}

Interface::~Interface()
{
	KConfig config("noatun/synaescope", false, false, "data");
	config.writeEntry("mode",core->fadeMode);
	config.writeEntry("diamonds", core->pointsAreDiamonds);
	config.writeEntry("brightness", core->brightnessTwiddler);
	config.writeEntry("starsize", core->starSize);
	config.writeEntry("FGRed", core->fgRedSlider);
	config.writeEntry("FGgreen", core->fgGreenSlider);
	config.writeEntry("BGRed", core->bgRedSlider);
	config.writeEntry("BGGreen", core->bgGreenSlider);
}

void Interface::addUI(UIObject *obj)
{
	uiObjects.append(obj);
}

void Interface::syncToState()
{
	starsButton->bright = (core->fadeMode == Stars); 
	flameButton->bright = (core->fadeMode == Flame); 
	waveButton->bright = (core->fadeMode == Wave);

	starButton->bright = !core->pointsAreDiamonds;
	diamondButton->bright = core->pointsAreDiamonds;

	setupPalette();
}

void Interface::changeState(int transitionSymbol)
{
	if (transitionSymbol < 0)
	{
		return ;
	}

	int retVal = 0;
	switch(transitionSymbol)
	{
		case Flame :
			starsButton->bright = false;
			flameButton->bright = true;
			waveButton->bright = false;
			core->fadeMode = Flame;
			core->setStarSize(core->starSize);
			break;
		case Wave :
			starsButton->bright = false;
			flameButton->bright = false;
			waveButton->bright = true;
			core->fadeMode = Wave;
			core->setStarSize(core->starSize);
			break;
		case Stars :
			starsButton->bright = true;
			flameButton->bright = false;
			waveButton->bright = false;
			core->fadeMode = Stars;
			core->setStarSize(core->starSize);
			break;

		case Star :
			core->pointsAreDiamonds = false;
			starButton->bright = true;
			diamondButton->bright = false;
			break;
		case Diamond :
			core->pointsAreDiamonds = true;
			starButton->bright = false;
			diamondButton->bright = true;
			break;

		case Exit  : 
			retVal = 1; break;
	}
//	return retVal;
}

bool Interface::go()
{
	int newVisibleMask = ALL;
	char keyHit;
	int action = NotASymbol;
	int oldButtons = mouseButtons;

	core->screen->sizeUpdate();
	if (!core->screen->inputUpdate(mouseX,mouseY,mouseButtons,keyHit))
		return false;

	bool mouseClick = (mouseButtons && !oldButtons);

	if ((mouseX != lastX || mouseY != lastY) && 
			lastX > 0 && lastY > 0 && 
			lastX < core->outWidth && lastY < core->outHeight)
		countDown = 40;

	int activeMask = ALL;

	if (countDown)
	{
		countDown--;

		double scale = 
			(core->outWidth*0.625 < core->outHeight ? core->outWidth*0.625 : core->outHeight);
		double scaledX = mouseX / scale;
		double scaledY = mouseY / scale;

		char hotKey = 0;

		core->polygonEngine.clear();

//		stateButton->icon = core->state;


		for (UIObject *i=uiObjects.first(); i!=0; i = uiObjects.next())
		{
			if ((i->visibleMask & visibleMask) && (i->activeMask & activeMask))
				newVisibleMask |= i->go(mouseButtons,mouseClick,
						(scaledX >= i->x &&
						 scaledY >= i->y &&
						 scaledX < i->x+i->width &&
						 scaledY < i->y+i->height),
						scaledX - i->x,
						scaledY - i->y,
						scale,
						hotKey,
						action);
		}

		visibleMask = newVisibleMask;  
		if (visibleMask != 1)
			countDown = 20;

		core->polygonEngine.icon(Icons[Pointer],0x0303,mouseX,mouseY,50,50);

		core->polygonEngine.apply(core->outputBmp.data);

		char hint[2] = " ";
		hint[0] = hotKey;
		putString(hint,mouseX+6,mouseY+7,0,0);
	}

	if (keyHit)
		for(UIObject *i=uiObjects.first(); i!=0; i = uiObjects.next()) 
			if (i->activeMask & activeMask)
				i->handleKey(keyHit,action);


	lastX = mouseX;
	lastY = mouseY;

	changeState(action);

	return true;
}

#undef output
