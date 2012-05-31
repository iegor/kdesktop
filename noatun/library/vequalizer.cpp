/*
 * Copyright (c) 2003 Charles Samuels <charles@kde.org>
 *
 * This file is hereby licensed under the GNU General Public License version
 * 2 or later at your option.
 *
 * This file is licensed under the Qt Public License version 1 with the
 * condition that the licensed will be governed under the Laws of California
 * (USA) instead of Norway.  Disputes will be settled in Santa Clara county
 * courts.
 *
 * This file is licensed under the following additional license.  Be aware
 * that it is identical to the BSD license, except for the added clause 3:

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. By integrating this software into any other software codebase, you waive
    all rights to any patents associated with the stated codebase.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "vequalizer.h"
#include "engine.h"
#include "spline.h"
#include "ksaver.h"

#include <noatunarts.h>
#include <app.h>
#include <player.h>

#include <common.h>
#include <dynamicrequest.h>
#include <artsflow.h>
#include <soundserver.h>

#include <ktempfile.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <klocale.h>

#include <qdom.h>
#include <qtextstream.h>
#include <qfile.h>

#include <math.h>

#define EQ (napp->vequalizer())
#define EQBACK (napp->player()->engine()->equalizer())

using std::vector;

static const double splineTension = 4.0;

VBandsInterface::VBandsInterface()
{

}

VBandsInterface::~VBandsInterface()
{

}


VBand VBandsInterface::operator [] (int num)
{
	return band(num);
}

struct VBand::Private
{
	int refs;

	int index;
	int start, end;
	VBandsInterface *bands;
};

VBand::VBand(VBandsInterface *bands, int index, int start, int end)
{
	d = new Private;
	d->refs=1;
	d->index = index;
	d->start = start;
	d->end = end;
	d->bands = bands;
}

VBand::~VBand()
{
	if (--d->refs == 0)
	{
		delete d;
	}
}

VBand::VBand(const VBand &copy)
{
	d=0;
	operator=(copy);
}

VBand & VBand::operator =(const VBand &copy)
{
	if (d && --d->refs == 0)
	{
		delete d;
	}

	d= copy.d;
	d->refs++;
	return *this;
}

int VBand::level() const
{
	return d->bands->level(d->index);
}

void VBand::setLevel(int l)
{
	d->bands->setLevel(d->index, l);
}

int VBand::start() const
{
	return d->start;
}

int VBand::end() const
{
	return d->end;
}

int VBand::center() const
{
	return (d->start + d->end)/2;
}

static QString formatFreq(int f, bool withHz)
{
	QString format;
	if (f<991)
		format=QString::number(f);
	else
		format=QString::number((int)((f+500)/1000.0))+"k";

	if (withHz)
		format+="Hz";

	return format;
}

QString VBand::formatStart(bool withHz) const
{
	return formatFreq(d->start, withHz);
}

QString VBand::formatEnd(bool withHz) const
{
	return formatFreq(d->end, withHz);
}

QString VBand::format(bool withHz) const
{
	return formatFreq(center(), withHz);
}




struct VInterpolation::Private
{
	int bands;
	Spline spline;

};

VInterpolation::VInterpolation(int bands)
{
	d = new Private;
	d->bands = bands;
}

VInterpolation::~VInterpolation()
{
	delete d;
}

int VInterpolation::bands() const
{
	return d->bands;
}

void VInterpolation::getFrequencies(int num, int *low, int *high) const
{
	QValueList<int> fs = VEqualizer::frequencies(bands());

	if (num == 0) *low = 1;
	else *low = fs[num-1]+1;
	*high=fs[num];
}


VBand VInterpolation::band(int num)
{
	int low, high;
	getFrequencies(num, &low, &high);
	return VBand(this, num, low, high);
}

int VInterpolation::level(int index) const
{
	const_cast<VInterpolation*>(this)->refresh();
	double x = onSpline(index);

	return int(d->spline[x*splineTension]);
}

void VInterpolation::setLevel(int index, int level)
{
	refresh();

	double numbands = double(bands());
	Spline spline;

	for (int i=0; i < numbands; ++i)
	{
		VBand b = band(i);
		spline.add(i*splineTension, double(index == i ? level : b.level()));
	}

	int realbands = EQ->bands();
	QValueList<int> values;
	for (int i=0; i < realbands; ++i)
	{
		//  i
		// realbands  numbands
		double x = double(i)*numbands/double(realbands)*splineTension;
		double value = spline[x];
		values.append(int(value));
	}
	EQ->setLevels(values);
}

double VInterpolation::onSpline(int bandNum) const
{
	double maxReal(EQ->bands()); // 2
	double maxInter(bands());    // 4
	double offset(bandNum);      // 4

	return maxReal/maxInter*offset;
}

void VInterpolation::refresh()
{
	d->spline.clear();

	VEqualizer *eq = EQ;

	for (int i=0; i < eq->bands(); ++i)
	{
		VBand band = eq->band(i);
		d->spline.add(double(i*splineTension), double(band.level()));
	}

}

struct VEqualizer::Private
{
	struct BandInfo
	{
		int level;
		int start;
		int end;
	};

	std::vector<BandInfo> bands;
	int preamp;
};

/* rate 4
27       54     0-108      108
81       163    109-217    108
243      514    218-810    269
729      1621   811-2431  1620
2187     4661   2432-7290 4858
6561     13645  7291+     12708
*/
VEqualizer::VEqualizer()
{
	d = new Private;
	d->preamp=1;
	setBands(6, false);
}

