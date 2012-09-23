// Copyright (c) 2001 Charles Samuels <charles@kde.org>
// Copyright (c) 2001 Neil Stevens <neil@qualityassistant.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "effects.h"
#include "effectview.h"
#include "app.h"

#include <kcombobox.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qdragobject.h>
#include <qheader.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qvgroupbox.h>
#include <qwhatsthis.h>

class EffectListItem : public QListViewItem
{
public:
	EffectListItem(QListView *parent, QListViewItem *after, Effect *e)
		: QListViewItem(parent, after, e->title()), mEffect(e)
	{
	}

	Effect *effect() const { return mEffect; }

private:
	Effect *mEffect;
};

EffectList::EffectList(QWidget *parent)
	: KListView(parent)
{
}

bool EffectList::acceptDrag(QDropEvent *event) const
{
	return QCString(event->format()) == "application/x-noatun-effectdrag";
}

QDragObject *EffectList::dragObject() const
{
	if (!currentItem()) return 0;
	return new QStoredDrag("application/x-noatun-effectdrag", (QWidget*)this);
}

EffectView::EffectView()
	: KDialogBase((QWidget*)0L, 0, false, i18n("Effects"), Help | Close, Close, true)
	, initialized(false)
{
}

void EffectView::show()
{
	init();
	KDialogBase::show();
}

namespace
{
QToolButton *newButton(const QIconSet &iconSet, const QString &textLabel, QObject *receiver, const char * slot, QWidget *parent, const char *name = 0)
{
	QToolButton *button = new QToolButton(parent, name);
	button->setIconSet(iconSet);
	button->setTextLabel(textLabel, true);
	QObject::connect(button, SIGNAL(clicked()), receiver, slot);
	button->setFixedSize(QSize(22, 22));
	return button;
}
}

void EffectView::init(void)
{
	if(initialized) return;
	initialized = true;

	setCaption(i18n("Effects - Noatun"));
	setIcon(SmallIcon("effect"));

	// Create widgets and layouts
	QFrame *box = makeMainWidget();
	QVBoxLayout *boxLayout = new QVBoxLayout(box, 0, KDialog::spacingHint());

	// Available
	QVGroupBox *topBox = new QVGroupBox(i18n("Available Effects"), box);
	topBox->setInsideSpacing(KDialog::spacingHint());

	QFrame *topTopFrame = new QFrame(topBox);
	QHBoxLayout *topTopLayout = new QHBoxLayout(topTopFrame, 0, KDialog::spacingHint());
	topTopLayout->setAutoAdd(true);
	available = new KComboBox(false, topTopFrame);
	QToolButton *add = newButton(BarIconSet("down", KIcon::SizeSmall), i18n("Add"), this, SLOT(addEffect()), topTopFrame);

	// Active
	QHGroupBox *bottomBox = new QHGroupBox(i18n("Active Effects"), box);
	bottomBox->setInsideSpacing(KDialog::spacingHint());

	active = new EffectList(bottomBox);

	boxLayout->addWidget(topBox);
	boxLayout->addWidget(bottomBox);

	// Fill and configure widgets
	available->insertStrList(napp->effects()->available());

	active->setAcceptDrops(true);
	active->addColumn("");
	active->header()->hide();
	active->setSorting(-1);
	active->setDropVisualizer(true);
	active->setItemsMovable(true);
	active->setSelectionMode(QListView::Single);
	active->setDragEnabled(true);
	connect(active, SIGNAL(dropped(QDropEvent *, QListViewItem *)), SLOT(activeDrop(QDropEvent *, QListViewItem *)));

	// when a new effect is added
	connect(napp->effects(), SIGNAL(added(Effect *)), SLOT(added(Effect *)));
	connect(napp->effects(), SIGNAL(removed(Effect *)), SLOT(removed(Effect *)));
	connect(napp->effects(), SIGNAL(moved(Effect *)), SLOT(moved(Effect *)));

	available->setCurrentItem(0);

	connect(active, SIGNAL(currentChanged(QListViewItem *)), SLOT(activeChanged(QListViewItem *)));
	active->setCurrentItem(0);

	// the buttons
	QFrame *bottomLeftFrame = new QFrame(bottomBox);
	QVBoxLayout *bottomLeftLayout = new QVBoxLayout(bottomLeftFrame, 0, KDialog::spacingHint());
	up = newButton(BarIconSet("up", KIcon::SizeSmall), i18n("Up"), this, SLOT(moveUp()), bottomLeftFrame);
	down = newButton(BarIconSet("down", KIcon::SizeSmall), i18n("Down"), this, SLOT(moveDown()), bottomLeftFrame);
	configure = newButton(BarIconSet("configure", KIcon::SizeSmall), i18n("Configure"), this, SLOT(configureEffect()), bottomLeftFrame);
	remove = newButton(BarIconSet("remove", KIcon::SizeSmall), i18n("Remove"), this, SLOT(removeEffect()), bottomLeftFrame);
	bottomLeftLayout->addWidget(up);
	bottomLeftLayout->addWidget(down);
	bottomLeftLayout->addWidget(configure);
	bottomLeftLayout->addWidget(remove);
	bottomLeftLayout->addStretch();


	activeChanged(active->currentItem());

	// Inline documentation
	QWhatsThis::add(available, i18n("This shows all available effects.\n\nTo activate a plugin, drag files from here to the active pane on the right."));
	QWhatsThis::add(add, i18n("This will place the selected effect at the bottom of your chain."));
	QWhatsThis::add(active, i18n("This shows your effect chain. Noatun supports an unlimited amount of effects in any order. You can even have the same effect twice.\n\nDrag the items to and from here to add and remove them, respectively. You may also reorder them with drag-and-drop. These actions can also be performed with the buttons to the right."));
	QWhatsThis::add(up, i18n("Move the currently selected effect up in the chain."));
	QWhatsThis::add(down, i18n("Move the currently selected effect down in the chain."));
	QWhatsThis::add(configure, i18n("Configure the currently selected effect.\n\nYou can change things such as intensity from here."));
	QWhatsThis::add(remove, i18n("This will remove the selected effect from your chain."));

	resize(300, 400);
}

