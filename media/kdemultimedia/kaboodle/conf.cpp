// conf.cpp
//
// Copyright (C) 2001 Neil Stevens <multivac@fcmail.com>
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

#include <kconfig.h>
#include <klocale.h>
#include <qcheckbox.h>
#include <qvbox.h>

#include "conf.h"
#include "view.h"

Kaboodle::Conf::Conf(QWidget *_parent, const char *_name)
	: KDialogBase(_parent, _name, true, QString::null, Ok | Cancel)
{
	QVBox *box = makeVBoxMainWidget();

	autoPlay = new QCheckBox(i18n("Start playing automatically"), box);
	quitAfterPlaying = new QCheckBox(i18n("Quit when finished playing"), box);

	KConfig &config = *KGlobal::config();
	config.setGroup("core");
	autoPlay->setChecked(config.readBoolEntry("autoPlay", true));
	quitAfterPlaying->setChecked(config.readBoolEntry("quitAfterPlaying", true));
}

void Kaboodle::Conf::accept(void)
{
	KConfig &config = *KGlobal::config();
	config.setGroup("core");
	config.writeEntry("autoPlay", autoPlay->isChecked());
	config.writeEntry("quitAfterPlaying", quitAfterPlaying->isChecked());
	config.sync();

	KDialogBase::accept();
}

#include "conf.moc"
