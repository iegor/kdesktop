/*****************************************************************

Copyright (c) 2001 Charles Samuels <charles@kde.org>
              2000 Rik Hemsley <rik@kde.org>

This code is released under the GNU General Public License 2.0, or any
later version, at your option.  I mean, you know the drill, just read
RMS's novel that I'm supposed to put at the top of every story.  He's insane.
******************************************************************/

#include "madness.h"

#include <kwin.h>
#include <kwinmodule.h>
#include <kiconloader.h>
#include <math.h>
#include <iostream>

#include <X11/Xlib.h>


extern "C"
{
Plugin *create_plugin()
{
	return new Madness();
}
}

Madness::Madness()
	: MonoFFTScope(100), Plugin(), mWm(this)
{
	connect(&mWm, SIGNAL(currentDesktopChanged(int)), SLOT(update()));
	connect(&mWm, SIGNAL(windowAdded(WId)), SLOT(update()));
	connect(&mWm, SIGNAL(windowRemoved(WId)), SLOT(update()));
	connect(&mWm, SIGNAL(strutChanged()), SLOT(update()));

}

Madness::~Madness()
{
	QMap<WId, QPoint>::ConstIterator it(mOriginalPositions.begin());

	for (; it != mOriginalPositions.end(); ++it)
		XMoveWindow(qt_xdisplay(), it.key(), (*it).x(), (*it).y());
}

void Madness::update()
{
	mWindowList = mWm.windows();
	mWorkArea = mWm.workArea();

	QValueList<WId>::ConstIterator it(mWindowList.begin());

	for (; it != mWindowList.end(); ++it)
	{
		QRect area=KWin::info(*it).frameGeometry;
		if (!mOriginalPositions.contains(*it))
			mOriginalPositions.insert(*it, area.topLeft());
	}
}

void Madness::init()
{
	update();
	MonoFFTScope::start();
}

void Madness::scopeEvent(float *d, int size)
{
	int delta=0; // in pixels
	for (int count=0; count<size; count++)
	{
		delta+=(int)((log10(d[count]+1)/log(2))*(size-count))/2;
	}
	
//	cout << "delta: " << delta << endl;
	
	QValueList<WId>::ConstIterator it(mWindowList.begin());
 
	for (; it != mWindowList.end(); ++it)
	{
		KWin::Info i(KWin::info(*it));

		if ((NET::Visible != i.mappingState) ||
				((NET::Unknown != i.windowType) &&
				 (NET::Normal  != i.windowType) &&
				 (NET::Tool    != i.windowType) &&
				 (NET::Menu    != i.windowType) &&
				 (NET::Dialog  != i.windowType)) ||	(NET::Max & i.state)
				|| (NET::Shaded & i.state)
				|| (mWm.currentDesktop() != i.desktop))
			continue;

		QRect area=i.frameGeometry;
		float lightness=100000.0/(area.width()*area.height());

		int x=area.x();
		int y=area.y();
		
		
		int dx=(int)((delta*lightness*(area.height()/10)/100))*(KApplication::random()%2 ? -1 : 1);
		int dy=(int)((delta*lightness*(area.width()/10)/100))*(KApplication::random()%2 ? -1 : 1);
		
		if (dx < 0 && (x - dx < mWorkArea.left()))
			dx = -dx;

		else if (dx > 0 && (x + dx + area.width() > mWorkArea.right()))
			dx = -dx;

		if (dy < 0 && (y - dy < mWorkArea.top()))
			dy = -dy;

		else if (dy > 0 && (y + dy + area.height() > mWorkArea.bottom()))
			dy = -dy;

		
		XMoveWindow(qt_xdisplay(), i.win, x + dx, y + dy);
	}

}

#include "madness.moc"
