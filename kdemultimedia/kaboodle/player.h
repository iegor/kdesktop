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

#ifndef PLAYER_H
#define PLAYER_H

#include <kmedia2.h>
#include <kmediaplayer/player.h>
#include <kparts/browserextension.h>
#include <kurl.h>
#include <qobject.h>
#include <qtimer.h>

class KAction;
class KToggleAction;

namespace Kaboodle
{
class Engine;
class Player;
class View;

class BrowserExtension : public KParts::BrowserExtension
{
Q_OBJECT

public:
	BrowserExtension(Player *parent);
};

class Player : public KMediaPlayer::Player
{
Q_OBJECT

public:
	Player(QWidget *widgetParent, const char *widgetName,
	       QObject *parent, const char *name);
	Player(QObject *parent, const char *name);
	virtual ~Player();

	virtual bool openURL(const KURL &);
	KURL currentURL(void);

	/**
	 * returns a string with the time that can
	 * be used in the UI:
	 * CC:CC/LL:LL (mm:ss)
	 **/
	static QString timeString(unsigned long);
	QString lengthString(void);
	QString positionString(void);

	virtual KMediaPlayer::View *view(void);

public slots:
	virtual void pause(void);
	virtual void play(void);
	virtual void stop(void);
	void loop(void);

	virtual void seek(unsigned long msec);

public:
	virtual bool isSeekable(void) const;
	virtual unsigned long position(void) const;
	virtual bool hasLength(void) const;
	virtual unsigned long length(void) const;

signals:
	void timeout(void);

protected:
	virtual bool openFile(void);

private slots:
	void tickerTimeout(void);
	void updateTitle(void);

private:
	Engine *engine;
	View *widget;

	BrowserExtension *extension;

	KAction *playAction, *pauseAction, *stopAction;
	KToggleAction *loopAction;

	QTimer ticker;
	KURL current;
	bool uncompleted;

	KURL lastEmitted;
	bool embedded;
};
}
#endif
