#include "vequalizer.h"
#define EQVIEW_CPP
#include "equalizerview.h"
#undef EQVIEW_CPP
#include "equalizerwidget.h"
#include "app.h"

#include <knuminput.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qlayout.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qframe.h>
#include <qgroupbox.h>

#define EQ (napp->vequalizer())


////////////////////////////////////////////////
// PresetList

PresetList::PresetList(QWidget *parent, const char *name)
	: KListView(parent, name)
{
	setItemsRenameable(true);
	setRenameable(0, true);
	addColumn(""); // first column is the visible one
	addColumn("", 0); // create one column to store cruft in
	setColumnWidthMode(0, QListView::Maximum);
	header()->setStretchEnabled(true,0);
	header()->hide();
	// a try to set a sne minimum width. unfortuately the custom item
	// still doesn't draw all text with that minimum width
	setMinimumWidth(kapp->fontMetrics().boundingRect(i18n("Custom")).width()+2*itemMargin());
}

void PresetList::rename(QListViewItem *item, int c)
{
	// We can't rename the "Custom" metapreset
	if (item->text(0)==i18n("Custom"))
		return;

	// Or presets we don't have write access to
	if (!QFileInfo(item->text(1)).isWritable())
		return;

	KListView::rename(item, c);
}

////////////////////////////////////////////////
// EqualizerLevel

EqualizerLevel::EqualizerLevel(QWidget *parent, VBand band)
	: QWidget(parent), mBand(band)
{
	QVBoxLayout *layout = new QVBoxLayout(this,
		0, 0, "EqualizerLevel::layout");

	mSlider = new QSlider(-200, 200, 25, 0, Qt::Vertical, this, "EqualizerLevel::mSlider");
	mSlider->setTickmarks(QSlider::Left);
	mSlider->setTickInterval(25);
	layout->addWidget(mSlider);
	connect(mSlider, SIGNAL(valueChanged(int)), SLOT(changed(int)));
	mLabel = new QLabel("", this, "EqualizerLevel::mLabel");
	mLabel->setAlignment(AlignHCenter | AlignVCenter);
	layout->addWidget(mLabel);

	setMinimumHeight(200);
//	setMinimumWidth(kapp->fontMetrics().width("158kHz"));
//	setMinimumWidth(kapp->fontMetrics().width("549kHz"));

	setBand(band);

	connect(EQ, SIGNAL(modified()), SLOT(changed()));
	connect(mSlider, SIGNAL(valueChanged(int)), SLOT(changed(int)));
}

void EqualizerLevel::setBand(VBand band)
{
	mBand = band;
	mLabel->setText(band.format());
	changed();
}

void EqualizerLevel::changed()
{
	mSlider->setValue(-mBand.level());
}

void EqualizerLevel::changed(int v)
{
	mBand.setLevel(-v);
}


///////////////////////////////////////////////
// EqualizerView

EqualizerView::EqualizerView()
	: KDialogBase(0L, "EqualizerView", false, i18n("Equalizer"), Help | Close, Close, true),
	  first(true), mWidget(0), bandsLayout(0), mPresets(0), mGoingPreset(false)
{
	mBands.setAutoDelete(true);
}

void EqualizerView::show()
{
	if (first)
	{
		first = false;
		setIcon(SmallIcon("noatun"));
		mWidget = new EqualizerWidget(this, "mWidget");
		setMainWidget(mWidget);

		bandsLayout = new QHBoxLayout(mWidget->bandsFrame,
			0, KDialog::spacingHint(), "bandsLayout");

		connect(mWidget->preampSlider, SIGNAL(valueChanged(int)),
			this, SLOT(setPreamp(int)));
		connect(EQ, SIGNAL(preampChanged(int)),
			this, SLOT(changedPreamp(int)));

		mWidget->bandCount->setRange(EQ->minBands(), EQ->maxBands());
		connect(mWidget->bandCount, SIGNAL(valueChanged(int)),
			EQ, SLOT(setBands(int)));

		QVBoxLayout *l = new QVBoxLayout(mWidget->presetFrame);
		mPresets = new PresetList(mWidget->presetFrame, "mPresets");
		l->addWidget(mPresets);

		connect(mWidget->removePresetButton, SIGNAL(clicked()), SLOT(remove()));
		connect(mWidget->addPresetButton, SIGNAL(clicked()), SLOT(create()));
		connect(mWidget->resetEqButton, SIGNAL(clicked()), SLOT(reset()));

		new KListViewItem(mPresets, i18n("Custom"));

		connect(mPresets, SIGNAL(currentChanged(QListViewItem*)),
			this, SLOT(select(QListViewItem*)));

		connect(mPresets, SIGNAL(itemRenamed(QListViewItem*)),
			this, SLOT(rename(QListViewItem*)));

		// populate the preset list
		QValueList<VPreset> presets = EQ->presets();
		QValueList<VPreset>::Iterator it;
		for (it=presets.begin(); it!=presets.end(); ++it)
		{
			created(*it);
		}

		connect(EQ, SIGNAL(created(VPreset)), SLOT(created(VPreset)));
		connect(EQ, SIGNAL(renamed(VPreset)), SLOT(renamed(VPreset)));
		connect(EQ, SIGNAL(removed(VPreset)), SLOT(removed(VPreset)));

		mWidget->enabledCheckBox->setChecked(EQ->isEnabled());
		connect(mWidget->enabledCheckBox, SIGNAL(toggled(bool)),
			EQ, SLOT(setEnabled(bool)));
		connect(EQ, SIGNAL(enabled(bool)),
			mWidget->enabledCheckBox, SLOT(setChecked(bool)));

		connect(EQ, SIGNAL(changed()),
			this, SLOT(changedEq()));
		connect(EQ, SIGNAL(changedBands()),
			this, SLOT(changedBands()));

		changedBands();
		changedEq();
	} // END if(first)

	if (isVisible())
		raise();
	else
		KDialogBase::show();
}

