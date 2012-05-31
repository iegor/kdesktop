#ifndef NOATUN_EQUALIZER_H
#define NOATUN_EQUALIZER_H

#include <qptrlist.h>
#include <qobject.h>
#include <kurl.h>
#include <noatun/vequalizer.h>

class Engine;
class Equalizer;

/**
 * a preset stores the state of the equalizer
 *
 * @short EQ Preset
 * @author Charles Samuels
 * @version 2.3
 **/
class Preset
{
friend class Equalizer;

	Preset(const QString &file);
	Preset();
	Preset(VPreset p);

public:
	QString name() const;
	bool setName(const QString &name);
	bool save() const;
	bool load();

	void remove();

	QString file() const;

	VPreset &vpreset() const;
private:
	QString mFile;
};

/**
 * This represents a single band in Noatuns %Equalizer
 *
 * @short EQ Band
 * @author Charles Samuels
 * @version 2.3
 **/
class Band
{
friend class Equalizer;
friend class QPtrList<Band>;

private:
	Band();
	Band(int start, int end);
	Band(int band);
	virtual ~Band();
public:
	/**
	 * the intensity of the change.
	 * it's logarithmic.  0 is no change
	 * negative numbers are loss in intensity
	 * positive numbers are a gain
	 * And +-100 is the most you'd need to go
	 **/
	int level();
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

private:
	int mOffset;
	int mNum, mEnd;
};


/**
 * @deprecated
 *
 * this API is deprecated!!!  Do not use it!
 * it acts as a wrapper around the new VEqualizer API
 * This only exists to keep compatibility in both
 * source and binary forms.  It will go away in the future.
 * @short old Equalizer
 * @author Charles Samuels
 * @version 2.3
 **/
class Equalizer : public QObject
{
friend class Band;
friend class Preset;
friend class Engine;

Q_OBJECT
public:
	Equalizer();
	~Equalizer();

	const QPtrList<Band> &bands() const;
	Band *band(int num) const;
	int bandCount() const;

	int preamp() const;
	bool isEnabled() const;

	void init();

public slots:
	/**
	 * set the preamplification
	 * it's logarithmic.  0 is no change
	 * negative numbers are loss in intensity
	 * positive numbers are a gain
	 * And +-100 is the most you'd need to go
	 **/
	void setPreamp(int p);
	void enable();
	void disable();
	void setEnabled(bool e);


public:
// saving eq stuff
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

	/**
	 * create a preset with such a name
	 * and remember that it'l return zero
	 * if the name already exists
	 *
	 * If smart is true, append a number to the end
	 * of the name, if one already exists by the given
	 **/
	Preset *createPreset(const QString &name, bool smart=true);

	/**
	 * return all the presets
	 * remember to setAutoDelete on this
	 **/
	QPtrList<Preset> presets() const;

	Preset *preset(const QString &file);
	bool presetExists(const QString &name) const;

signals:
	void changed(Band *band);
	void changed();
	void enabled();
	void disabled();
	void enabled(bool e);

	void preampChanged(int p);
	void preampChanged();

	/**
	 * the preset with the given name
	 * was selected
	 **/
	void changed(Preset *);

	/**
	 * when a new preset has been created
	 **/
	void created(Preset*);

	/**
	 * when @p preset has been renamed to @p newname
	 **/
	void renamed(Preset *);

	/**
	 * the given preset has been removed
	 **/
	void removed(Preset *);

private slots:
	void created(VPreset preset);
	void selected(VPreset preset);
	void renamed(VPreset preset);
	void removed(VPreset preset);

private:
	void add(Band*);
	void remove(Band*);
	// apply the data to artsd
	void enableUpdates(bool on=true);
	void update(bool full=false);

private:
	QPtrList<Band> mBands;
	bool mUpdates;
	int mPreamp;
};



#endif

