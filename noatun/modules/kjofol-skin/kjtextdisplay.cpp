/***************************************************************************
	kjtexdisplay.cpp
	---------------------------------------------
	Displays for time and other things
	using fonts provided by KJFont
	---------------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "kjtextdisplay.h"
#include "kjfont.h"
#include "kjprefs.h"

// kde includes
#include <klocale.h>
#include <kdebug.h>
#include <kpixmap.h>
#include <kurl.h>
#include <krun.h>
#include <kmimemagic.h>

// arts-includes, needed for pitch
#include <artsmodules.h>
#include <arts/reference.h>
#include <arts/soundserver.h>
#include <arts/kmedia2.h>

// noatun includes
#include <noatun/player.h>
#include <noatun/engine.h>

/*******************************************
 * KJFilename
 *******************************************/

KJFilename::KJFilename(const QStringList &l, KJLoader *p)
	: QObject(0), KJWidget(p), mBack(0)
{
	int x  = l[1].toInt();
	int y  = l[2].toInt();
	int xs = l[3].toInt() - x;
	int ys = l[4].toInt() - y;

	// fix for all those weird skins where the filenamewindow has more
	// height than needed for the font
	// ( ... usually resulting in garbage on-screen )
	if ( ys > (textFont().fontHeight()) )
		ys = textFont().fontHeight();

	// background under filename-scroller
	QPixmap tmp  = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	setRect(x,y,xs,ys);

	// how far it moves per cycle
	// TODO: make that configurable for the user

	//mDistance = 1;
//	mDistance = (int)(textFont().fontWidth()/2);
	readConfig();

	prepareString(i18n("Welcome to Noatun").local8Bit());
	killTimers();
}

KJFilename::~KJFilename()
{
	delete mBack;
}

void KJFilename::paint(QPainter *p, const QRect &)
{
	QPixmap temp ( rect().width(), rect().height() );

	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw font into buffer
	bitBlt( &temp, 0, 0, &mView, 0, 0, rect().width(), rect().height(), Qt::CopyROP);
	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp,
	       QRect(0,0,-1,-1), Qt::CopyROP);
}

void KJFilename::timerEvent(QTimerEvent *)
{
	int height = mView.height();
	int width = mView.width();

	QBitmap cycleMask ( mDistance, height ); // temporary-space for moving parts of the mask
	QPixmap cycle ( mDistance, height );	// temporary-space for moving parts of the pixmap
	QBitmap newMask ( *mView.mask() );		// save old mask

	// copy mask like the same way we're doing it with the pixmap
	// a mask does not get copied on a bitblt automatically, we have to do
	// it "by hand"
	bitBlt(&cycleMask, 0,0, &newMask, 0,0, mDistance, height, Qt::CopyROP);
	bitBlt(&newMask, 0,0, &newMask, mDistance, 0, width-mDistance, height, Qt::CopyROP);
	bitBlt(&newMask, width-mDistance, 0, &cycleMask, 0,0, mDistance, height, Qt::CopyROP);

	bitBlt(&cycle, 0,0, &mView, 0,0, mDistance, height, Qt::CopyROP);
	bitBlt(&mView, 0,0, &mView, mDistance, 0, width-mDistance, height, Qt::CopyROP);
	bitBlt(&mView, width-mDistance, 0, &cycle, 0,0, mDistance, height, Qt::CopyROP);

	// apply the newly created mask
	mView.setMask(newMask);

	repaint();
}

bool KJFilename::mousePress(const QPoint &)
{
	return true;
}

void KJFilename::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;

	if ( !napp->player()->current() )
		return;

	KURL dirURL = napp->player()->current().url().upURL();

	KMimeMagicResult *result = KMimeMagic::self()->findFileType( dirURL.path() );

	// TODO: Maybe test for protocol type?
//	if ( napp->player()->current().url().protocol() == "file" )
	if ( result->isValid() )
		KRun::runURL ( dirURL, result->mimeType() );
}

