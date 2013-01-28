#include "nex.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <qlayout.h>
#include <klocale.h>

struct HorizontalPair : public Renderer
{
	HorizontalPair() : color(0x578cd8) 
	{
		// 0x578cd8 0x3cff3f 0x10FFFF
		horizontal=false;
		pair=true;
	}
	
	QWidget *configure(QWidget *parent)
	{
		QWidget *config=new QWidget(parent);
		(new QVBoxLayout(config))->setAutoAdd(true);

		new NexColorButton(config, &color);
		new NexCheckBox(config, i18n("Horizontal"), &horizontal);
		new NexCheckBox(config, i18n("Pair"), &pair);
		new NexCheckBox(config, i18n("Solid"), &solid);
		new Spacer(config);
		
		return config;
	}

	static inline void processV(int h, int start, int end, Bitmap *d,
	                            float *ch, Pixel c, register bool solid)
	{
		int oldx=(start+end)/2;
		int mid=oldx;
		float quarter=(end-start)/2.0;
		for (int y=0; y<h; ++y)
		{
			int newx=mid+(int)(ch[y]*quarter);
			if (newx<start) newx=start;
			if (newx>end) newx=end;
			if (newx>oldx)
				d->drawHorizontalLine(oldx, newx, y, c);
			else
				d->drawHorizontalLine(newx, oldx, y, c);
			if (!solid)
				oldx=newx;
		}
	}
	
	static inline void processH(int h, int start, int end, Bitmap *d,
	                            float *ch, Pixel c, register bool solid)
	{
		int oldx=(start+end)/2;
		int mid=oldx;
		float quarter=(end-start)/2.0;
		for (int y=0; y<h; ++y)
		{
			int newx=mid+(int)(ch[y]*quarter);
			if (newx<start) newx=start;
			if (newx>end) newx=end;
			if (newx>oldx)
				d->drawVerticalLine(y, newx, oldx, c);
			else
				d->drawVerticalLine(y, oldx, newx, c);
			if (!solid)
				oldx=newx;
		}
	}

	virtual Bitmap *render(float **pcm, Bitmap *src)
	{
		if (horizontal)
		{
			if (pair)
			{
				processH(width, 0, height/2, src, pcm[0], color, solid);
				processH(width, height/2, height, src, pcm[1], color, solid);
			}
			else
			{
				processH(width, 0, height, src, pcm[0], color, solid);
			}
		}
		else
		{
			if (pair)
			{
				processV(height, 0, width/2, src, pcm[0], color, solid);
				processV(height, width/2, width, src, pcm[1], color, solid);
			}
			else
			{
				processV(height, 0, width, src, pcm[0], color, solid);
			}
		}
		
		return src;
	}
	
	virtual void save(QDomElement &e)
	{
		e.setTagName("Waveform");
		e.setAttribute("horizontal", (int)horizontal);
		e.setAttribute("pair", (int)pair);
		e.setAttribute("solid", (int)solid);
		e.setAttribute("color", COLORSTR(color));
	}
	
	virtual void load(const QDomElement &e)
	{
		horizontal=(bool)e.attribute("horizontal", 0).toInt();
		pair=(bool)e.attribute("pair", 0).toInt();
		solid=(bool)e.attribute("solid", 0).toInt();
		color=STRCOLOR(e.attribute("color", "#FFFFFF"));
	}

	Pixel color;
	bool horizontal;
	bool pair;
	bool solid;
};

struct Hartley : public Renderer
{
	virtual Bitmap *render(float **pcm, Bitmap *src)
	{
		float mult=height/2.0;
		for (int i=0; i<width; i++)
		{
			int h=pcm[3][i]*mult;
			if (h>height) h=height;
			
			src->drawVerticalLine(i, height, height-h,0xFFFFFF);
		}
		return src;
	}
};

class Fade : public Renderer
{
public:
	virtual Bitmap *render(float *[2], Bitmap *src)
	{
		register int i=src->bytes()/sizeof(Pixel);
		
		register Pixel *d=src->pixels();
		do
		{
			*d -= ((*d & 0xf0f0f0f0) >> 4) +
			      ((*d & 0xe0e0e0e0) >> 5);
			++d;
		} while (--i > 0);
		return src;
	}
};


struct Doubler : public Renderer
{
	Doubler()
	{
		srcX=20;
		srcY=100;
		destX=240;
		destY=160;
		srcWidth=40;
		srcHeight=40;
	
	}
	
	inline static void copyAndDouble(Pixel *src, Pixel *dest1,
	                                 Pixel *dest2, int w)
	{
		while (w--)
		{
			*dest1=*dest2=*src;
			dest1++;
			dest2++;
			*dest1=*dest2=*src;
			dest1++;
			dest2++;
			src++;
		}
	}
	
