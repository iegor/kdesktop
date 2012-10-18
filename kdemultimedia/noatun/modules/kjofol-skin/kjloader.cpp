/***************************************************************************
	kjloader.cpp  -  The KJöfol-GUI itself
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "kjloader.h"
#include "kjloader.moc"
#include "kjwidget.h"
#include "kjbackground.h"
#include "kjbutton.h"
#include "kjfont.h"
#include "kjseeker.h"
#include "kjsliders.h"
#include "kjtextdisplay.h"
#include "kjvis.h"
#include "kjprefs.h"
#include "kjequalizer.h"

#include "helpers.cpp"

// arts-includes, needed for pitch
#include <artsmodules.h>
#include <reference.h>
#include <soundserver.h>
#include <kmedia2.h>

// noatun-specific includes
#include <noatun/engine.h>
#include <noatunarts/noatunarts.h>
#include <noatun/stdaction.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/vequalizer.h>

// system includes
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <qdragobject.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qptrvector.h>
#include <qvbox.h>
#include <qlabel.h>

#include <kaction.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <khelpmenu.h>
#include <kstdaction.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <knotifyclient.h>
#include <kpixmapeffect.h>
#include <kurldrag.h>

#include <kwin.h>
#include <kiconloader.h>

class KJToolTip : public QToolTip
{
public:
	KJToolTip(KJLoader *parent)
		: QToolTip(parent), mParent(parent)
	{}

protected:
	virtual void maybeTip(const QPoint &p)
	{
		if ( !mParent->prefs()->displayTooltips() )
			return;

		QPtrList<KJWidget> things=mParent->widgetsAt(p);
		for (KJWidget *i=things.first(); i!=0; i=things.next())
		{
			QString string=i->tip();
			if (string.length())
			{
				tip(i->rect(), string);
				return;
			}
		}
	}

private:
	KJLoader *mParent;
};


KJLoader *KJLoader::kjofol=0;


KJLoader::KJLoader()
	: QWidget(0, "NoatunKJLoader",
		WType_TopLevel | WStyle_NoBorder | WRepaintNoErase ),
		UserInterface(),
		moving(false),
		mClickedIn(0),
		mText(0),
		mNumbers(0),
		mVolumeFont(0),
		mPitchFont(0),
		splashScreen(0)
{
	kjofol = this;

	mTooltips = new KJToolTip(this);

	// Windowname and Icon
	setCaption(i18n("Noatun"));
	setIcon(SmallIcon("noatun"));
	setAcceptDrops(true);

	// We're going to draw over everything, there is no point in drawing the grey background first
	setBackgroundMode(NoBackground);

	// used for dockmode
	mWin = new KWinModule();

	subwidgets.setAutoDelete(true);

	mPrefs = new KJPrefs(this);
	connect ( mPrefs, SIGNAL(configChanged()), this, SLOT(readConfig()) );

	QString skin = mPrefs->skin();
	if ( QFile(skin).exists() )
	{
		loadSkin(skin);
	}
	else
	{
		KNotifyClient::event(winId(), "warning",
			i18n("There was trouble loading skin %1. Please select another skin file.").arg(skin));
		napp->preferences();
	}

	mHelpMenu = new KHelpMenu(this, kapp->aboutData());
	connect(napp->player(), SIGNAL(timeout()), SLOT(timeUpdate()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(timeUpdate()));
	connect(napp->player(), SIGNAL(newSong()), SLOT(newSong()));

	connect(napp, SIGNAL(hideYourself()), SLOT(hide()));
	connect(napp, SIGNAL(showYourself()), SLOT(show()));
//	KStdAction::quit(napp, SLOT(quit()), actionCollection());

	QApplication::restoreOverrideCursor();
//	newSong();
}

QPtrList<KJWidget> KJLoader::widgetsAt(const QPoint &pt) const
{
	QPtrList<KJWidget> things;
	for ( QPtrListIterator<KJWidget> i(subwidgets); i.current(); ++i )
		if ( (*i)->rect().contains(pt) )
			things.append((*i));

	return things;
}

void KJLoader::removeChild(KJWidget *c)
{
	if ( mClickedIn == c )
		mClickedIn = 0;
	if (subwidgets.findRef(c) != -1)
		subwidgets.take();
}

void KJLoader::addChild(KJWidget *c)
{
	subwidgets.append(c);
}

// The BIG one ;)
// this methode does all the hard work on loading these weird skins
void KJLoader::loadSkin(const QString &file)
{
//	kdDebug(66666) << "<KJLoader::loadSkin(const QString &file)>" << endl;
//	kdDebug(66666) << " file = " << file.latin1() << endl;

	if ( file == mCurrentSkin ) // we don't load the same skin again
		return;

	mCurrentSkin = file;

	// don't overwrite path to *.rc when we are loading dock- or winshade-mode
	if ( (file != mCurrentWinshadeModeSkin) && (file != mCurrentDockModeSkin) )
	{
		mCurrentDefaultSkin = file;
//		kdDebug(66666) << " setting mCurrentDefaultSkin: '" << file.latin1() << "'" << endl;
	}
	unloadSkin();

	Parser::open( filenameNoCase(file) );

	KJPitchText *pitchText=0;
	KJVolumeText *volumeText=0;
	mText = 0;
	mNumbers = 0;
	mVolumeFont = 0;
	mPitchFont = 0;

	if ( exist("splashscreen") && mPrefs->displaySplash() )
		showSplash();

	if ( (file != mCurrentWinshadeModeSkin) && (file != mCurrentDockModeSkin) )
	{
		if ( exist("dockmodercfile") )
		{
			// set path to dockmode rc-file (its not always skinname.dck)
			mCurrentDockModeSkin = file.left(file.findRev("/")+1) + (item("dockmodercfile")[1]);
			mDockPosition  = item("dockmodeposition")[1].toInt();
			mDockPositionX = item("dockmodepositionxy")[1].toInt();
			mDockPositionY = item("dockmodepositionxy")[2].toInt();
		}
		else	// NO DockMode
			mCurrentDockModeSkin="";

		if ( exist("winshademodercfile") )
			mCurrentWinshadeModeSkin = file.left(file.findRev("/")+1) + (item("winshademodercfile")[1]);
		else	// no WinshadeMode
			mCurrentWinshadeModeSkin="";
	}

	// Font-loading
	if ( exist("fontimage") )
		mText = new KJFont("font", this);

	if ( exist("timefontimage") )
		mNumbers = new KJFont("timefont", this);

	if (exist("volumefontimage"))
		mVolumeFont = new KJFont("volumefont", this);

	// our skin-background, There has to be one so no check with exist()
	subwidgets.append( new KJBackground(this) );

	if ( exist("pitchtext") )
	{
		if (exist("pitchfontimage"))
		{
			mPitchFont = new KJFont("pitchfont", this);
		}
		else
		{
			mPitchFont = mNumbers;
			kdDebug(66666) << "no pitchfont but pitchtext!" << endl;
			kdDebug(66666) << "replacing pitchfont with timefont" << endl;
		}
		subwidgets.append( pitchText=new KJPitchText(item("pitchtext"), this) );
	}

	if (exist("volumetext"))
		subwidgets.append(volumeText=new KJVolumeText(item("volumetext"), this));

	if ( exist("volumecontroltype") )
	{
		if ( item("volumecontroltype")[1] == "bmp" )
		{
			KJVolumeBMP *b;
			subwidgets.append(b=new KJVolumeBMP(item("volumecontrolbutton"), this));
			b->setText(volumeText);
		}
		else if ( item("volumecontroltype")[1] == "bar" )
		{
			KJVolumeBar *b;
			subwidgets.append(b=new KJVolumeBar(item("volumecontrolbutton"), this));
			b->setText(volumeText);
		}
/*		else
		{
			kdDebug(66666) << "unknown volumecontrol: " << item("volumecontroltype")[1].latin1() << endl;
		} */
	}
	else
	{
		kdDebug(66666) << "guessing type of volumecontrol" << endl;
		if (exist("volumecontrolbutton") &&
			exist("volumecontrolimage") &&
			exist("volumecontrolimagexsize") &&
			exist("volumecontrolimageposition") &&
			exist("volumecontrolimagenb") )
		{
			KJVolumeBMP *b;
			subwidgets.append(b=new KJVolumeBMP(item("volumecontrolbutton"), this));
			b->setText(volumeText);
		}
		else if (exist("volumecontrolimage") &&
				exist("volumecontrolbutton") )
		{
			KJVolumeBar *b;
			subwidgets.append(b=new KJVolumeBar(item("volumecontrolbutton"), this));
			b->setText(volumeText);
		}
/*		else
		{
			kdDebug(66666) << " no volumecontrol" << endl;
		} */
	}

	if (exist("pitchcontrolbutton") &&
		exist("pitchcontrolimage") &&
		exist("pitchcontrolimagexsize") &&
		exist("pitchcontrolimageposition") &&
		exist("pitchcontrolimagenb") )
	{
//		kdDebug(66666) << "added KJPitchBMP" << endl;
		KJPitchBMP *b;
		subwidgets.append(b=new KJPitchBMP(item("pitchcontrolbutton"), this));
		b->setText(pitchText);
	}
	else
	{
		// make sure we reset speed to 100% as the user won't be able
		// to reset it without a pitchcontrol
		Arts::PlayObject playobject = napp->player()->engine()->playObject();
		Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

		if ( !pitchable.isNull() )
		{
			if ( pitchable.speed() > 1.0f )
				pitchable.speed(1.0f);
		}
	}

	if (exist("filenamewindow"))
		subwidgets.append(new KJFilename(item("filenamewindow"), this));

	if (exist("mp3timewindow"))
		subwidgets.append(new KJTime(item("mp3timewindow"), this));

	if (exist("mp3kbpswindow"))
		subwidgets.append(new KJFileInfo(item("mp3kbpswindow"), this));

	if (exist("mp3khzwindow"))
		subwidgets.append(new KJFileInfo(item("mp3khzwindow"), this));

	if (exist("analyzerwindow"))
	{
		int vistype = mPrefs->visType();
		switch ( vistype )
		{
			case KJVisScope::Null:
				subwidgets.append(new KJNullScope(item("analyzerwindow"), this));
				break;
			case KJVisScope::FFT:
				subwidgets.append(new KJFFT(item("analyzerwindow"), this));
				break;
			case KJVisScope::StereoFFT:
				subwidgets.append(new KJStereoFFT(item("analyzerwindow"), this));
				break;
			case KJVisScope::Mono:
				subwidgets.append(new KJScope(item("analyzerwindow"), this));
				break;
		}
	}

	if (exist("EqualizerWindow"))
		subwidgets.append(new KJEqualizer(item("EqualizerWindow"), this));

	// I cant believe it, there are skins without a seeker, now THATS stupid :)
	if (exist("seekregion"))
		QTimer::singleShot(0, this, SLOT(loadSeeker()));

	// all the regular buttons
	for (QDictIterator<QStringList> i(*this); i.current(); ++i)
	{
		QString d=i.currentKey();
		if(d.contains("button") &&
			!d.startsWith("playlistwindow") && // don't add buttons that belong to the playlistwindow
			d != "pitchcontrolbutton" &&  // both already handled above as they aren't buttons but sliders
			d != "volumecontrolbutton" &&
			d != "spectrumanalyzerbutton" && // FIXME: unsupported button
			d != "oscilloscopebutton" && // FIXME: unsupported button
			i.count() >= 7 )
		{
			subwidgets.append(new KJButton(*(*i), this));
		}
	}

	show();
	conserveMemory();

	repaint();

	// update displays if we are already playing
	// This happens while changing skins
	if (napp->player()->isPlaying())
		newSong();

