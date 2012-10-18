#ifndef EFFECTS_H
#define EFFECTS_H

#include <qptrlist.h>
#include <qcstring.h>
#include <qstrlist.h>
#include <qobject.h>

namespace Arts { class StereoEffect; }
class Engine;
class EffectConfigWidget;

/**
 * @short Base class for a noatun effect
 *
 * Example:
 * \code
 * new Effect("Arts::SomeEffect");
 * \endcode
 * Then you can add, insert, etc. it using Effects
 **/
class Effect
{
friend class Effects;
friend class EffectConfigWidget;
public:
	Effect(const char *name);
	~Effect();

	/**
	 * return the effect processed
	 * directly before this one
	 **/
	Effect *before() const;
	/**
	 * return the effect processed
	 * directly after this one
	 **/
	Effect *after() const;
	long id() const;

	/**
	 * get the Arts object.
	 * @internal
	 **/
	Arts::StereoEffect *effect() const;

	/**
	 * Get the name of the object.
	 **/
	QCString name() const;

	/**
	 * get the "clean" title of effect
	 **/
	QString title() const;

	/**
	 * @return true if this effect name is invalid, false otherwise
	 * <b>Note:</b> If you call StereoEffect::start() on an invalid Effect you
	 * will probably be punished with a segmentation fault.
	 **/
	bool isNull() const;

	/**
	 * show the configure dialog box for
	 * this effect.  if friendly is true,
	 * then create a top-level window,
	 * set an icon and make it purdy. Otherwise
	 * create a plan widget that you can reparent.
	 **/
	QWidget *configure(bool friendly=true);

	/**
	 * Does this widget have a configurable
	 * dialog box.  E.g., will configure
	 * return null?
	 **/
	bool configurable() const;

	/**
	 * Return an effect name that can be presented to a user
	 * i.e. Arts::FREEVERB will end up as FREEVERB
	 **/
	static QString clean(const QCString &name);
private:
	long mId;
	Arts::StereoEffect *mEffect;
	QCString mName;
	QWidget *mConfig;
};

/**
 * Noatuns effect stack
 * @author Charles Samuels
 **/
class Effects : public QObject
{
Q_OBJECT
friend class Effect;
public:
	Effects();

	bool insert(const Effect *after, Effect *item);

	/**
	 * create the Effect, by getting the proper item
	 * from the list, then append it here.
	 *
	 * for example
	 * \code
	 * append(new Effect(available()[0]));
	 * \endcode
	 **/
	bool append(Effect *item);

	/**
	 * reorder the effect stack. If @p after is null,
	 * it'll be first
	 **/
	void move(const Effect *after, Effect *item);
	/**
	 * Removes an item from the effect stack
	 * @param item item to remove
	 * @param del delete the item from the effect stack as well (default true)
	 **/
	void remove(Effect *item, bool del=true);

	/**
	 * Removes all items from the effect stack.
	 * @param del delete the items from the effect stack as well (default true)
	 **/
	void removeAll(bool del=true);

	/**
	 * A list of all available effects, by name
	 * each of these can be given to the first
	 * argument of the Effect constructor
	 **/
	QStrList available() const;

	/**
	 * A list of all available effects objects
	 **/
	QPtrList<Effect> effects() const;

	/**
	 * Get the Effect that has the following id
	 **/
	Effect *findId(long id) const;

private:
	QPtrListIterator<Effect> stackPosition() const;

signals:
	/**
	 * Emitted when an effect has been added to the effect stack
	 * @param effect the effect that got added
	 **/
	void added(Effect *effect);
	/**
	 * Emitted when an effect has been removed to the effect stack
	 * @param effect the effect that got removed
	 **/
	void removed(Effect *effect);
	/**
	 * Emitted when an effect has been moved
	 * @param effect the effect that got moved
	 **/
	void moved(Effect *effect);
	/**
	 * Emitted when an effect is about to be
	 * deleted (from memory)
	 * @param effect the effect to be deleted
	 **/
	void deleting(Effect *effect);

private:
	// stored in no specific order
	QPtrList<Effect> mItems;
};



#endif