void VEqualizer::init()
{
	KURL url;
	url.setPath(kapp->dirs()->localkdedir()+"/share/apps/noatun/equalizer");
	if(!load(url))
	{
		setPreamp(0);
		disable();
	}
	else
	{
		KConfig *config=kapp->config();
		config->setGroup("Equalizer");
		setEnabled(config->readBoolEntry("enabled", false));
	}
}


VEqualizer::~VEqualizer()
{
	KURL url;
	url.setPath(kapp->dirs()->localkdedir()+"/share/apps/noatun/equalizer");
	save(url, "auto");

	delete d;
}

int VEqualizer::start()
{
	return 20;
}

int VEqualizer::end()
{
	return 20000;
}

int VEqualizer::maxBands() const
{
	return 14;
}

int VEqualizer::minBands() const
{
	return 2;
}


QValueList<int> VEqualizer::frequencies(int _num)
{
#if 0
	QValueList<int> fs;
	fs += 108;
	fs += 217;
	fs += 810;
	fs += 2431;
	fs += 7290;
	fs += 19999;
	return fs;

#else



	// we're looking for
	// ?^_num = end()-start()
	// so log[?] (end()-start()) = _num
	// log(end()-start()) / log(?) = _num
	// log(end()-start()) = _num * log(?)
	// log(end()-start()) / _num = log(?)
	// ? = 10^(log(end()-start()) / _num)

	double num = double(_num);
	double vstart = double(start());
	double vend = double(end());
	const double base = ::pow(10.0, ::log10(vend-vstart)/num);

	QValueList<int> fs;

	for (double i=1.0; i <= num; i++)
	{
		double f = ::pow(base, i);
		f += vstart;
		fs.append(int(f));
	}

	return fs;
#endif
}

void VEqualizer::setBands(int num, bool interpolate)
{
	if (interpolate)
	{
		setBands(num);
		return;
	}

	if (num > maxBands())
	num=maxBands();
	else if (num < minBands()) num = minBands();

	if (num == bands()) return;

	QValueList<int> fs = VEqualizer::frequencies(num);
	std::vector<Private::BandInfo> modified;

	int bstart=0;
	for (QValueList<int>::Iterator i(fs.begin()); i != fs.end(); ++i)
	{
		Private::BandInfo info;
		info.start = bstart+1;
		info.level = 0;
		info.end = *i;
		bstart = info.end;

		modified.push_back(info);
	}
	d->bands = modified;
	update(true);

	emit changedBands();
	emit changed();
}

void VEqualizer::setPreamp(int preamp)
{
	d->preamp = preamp;
	EQBACK->preamp(pow(2,float(preamp)/100.0));
	emit changed();
	emit preampChanged();
	emit preampChanged(preamp);
}


void VEqualizer::setBands(int num)
{
	if (num == bands()) return;
	VInterpolation ip(num);

	std::vector<Private::BandInfo> modified;

	for (int i=0; i < num; ++i)
	{
		Private::BandInfo info;
		VBand b = ip[i];
		info.level = b.level();
		info.start = b.start();
		info.end = b.end();

		modified.push_back(info);
	}
	d->bands = modified;
	update(true);

	emit changedBands();
	emit changed();
}

VBand VEqualizer::band(int num)
{
	return VBand(this, num, d->bands[num].start, d->bands[num].end);
}

int VEqualizer::bands() const
{
	return d->bands.size();
}

bool VEqualizer::isEnabled() const
{
	return bool(EQBACK->enabled());
}

int VEqualizer::preamp() const
{
	return d->preamp;
}

void VEqualizer::enable()
{
	setEnabled(true);
}

void VEqualizer::disable()
{
	setEnabled(false);
}

int VEqualizer::level(int index) const
{
	return d->bands[index].level;
}