void KJFilename::readConfig()
{
	kdDebug(66666) << "KJFilename::readConfig()" << endl;
	mDistance = (int)( textFont().fontWidth() * KJLoader::kjofol->prefs()->titleMovingDistance() );
	if ( mDistance <= 0 )
		mDistance = 1;
	mTimerUpdates = KJLoader::kjofol->prefs()->titleMovingUpdates();
	textFont().recalcSysFont();
	mLastTitle=""; // invalidate title so it gets repainted on next timeUpdate()
}

void KJFilename::prepareString(const QCString &str)
{
	killTimers(); // i.e. stop timers

	mView = textFont().draw(str, rect().width());

	startTimer(mTimerUpdates);
}

void KJFilename::timeUpdate(int)
{
	if ( !napp->player()->current() ) // just for safety
		return;

	QCString title = QCString( napp->player()->current().title().local8Bit() );

	if ( title == mLastTitle )
		return;

	mLastTitle = title;

	QCString timestring = napp->player()->lengthString().local8Bit();
	timestring = timestring.mid(timestring.find('/')+1);
	prepareString ( title + " (" + timestring + ")   ");
}

QString KJFilename::tip()
{
	if ( !napp->player()->current() ) // just for safety
		return i18n("Filename");
	else
		return napp->player()->current().url().prettyURL();
}


/*******************************************
 * KJTime
 *******************************************/

KJTime::KJTime(const QStringList &l, KJLoader *p)
	: KJWidget(p), mBack(0)
{
	int x  = l[1].toInt();
	int y  = l[2].toInt();
	int xs = l[3].toInt() - x;
	int ys = l[4].toInt() - y;

	// fix for all those weird skins where the timewindow
	// has more space than needed for the font
	int maxNeededHeight = timeFont().fontHeight();
	if ( ys > maxNeededHeight )
		ys = maxNeededHeight;

	// five digits + spacing between them
	int maxNeededWidth = ( 5 *timeFont().fontWidth() ) + ( 4 * timeFont().fontSpacing() );
	if ( xs > maxNeededWidth )
		xs = maxNeededWidth;

	// background under time-display
	QPixmap tmp  = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	setRect(x,y,xs,ys);

	readConfig();

	prepareString("00:00");
}

KJTime::~KJTime()
{
	delete mBack;
}

void KJTime::paint(QPainter *p, const QRect &)
{
//	kdDebug(66666) << "KJTime::paint(QPainter *p, const QRect &)" << endl;
	QPixmap temp ( rect().width(), rect().height() );

	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw time-display into buffer (that's a pixmap with a mask applied)
	bitBlt( &temp, 0, 0, &mTime, 0, 0, rect().width(), rect().height(), Qt::CopyROP);

	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp,
	       QRect(0,0, rect().width(), rect().height()), Qt::CopyROP);
}

bool KJTime::mousePress(const QPoint &)
{
	return true;
}

void KJTime::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;

	countDown = !countDown;
	napp->setDisplayRemaining( countDown );
//	KJLoader::kjofol->prefs()->setTimeCountMode( countDown );
}

void KJTime::readConfig()
{
//	kdDebug(66666) << "KJTime::readConfig()" << endl;
	countDown = napp->displayRemaining();
	timeFont().recalcSysFont();
	mLastTime=""; // invalidate time so it gets repainted on next timeUpdate()
}

QString KJTime::lengthString ( void )
{
	int pos = 0;
	QString posString;
	int secs = 0,
	seconds = 0,
	minutes = 0,
	hours = 0;

	if ( countDown )
	{	// current remaining time
		pos = napp->player()->getLength() - napp->player()->getTime();
	}
	else
	{	// current time
		pos = napp->player()->getTime();
	}

	if ( pos < 0 )
	{
		posString = "00:00";
	}
	else
	{ // get the position
		secs = pos / 1000; // convert milliseconds -> seconds

		seconds = secs % 60;
		minutes = (secs - seconds) / 60;
		hours = minutes / 60;
		minutes %= 60; // remove the hours from minutes ;)

//		cerr << "  " << hours << ":" << minutes << ":" << seconds << endl;

//		if ( hours > 0 ) // looks ugly :)
		if ( (napp->player()->getLength()/1000) >= 3600 ) // displays hh:mm if file is long
		{
			posString.sprintf("%d:%.2d", hours, minutes);
		}
		else // displays mm:ss
		{
			posString.sprintf("%.2d:%.2d", minutes, seconds);
		}
	}

	return posString;
}

