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

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "kaboodleapp.h"
#include <kaction.h>
#include <kparts/mainwindow.h>
#include <kurl.h>

#include "player.h"

namespace Kaboodle
{
/**
 * @short Main window class
 * @author Neil Stevens <multivac@fcmail.com>
 * @author Charles Samuels <charles@kde.org>
 */
class UserInterface : public KParts::MainWindow
{
Q_OBJECT
public:
	UserInterface(QWidget *parent, const KURL &initialFile = KURL());
	virtual ~UserInterface(void);

	void load(const QString& url);

protected:
	virtual void dragEnterEvent(QDragEnterEvent *);
	virtual void dropEvent(QDropEvent *);

public slots:
	void playerPreferences(void);

private slots:
	void fileOpen(void);
	void showMenubar(void);
	void updateTitle(const QString &text);
	void applySettings(void);
	void properties(void);
	void adaptSize(int width, int height);
    void slotConfigureKeys();
private:
	Player *part;
	KToggleAction *menubarAction;
	KAction *propertiesAction;
};
}
#endif
