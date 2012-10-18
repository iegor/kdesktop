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

#ifndef NOATUN_VEQUALIZER_H
#define NOATUN_VEQUALIZER_H

#include <qptrlist.h>
#include <qobject.h>
#include <kurl.h>
#include <kdemacros.h>
class VBand;

/**
 * Abstract base class for VInterpolation and VEqualizer
 **/
class VBandsInterface
{
	friend class VBand;

public:
	VBandsInterface();
	virtual ~VBandsInterface();

	virtual int bands() const=0;
	virtual VBand band(int num)=0;
	/**
	 * same as @ref band(num)
	 **/
	VBand operator [] (int num);

private:
	virtual int level(int index) const=0;
	virtual void setLevel(int index, int level)=0;
};


class VInterpolation;
class VEqualizer;

/**
 * Represents a single band in a vequalizer
 * @author Charles Samuels
 **/
class KDE_EXPORT VBand
{
	friend class VInterpolation;
	friend class VEqualizer;

	struct Private;
	VBand::Private *d;

private:
	VBand(VBandsInterface *bands, int index, int start, int end);

public:
	~VBand();

	VBand(const VBand &copy);
	VBand & operator =(const VBand &copy);

	/**
	* the intensity of the change.
	* it's logarithmic.  0 is no change
	* negative numbers are loss in intensity
	* positive numbers are a gain
	* And +-100 is the most you'd need to go
	**/
	int level() const;
	void setLevel(int l);

	int start() const;
	int end() const;

	/**
	* the middle between start and end
	**/
	int center() const;

	QString formatStart(bool withHz=true) const;
	QString formatEnd(bool withHz=true) const;
	/**
	* return the format for center()
	**/
	QString format(bool withHz=true) const;
};


/**
 * This class is an interpolated representation to the Eq data.
 * This means that no matter how many bands
 * the real equalizer has, your interpolated
 * version appears like it has only as many
 * bands as you asked for. You can continue
 * to interact with all interpolations and
 * the true equalizer as normal. They even
 * modify eachother according to a Spline
 * interpolation.
 *
 * @short interpolated representation of Eq data
 * @author Charles Samuels
 **/
class KDE_EXPORT VInterpolation : public QObject, public VBandsInterface
{
	Q_OBJECT
	struct Private;
	Private *d;
	friend class VBand;

public:
	/**
	* create an interpolation with the one and only
	* Noatun equalizer, and @p bands
	**/
	VInterpolation(int bands);
	virtual ~VInterpolation();

	virtual int bands() const;
	virtual VBand band(int num);
	virtual int level(int index) const;
	virtual void setLevel(int index, int level);

signals:
	void changed();

private:
	void refresh();
	void getFrequencies(int num, int *high, int *low) const;
	/**
	 * where on the spline is my own interpolation's bandNum
	 * @returns an x value on the spline
	 **/
	double onSpline(int bandNum) const;
};

class VPreset;

/**
 * @short Noatun EQ
 * @author Charles Samuels
 **/
class KDE_EXPORT VEqualizer : public QObject, public VBandsInterface
{
	Q_OBJECT
	friend class VBand;
	friend class VPreset;
	friend class Engine;
	friend class NoatunApp;
	friend class VInterpolation;

	struct Private;
	Private *d;

	VEqualizer();
	~VEqualizer();
	void init();

public:

	/**
	 * @return the frequencies to use with @p num many
	 * bands.  This is a list of the upper frequency
	 * of each band, for example: { 40, 60, 100 } if
	 * you hear up to 40hz
	 **/
	static QValueList<int> frequencies(int num);

	/**
	 * @return number of bands I have, which may be different
	 * than what setBands was called with
	 * @sa setBands
	 **/
	int bands() const;

	/**
	 * @return first frequency I operate on, currently 20
	 *
	 * does not apply to an interpolation
	 **/
	static int start();
	/**
	 * @return last frequency I operate on, currently 20000
	 */
	static int end();

	/**
	 * @return maximum amount of bands I may have
	 **/
	int maxBands() const;

	/**
	 * the minimum number of bands I may have (2)
	 **/
	int minBands() const;

	VBand band(int num);

	bool isEnabled() const;

	/**
	 * returns the level of preamp (-100 to 100 normally)
	 **/
	int preamp() const;

public: // serialization
	/**
	 * save the current levels
	 * all noatun equalizer files have the "*.noatunequalizer"
	 * pattern.  Nevertheless, the file can be identified
	 * by magic, so it's not required
	 **/
	bool save(const KURL &file, const QString &friendly) const;

