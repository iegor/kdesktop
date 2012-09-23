#include "tags.h"
#include "tagsgetter.h"
#include <klocale.h>
#include <qslider.h>
#include <qspinbox.h>
#include <kconfig.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qtimer.h>
#include <noatun/player.h>
#include <qcheckbox.h>

TagsGetter *Tags::getter=0;

TagsGetter::TagsGetter()
{
	new Control(this);
	connect(napp->player(), SIGNAL(newSong()), SLOT(newSong()));
}

int TagsGetter::interval() const
{
	KGlobal::config()->setGroup("Tags");
	return KGlobal::config()->readNumEntry("interval", 250);
}

bool TagsGetter::loadAuto() const
{
	KGlobal::config()->setGroup("Tags");
	return KGlobal::config()->readBoolEntry("LoadAuto", true);
}

void TagsGetter::added(PlaylistItem &i)
{
	items += i;
	killTimers();
	startTimer(interval());
}

void TagsGetter::removed(PlaylistItem &i)
{
	items.remove(i);
}

void TagsGetter::getSongs()
{
	items = napp->playlist()->select("Tags::tagged_", "", -1, true, true);
	killTimers();
	startTimer(interval());
}

void TagsGetter::timerEvent(QTimerEvent *)
{
	if (!items.size())
	{
		killTimers();
		return;
	}

	PlaylistItem item=items.first();
	for (Tags *i=tags.first(); i; i=tags.next())
	{
		if (i->update(item))
		{
			item.setProperty("Tags::tagged_", "1");
			if (item==napp->player()->current())
				napp->player()->handleButtons();
		}
	}

	items.remove(items.begin());
}

void TagsGetter::newSong()
{
	PlaylistItem item=napp->player()->current();
	if (!item) return;
	
	for (Tags *i=tags.first(); i; i=tags.next())
	{
		if (i->update(item))
		{
			item.setProperty("Tags::tagged_", "1");
			napp->player()->handleButtons();
		}
	}
	items.remove(item);
}

void TagsGetter::setInterval(int ms)
{
	killTimers();
	startTimer(ms);
	
	KGlobal::config()->setGroup("Tags");
	KGlobal::config()->writeEntry("interval", ms);
	KGlobal::config()->sync();
}

void TagsGetter::setLoadAuto(bool eh)
{
	
	KGlobal::config()->setGroup("Tags");
	KGlobal::config()->writeEntry("LoadAuto", eh);
	KGlobal::config()->sync();
	
	killTimers();
	
	if (eh) startTimer(interval());
}

void TagsGetter::associate(Tags *t)
{
	tags.append(t);
	sortPriority();
//	getSongs();
	QTimer::singleShot(interval(), this, SLOT(getSongs()));
}

void TagsGetter::sortPriority()
{
	// find the lowest one, since it comes first
	
	int lowest=0;
	for (Tags *i=tags.first(); i; i=tags.next())
	{
		if (lowest>i->mPriority)
			lowest=i->mPriority;
	}
	
	QPtrList<Tags> sorted;
	while (tags.count())
	{
		// find the one equal to myself
		for (Tags *i=tags.first(); i;)
		{
			if (lowest==i->mPriority)
			{
				sorted.append(i);
				tags.removeRef(i);
				i=tags.first();
			}
			else
			{
				i=tags.next();
			}
		}
		lowest++;
	}
	
	tags=sorted;
}

bool TagsGetter::unassociate(Tags *t)
{
	tags.removeRef(t);
	if (tags.count()==0)
	{
		delete this;
		return true;
	}
	return false;
}

Tags::Tags(int priority) : mPriority(priority)
{
	if (!getter)
		getter=new TagsGetter;
	getter->associate(this);
}

Tags::~Tags()
{
	if (getter->unassociate(this))
		getter=0;
}


Control::Control(TagsGetter *parent)
	: CModule(i18n("Tagging"), i18n("Settings for Tag Loaders"), "edit", parent)
{
	// todo
	(void)I18N_NOOP("Rescan All Tags");
	
	QVBoxLayout *l=new QVBoxLayout(this);
	QCheckBox *onPlay;
	{
		onPlay=new QCheckBox(i18n("Load tags &automatically"), this);
		l->addWidget(onPlay);
		onPlay->show();
	}
	
	{
		QHBox *intervalLine=new QHBox(this);
		l->addWidget(intervalLine);
		l->addStretch();
	
		new QLabel(i18n(
				"The time between each time noatun scans for a new file"
				", and updates tags (e.g., ID3)",
				"Interval:"), intervalLine);
		QSlider *slider=new QSlider(
				0, 2000, 100, 0, Horizontal, intervalLine
			);
		QSpinBox *spin=new QSpinBox(
				0, 2000, 10, intervalLine
			);
		
		spin->setSuffix(i18n("Milliseconds", " ms"));
		
		
		connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
		connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
		slider->setValue(parent->interval());
		connect(slider, SIGNAL(valueChanged(int)), parent, SLOT(setInterval(int)));
	
		connect(onPlay, SIGNAL(toggled(bool)), intervalLine, SLOT(setEnabled(bool)));
	}
	connect(onPlay, SIGNAL(toggled(bool)), parent, SLOT(setLoadAuto(bool)));

	onPlay->setChecked(parent->loadAuto());
}




#include "tagsgetter.moc"

