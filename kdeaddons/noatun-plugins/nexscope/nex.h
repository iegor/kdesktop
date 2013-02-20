#ifndef NEX_H
#define NEX_H 

#include <qwidget.h>
#include <qptrlist.h>
#include <stdint.h>
#include <vector>
#include <qdict.h>

#include <qdatetime.h>
#include <iostream>

#include <qdom.h>

#include "SDL.h"
#include "SDL_thread.h"

typedef uint32_t Pixel;
typedef uint8_t Byte;

#define COLOR(r,g,b) ((r<<16) | (g<<8) | (b))
#define COLORSTR(pixel) \
	QString("#%1%2%3").arg(QString::number((pixel>>16) & 8, 16)) \
		.arg(QString::number((pixel>>8) & 8, 16)).arg(QString::number(pixel& 8, 16))

#define STRCOLOR(pixel) \
	Pixel(((pixel.mid(1,2).toInt(0, 16)) <<16) \
	| ((pixel.mid(3,2).toInt(0, 16)) <<8) \
	| (pixel.mid(5,2).toInt(0, 16)))
	

const int samples=512+256;
const int fhtsamples=512;
const int width=320*2;
const int height=240*2;

#define PI 3.141592654

class Mutex
{
public:
	Mutex() { mMutex=::SDL_CreateMutex(); }
	~Mutex() { ::SDL_DestroyMutex(mMutex); }
	
	inline bool lock() { return !::SDL_mutexP(mMutex); }
	inline bool unlock() { return !::SDL_mutexV(mMutex); }
	
private:
	SDL_mutex *mMutex;
};

class Thread 
{
public:
	Thread();
	
	/**
	 * kill() the thread
	 **/
	virtual ~Thread();
	
	void start();
	void kill();
	int wait();
	
protected:
	virtual int run()=0;

private:
	static int threadRun(void *);
	
	SDL_Thread *mThread;
};

class Spacer : public QWidget
{
Q_OBJECT
public:
	Spacer(QWidget *parent) : QWidget(parent)
	{
		setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
			QSizePolicy::MinimumExpanding));
	}
};

class Bitmap
{
public:
	Bitmap() : mData(0)
		{ }
	~Bitmap() { delete [] mData; }
	
	void resize(int w, int h);
	
	inline void setPixel(int x, int y, Pixel c)
		{ mData[y*width+x] = c; }
	
	inline void drawVerticalLine(int x, int yBottom, int yTop, Pixel c)
	{
		register int w=width;
		Pixel *d=mData+x+yTop*w;
		yBottom-=yTop;
		do
		{
			*d=c;
			d+=width;
		} while (yBottom--);
	}
	
	inline void drawHorizontalLine(int left, int right, int y, Pixel c)
	{
		Pixel *d=mData+y*width+left;
		right-=left;
		do
		{
			*(d++)=c;
		} while (right--);
	}
	
	void drawCircle(int x, int y, int radius, Pixel color);
	void fillCircle(int x, int y, int radius, Pixel color);
	void drawLine(int x1, int y1, int x2, int y2, Pixel color);
	
	inline int bytes() const { return width*height*sizeof(Pixel); }
	inline Pixel *pixels(int x, int y) { return &(mData[y*width+x]); }
	inline Pixel *pixels() const { return mData; }
	inline Pixel pixel(int x, int y) { return mData[y*width+x]; }
	inline Pixel *lastPixel() { return pixels(width-1, height-1); }
	
	void clear();

private:
	Pixel *mData;
};




/**
 * maintains a list of bitmaps, so you
 * can recycle allocated segments
 *
 * Thread safe
 **/
class BitmapPool
{
public:
	BitmapPool();
	~BitmapPool();
	
	Bitmap *get(bool clear=false);
	void release(Bitmap *bitmap);

private:
	struct PoolItem;
	QPtrList<BitmapPool::PoolItem> mBitmaps;
	Mutex mMutex;
};

class StereoScope;

struct convolve_state;

class Input
{
public:
	Input();
	~Input();
	
	/**
	 * audio is a pair of pointers to the buffers,
	 * one for each channel
	 *
	 * get the amount stated in the const global
	 * samples
	 **/
	void getAudio(float **audio);

	void setConvolve(bool state);

	bool convolve() { return state; }

private:
	void fht(float *p);
	void transform(float *p, long n, long k);
	void transform8(float *p);
	void connect();
	
private:
	StereoScope *mScope;
	float outleft[samples], outright[samples];
	float haystack[512];
	float temp[samples+256];
	convolve_state *state;
	volatile bool mConvolve;

	float fhtBuf[samples-256];
	float fhtTab[(samples-256)*2];
	
	bool ok;
	QObject *notifier;
};

class OutputSDL
{
public:
	enum Event
		{ None=0, Resize, Exit };
	
	OutputSDL();
	~OutputSDL();
	
	int display(Bitmap *source);
	
private:
	// static for speed, since there can be only one anyway because SDL sucks
	static SDL_Surface *surface;
};

#include <qcheckbox.h>
#include <kcolorbutton.h>

class NexCheckBox : public QCheckBox
{
Q_OBJECT
public:
	NexCheckBox(QWidget *parent, const QString &, bool *v);

private slots:
	void change(bool b);

private:
	bool *value;

};

class NexColorButton : public KColorButton
{
Q_OBJECT
public:
	NexColorButton(QWidget *parent, Pixel *color);

private slots:
	void change(const QColor &c);

private:
	Pixel *c;
};

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	virtual Bitmap *render(float *pcm[4], Bitmap *source) = 0;
	
	virtual QWidget *configure(QWidget*) { return 0; }
	
	virtual void save(QDomElement &) {}
	
	virtual void load(const QDomElement &) {}
};

class QCheckBox;
class QMultiLineEdit;
class RendererList;	

class RendererListConfigurator : public QWidget
{
Q_OBJECT
public:
	RendererListConfigurator(RendererList *l, QWidget *parent);
	~RendererListConfigurator();
	
public slots:
	void eraseOn(bool state);
	void convolve(bool state);

private:
	QCheckBox *mErase;
	QMultiLineEdit *mComments;
	
	RendererList *mList;
};

class RendererList : public Renderer, public Mutex
{
	friend class RendererListConfigurator;

public:
	RendererList();
	virtual ~RendererList();
	virtual Bitmap *render(float *pcm[4], Bitmap *source);
	
	QPtrList<Renderer> &renderers() { return mRendererList; }
	const QPtrList<Renderer> &renderers() const { return mRendererList; }

	bool clearAfter() const { return mClearAfter; }
	void setClearAfter(bool b) { mClearAfter=b; }
	
	virtual QWidget *configure(QWidget *parent);
	
	virtual void save(QDomElement &e);
	
	virtual void load(const QDomElement &e);
	
private:
	QPtrList<Renderer> mRendererList;
	volatile bool mClearAfter;
	QString mComments;
	
	Bitmap *mFrame;
};

typedef Renderer* (CreatorSig)();

class Nex
{
public:
	Nex();
	~Nex();
	
	void go();
	void setupSize(int w, int h);
	
	static Nex *nex() { return sNex; }
	BitmapPool *bitmapPool() { return mBitmapPool; }

	RendererList *rendererList() { return mRendererList; }

	Input *input() { return mInput; }
	
	Renderer *renderer(const QString &name);
	
	QStringList renderers() const;

public:
	static Nex *sNex;
	
private:
	Input *mInput;
	OutputSDL mOutput;
	BitmapPool *mBitmapPool;
	RendererList *mRendererList;
	QDict<CreatorSig*> mCreators;
};

#define nex Nex::nex()


#endif
