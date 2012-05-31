#ifndef KJLOADER_H
#define KJLOADER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// local includes
#include "parser.h"

// noatun-specific includes
#include <noatun/plugin.h>
#include <noatun/app.h>

// system includes
#include <qwidget.h>
#include <qbitmap.h>
#include <qptrlist.h>
#include <qcstring.h>

#include <kwinmodule.h>

class QLabel;
class KJWidget;
class KHelpMenu;
class KJSeeker;
class NoatunPreferences;
class KJToolTip;
class KJFont;
class KJPrefs;


class KJLoader :  public QWidget, public UserInterface, public Parser
{
Q_OBJECT
NOATUNPLUGIND

	friend class KJWidget;
public:
	KJLoader();
	~KJLoader();

public:
	void minimize();
	KHelpMenu *helpMenu() const { return mHelpMenu; }
	QStringList &item(const QString &key) { return *Parser::find(key); }

	// returns path to currently loaded configfile
	// can be either a newly loaded one or one of the three below
	QString currentSkin() { return mCurrentSkin; }

	// returns path to mainskin-configfile
	QString currentDefaultSkin() { return mCurrentDefaultSkin; }

	//returns path to dockmode-configfile if present
	QString currentDockModeSkin() { return mCurrentDockModeSkin; }

	//returns path to winshademode-configfile if present (not supported yet)
	QString currentWinshadeModeSkin() { return mCurrentWinshadeModeSkin; }

	KJPrefs *prefs() const { return mPrefs; }

	QPtrList<KJWidget> widgetsAt(const QPoint &pt) const;

	void removeChild(KJWidget *c);
	void addChild(KJWidget *c);

public slots:
	void loadSkin(const QString &file);
	void readConfig();
	void switchToDockmode();
	void returnFromDockmode();

protected:
	void unloadSkin();
	void showSplash();

public slots:
	void timeUpdate();
	void newSong();

private slots:
	void loadSeeker();
	void slotWindowActivate(WId win);
	void slotWindowRemove(WId win);
	void slotWindowChange(WId win);
	void slotDesktopChange(int);
	void slotStackingChanged();
	void restack();
	void hideSplash();

protected:
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);
	virtual void closeEvent(QCloseEvent*e);
	virtual void wheelEvent(QWheelEvent *e);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

public:
	static KJLoader* kjofol;

private:
	// ==== docking stuff ====
	KWinModule *mWin;
	WId mDockToWin;
	int mDockPositionX, mDockPositionY, mDockPosition;
	QRect mDockWindowRect;
	// ==== end of docking stuff ====
	bool moving;
	QPoint mMousePoint;
	QPtrList<KJWidget> subwidgets;
	KJWidget *mClickedIn;
	KHelpMenu *mHelpMenu;
	KJFont *mText, *mNumbers, *mVolumeFont, *mPitchFont;
	QLabel *splashScreen;
	KJToolTip *mTooltips;
	QString mCurrentSkin;
	QString mCurrentDefaultSkin;
	QString mCurrentDockModeSkin;
	QString mCurrentWinshadeModeSkin;

	KJPrefs *mPrefs;
};

#endif // KJLOADER_H
