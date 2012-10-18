/***************************************************************************
	kjbutton.cpp
	--------------------------------------
	Handles all ordinary Buttons like stop, play, pause, etc.
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

#include "kjbutton.h"
#include "kjbutton.moc"
#include "kjloader.h"
#include "kjprefs.h"

#include <noatun/pref.h>
#include <noatun/player.h>
#include <noatun/vequalizer.h>

#include <qcursor.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

/*******************************************
 * KJButton
 *******************************************/

KJButton::KJButton(const QStringList &i, KJLoader *parent)
	: QObject(0), KJWidget(parent), mTitle(i[0]), mShowPressed(false)
{
//	kdDebug(66666) << k_funcinfo << "new button: " << i[0].latin1() << endl;
	mPushedPixmap = (i.count() >= 7);

	// get the rectangle
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x; // width
	ys=i[4].toInt()-y; // height
	setRect ( x, y, xs, ys );

	QStringList temp = i;

	// search for selected button-type
	// can be either BMPx where x is a number representing one
	// of the background-images
	// or darken which means just darken the button on click
	bool gotBack = false; // in case any of these keys is duplicated
	for(QStringList::Iterator it = temp.begin(); it != temp.end(); ++it)
	{
		if((*it).contains("bmp"))
		{
			QString pressedTmp = backgroundPressed((*it));
			if(!pressedTmp.isEmpty())
			{
				mPressed = parent->pixmap(pressedTmp);
				gotBack = true;
			}
		}
		else if((*it) == "darken")
		{
			// take background and darken the buttons rectangle
			// FIXME: what KPixmapEffect causes the desired effect?
			//        intensity is the wrong one
			KPixmap temp = parent->pixmap(parser()["backgroundimage"][1]);
			mPressed = (QPixmap)KPixmapEffect::intensity ( temp, 1.2f );
			gotBack = true;
		}
		if(gotBack)
			break;
	}

	if(!gotBack)
	{
		kdDebug(66666) << k_funcinfo << "Couldn't find valid background for button '" <<
			mTitle << "', dafulting to backgroundimage" << endl;
		mPressed = parent->pixmap(parser()["backgroundimage"][1]);
	}

	// playlistbutton has to show if playlistwindow is open
	// right after the button appears on screen
	if (mTitle=="playlistbutton")
	{
		mShowPressed = napp->playlist()->listVisible();
		connect( napp->player(), SIGNAL(playlistShown()), this, SLOT(slotPlaylistShown()) );
		connect( napp->player(), SIGNAL(playlistHidden()), this, SLOT(slotPlaylistHidden()) );
	}
	else if ( mTitle=="equalizeroffbutton") // same goes for EQ buttons
	{
		mShowPressed = (!napp->vequalizer()->isEnabled());
		connect( napp->vequalizer(), SIGNAL(enabled(bool)), SLOT(slotEqEnabled(bool)));
	}
	else if (mTitle=="equalizeronbutton")
	{
		mShowPressed = napp->vequalizer()->isEnabled();
		connect( napp->vequalizer(), SIGNAL(enabled(bool)), SLOT(slotEqEnabled(bool)));
	}
}

QString KJButton::tip()
{
	QString str;
	if (mTitle=="closebutton")
		str=i18n("Close");
	else if (mTitle=="minimizebutton")
		str=i18n("Minimize");
	else if (mTitle=="aboutbutton")
		str=i18n("About");
	else if (mTitle=="stopbutton")
		str=i18n("Stop");
	else if (mTitle=="playbutton")
		str=i18n("Play");
	else if (mTitle=="pausebutton")
		str=i18n("Pause");
	else if (mTitle=="openfilebutton")
		str=i18n("Open");
	else if (mTitle=="playlistbutton")
		str=i18n("Playlist");
	else if (mTitle=="repeatbutton")
		str=i18n("Loop");
	else if (mTitle=="equalizerbutton")
		str=i18n("Show Equalizer Window");
	else if (mTitle=="equalizeronbutton")
		str=i18n("Turn on Equalizer");
	else if (mTitle=="equalizeroffbutton")
		str=i18n("Turn off Equalizer");
	else if (mTitle=="equalizerresetbutton")
		str=i18n("Reset Equalizer");
	else if (mTitle=="nextsongbutton")
		str=i18n("Next");
	else if (mTitle=="previoussongbutton")
		str=i18n("Previous");
	else if (mTitle=="forwardbutton")
		str=i18n("Forward");
	else if (mTitle=="rewindbutton")
		str=i18n("Rewind");
	else if (mTitle=="preferencesbutton")
		str=i18n("K-Jöfol Preferences");
	else if (mTitle=="dockmodebutton")
		str=i18n("Switch to dockmode");
	else if (mTitle=="undockmodebutton")
		str=i18n("Return from dockmode");

	return str;
}

void KJButton::paint(QPainter *, const QRect &)
{
	if (mShowPressed)
		bitBlt(KJWidget::parent(), rect().topLeft(), &mPressed, rect(), Qt::CopyROP);
}