//	kdDebug(66666) << "</KJLoader::loadSkin(const QString &file)>" << endl;
}

void KJLoader::loadSeeker()
{
	subwidgets.append(new KJSeeker(item("seekregion"), this));
}

void KJLoader::unloadSkin()
{
//	kdDebug(66666) << "<KJLoader::unloadSkin()>" << endl;

	KWin::clearState(winId(), NET::SkipTaskbar);

//	kdDebug(66666) << " freeing subwidgets" << endl;
	subwidgets.clear();

	// This is special because mPitchfont can also point to mNumbers
	// as some skins use the NumberFont for pitchtext
	if ( mPitchFont && mPitchFont != mNumbers )
	{
//		kdDebug(66666) << " freeing mPitchFont" << endl;
		delete mPitchFont;
	}

	if ( mText )
	{
//		kdDebug(66666) << " freeing mText" << endl;
		delete mText;
	}

	if ( mNumbers )
	{
//		kdDebug(66666) << " freeing mNumbers" << endl;
		delete mNumbers;
	}

	if ( mVolumeFont )
	{
//		kdDebug(66666) << " freeing mVolumeFont" << endl;
		delete mVolumeFont;
	}

//	kdDebug(66666) << "</KJLoader::unloadSkin()>" << endl;
}

void KJLoader::minimize()
{
//	kdDebug(66666) << "KJLoader::minimize()" << endl;
	showMinimized();
}