void VEqualizer::setLevel(int index, int level)
{
	d->bands[index].level = level;
	update();
	emit changed();
	emit modified();
}

void VEqualizer::setLevels(const QValueList<int> &levels)
{
	int index=0;
	for (
			QValueList<int>::ConstIterator i(levels.begin());
			i != levels.end(); ++i
		)
	{
		d->bands[index].level = *i;
		index++;
	}
	update();
	emit changed();
	emit modified();
}


void VEqualizer::setEnabled(bool e)
{
	update(true); // just in case
	EQBACK->enabled((long)e);
	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	config->writeEntry("enabled", e);
	config->sync();

	emit enabled(e);
	if (e)
		emit enabled();
	else
		emit disabled();
}

void VEqualizer::update(bool full)
{
	std::vector<float> levels;
	std::vector<float> mids;
	std::vector<float> widths;

	for (unsigned int i=0; i < d->bands.size(); ++i)
	{
		Private::BandInfo &info = d->bands[i];
		levels.push_back(::pow(2.0, float(info.level)/50.0));
		if (full)
		{
			int mid = info.start + info.end;
			mids.push_back(float(mid)*0.5);
			widths.push_back(float(info.end - info.start));
		}
	}
	if (full)
		EQBACK->set(levels, mids, widths);
	else
		EQBACK->levels(levels);
}



bool VEqualizer::save(const KURL &filename, const QString &friendly) const
{
	Noatun::KSaver saver(filename);
	if(!saver.open()) return false;

	saver.textStream() << toString(friendly);
	saver.close();

	return saver.close();
}

bool VEqualizer::load(const KURL &filename)
{
	QString dest;
	if(KIO::NetAccess::download(filename, dest, 0L))
	{
		QFile file(dest);
		if (!file.open(IO_ReadOnly))
			return false;

		QTextStream t(&file);
		QString str = t.read();
		fromString(str);
		return true;
	}
	return false;
}

QString VEqualizer::toString(const QString &name) const
{
	QDomDocument doc("noatunequalizer");
	doc.setContent(QString("<!DOCTYPE NoatunEqualizer><noatunequalizer/>"));
	QDomElement docElem = doc.documentElement();

	{
		docElem.setAttribute("level", preamp());
		docElem.setAttribute("name", name);
		docElem.setAttribute("version", napp->version());
	}

	int bandc = bands();
	for (int i=0; i < bandc; ++i)
	{
		VBand band = const_cast<VEqualizer*>(this)->operator[](i);
		QDomElement elem = doc.createElement("band");
		elem.setAttribute("start", band.start());
		elem.setAttribute("end", band.end());
		elem.setAttribute("level", band.level());

		docElem.appendChild(elem);
	}

	return doc.toString();
}

bool VEqualizer::fromString(const QString &str)
{
	QDomDocument doc("noatunequalizer");
	if (!doc.setContent(str))
		return false;

	QDomElement docElem = doc.documentElement();
	if (docElem.tagName()!="noatunequalizer")
		return false;

	setPreamp(docElem.attribute("level", "0").toInt());

	std::vector<Private::BandInfo> modified;
	for (QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if(e.isNull()) continue;
		if (e.tagName().lower() != "band") continue;

		Private::BandInfo data;

		data.level = e.attribute("level", "0").toInt();
		data.start = e.attribute("start", "1").toInt();
		data.end = e.attribute("end", "19999").toInt();

		modified.push_back(data);
	}
	d->bands = modified;
	update(true);

	emit changedBands();
	emit changed();
	return true;
}

static QString makePresetFile()
{
	QString basedir=kapp->dirs()->localkdedir()+"/share/apps/noatun/eq.preset/";
	// now append a filename that doesn't exist
	KStandardDirs::makeDir(basedir);
	QString fullpath;
	int num=0;
	do
	{
		if (num)
			fullpath=basedir+"preset."+QString::number(num);
		else
			fullpath=basedir+"preset";
		num++;
	}
	while (QFile(fullpath).exists());
	return fullpath;
}

VPreset VEqualizer::createPreset(const QString &name, bool smart)
{
	if (presetExists(name) && !smart) return VPreset();
	QString nameReal=name;
	{
		int number=1;
		while (presetExists(nameReal))
		{
			nameReal=name+" ("+QString::number(number)+')';
			number++;
		}
	}

	VPreset preset(makePresetFile());
	preset.setName(nameReal);

	save(preset.file(), nameReal);
	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	QStringList list = config->readListEntry("presets");
	list += preset.file();
	config->writeEntry("presets", list);
	config->sync();

	emit created(preset);
	return preset;
}