bool KJButton::mousePress(const QPoint &)
{
	bitBlt(KJWidget::parent(), rect().topLeft(), &mPressed, rect(), Qt::CopyROP);
	return true;
}

void KJButton::showPressed(bool b)
{
	mShowPressed = b;
	if ( mShowPressed )
		repaint(true); // repaint with selected image
	else
		repaint(false); // repaint with default image (player-background)
}

void KJButton::slotPlaylistShown(void)
{
//	kdDebug(66666) << "KJButton::slotPlaylistShown()" << endl;
	showPressed(true);
}

void KJButton::slotPlaylistHidden(void)
{
//	kdDebug(66666) << "KJButton::slotPlaylistHidden()" << endl;
	showPressed(false);
}

void KJButton::slotEqEnabled(bool on)
{
//	kdDebug(66666) << "KJButton::slotEqEnabled(" << on << ") for " << mTitle << endl;
	if (mTitle=="equalizeronbutton")
		showPressed(on);
	else if (mTitle=="equalizeroffbutton")
		showPressed(!on);
}

void KJButton::mouseRelease(const QPoint &, bool in)
{
	// repaint with default image (player-background)
	repaint(false);

	if (!in) // only do something if users is still inside the button
		return;

	// now, find what widget I am and do the proper action
	if (mTitle=="closebutton")
		KJWidget::parent()->close();
	else if (mTitle=="minimizebutton")
		KJWidget::parent()->minimize();
	else if (mTitle=="aboutbutton")
		KJWidget::parent()->helpMenu()->aboutApplication();
	else if (mTitle=="stopbutton")
		napp->player()->stop();
	else if (mTitle=="playbutton")
		napp->player()->play();
	else if (mTitle=="pausebutton")
		napp->player()->playpause();
	else if (mTitle=="openfilebutton")
	{
		KURL file(KFileDialog::getOpenURL(0, napp->mimeTypes(), KJWidget::parent(), i18n("Select File to Play")));
		if (file.isValid())
			napp->player()->openFile(file);
	}
	else if (mTitle=="playlistbutton")
		napp->player()->toggleListView();
	else if (mTitle=="repeatbutton")
	{
		KPopupMenu *loopMenu = new KPopupMenu(KJWidget::parent(),"loopMenu");
		int selectedItem = 0;

		loopMenu->setCheckable(true);
		loopMenu->insertTitle(i18n("Loop Style"));
		loopMenu->insertItem(i18n("&None"), static_cast<int>(Player::None));
		loopMenu->insertItem(i18n("&Song"), static_cast<int>(Player::Song));
		loopMenu->insertItem(i18n("&Playlist"), static_cast<int>(Player::Playlist));
		loopMenu->insertItem(i18n("&Random"), static_cast<int>(Player::Random));

		loopMenu->setItemChecked(static_cast<int>(napp->player()->loopStyle()), true); // select current loopstyle in menu
		selectedItem = loopMenu->exec(QCursor::pos());
		if (selectedItem != -1)
			napp->player()->loop(selectedItem); // set new loopstyle

		delete loopMenu;

/*
		if ( napp->player()->loopStyle() == 1)
		{
//			kdDebug(66666) << "loop song is OFF" << endl;

// bah, xlib.h already defined None
#undef None
			napp->player()->loop( Player::None );
			showPressed( false );
		}
		else
		{
//			kdDebug(66666) << "loop song is ON" << endl;
			napp->player()->loop( Player::Song );
			showPressed ( true );
		}
*/
	}
	else if (mTitle=="equalizerbutton")
	{
		napp->equalizerView();
	}
	else if (mTitle=="equalizeronbutton")
	{
		if (!napp->vequalizer()->isEnabled())
			napp->vequalizer()->enable();
	}
	else if (mTitle=="equalizeroffbutton")
	{
		if (napp->vequalizer()->isEnabled())
			napp->vequalizer()->disable();
	}
	else if (mTitle=="equalizerresetbutton")
	{
		for (int band=0; band<napp->vequalizer()->bands(); band++)
			napp->vequalizer()->band(band).setLevel(0);
	/*
		// That preset resets to 6 bands, that's not what we want
		VPreset set = napp->vequalizer()->presetByName("Zero");
		if (set) // tests if that preset is valid
			set.load();
	*/
	}
	else if (mTitle=="nextsongbutton")
		napp->player()->forward();
	else if (mTitle=="previoussongbutton")
		napp->player()->back();
	else if (mTitle=="forwardbutton")
		napp->player()->skipTo(napp->player()->getTime()+10000); // TODO: make +- 10 secs configurable
	else if (mTitle=="rewindbutton")
		napp->player()->skipTo(napp->player()->getTime()-10000);
	else if (mTitle=="preferencesbutton")
		napp->preferencesBox()->show(static_cast<CModule *>(KJWidget::parent()->prefs()));
	else if (mTitle=="dockmodebutton")
		KJWidget::parent()->switchToDockmode();
	else if (mTitle=="undockmodebutton")
		KJWidget::parent()->returnFromDockmode();
	else
		kdDebug(66666) << "unknown buttontype: " << mTitle.latin1() << endl;
}