void KJLoader::closeEvent(QCloseEvent*)
{
//	kdDebug(66666) << "KJLoader::closeEvent(QCloseEvent*)" << endl;
	unload();
}

void KJLoader::dragEnterEvent(QDragEnterEvent *event)
{
	// accept uri drops only
	event->accept(KURLDrag::canDecode(event));
}

void KJLoader::dropEvent(QDropEvent *event)
{
	KURL::List urls;
	if ( KURLDrag::decode(event,urls) )
	{
		for ( KURL::List::iterator it = urls.begin(); it != urls.end(); ++it )
			napp->player()->openFile((*it), false);
	}
}

void KJLoader::wheelEvent(QWheelEvent *e)
{	// from QT-Docu: delta() is 120 for one step
	if (e->state() & ControlButton)
		napp->player()->setVolume ( napp->player()->volume() + (e->delta()/8) ); // 15% volumechange
	else
		napp->player()->setVolume ( napp->player()->volume() + (e->delta()/24) ); // 5% volumechange
}

// now for some dockmode stuff
void KJLoader::switchToDockmode()
{
//	kdDebug(66666) << "KJLoader::switchToDockmode()" << endl;
	loadSkin( mCurrentDockModeSkin );

	connect(mWin, SIGNAL(activeWindowChanged(WId)), this, SLOT(slotWindowActivate(WId)));
	connect(mWin, SIGNAL(windowRemoved(WId)), this, SLOT(slotWindowRemove(WId)));
	connect(mWin, SIGNAL(stackingOrderChanged()), this, SLOT(slotStackingChanged()));
	connect(mWin, SIGNAL(windowChanged(WId)), this, SLOT(slotWindowChange(WId)));
	connect(mWin, SIGNAL(currentDesktopChanged(int)), this, SLOT(slotDesktopChange(int)));

	WId activeWin = mWin->activeWindow();
	if (activeWin && (activeWin != winId()))
	{
		KWin::WindowInfo winInf = KWin::windowInfo(activeWin, NET::WMKDEFrameStrut);
		if(winInf.valid())
		{
			mDockToWin = activeWin;
			mDockWindowRect = winInf.frameGeometry();
			slotWindowActivate(mDockToWin);
			hide();
			restack();
		}
	}
}