void EffectView::activeDrop(QDropEvent *, QListViewItem *pafter)
{
	EffectListItem *after(static_cast<EffectListItem*>(pafter));
	napp->effects()->move(after ? after->effect() : 0,
	                      static_cast<EffectListItem*>(active->currentItem())->effect());
	activeChanged(active->currentItem());
}

QListViewItem *EffectView::toListItem(Effect *e) const
{
	for(QListViewItem *i = active->firstChild(); i; i = i->itemBelow())
		if(static_cast<EffectListItem*>(i)->effect() == e)
			return i;
	return 0;
}

void EffectView::added(Effect *item)
{
	new EffectListItem(active, toListItem(item->before()), item);
	activeChanged(active->currentItem());
}

void EffectView::moved(Effect *item)
{
	delete toListItem(item);
	added(item);
}

void EffectView::removed(Effect *item)
{
	delete toListItem(item);
	activeChanged(active->currentItem());
}

void EffectView::moveDown()
{
	Effect *e = static_cast<EffectListItem*>(active->currentItem())->effect();

	if(e->after())
		napp->effects()->move(e->after(), e);
	active->setCurrentItem(toListItem(e));
	active->setSelected(toListItem(e), true);
	activeChanged(active->currentItem());
}

void EffectView::moveUp()
{
	Effect *e = static_cast<EffectListItem*>(active->currentItem())->effect();
	if (e->before())
	{
		if (e->before()->before())
			napp->effects()->move(e->before()->before(), e);
		else
			napp->effects()->move(0, e);
	}
	active->setCurrentItem(toListItem(e));
	active->setSelected(toListItem(e), true);
	activeChanged(active->currentItem());
}

void EffectView::removeEffect()
{
	EffectListItem *item = static_cast<EffectListItem*>(active->currentItem());
	napp->effects()->remove(item->effect());
	activeChanged(active->currentItem());
}

void EffectView::addEffect()
{
	// local8Bit() and arts makes me nervous
	napp->effects()->append(new Effect(available->currentText().local8Bit()));
	activeChanged(active->currentItem());
}

void EffectView::configureEffect()
{
	Effect *e = static_cast<EffectListItem*>(active->currentItem())->effect();
	if(!e) return;

	QWidget *c = e->configure();
	if(c) c->show();
}

void EffectView::activeChanged(QListViewItem *i)
{
	if(i)
	{
		up->setEnabled(i->itemAbove());
		down->setEnabled(i->itemBelow());
		remove->setEnabled(true);

		Effect *e = static_cast<EffectListItem*>(active->currentItem())->effect();
		configure->setEnabled(e->configurable());
	}
	else
	{
		up->setEnabled(false);
		down->setEnabled(false);
		remove->setEnabled(false);
		configure->setEnabled(false);
	}
}

#include "effectview.moc"
