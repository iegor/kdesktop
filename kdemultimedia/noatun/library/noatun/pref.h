#ifndef NOATUNPREF_H
#define NOATUNPREF_H

#include <kdialogbase.h>
#include <qptrlist.h>
#include <kdemacros.h>

class CModule;

/**
 * Noatun configuration dialog
 **/
class NoatunPreferences : public KDialogBase
{
Q_OBJECT
friend class CModule;

public:
	/**
	 * @internal
	 **/
	NoatunPreferences(QWidget *);

public:
	/**
	 * Display noatun preferences dialog
	 **/
	virtual void show();
	/**
	 * Display noatun preferences dialog showing @p module
	 * Useful if you want to display your own plugin configuration tab
	 **/
	virtual void show(CModule *module);

protected:
	virtual void slotOk();
	virtual void slotApply();

private:
	void add(CModule *);
	void remove(CModule *);

private:
	class NoatunPreferencesPrivate;
	NoatunPreferencesPrivate *d;

	QPtrList<CModule> mModules;
};

/**
 * @short Base class for a configuration sheet that is shown in preferences dialog
 *
 * Create your GUI in constructor, reimplement reopen() and save() and
 * you're all set.
 **/
class KDE_EXPORT CModule : public QWidget
{
Q_OBJECT

public:
	/**
	 * arguments are short and long descriptions
	 * for this module, respectively
	 *
	 * parent is the object that is this modules virtual-parent.
	 * When that is deleted, this also will go away, automagically.
	 **/
	CModule(const QString &name, const QString &description, const QString &icon, QObject *parent=0);

	virtual ~CModule();

public slots:
	/**
	 * save all your options, and apply them
	 **/
	virtual void save() {}
	/**
	 * reload all options (e.g., read config files)
	 **/
	virtual void reopen() {}

private slots:
	void ownerDeleted();

private:
	class CModulePrivate;
	CModulePrivate *d;
};


#endif // NOATUNPREF_H