void KJLoader::returnFromDockmode()
{
//	kdDebug(66666) << "KJLoader::returnFromDockmode()" << endl;
	mWin->disconnect();
	loadSkin(mCurrentDefaultSkin);
}

void KJLoader::slotWindowActivate(WId win)
{
	if(mCurrentSkin != mCurrentDockModeSkin)
		return;

	KWin::WindowInfo winInf = KWin::windowInfo(
		win, NET::WMWindowType);
	if((win != winId()) && winInf.valid())
	{
		// ensure we dock to the active window _except_ our own
		// and stick to the last window if the NEW current one is a desktop
		NET::WindowType winType = winInf.windowType(
			NET::NormalMask|NET::DesktopMask|NET::DockMask|
			NET::ToolbarMask|NET::MenuMask|NET::DialogMask|
			NET::OverrideMask|NET::TopMenuMask|NET::UtilityMask|
			NET::SplashMask);

		if(winType == NET::Unknown || winType == NET::Normal || winType == NET::Dialog)
		{
			//kdDebug(66666) << k_funcinfo << "Now docking to window: " << win << endl;
			mDockToWin = win;
		}

	}

	if(mDockToWin != 0)
	{
		mDockWindowRect = KWin::windowInfo(mDockToWin, NET::WMKDEFrameStrut).frameGeometry();
		/*kdDebug(66666) << k_funcinfo << "winrect: " << mDockWindowRect.x() << ", " <<
			mDockWindowRect.y() << endl;*/
		switch ( mDockPosition )
		{
			case 0:
				move( mDockWindowRect.x() + mDockPositionX, mDockWindowRect.y() + mDockPositionY );
				break;
			case 2:
				move( mDockWindowRect.x() + mDockPositionX, mDockWindowRect.y() + mDockWindowRect.height() + mDockPositionY );
				break;
		}

		if(!isVisible())
		{
			show();
			KWin::setState(winId(), NET::SkipTaskbar);
		}
		restack();
	}
	else
	{
		// We don't want to do anything until a window comes into
		// focus.
		//kdDebug(66666) << "No window having focus, hiding" << endl;
		hide();
	}

//	kdDebug(66666) << "END slotWindowActivate()" << endl;
}

