#include "nex.h"
#include "gui.h"

#include <unistd.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>

#include "renderers.h"

Mutex runLock;
Nex *Nex::sNex=0;
QTime timer;

Renderer::Renderer() {}
Renderer::~Renderer() {}

Thread::Thread() : mThread(0)
{

}

Thread::~Thread()
{
	if (mThread)
		kill();
}

void Thread::start()
{
	mThread=SDL_CreateThread(&threadRun, (void*)this);
}

void Thread::kill()
{
	SDL_KillThread(mThread);
}

int Thread::wait()
{
	int val;
	SDL_WaitThread(mThread, &val);
	return val;
}

int Thread::threadRun(void *v)
{
	Thread *t=(Thread*)v;
	return t->run();
}

NexCheckBox::NexCheckBox(QWidget *parent,
                         const QString &name, bool *v)
	: QCheckBox(name, parent)
{
	value=v;
	setChecked(*v);
	connect(this, SIGNAL(toggled(bool)), SLOT(change(bool)));
}
void NexCheckBox::change(bool b)
{
	*value=b;
}



NexColorButton::NexColorButton(QWidget *parent, Pixel *color)
	: KColorButton(parent)
{
	c=color;
	QColor temp( (*c >> 16) & 0xFF, (*c >> 8) & 0xFF, *c & 0xFF);
	setColor(temp);
	connect(this, SIGNAL(changed(const QColor&)), SLOT(change(const QColor&)));
}

void NexColorButton::change(const QColor &co)
{
	*c= COLOR(qRed(co.rgb()), qGreen(co.rgb()), qBlue(co.rgb()));
}

void Bitmap::resize(int w, int h)
{
	delete [] mData;
	mData=new Pixel[w*h];
}

void Bitmap::clear()
{
	memset(mData, 0, bytes());
}

void Bitmap::drawCircle(int x, int y, int r, Pixel color)
{
	int16_t cx = 0;
	int16_t cy = r;
	int16_t ocx = -1;
	int16_t ocy = -1;
	int16_t df = 1 - r;
	int16_t d_e = 3;
	int16_t d_se = -2 * r + 5;
	int16_t xpcx, xmcx, xpcy, xmcy;
	int16_t ypcy, ymcy, ypcx, ymcx;

	do
	{ // Draw
		if ((ocy!=cy) || (ocx!=cx))
		{
			xpcx=x+cx;
			xmcx=x-cx;
			if (cy>0)
			{
				ypcy=y+cy;
				ymcy=y-cy;
				setPixel(xmcx,ypcy,color);
				setPixel(xpcx,ypcy,color);
				setPixel(xmcx,ymcy,color);
				setPixel(xpcx,ymcy,color);
			}
			else
			{
				setPixel(xmcx,y,color);
				setPixel(xpcx,y,color);
			}

			ocy=cy;
			xpcy=x+cy;
			xmcy=x-cy;
			if (cx>0)
			{
				ypcx=y+cx;
				ymcx=y-cx;
				setPixel(xmcy,ypcx,color);
				setPixel(xpcy,ypcx,color);
				setPixel(xmcy,ymcx,color);
				setPixel(xpcy,ymcx,color);
			}
			else
			{
				setPixel(xmcy,y,color);
				setPixel(xpcy,y,color);
			}
			ocx=cx;
		}
		// Update
		if (df < 0)
		{
			df += d_e;
			d_e += 2;
			d_se += 2;
		}
		else
		{
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}
		cx++;
	} while(cx <= cy);
}


