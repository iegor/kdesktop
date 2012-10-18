/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef KABOODLEVIEW_H
#define KABOODLEVIEW_H

#include <kmediaplayer/player.h>
#include <kmediaplayer/view.h>
#include <kurl.h>
#include <qevent.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qwidget.h>

#include "controls.h"
#include "player.h"

class QButton;
class QLabel;
class KVideoWidget;

namespace Kaboodle
{
class View : public KMediaPlayer::View
{
Q_OBJECT

public:
	View(QWidget *parent, const char *name, Player *player);
	virtual ~View(void);
	
	/**
	 * automatically play a file after opening it
	 */
	bool isAutoPlay(void);
	void setAutoPlay(bool);

	/**
	 * if we opened a file on the command line, quit after playing it
	 */
	bool isQuitAfterPlaying(void);
	void setQuitAfterPlaying(bool);

	void embed(Arts::PlayObject);
	KVideoWidget *videoWidget() { return video; }

public slots:
	void stateChanged(int);
	void updateButtons(int);
	void playerFinished(void);
	void playerTimeout(void);

	void sliderMoved(int);
	void skipToWrapper(int);

signals:
	void adaptSize(int width, int height);

private slots:
	void updateLabel(const QString &text);
	void slotButtonPressed( int, const QPoint &, int state );
	void slotDblClick( const QPoint &, int state );

	void calculateSize(int width, int height);

private:
	KMediaPlayer::Player::State state;
	bool autoPlay, quitAfterPlaying;
	Player *player;

	void updateTicks(void);

	L33tSlider *slider;
	QLabel *elapsedLabel;
	KVideoWidget *video;
	bool firstVideo;

	int lastWidth;
	
	QButton *playButton, *pauseButton, *stopButton;
};
}
#endif