void KJLoader::slotWindowRemove(WId win)
{
//	kdDebug(66666) << "START slotWindowRemove()" << endl;
	if ( mCurrentSkin != mCurrentDockModeSkin )
		return;

	if (win == mDockToWin)
	{
//		kdDebug(66666) << "our window removed: " << win << endl;
		hide();
		mDockToWin = 0;
	}
//	kdDebug(66666) << "END slotWindowRemove()" << endl;
}

void KJLoader::slotWindowChange(WId win)
{
//	kdDebug(66666) << "START slotWindowChange()" << endl;
	if ( mCurrentSkin != mCurrentDockModeSkin )
		return;

	if ( win == mDockToWin )
	{
//		kdDebug(66666) << "changed our window:" << win << endl;
		KWin::WindowInfo winInf = KWin::windowInfo(
			mDockToWin, NET::WMKDEFrameStrut|NET::WMWindowType|
			NET::WMState|NET::XAWMState|NET::WMDesktop);

		if(!winInf.valid())
		{
			/*kdDebug(66666) << k_funcinfo <<
				"No valid WindowInfo for tracked window: " << win << endl;*/
			hide();
			mDockToWin = 0;
			return;
		}

		NET::WindowType winType = winInf.windowType(
			NET::NormalMask|NET::DesktopMask|NET::DockMask|
			NET::ToolbarMask|NET::MenuMask|NET::DialogMask|
			NET::OverrideMask|NET::TopMenuMask|NET::UtilityMask|
			NET::SplashMask);

		if (
			(winInf.state() & NET::Hidden) ||
			(winInf.state() & NET::FullScreen) ||
			(winType != NET::Unknown && winType != NET::Normal && winType != NET::Dialog)
			)
		{
			/*kdDebug(66666) << k_funcinfo <<
				"Our window changed: " << win <<
				". Either iconified or special window" << endl;*/
			// target-window has been iconified or window is desktop
			hide();
			mDockToWin = 0;
			return;
		}

		// Size or position of target-window changed.
		mDockWindowRect = winInf.frameGeometry();
		/*kdDebug(66666) << k_funcinfo << "winrect: " << mDockWindowRect.x() << ", " <<
			mDockWindowRect.y() << endl;*/
		// Ensure we are still on the window.
		switch(mDockPosition)
		{
			case 0:
			{
				move(
					mDockWindowRect.x() + mDockPositionX,
					mDockWindowRect.y() + mDockPositionY);
				break;
			}
			case 2:
			{
				move(
					mDockWindowRect.x() + mDockPositionX,
					mDockWindowRect.y() + mDockWindowRect.height() + mDockPositionY);
				break;
			}
		}
		restack();
	}
}

void KJLoader::slotDesktopChange(int)
{
//	kdDebug(66666) << "START slotDesktopChange()" << endl;
	if ( mCurrentSkin != mCurrentDockModeSkin )
		return;

	hide();
	mDockToWin = 0L;
//	kdDebug(66666) << "END slotDesktopChange()" << endl;
}

void KJLoader::slotStackingChanged()
{
//	kdDebug(66666) << "START slotStackingChanged()" << endl;
	if ( mCurrentSkin != mCurrentDockModeSkin )
		return;

	// We seem to get this signal before the window has been restacked,
	// so we just schedule a restack.
	QTimer::singleShot ( 10, this, SLOT(restack()) );

//	kdDebug(66666) << "END slotStackingChanged()" << endl;
}

