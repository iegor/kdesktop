// cmodule.cpp
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

#include "cmodule.h"
#include "yhconfig.h"
#include "yhconfigwidget.h"

#include <kdebug.h>
//#include <kglobal.h>
#include <klocale.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <noatun/app.h>
#include <noatun/pluginloader.h>

#include <fixx11h.h>

YHModule::YHModule(QObject *_parent)
	: CModule(i18n("System Tray Icon"), i18n("Configure System Tray Icon"),
		"bottom", _parent)
{
	QVBoxLayout *top = new QVBoxLayout(this);
	mWidget = new YHConfigWidget(this);
	top->addWidget(mWidget);

	mWidget->cmbModifier->insertItem(i18n("None"), YHConfig::None);
	mWidget->cmbModifier->insertItem(i18n("Shift"), YHConfig::Shift);
	mWidget->cmbModifier->insertItem(i18n("Alt"), YHConfig::Alt);
	mWidget->cmbModifier->insertItem(i18n("Ctrl"), YHConfig::Ctrl);
	mWidget->cmbModifier->setCurrentItem(YHConfig::None);

	connect(mWidget->chkUsePopup, SIGNAL(toggled(bool)), this, SLOT(slotUsePopupToggled(bool)));
	connect(mWidget->cmbModifier, SIGNAL(activated(int)), this, SLOT(slotModifierActivated(int)));
	connect(mWidget->grpMwheel, SIGNAL(clicked(int)), this, SLOT(slotMwheelClicked(int)));

	reopen();
}


void YHModule::reopen()
{
	kdDebug(66666) << k_funcinfo << endl;
	YHConfig *c = YHConfig::self();

	/** General TAB **/

	mWidget->chkUseTooltip->setChecked(c->tip());
	mWidget->chkUseCovers->setChecked(c->passivePopupCovers());

	mWidget->chkUsePopup->setChecked(c->passivePopup());
	mWidget->spinPopupTimeout->setValue(c->passivePopupTimeout());
	mWidget->chkPopupButtons->setChecked(c->passivePopupButtons());

	switch(c->stateIconDisplay())
	{
		case (YHConfig::Animation):
			mWidget->rbStateAnim->setChecked(true);
			break;
		case (YHConfig::FlashingIcon):
			mWidget->rbStateFlashing->setChecked(true);
			break;
		case (YHConfig::StaticIcon):
			mWidget->rbStateStatic->setChecked(true);
			break;
		case (YHConfig::NoIcon):
			mWidget->rbStateNone->setChecked(true);
			break;
	}

	/** Advanced TAB **/

	if (c->middleMouseAction() == YHConfig::PlayPause)
		mWidget->rbPlayPause->setChecked(true);
	else
		mWidget->rbHideShowPlaylist->setChecked(true);

	mActionMap[YHConfig::None]  = YHConfig::self()->mouseWheelAction(YHConfig::None);
	mActionMap[YHConfig::Shift] = YHConfig::self()->mouseWheelAction(YHConfig::Shift);
	mActionMap[YHConfig::Alt]   = YHConfig::self()->mouseWheelAction(YHConfig::Alt);
	mActionMap[YHConfig::Ctrl]  = YHConfig::self()->mouseWheelAction(YHConfig::Ctrl);

	slotModifierActivated(mWidget->cmbModifier->currentItem());
}


void YHModule::save()
{
	kdDebug(66666) << k_funcinfo << endl;

	YHConfig *c = YHConfig::self();

	/** General TAB **/

	c->setTip(mWidget->chkUseTooltip->isChecked());
	c->setPassivePopupCovers(mWidget->chkUseCovers->isChecked());

	c->setPassivePopup(mWidget->chkUsePopup->isChecked());
	c->setPassivePopupTimeout(mWidget->spinPopupTimeout->value());
	c->setPassivePopupButtons(mWidget->chkPopupButtons->isChecked());

	if (mWidget->rbStateAnim->isChecked())
		c->setStateIconDisplay(YHConfig::Animation);
	else if (mWidget->rbStateFlashing->isChecked())
		c->setStateIconDisplay(YHConfig::FlashingIcon);
	else if (mWidget->rbStateStatic->isChecked())
		c->setStateIconDisplay(YHConfig::StaticIcon);
	else
		c->setStateIconDisplay(YHConfig::NoIcon);

	/** Advanced TAB **/

	if (mWidget->rbPlayPause->isChecked())
		c->setMiddleMouseAction(YHConfig::PlayPause);
	else
		c->setMiddleMouseAction(YHConfig::HideShowPlaylist);

	c->setMouseWheelAction(YHConfig::None, mActionMap[YHConfig::None]);
	c->setMouseWheelAction(YHConfig::Shift, mActionMap[YHConfig::Shift]);
	c->setMouseWheelAction(YHConfig::Alt, mActionMap[YHConfig::Alt]);
	c->setMouseWheelAction(YHConfig::Ctrl, mActionMap[YHConfig::Ctrl]);

	c->writeConfig();
	emit saved();
}


void YHModule::slotUsePopupToggled(bool on)
{
	mWidget->lblPopupTimeout->setEnabled(on);
	mWidget->spinPopupTimeout->setEnabled(on);
	mWidget->chkPopupButtons->setEnabled(on);
}


void YHModule::slotModifierActivated(int index)
{
	kdDebug(66666) << k_funcinfo << endl;
	switch(mActionMap[index])
	{
		case (YHConfig::Nothing):
			mWidget->rbActNothing->setChecked(true);
			break;
		case (YHConfig::ChangeVolume):
			mWidget->rbActVolume->setChecked(true);
			break;
		case (YHConfig::ChangeTrack):
			mWidget->rbActTrack->setChecked(true);
			break;
	}
}

void YHModule::slotMwheelClicked(int index)
{
	kdDebug(66666) << k_funcinfo << endl;
	if (index == 0)
		mActionMap[mWidget->cmbModifier->currentItem()] = YHConfig::Nothing;
	else if (index == 1)
		mActionMap[mWidget->cmbModifier->currentItem()] = YHConfig::ChangeVolume;
	else
		mActionMap[mWidget->cmbModifier->currentItem()] = YHConfig::ChangeTrack;
}

#include "cmodule.moc"
