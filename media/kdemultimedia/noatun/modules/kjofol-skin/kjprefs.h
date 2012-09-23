#ifndef KJPREFS_H
#define KJPREFS_H

//#include "kjprefswidget.h"
#include "kjskinselectorwidget.h"
#include "kjguisettingswidget.h"

// system includes
#include <qwidget.h>
#include <noatun/pref.h>

#include <kio/job.h>
#include <kurlrequester.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QPushButton;
class QTabWidget;
class KConfig;
class KJLoader;

class KJPrefs : public CModule
{
Q_OBJECT
public:
	KJPrefs(QObject* parent);

	// Save which Skin is currently selected
	virtual void save();

	// Rebuild the Skinlist
	virtual void reopen();

	QString skin( void ) const;

	int minimumPitch( void ) const;
	int maximumPitch( void ) const;

	int visTimerValue ( void ) const;

	int titleMovingUpdates ( void ) const;
	float titleMovingDistance ( void ) const;

	int visType ( void ) const;
	void setVisType ( int vis );

	bool useSysFont( void ) const;
	void setUseSysFont( bool );

	QFont sysFont(void) const;
	void setSysFont(QFont&);

	QColor sysFontColor(void) const;
	void sysFontColor(QColor &);

	bool displayTooltips( void ) const;
	bool displaySplash( void ) const;

public slots:
	// Installs a skin defined by the URL in mSkinRequester
	void installNewSkin( void );

	// Delete the currently selected Skin (does not work for systemwide skins!)
	void removeSelectedSkin ( void );

	// Show a preview of "skin" in mPixmap
	void showPreview(const QString &skin);

	// gets called after a KIO-action has finished
	// KIO is used for installing/removing skins
	void slotResult(KIO::Job *job);

signals:
	void configChanged();

private:
	QPixmap mPixmap; // preview Pixmap
	KConfig *cfg;

	// Dialog-Widgets
	QTabWidget *mTabWidget;
	KJSkinselector *mSkinselectorWidget;
	KJGuiSettings *mGuiSettingsWidget;
};

/**
 * resolve a filename to its correct case.
 * badNodes is the amount of directories/files (at the end)
 * that aren't known)
 **/
QString filenameNoCase(const QString &filename, int badNodes=1);

#endif // KJPREFS_H