QListViewItem *EqualizerView::itemFor(const QString &filename)
{
	for (QListViewItem *i=mPresets->firstChild(); i!=0; i=i->itemBelow())
	{
		QString t = i->text(1);
		if ((t.length()==0 && filename.length()==0) || t==filename)
			return i;
	}
	return 0;
}

QListViewItem *EqualizerView::itemFor(const VPreset &preset)
{
	return itemFor(preset.file());
}

// why is it that when you move a QSlider up, it goes down?
void EqualizerView::setPreamp(int x)
{
	EQ->setPreamp(-x);
}

void EqualizerView::changedPreamp(int x)
{
	mWidget->preampSlider->setValue(-x);
}


void EqualizerView::changedBands()
{
	mBands.clear();

	VEqualizer &eq = *EQ;
	for (int i=0; i < eq.bands(); ++i)
	{
		EqualizerLevel *l = new EqualizerLevel(mWidget->bandsFrame, eq[i]);
		bandsLayout->addWidget(l);
		l->show();
		mBands.append(l);
	}

	mWidget->bandCount->setValue(eq.bands());
	changedEq();
}

void EqualizerView::changedEq()
{
	if (!mGoingPreset)
	{
		QListViewItem *customitem = itemFor("");
		if (!customitem) // this should never happen!
			return;
		mPresets->setSelected(customitem, true);
	}
}

void EqualizerView::removed(VPreset p)
{
	delete itemFor(p);
}

void EqualizerView::created(VPreset p)
{
	// store the filename in QListViewItem::text(0)
	QString n = p.name();
	QString f = p.file();
	new KListViewItem(mPresets, n, f);
}

void EqualizerView::renamed(VPreset p)
{
	QListViewItem *renamed = itemFor(p);
	if (!renamed) // WTF !
	{
		created(p);
		return;
	}
	renamed->setText(0, p.name());
}

void EqualizerView::remove()
{
	QListViewItem *current=mPresets->currentItem();
	if (current->text(0)==i18n("Custom"))
		return;
	QListViewItem *then=current->itemAbove();
	if (!then) then=current->itemBelow();

	if (then)
		mPresets->setSelected(then, true);

	VPreset p = EQ->presetByFile(current->text(1));
	p.remove();
}

void EqualizerView::create()
{
	VPreset p = EQ->createPreset(i18n("New Preset"));

	mGoingPreset = true;

	// Load the new preset
	p.load();

	// We should have just made a list view item for this preset
	// See EquailizerView::presetAdded()
	QListViewItem *i = itemFor(p);

	if (i)
		mPresets->setSelected(i, true);

	mGoingPreset = false;
}

void EqualizerView::reset()
{
	VEqualizer &eq = *EQ;
	eq.setPreamp(0);
	for (int i=0; i < eq.bands(); ++i)
		eq.band(i).setLevel(0);

}

void EqualizerView::rename(QListViewItem *item)
{
	EQ->presetByFile(item->text(1)).setName(item->text(0));
	item->setText(0, EQ->presetByFile(item->text(1)).name());
}

void EqualizerView::select(QListViewItem *item)
{
	mGoingPreset = true;
	EQ->presetByFile(item->text(1)).load();
	mGoingPreset = false;
	mWidget->removePresetButton->setEnabled(item->text(1).length());
}

#undef EQ
#include "equalizerview.moc"
