#include <noatun/engine.h>
#include <noatun/player.h>
#include <noatun/app.h>

#include <math.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <klocale.h>
#include <kconfig.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <kwin.h>


#include "ffrs.h"

extern "C" Plugin *create_plugin()
{
	KGlobal::locale()->insertCatalogue("ffrs");
	return new FFRS();
}


View::View(int width, int height, int block, int unblock, QColor front, QColor back, int channel)
	: QWidget(0,0, Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_StaysOnTop | Qt::WType_TopLevel), mChannel(channel)
{
	fg = front;
	bg = back;
	resize(width, height);
	setPaletteBackgroundColor(back);
	KWin::setState(handle(), NET::SkipTaskbar);

	KConfig *c = napp->config();
	c->setGroup("FFRS");

	QSize size = napp->desktop()->size();
	QRect desktop(0,0, size.width(), size.height());

	QPoint at;
	if (channel==0)
	{
		at = QPoint(size.width()-width*4, size.height()-height-32);
		QToolTip::add(this, i18n("Left"));
	}
	else // if (channel==1)
	{
		at = QPoint(size.width()-width*2, size.height()-height-32);
		QToolTip::add(this, i18n("Right"));
	}

	move(c->readPointEntry("at"+QString::number(mChannel), &at));

	// make sure we're on the desktop
	if (
			!desktop.contains(rect().topLeft())
			 || !desktop.contains(rect().bottomRight())
		)
	{
		move(at);
	}


	QBitmap mask(width, height);
	QPainter p(&mask);

//	Qt::color0 = transparent
//	Qt::color1 = opaque
	p.fillRect(0, 0, width, height, Qt::color0);
	for (int i=0; i < height; )
	{
		p.fillRect(0, height-i-block, width, block, Qt::color1);
		i += block + unblock;
	}
	setMask(mask);
	units = block+unblock;
	show();

	moving=false;
}

View::~View()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	c->writeEntry("at"+QString::number(mChannel), pos());
}

void View::mouseMoveEvent(QMouseEvent *)
{
	if (moving)
	{
		move ( QCursor::pos()-mMousePoint );
	}
}

void View::mousePressEvent(QMouseEvent *)
{
	moving = true;
	mMousePoint = mapFromGlobal(QCursor::pos());
}

void View::mouseReleaseEvent(QMouseEvent *)
{
	moving = false;
}

void View::draw(float level)
{
	int w = width();
	int h = height();

	// now convert level to pixels

	static const float LEVEL_MIN = 1.0/(1<<20);
	if (level < LEVEL_MIN) level = LEVEL_MIN;
	level = (2.0/log(2.0))*log(level+1.0);

	float fpix = level * (float)h;
	fpix  = fabs(fpix);
	if (fpix - (int)fpix > .5) fpix += .5;

	int pix = (int)(fpix / units) * units;

	// and draw it (it updates too quickly for it to
	// need a paintEvent)
	QPainter p(this);
	p.fillRect(0, 0, w, h-pix, bg);
	p.fillRect(0, h-pix, w, h - (h-pix), fg);
}



FFRS::FFRS() : QObject(), Plugin()
{
	dpyleft = dpyright = 0;
	changed();
	prefs = new FFRSPrefs(this);
	connect(prefs, SIGNAL(changed()), SLOT(changed()));

	setSamples(256);

	start();
}

FFRS::~FFRS()
{
	delete dpyleft;
	delete dpyright;
}

void FFRS::scopeEvent(float *left, float *right, int len)
{
	float avl=0;
	float avr=0;

	for (int i=0; i < len; i++)
	{
		avl += fabs(left[i]);
		avr += fabs(right[i]);
	}
	avl /= len;
	avr /= len;

	dpyleft->draw(avl);
	if (dpyright)
		dpyright->draw(avr);
}

void FFRS::changed()
{
	delete dpyleft;
	delete dpyright;

	dpyleft = new View(prefs->width(), prefs->height(), prefs->fgblock(), prefs->bgblock(), prefs->fgcolor(), prefs->bgcolor(), 0);
	dpyright = new View(prefs->width(), prefs->height(), prefs->fgblock(), prefs->bgblock(), prefs->fgcolor(), prefs->bgcolor(), 1);

	setInterval(prefs->rate());
}


#include <knuminput.h>
#include <kcolorbutton.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>

FFRSPrefs::FFRSPrefs( QObject *parent )
	: CModule(i18n("Foreign Region"), i18n("French Foreign Region"),"",parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QHBox *box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Width:"), box);
	mWidth = new KIntNumInput(width(), box);
	mWidth->setMinValue(0);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Height:"), box);
	mHeight = new KIntNumInput(height(), box);
	mHeight->setMinValue(0);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Visible block size:"), box);
	mFgblock = new KIntNumInput(fgblock(), box);
	mFgblock->setMinValue(0);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Transparent block size:"), box);
	mBgblock = new KIntNumInput(bgblock(), box);
	mBgblock->setMinValue(0);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Update interval:"), box);
	mRate = new KIntNumInput(rate(), box);
	mRate->setMinValue(0);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Foreground color:"), box);
	mFgcolor = new KColorButton(fgcolor(), box);

	box = new QHBox(this);
	layout->addWidget(box);
	new QLabel(i18n("Background color:"), box);
	mBgcolor = new KColorButton(bgcolor(), box);

	layout->addStretch();
}

void FFRSPrefs::save()
{
	KConfig *c = napp->config();

	c->setGroup("FFRS");
	c->writeEntry("width", mWidth->value());
	c->writeEntry("height", mHeight->value());
	c->writeEntry("fgblock", mFgblock->value());
	c->writeEntry("bgblock", mBgblock->value());
	c->writeEntry("rate", mRate->value());

	c->writeEntry("bgcolor", mBgcolor->color());
	c->writeEntry("fgcolor", mFgcolor->color());

	emit changed();
}

int FFRSPrefs::width()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	return c->readNumEntry("width", 22);
}

int FFRSPrefs::height()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	return c->readNumEntry("height", 162);
}

int FFRSPrefs::fgblock()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	return c->readNumEntry("fgblock", 27-12);
}

int FFRSPrefs::bgblock()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	return c->readNumEntry("bgblock", 12);
}

int FFRSPrefs::rate()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	return c->readNumEntry("rate", 110);
}


QColor FFRSPrefs::bgcolor()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	QColor dumbass(0, 64, 0);
	return c->readColorEntry("bgcolor", &dumbass);
}

QColor FFRSPrefs::fgcolor()
{
	KConfig *c = napp->config();
	c->setGroup("FFRS");
	QColor dumbass(0, 255, 0);
	return c->readColorEntry("fgcolor", &dumbass);
}

#include "ffrs.moc"