	/**
	 * restore the EQ settings from this file
	 **/
	bool load(const KURL &file);

	/**
	 * convert the current EQ settings to string form,
	 * suitable for storage, the given string is the title
	 *
	 * @see fromString
	 **/
	QString toString(const QString &name="stored") const;

	/**
	 * undo a toString, restoring the EQ
	 * to the settings in the given string,
	 * emitting the changed signals
	 **/
	bool fromString(const QString &str);

public: // presets
	/**
	 * create a preset with such a name
	 * and remember that it'l return an invalied Preset
	 * if the name already exists
	 *
	 * If smart is true, append a number to the end
	 * of the name, if one already exists by the given
	 **/
	VPreset createPreset(const QString &name, bool smart=true);

	/**
	 * return all the presets
	 * remember to setAutoDelete on this
	 **/
	QValueList<VPreset> presets() const;

	/**
	 * @returns the preset with the given name
	 * or an invalid Preset if none exists
	 **/
	VPreset presetByName(const QString &name);

	/**
	 * @returns the preset in the given file
	 * or an invalid Preset if none exists
	 **/
	VPreset presetByFile(const QString &file);

	/**
	 * @returns whether a preset with the
	 * given name exists
	 **/
	bool presetExists(const QString &name) const;

signals:
	/**
	 * emitted when the number of bands has changed (and hence
	 * all my associated Bands are useless
	 **/
	void changedBands();

	/**
	 * emitted when something changes.  Preamplication, band level,
	 * number of bands, enabled/disabled
	 **/
	void changed();
	/**
	 * emitted when the value of one or more of the bands
	 * has changed, but not immediately after
	 * a changedBands
	 **/
	void modified();

	void preampChanged();
	void preampChanged(int);

	void enabled();
	void disabled();

	void enabled(bool e);

	/**
	 * emitted when a preset is created
	 **/
	void created(VPreset preset);

	/**
	 * emitted when the given @p preset is
	 * selected
	 **/
	void selected(VPreset preset);

	/**
	 * when @p preset has been renamed
	 **/
	void renamed(VPreset preset);

	/**
	 * the given @p preset has been removed
	 **/
	void removed(VPreset preset);
public slots:
	/**
	 * set the preamplification
	 * it's logarithmic.  0 is no change
	 * negative numbers are loss in intensity
	 * positive numbers are a gain
	 * And +-100 is the most you'd need to go
	 **/
	void setPreamp(int p);
	/**
	 * turn on EQ
	 */
	void enable();
	/**
	 * turn off EQ
	 */
	void disable();
	/**
	 * turn on/off EQ depending on @p e
	 */
	void setEnabled(bool e);
	void setBands(int bands);
	void setBands(int bands, bool interpolate);

private:
	virtual int level(int index) const;
	virtual void setLevel(int index, int level);
	void setLevels(const QValueList<int> &levels);

private:
	/**
	 * tell the filter to start using the new settings
	 * if @p full is false, it doesn't take the full
	 * update, but just the values (not the number of
	 * bands
	 **/
	void update(bool full=false);
};

/**
 * a preset stores the state of the equalizer
 *
 * VEqualizer provides a way to get a list of these
 * or access them by name
 *
 * this acts as a reference to the preset, so
 * it might be invalid in which case
 * isValid() is false, isNull() is true, and
 * operator bool() return false
 **/
class VPreset
{
	friend class VEqualizer;

	struct Private;
	Private *d;

	void *_bc;
	VPreset(const QString &file);
public:
	VPreset();
	VPreset(const VPreset &copy);
	VPreset & operator=(const VPreset &copy);
	~VPreset();
	bool operator ==(const VPreset &other) const;

	/**
	 * @returns the name of the preset, which is user visible
	 **/
	QString name() const;
	/**
	 * set the user-presentable name of the preset
	 *
	 * Equalizer will emit renamed
	 *
	 * @returns success.  If another preset is named
	 * this, it'l fail.
	 **/
	bool setName(const QString &name);

	/**
	 * @returns the file that this preset is in
	 **/
	QString file() const;

	bool isValid() const;
	bool isNull() const { return !isValid(); }
	operator bool() const { return isValid(); }

	/**
	 * save the state of the equalizer into this preset
	 **/
	void save();

	/**
	 * load the preset into the equalizer
	 **/
	void load() const;

	/**
	 * obliterate this preset!
	 **/
	void remove();
};

#endif