// Set the animation's stacking order to be just above the target window's
// window decoration, or on top.
void KJLoader::restack()
{
//	kdDebug(66666) << "START restack()" << endl;

	if ( !mDockToWin )
	{
//		kdDebug(66666) << "No window to dock to, no restacking" << endl;
		hide();
		return;
	}

	// simply raise ourselves to the top
	raise();
	// and then ensure our target-window gets focus
//	NET::setActiveWindow (mDockToWin);

//	kdDebug(66666) << "END restack()" << endl;
}

KJLoader::~KJLoader()
{
//	kdDebug(66666) << "KJLoader::~KJLoader()" << endl;
	delete mWin;
}

void KJLoader::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
		if (i->rect().intersects(e->rect()))
			i->paint(&p, e->rect().intersect(i->rect()));
//	QWidget::paintEvent(e);
}

void KJLoader::mouseMoveEvent(QMouseEvent *e)
{
	if (moving)
	{
		move ( QCursor::pos()-mMousePoint );
		return;
	}


//	QWidget::mouseMoveEvent(e);
	// not on background but on a widget: pass event to subwidget
	if ( !moving && mClickedIn && subwidgets.findRef(mClickedIn) != -1 )
	{
		mClickedIn->mouseMove (
			e->pos()-mClickedIn->rect().topLeft(),
			mClickedIn->rect().contains(mapFromGlobal(QCursor::pos())) );
	}
}

void KJLoader::mousePressEvent(QMouseEvent *e)
{
//	kdDebug(66666) << "KJLoader::mousePressEvent(QMouseEvent *e)" << endl;

//	QWidget::mousePressEvent(e);

	if ( e->button()==RightButton )
		NoatunStdAction::ContextMenu::showContextMenu();
	else /* if ( e->button()==LeftButton ) */
	{
		mMousePoint = mapFromGlobal(QCursor::pos());
		// try to find a KJWidget that is here
		for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
			if (i->rect().contains(mMousePoint))
			{
				if (i->mousePress(mMousePoint-i->rect().topLeft()))
				{
					mClickedIn=i;
					return;
				}
			}
		// can't find a widget, so move the window
		if ( mCurrentSkin != mCurrentDockModeSkin)
			moving = true;
	}
}

void KJLoader::mouseReleaseEvent(QMouseEvent */*e*/)
{
//	kdDebug(66666) << "KJLoader::mouseReleaseEvent(QMouseEvent *e)" << endl;

//	QWidget::mouseReleaseEvent(e);

	if (!moving && mClickedIn && subwidgets.findRef(mClickedIn)!=-1)
	{
		mClickedIn->mouseRelease(mapFromGlobal(QCursor::pos())-
		                                       mClickedIn->rect().topLeft(),
		                         mClickedIn->rect().contains(
			                         mapFromGlobal(QCursor::pos())));
		mClickedIn=0;
	}

	moving = false;
}

void KJLoader::timeUpdate()
{
	for (KJWidget* widget=subwidgets.first(); widget; widget=subwidgets.next())
		widget->timeUpdate(napp->player()->getTime()/1000); // pass seconds to all Widgets
}

void KJLoader::newSong()
{
	if (!napp->player()->current())
		return;
	for ( KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next() )
		i->newFile();
}

void KJLoader::readConfig()
{
//	kdDebug(66666) << "KJLoader::readConfig()" << endl;
	for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
		i->readConfig();
}

void KJLoader::showSplash()
{
	splashScreen = new QLabel( 0L, "SplashScreen",
		WType_TopLevel | WStyle_NoBorder | WRepaintNoErase | WX11BypassWM );

	QPixmap splashPix = pixmap(item("splashscreen")[1]);
	splashScreen->setPixmap( splashPix );
	splashScreen->setBackgroundMode ( NoBackground );
	splashScreen->setMask( KJWidget::getMask(image(item("splashscreen")[1])) );

	QSize sh = splashScreen->sizeHint();

	QRect desk = KGlobalSettings::splashScreenDesktopGeometry();
	splashScreen->move (desk.x() + (desk.width() - sh.width())/2,
		desk.y() + (desk.height() - sh.height())/2 );

	splashScreen->setFixedSize(sh);
	splashScreen->show();
	napp->processEvents(); // we want this one time to get the splash actually displayed ASAP

	QTimer::singleShot(3000, this, SLOT(hideSplash()) );
}

void KJLoader::hideSplash()
{
	splashScreen->hide();
	delete splashScreen;
}