void Bitmap::fillCircle(int x, int y, int r, Pixel color)
{
	int16_t cx = 0;
	int16_t cy = r;
	int16_t ocx = -1;
	int16_t ocy = -1;
	int16_t df = 1 - r;
	int16_t d_e = 3;
	int16_t d_se = -2 * r + 5;
	int16_t xpcx, xmcx, xpcy, xmcy;
	int16_t ypcy, ymcy, ypcx, ymcx;

	do
	{ // Draw
		if ((ocy!=cy) || (ocx!=cx))
		{
			xpcx=x+cx;
			xmcx=x-cx;
			if (cy>0)
			{
				ypcy=y+cy;
				ymcy=y-cy;
				setPixel(xmcx,ypcy,color);
				setPixel(xpcx,ypcy,color);
				setPixel(xmcx,ymcy,color);
				setPixel(xpcx,ymcy,color);
				for (int h=xmcx; h<xpcx; h++)
					setPixel(h, ypcy, color);

				for (int h=xmcx; h<xpcx; h++)
					setPixel(h, ymcy, color);
			}
			else
			{
				setPixel(xmcx,y,color);
				setPixel(xpcx,y,color);
				for (int h=xmcx; h<xpcx; h++)
					setPixel(h, y, color);
			}


			ocy=cy;
			xpcy=x+cy;
			xmcy=x-cy;
			if (cx>0)
			{
				ypcx=y+cx;
				ymcx=y-cx;
				setPixel(xmcy,ypcx,color);
				setPixel(xpcy,ypcx,color);
				setPixel(xmcy,ymcx,color);
				setPixel(xpcy,ymcx,color);
				for (int h=xmcy; h<xpcy; h++)
					setPixel(h, ypcx, color);

				for (int h=xmcy; h<xpcy; h++)
					setPixel(h, ymcx, color);
			}
			else
			{
				setPixel(xmcy,y,color);
				setPixel(xpcy,y,color);
				for (int h=xmcy; h<xpcy; h++)
					setPixel(h, y, color);
			}
			ocx=cx;
		}
		// Update
		if (df < 0)
		{
			df += d_e;
			d_e += 2;
			d_se += 2;
		}
		else
		{
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}
		cx++;
	} while(cx <= cy);
}


void Bitmap::drawLine(int x1, int y1, int x2, int y2, Pixel color)
{
	// Variable setup
	int dx = x2 - x1;
	int sx = (dx >= 0) ? 1 : -1;
	dx = sx * dx + 1;

	int dy = y2 - y1;
	int sy = (dy >= 0) ? 1 : -1;
	dy = sy * dy + 1;


	int pixx = sizeof(Pixel);
	int pixy = width*sizeof(Pixel);
	uint8_t *pixel = (uint8_t*)pixels() + pixx * x1 + pixy * y1;
	pixx *= sx;
	pixy *= sy;

	if (dx < dy)
	{
		int swaptmp = dx;
		dx = dy;
		dy = swaptmp;
		swaptmp = pixx;
		pixx = pixy;
		pixy = swaptmp;
	}


	// Draw
	int y=0;

	for(int x=0; x < dx; x++, pixel += pixx)
	{
		*(Pixel*)pixel=color;
		y += dy;
		if (y >= dx)
		{
			y -= dx;
			pixel += pixy;
		}
	}

}

RendererList::RendererList() : mClearAfter(false)
{
	mFrame=nex->bitmapPool()->get(true);
}

RendererList::~RendererList()
{
	nex->bitmapPool()->release(mFrame);
}

Bitmap *RendererList::render(float *pcm[4], Bitmap *source)
{
	if (mClearAfter) mFrame->clear();

	lock();
	for (QPtrListIterator<Renderer> i(mRendererList); i.current(); ++i)
	{
		Bitmap *newframe=(*i)->render(pcm, mFrame);

		if (newframe!=mFrame)
		{
			nex->bitmapPool()->release(mFrame);
			mFrame=newframe;
		}
	}

	unlock();

	// add source+=source; return source;

	uint8_t *d=(uint8_t*)source->pixels();
	uint8_t *end=(uint8_t*)((uint8_t*)d+source->bytes());
	uint8_t *s=(uint8_t*)mFrame->pixels();

	while (d<end)
	{
		register int dest=*d;
		if (dest && ((dest | *s) & 128))
		{
			// there's danger of going past 0xFF
			dest+=*s;
			if (dest & 256)
				*d=0xFF; // oops, we did!
			else
				*d=dest;
		}
		else
		{
			// if neither touch the 128th bit, then the sum
			// can't possibly be more than 0xFF
			*d=dest+*s;
		}


		++s;
		++d;
	}

	return source;
}

void RendererList::save(QDomElement &e)
{
	lock();
	e.setTagName("List");

	for (QPtrListIterator<Renderer> i(mRendererList); *i; ++i)
	{
		QDomElement item;
		(*i)->save(item);
		e.appendChild(item);
	}

	unlock();
}

void RendererList::load(const QDomElement &e)
{
	lock();

	for (QDomNode n=e.firstChild(); !n.isNull(); n=n.nextSibling())
	{
		if (!n.isElement()) continue;
		QDomElement child=n.toElement();

		Renderer *r=0;

		if (e.tagName()=="List")
			r=new RendererList;
		else
			r=nex->renderer(e.tagName());

		if (!r) continue;

		r->load(child);
		mRendererList.append(r);
	}

	unlock();
}