QValueList<VPreset> VEqualizer::presets() const
{
	KConfig *conf=KGlobal::config();
	conf->setGroup("Equalizer");

	QStringList list;
	if (conf->hasKey("presets"))
	{
		list=conf->readListEntry("presets");
	}
	else
	{
		list=kapp->dirs()->findAllResources("data", "noatun/eq.preset/*");
		conf->writeEntry("presets", list);
		conf->sync();
	}

	QValueList<VPreset> presets;

	for (QStringList::Iterator i = list.begin(); i!=list.end(); ++i)
	{
		QFile file(*i);
		if (!file.open(IO_ReadOnly)) continue;

		QDomDocument doc("noatunequalizer");
		if (!doc.setContent(&file)) continue;

		QDomElement docElem = doc.documentElement();
		if (docElem.tagName()!="noatunequalizer") continue;

		presets.append(VPreset(*i));
	}
	return presets;
}

VPreset VEqualizer::presetByName(const QString &name)
{
	QValueList<VPreset> pr = presets();
	for (
			QValueList<VPreset>::Iterator i(pr.begin());
			i != pr.end(); ++i
		)
	{
		if ((*i).name() == name)
			return *i;
	}
	return VPreset();
}

VPreset VEqualizer::presetByFile(const QString &file)
{
	KConfig *conf=KGlobal::config();
	conf->setGroup("Equalizer");
	QStringList list=kapp->config()->readListEntry("presets");
	if (list.contains(file))
		return VPreset(file);
	return VPreset();
}

bool VEqualizer::presetExists(const QString &name) const
{
	QValueList<VPreset> list=presets();
	for (
			QValueList<VPreset>::Iterator i(list.begin());
			i != list.end(); ++i
		)
	{
		if ((*i).name() == name)
			return true;
	}
	return false;
}



struct VPreset::Private
{
	QString file;
};



VPreset::VPreset(const QString &file)
{
	d = new Private;
	d->file = file;

}


VPreset::VPreset()
{
	d = new Private;
}

VPreset::VPreset(const VPreset &copy)
{
	d = new Private;
	operator =(copy);
}

VPreset::~VPreset()
{
	delete d;
}

bool VPreset::operator ==(const VPreset &other) const
{
	return name() == other.name();
}

VPreset & VPreset::operator=(const VPreset &copy)
{
	d->file = copy.file();
	return *this;
}

QString VPreset::name() const
{
	QFile file(d->file);
	if (!file.open(IO_ReadOnly)) return 0;

	QDomDocument doc("noatunequalizer");
	if (!doc.setContent(&file)) return 0;

	QDomElement docElem = doc.documentElement();
	if (docElem.tagName()!="noatunequalizer") return 0;
	bool standard=docElem.attribute("default", "0")=="0";
	QString n=docElem.attribute("name", 0);

	{ // All the translations for the presets
#		ifdef I18N_STUFF
		I18N_NOOP("Trance");
		I18N_NOOP("Dance");
		I18N_NOOP("Metal");
		I18N_NOOP("Jazz");
		I18N_NOOP("Zero");
		I18N_NOOP("Eclectic Guitar");
#		endif
	}

	if (standard)
		n=i18n(n.local8Bit());

	return n;
}

bool VPreset::setName(const QString &name)
{
	QFile file(d->file);
	if (!file.open(IO_ReadOnly)) return false;

	QDomDocument doc("noatunequalizer");
	if (!doc.setContent(&file)) return false;

	QDomElement docElem = doc.documentElement();
	if (docElem.tagName()!="noatunequalizer") return false;

	if (docElem.attribute("name") == name) return true;
	if (EQ->presetByName(name)) return false;

	docElem.setAttribute("name", name);
	file.close();

	if (!file.open(IO_ReadWrite | IO_Truncate)) return false;

	QTextStream s(&file);
	s << doc.toString();
	file.close();

	emit EQ->renamed(*this);

	return true;
}

bool VPreset::isValid() const
{
	return d->file.length();
}

void VPreset::save()
{
	KURL url;
	url.setPath(d->file);
	EQ->load(url);
}

void VPreset::load() const
{
	KURL url;
	url.setPath(d->file);
	EQ->load(url);
}

QString VPreset::file() const
{
	return d->file;
}

void VPreset::remove()
{
	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	QStringList items=config->readListEntry("presets");
	items.remove(file());
	config->writeEntry("presets", items);
	config->sync();

	emit EQ->removed(*this);

	if (file().find(kapp->dirs()->localkdedir())==0)
	{
		QFile f(file());
		f.remove();
	}
	d->file = "";
}


#undef EQ
#undef EQBACK

#include "vequalizer.moc"