void KJTime::timeUpdate(int)
{
//	kdDebug(66666) << "START KJTime::timeUpdate(int)" << endl;
	if (!napp->player()->current())
		return;

	prepareString( (lengthString()).latin1() );

//	kdDebug(66666) << "END KJTime::timeUpdate(int)" << endl;
}

void KJTime::prepareString(const QCString &str)
{
//	kdDebug(66666) << "START KJTime::prepareString(const QCString &str)" << endl;
	if ( str == mLastTime )
		return;

	mLastTime = str;
	mTime = timeFont().draw(str, rect().width());

	repaint();
//	kdDebug(66666) << "END KJTime::prepareString(const QCString &str)" << endl;
}

QString KJTime::tip()
{
	if ( countDown )
		return i18n("Play time left");
	else
		return i18n("Current play time");
}


/*******************************************
 * KJVolumeText
 *******************************************/

KJVolumeText::KJVolumeText(const QStringList &l, KJLoader *p)
	: KJWidget(p), mBack(0)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;

	// fix for all those weird skins where the timewindow has more space than needed for the font
	if ( ys > (volumeFont().fontHeight()) )
		ys = volumeFont().fontHeight();

	// 3 digits for volume (1-100)
	// + spaces according to spacing
	// + percentage letter (seems to be 1px wider than a normal char)
	int tempWidth = (3*volumeFont().fontWidth()) + (2*volumeFont().fontSpacing()) + ((volumeFont().fontWidth()+1));
	if ( xs > ( tempWidth ) )
		xs = tempWidth;

	// background under volumetext-display
	QPixmap tmp  = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	setRect(x,y,xs,ys);

	prepareString("100%");
}

KJVolumeText::~KJVolumeText()
{
	delete mBack;
}

void KJVolumeText::paint(QPainter *p, const QRect &)
{
	QPixmap temp ( rect().width(), rect().height() );

	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw time-display into buffer
	bitBlt( &temp, 0, 0, &mVolume, 0, 0, rect().width(), rect().height(), Qt::CopyROP);

	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp,
	       QRect(0,0,-1,-1), Qt::CopyROP);
}

bool KJVolumeText::mousePress(const QPoint &)
{
	return false;
}

void KJVolumeText::readConfig()
{
	volumeFont().recalcSysFont();
	mLastVolume=""; // invalidate value so it gets repainted on next timeUpdate()
}

void KJVolumeText::timeUpdate(int)
{
	QCString volume;

	if (!napp->player()->current())
		return;

	volume.sprintf("%d%%", napp->player()->volume() ); // FIXME: is sprintf safe to use?

	prepareString(volume);
}

void KJVolumeText::prepareString(const QCString &str)
{
	if ( str == mLastVolume )
		return;

	mLastVolume = str;
	mVolume = volumeFont().draw(str, rect().width());

	repaint();
}

QString KJVolumeText::tip()
{
	return i18n("Volume");
}


/*******************************************
 * KJPitchText
 *******************************************/

KJPitchText::KJPitchText(const QStringList &l, KJLoader *p)
	: KJWidget(p), mBack(0)
{
	int x  = l[1].toInt();
	int y  = l[2].toInt();
	int xs = l[3].toInt() - x;
	int ys = l[4].toInt() - y;

	// fix for all those weird skins where the timewindow has more space than needed for the font
	if ( ys > (pitchFont().fontHeight()) )
		ys = pitchFont().fontHeight();

	// 3 digits for volume (1-100), spaces according to spacing and percentage letter
	int tempWidth = (3*pitchFont().fontWidth()) + (2*pitchFont().fontSpacing());
	if ( xs > tempWidth )
		xs = tempWidth;

	// background under time-display
	QPixmap tmp  = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	setRect(x,y,xs,ys);

	prepareString("100");
}

KJPitchText::~KJPitchText()
{
	delete mBack;
}