	virtual Bitmap *render(float *[2], Bitmap *src)
	{
		for (int y=0; y<srcHeight; y++)
		{
			copyAndDouble(src->pixels(srcX, y+srcY),
			              src->pixels(destX, destY+y*2),
			              src->pixels(destX, destY+y*2+1),
				      srcWidth);
		}
		return src;
	}
	
	int srcX, srcY;
	int destX, destY;
	int srcWidth, srcHeight;
};

struct Blur : public Renderer
{
	virtual Bitmap *render(float *[2], Bitmap *src)
	{
		Bitmap *b=nex->bitmapPool()->get();
		
		register Byte *buffer=(Byte*)b->pixels();
		register unsigned short pitch=width*sizeof(Pixel);
		
		// skip the first row
		buffer+=pitch;
		
		// skip the first pixel on the second row;
		buffer+=sizeof(Pixel);
		
		// we also don't want to do the last row
		register Pixel *end=b->lastPixel();
		end-=pitch;
		// and the last pixel on the second-to-last row
		end-=sizeof(Pixel);
		
		while (buffer<(Byte*)end)
		{
		
		
		}
		return b;
	}
};

#ifdef cow
struct FadeHeat : public Renderer
{
	inline void fadePixelHeat(Bitmap *out, int x, int y)
	{
		Pixel up    = lastOutput->pixel(x,   y-1);
		Pixel down  = lastOutput->pixel(x,   y+1);
		Pixel left  = lastOutput->pixel(x-1, y);
		Pixel right = lastOutput->pixel(x+1, y);

		// assuming Pixel AARRGGBB
		Pixel r, g, b, pixel;
		const int rMask = 0xFFFF0000;
		const int gMask = 0x00FFFF00;
		const int bMask = 0x0000FFFF;
		
		// average the up down left right for each component
		r = up & rMask + 
		    right & rMask +
		    left & rMask +
		    bottom & rMask;
		r >>= 2;
		r &= rMask;

		g = up & gMask +
		    right & gMask +
		    left & gMask +
		    bottom & gMask;
		g >>= 2;
		g &= gMask;

		b = up & bMask +
		    right & bMask +
		    left & bMask +
		    bottom & bMask;
		b >>= 2;
		b &= bMask;

		Pixel all=r|g|b;
		if(!all)
		{
			out->pixel(x, y, 0);
			return;
		}

		// some more input
		Pixel c=lastLastOutput->getPixel(x, y);
		c+=0x00010101;
		
		
		
		all -= c;

		// bounds check
/* slow:	if (all & 0xFF000000) all=0;
		else if (all & 0x7F000000) all = 0x00FFFFFF;
		out->setPixel(x, y, all); */
		
		// everything is normal
		if(!(all & 0xFF000000)) out->setPixel(x,y, all);
		// wraparound occured (0x80==0b10000000)
		else if(all & 0x80000000) out->setPixel(x,y, 0);
		else out->setPixel(x,y, 0x00FFFFFF);
	}
	
	virtual Bitmap *render(float *[2], Bitmap *src)
	{
		Bitmap *lastLastTemp = lastLastOutput;
		lastLastOutput = lastOutput;
		lastOutputBmp.data = src;
		src = lastLastTemp;

		int x,y,i,j,start,end;
		int step = outWidth*2;
		for(x=i=0, j=outWidth*(outHeight-1)*2; 
		    x<outWidth;
		    x++, i+=2, j+=2)
		{
			fadePixelHeat(src, x,0,i);
			fadePixelHeat(src, x,0,i+1);
			fadePixelHeat(src, x,outHeight-1,j);
			fadePixelHeat(src, x,outHeight-1,j+1);
		}

		for(y=1, i=outWidth*2, j=outWidth*4-2;
		    y<outHeight;
		    y++, i+=step, j+=step)
		{
			fadePixelHeat(src, 0, y,i);
			fadePixelHeat(src, 0, y,i+1);
			fadePixelHeat(src, outWidth-1,y,j);
			fadePixelHeat(src, outWidth-1,y,j+1);
		}

		
		for(y=1,start=outWidth*2+2,
			end=outWidth*4-2; y<outHeight-1;
			y++,start+=step,end+=step)
		{
			int i = start;
			do
			{
				short j = 
					( short(Dlo[i-2])+
					Dlo[i+2]+
					+Dlo[i-step]
					+Dlo[i+step]
					>> 2)
					+Dlo[i];
				if (!j)
				{ 
					Do[i] = 0; 
				}
				else
				{
					j = j-Dllo[i]+(Dllo[i] -Dlo[i]>>2)-1;
					if (j < 0) Do[i] = 0;
					else if (j & (255*256)) Do[i] = 255;
					else Do[i] = j;
				}
			} while(++i < end);
		}
		
		return src;
	}
	
	Bitmap *lastOutput, Bitmap *lastLastOutput;
};
#endif

#define RENDERERS_CPP
#include "renderers.h"
#undef RENDERERS_CPP


