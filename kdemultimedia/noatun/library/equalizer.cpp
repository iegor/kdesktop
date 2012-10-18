#include "equalizer.h"
#include "engine.h"
#include <common.h>
#include <dynamicrequest.h>
#include <artsflow.h>
#include <app.h>
#include <player.h>
#include <soundserver.h>
#include <noatunarts.h>
#include <ktempfile.h>
#include <qdom.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <qtextstream.h>
#include <math.h>
#include <kconfig.h>
#include <klocale.h>
#include "ksaver.h"

#define EQ napp->equalizer()
#define VEQ napp->vequalizer()

struct OldEqCruft
{
	VInterpolation *interpolated;

};

static OldEqCruft *eqCruft=0;

Preset::Preset(const QString &)
{ } // unused

Preset::Preset(VPreset p)
{
	VPreset *copy = new VPreset(p);
	
	// isn't this horrible? :)
	mFile = QString::number((unsigned long)copy);
}

Preset::Preset()
{ } // unused

VPreset &Preset::vpreset() const
{
	unsigned long addr = mFile.toULong();
	return *(VPreset*)addr;
}

QString Preset::name() const
{
	return vpreset().name();
}

bool Preset::setName(const QString &name)
{
	return vpreset().setName(name);
}

bool Preset::save() const
{
	vpreset().save();
	return true;
}

bool Preset::load()
{
	vpreset().load();
	return true;
}

void Preset::remove()
{
	vpreset().remove();
}

QString Preset::file() const
{
	return vpreset().file();
}


Band::Band(int, int)
{
	// Never used
}

Band::Band(int num)
	: mNum(num)
{

}

Band::~Band()
{}

int Band::level()
{
	return eqCruft->interpolated->band(mNum).level();
}

void Band::setLevel(int l)
{
	eqCruft->interpolated->band(mNum).setLevel(l);
}

int Band::start() const
{
	return eqCruft->interpolated->band(mNum).start();
}

int Band::end() const
{
	return eqCruft->interpolated->band(mNum).end();
}

int Band::center() const
{
	return eqCruft->interpolated->band(mNum).center();
}

QString Band::formatStart(bool withHz) const
{
	return eqCruft->interpolated->band(mNum).formatStart(withHz);
}

QString Band::formatEnd(bool withHz) const
{
	return eqCruft->interpolated->band(mNum).formatEnd(withHz);
}

QString Band::format(bool withHz) const
{
	return eqCruft->interpolated->band(mNum).format(withHz);
}


Equalizer::Equalizer() 
{
}

Equalizer::~Equalizer()
{
	delete eqCruft->interpolated;
	delete eqCruft;

//	save(napp->dirs()->saveLocation("data", "noatun/") + "equalizer", "auto");
	for (Band *i=mBands.first(); i!=0; i=mBands.next())
		delete i;
}


void Equalizer::init()
{
	// must be called after VEqualizer::init 
	eqCruft = new OldEqCruft;
	eqCruft->interpolated = new VInterpolation(6);
	
	mBands.append(new Band(0));
	mBands.append(new Band(1));
	mBands.append(new Band(2));
	mBands.append(new Band(3));
	mBands.append(new Band(4));
	mBands.append(new Band(5));
	
	connect(VEQ, SIGNAL(changed()), SIGNAL(changed()));
	
	connect(VEQ, SIGNAL(created(VPreset)), SLOT(created(VPreset)));
	connect(VEQ, SIGNAL(selected(VPreset)), SLOT(selected(VPreset)));
	connect(VEQ, SIGNAL(renamed(VPreset)), SLOT(renamed(VPreset)));
	connect(VEQ, SIGNAL(removed(VPreset)), SLOT(removed(VPreset)));
	
	connect(VEQ, SIGNAL(enabled()), SIGNAL(enabled()));
	connect(VEQ, SIGNAL(disabled()), SIGNAL(disabled()));
	connect(VEQ, SIGNAL(enabled(bool)), SIGNAL(enabled(bool)));
	
	connect(VEQ, SIGNAL(preampChanged(int)), SIGNAL(preampChanged(int)));
	connect(VEQ, SIGNAL(preampChanged(int)), SIGNAL(preampChanged(int)));
}

void Equalizer::created(VPreset preset)
{
	Preset *p = new Preset(preset);
	emit created(p);
	delete p;
}

void Equalizer::selected(VPreset preset)
{
	Preset *p = new Preset(preset);
	emit changed(p);
	delete p;
}

void Equalizer::renamed(VPreset preset)
{
	Preset *p = new Preset(preset);
	emit renamed(p);
	delete p;
}

void Equalizer::removed(VPreset preset)
{
	Preset *p = new Preset(preset);
	emit removed(p);
	delete p;
}

QPtrList<Preset> Equalizer::presets() const
{
	QValueList<VPreset> presets = VEQ->presets();
	QPtrList<Preset> list;
	for (
			QValueList<VPreset>::Iterator i(presets.begin());
			i != presets.end(); ++i
		)
	{
		list.append(new Preset(*i));
	}
	return list;
}

Preset *Equalizer::preset(const QString &file)
{
	VPreset p = VEQ->presetByFile(file);
	if (!p) return 0;
	return new Preset(p);
}

bool Equalizer::presetExists(const QString &name) const
{
	return VEQ->presetExists(name);
}

Preset *Equalizer::createPreset(const QString &name, bool smart)
{
	VPreset p = VEQ->createPreset(name, smart);
	if (!p) return 0;
	return new Preset(p);
}

const QPtrList<Band> &Equalizer::bands() const
{
	return mBands;
}

Band *Equalizer::band(int num) const
{
	// can't use QPtrList::at since it sets current

	QPtrListIterator<Band> item(mBands);
	item+=(unsigned int)num;
	return *item;
}

int Equalizer::bandCount() const
{
	return 6; // hmm ;)
}

int Equalizer::preamp() const
{
	return VEQ->preamp();
}

bool Equalizer::isEnabled() const
{
	return VEQ->isEnabled();

}

void Equalizer::setPreamp(int p)
{
	VEQ->setPreamp(p);
}

void Equalizer::enable()
{
	setEnabled(true);
}

void Equalizer::disable()
{
	setEnabled(false);
}

void Equalizer::setEnabled(bool e)
{
	VEQ->setEnabled(e);
}

QString Equalizer::toString(const QString &name) const
{
	return VEQ->toString(name);
}

bool Equalizer::fromString(const QString &str)
{
	return VEQ->fromString(str);
}

bool Equalizer::save(const KURL &filename, const QString &name) const
{
	return VEQ->save(filename, name);
}



bool Equalizer::load(const KURL &filename)
{
	return VEQ->load(filename);
}

void Equalizer::add(Band *)
{
	// should never be called
}

void Equalizer::remove(Band *)
{
	// should never be called
}

void Equalizer::update(bool)
{
	// should never be called
}

void Equalizer::enableUpdates(bool)
{
	// should never be called
}

#undef EQ
#undef EQBACK

#include "equalizer.moc"

