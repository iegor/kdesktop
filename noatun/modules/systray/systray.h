// systray.h
//
// Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <noatun/plugin.h>
#include <noatun/app.h>

#include <qtimer.h>

#include <kmainwindow.h>

class KitSystemTray;
class QTimer;
class PassivePopup;

class NoatunSystray : public KMainWindow, public Plugin
{
Q_OBJECT
public:
	NoatunSystray();
	virtual ~NoatunSystray();
	virtual void init();

protected:
	virtual void closeEvent(QCloseEvent *);

public slots:
	void slotPlayPause();
	void slotStopped();

private slots:
	void changeTray(const QString &);
	void slotLoadSettings();
	void slotBlinkTimer();
	void showPassivePopup();
	QPixmap *renderIcon(const QString &, const QString &) const;

private:
	void setTipText(const QString&);
	void updateCover();
	void removeCover();

private:
	KitSystemTray *mTray;
	QTimer *mBlinkTimer;
	QPixmap *trayStatus;
	QPixmap *trayBase;
	PassivePopup *mPassivePopup;

	bool showingTrayStatus;
	QString tipText;
	QString tmpCoverPath;
};

#endif
