// Copyright (c) 2001 Neil Stevens <multivac@fcmail.com>
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
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include "cmodule.h"
#include "synaescope.h"

SynaePrefs::SynaePrefs(QObject *parent)
	: CModule(i18n("Synaescope"), i18n("Noatun Visualization"), "synaescope", parent)
{
// 	kdDebug(66666) << k_funcinfo << endl;
	xRes = new QSpinBox(320, 1024, 16, this);
	yRes = new QSpinBox(240, 768, 12, this);

	QVBoxLayout *layout = new QVBoxLayout(this, 0, KDialog::spacingHint());
	QHBoxLayout *xResLayout = new QHBoxLayout(0, 0, KDialog::spacingHint());
	QHBoxLayout *yResLayout = new QHBoxLayout(0, 0, KDialog::spacingHint());

	xResLayout->addWidget(new QLabel(i18n("Display width:"), this));
	xResLayout->addWidget(xRes);

	yResLayout->addWidget(new QLabel(i18n("Display height:"), this));
	yResLayout->addWidget(yRes);

	layout->addLayout(xResLayout);
	layout->addLayout(yResLayout);
	layout->addStretch();

	changed=false;
	connect(xRes, SIGNAL(valueChanged(int)), this, SLOT(slotChanges()));
	connect(yRes, SIGNAL(valueChanged(int)), this, SLOT(slotChanges()));
}

void SynaePrefs::save()
{
// 	kdDebug(66666) << k_funcinfo << endl;
	if(!changed)
		return;

	KConfig *c = KGlobal::config();
	c->setGroup("Synaescope");
	c->writeEntry("xResolution", xRes->value());
	c->writeEntry("yResolution", yRes->value());
	c->sync();
	emit configChanged();
}

void SynaePrefs::reopen()
{
//	kdDebug(66666) << k_funcinfo << endl;
	KConfig *c = KGlobal::config();
	c->setGroup("Synaescope");

	xRes->setValue(c->readNumEntry("xResolution", 320));
	yRes->setValue(c->readNumEntry("yResolution", 240));
	changed=false;
}

void SynaePrefs::slotChanges()
{
	changed=true;
}
#include "cmodule.moc"