QWidget *RendererList::configure(QWidget *parent)
{
	return new RendererListConfigurator(this, parent);
}

RendererListConfigurator::RendererListConfigurator(RendererList *l, QWidget *parent)
	: QWidget(parent), mList(l)
{
	(new QVBoxLayout(this))->setAutoAdd(true);
	mErase=new QCheckBox(i18n("&Erase between frames"), this);
	connect(mErase, SIGNAL(toggled(bool)), SLOT(eraseOn(bool)));
	mErase->setChecked(mList->mClearAfter);

	if (nex->rendererList()==l)
	{
		QCheckBox *mConvolve=new QCheckBox(i18n("&Convolve audio"), this);
		connect(mConvolve, SIGNAL(toggled(bool)), SLOT(convolve(bool)));
		mConvolve->setChecked(nex->input()->convolve());
	}

	new QLabel(i18n("Comments"), this);

	mComments=new QMultiLineEdit(this);
	mComments->setText(l->mComments);
	mComments->setWordWrap(QMultiLineEdit::WidgetWidth);
}

RendererListConfigurator::~RendererListConfigurator()
{
	mList->mComments=mComments->text();
}

void RendererListConfigurator::eraseOn(bool state)
{
	mList->mClearAfter=state;
}

void RendererListConfigurator::convolve(bool state)
{
	nex->input()->setConvolve(state);
}



#define INSERT(name, func) mCreators.insert(name, new CreatorSig*(&func))

Nex::Nex()
{
	sNex=this;
	mBitmapPool=0;
	mRendererList=0;

	setupSize(width, height);

	INSERT("Fade", Creators::fade);
	INSERT("Doubler", Creators::doubler);
	INSERT("Waveform", Creators::waveform);
	INSERT("Hartley", Creators::hartley);
}
#undef INSERT

void Nex::setupSize(int , int )
{
	mInput=new Input;
	delete mBitmapPool;
	delete mRendererList;
	mBitmapPool=new BitmapPool();
	mRendererList=new RendererList;
}

Nex::~Nex()
{
	delete mRendererList;
	delete mBitmapPool;
}

#define NOTHREAD

void Nex::go()
{
	runLock.unlock();
	float *audio[6];

	Bitmap *frame;
	frame=mBitmapPool->get(true);

	int frames=0;
	QTime start(QTime::currentTime());

	while (1)
	{
		mInput->getAudio(audio);
		mRendererList->render(audio, frame);
		int result=mOutput.display(frame);

		frames++;

		switch (result)
		{
		case OutputSDL::Exit:
			std::cerr << "Trying" << std::endl;
			delete mInput;
			std::cerr << "Deleted" << std::endl;

			std::cout << "Frames per Second: "
			     << frames/start.secsTo(QTime::currentTime()) << std::endl;
			return;
		case OutputSDL::Resize:
//			setupSize(width, height);
			break;
		}
#ifdef NOTHREAD
		kapp->processEvents();
#endif
		frame->clear();
	}
}


Renderer *Nex::renderer(const QString &name)
{
	CreatorSig **sig=mCreators[name];
	if (sig)
		return (**sig)();
	else
		return 0;
}

QStringList Nex::renderers() const
{
	QDictIterator<CreatorSig*> i(mCreators);
	QStringList list;

	for (;i.current(); ++i)
		list += i.currentKey();

	return list;
}


#ifndef NOTHREAD
class VisThread : public Thread
{
public:
	virtual int run()
	{
		Nex::sNex->go();
		exit(0);
		return 0;
	}
};
#endif

int main(int argc, char **argv)
{
	Nex theNex;
	Nex::sNex=&theNex;

#ifndef NOTHREAD
	runLock.lock();

	VisThread vis;
	vis.start();

	runLock.lock();
#endif

	KAboutData aboutData("nex", I18N_NOOP("Nex"), "0.0.1",
		I18N_NOOP("The awesome customizable scope"),
		KAboutData::License_LGPL, "(C) 2001 Charles Samuels", 0,
		"http://noatun.kde.org");

	aboutData.addAuthor("Charles Samuels", I18N_NOOP("Nex Author"),
	                    "charles@kde.org");

	KCmdLineArgs::init( argc, argv, &aboutData );
	KApplication app;


	(new Control)->show();
#ifdef NOTHREAD
	theNex.go();

#else
	app.exec();

	vis.wait();
#endif
	exit(0); //prevent segfault on exit, for some reason
	return 0;
}

#include "nex.moc"