void KJPitchText::paint(QPainter *p, const QRect &)
{
	QPixmap temp ( rect().width(), rect().height() );

	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw time-display into buffer
	bitBlt( &temp, 0, 0, &mSpeed, 0, 0, rect().width(), rect().height(), Qt::CopyROP);
	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp, QRect(0,0,-1,-1), Qt::CopyROP);
}

bool KJPitchText::mousePress(const QPoint &)
{
	return true;
}

void KJPitchText::mouseRelease(const QPoint &, bool in)
{
	if (!in)
		return;

	Arts::PlayObject playobject = napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if (pitchable.isNull())
		return;

	pitchable.speed( 1.00f ); // reset pitch
}

void KJPitchText::readConfig()
{
	pitchFont().recalcSysFont();
	mLastPitch=""; // invalidate value so it gets repainted on next timeUpdate()
}

void KJPitchText::timeUpdate(int)
{
	QCString speed;

	if (!napp->player()->current())
		return;

	Arts::PlayObject playobject = napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if (pitchable.isNull())
		return;

	speed.setNum ( (int) ((float)pitchable.speed()*(float)100) );
	prepareString ( speed );
}

void KJPitchText::prepareString(const QCString &str)
{
	if ( str == mLastPitch )
		return;

	mLastPitch = str;
	mSpeed = pitchFont().draw(str, rect().width());

	repaint();
}

QString KJPitchText::tip()
{
	return i18n("Pitch");
}


/*******************************************
 * KJFileInfo
 *******************************************/

KJFileInfo::KJFileInfo(const QStringList &l, KJLoader *p)
	: KJWidget(p), mBack(0)
{
	mInfoType = l[0]; // type of info-display

	int x  = l[1].toInt();
	int y  = l[2].toInt();
	int xs = l[3].toInt() - x;
	int ys = l[4].toInt() - y;

	// fix for all those weird skins where the timewindow
	// has more space than needed for the font
	int maxNeededHeight = timeFont().fontHeight();
	if ( ys > maxNeededHeight )
		ys = maxNeededHeight;

	// five digits + spacing between them
	int maxNeededWidth = ( 3 *timeFont().fontWidth() ) + ( 2 * timeFont().fontSpacing() );
	if ( xs > maxNeededWidth )
		xs = maxNeededWidth;

	// background under info-display
	QPixmap tmp  = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	setRect(x,y,xs,ys);

	prepareString("");
}

KJFileInfo::~KJFileInfo()
{
	delete mBack;
}

void KJFileInfo::paint(QPainter *p, const QRect &)
{
	QPixmap temp ( rect().width(), rect().height() );

	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw time-display into buffer (that's a pixmap with a mask applied)
	bitBlt( &temp, 0, 0, &mTime, 0, 0, rect().width(), rect().height(), Qt::CopyROP);

	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp,
	       QRect(0,0, rect().width(), rect().height()), Qt::CopyROP);
}

bool KJFileInfo::mousePress(const QPoint &)
{
	return false;
}

void KJFileInfo::readConfig()
{
	textFont().recalcSysFont();
	mLastTime=""; // invalidate value so it gets repainted on next timeUpdate()
}

void KJFileInfo::timeUpdate(int)
{
	if (!napp->player()->current())
		return;

	const PlaylistItem &item = napp->player()->current();
	QString prop;

	if ( mInfoType == "mp3khzwindow" )
	{
		prop = item.property("samplerate");
		prop.truncate(2); // we just want 44 instead of 44100
	}
	else if ( mInfoType == "mp3kbpswindow" )
	{
		prop = item.property("bitrate");
	}
	else	// for safety: no infoType we know of
		return;

	if (prop.isNull())
		prop="";
	prepareString( prop.latin1() );
}

void KJFileInfo::prepareString(const QCString &str)
{
	if ( str == mLastTime )
		return;
	mLastTime = str;
	mTime = textFont().draw(str, rect().width());
	repaint();
}

QString KJFileInfo::tip()
{
	if ( mInfoType == "mp3khzwindow" )
		return i18n("Sample rate in kHz");
	else if ( mInfoType == "mp3kbpswindow" )
		return i18n("Bitrate in kbps");

	return QString();
}

#include "kjtextdisplay.moc"
