#ifndef NOATUN_H
#define NOATUN_H

#include <kuniqueapplication.h>
#include <kdemacros.h>
class Playlist;
class Player;
class LibraryLoader;
class KPopupMenu;
class NoatunPreferences;
class Downloader;
class Effects;
class EffectView;
class Equalizer;
class KDialogBase;
class VEqualizer;

namespace NoatunStdAction
{
	class PluginActionMenu;
}

/**
 * @class NoatunApp app.h noatun/app.h
 * Can be accessed from every plugin by using "napp"
 *
 * @short Noatun Application class
 * @author Charles Samuels
 * @version 2.3
 */
class KDE_EXPORT NoatunApp : public KUniqueApplication
{
Q_OBJECT
friend class Playlist;

public:
	NoatunApp();
	~NoatunApp();

	/**
	 * Provides access to the central playlist object.
	 * Any plugin can access the noatun playlist with
	 * <pre>napp->playlist();</pre>
	 **/
	Playlist *playlist() const;
	/**
	 * access to the central player object
	 * Any plugin can access the noatun player backend with
	 * <pre>napp->playlist();</pre>
	 **/
	Player *player() const { return mPlayer; }
	/**
	 * access to NoatunPreferences
	 **/
	NoatunPreferences *preferencesBox() const { return mPref; }

	/**
	 * @return a list of mimetypes aRts (and thus Noatun) can play
	 * KFileDialog accepts this QString instead of the shell globs in
	 * its static functions, make use of it :)
	 **/
	QString mimeTypes();

	LibraryLoader *libraryLoader() const { return mLibraryLoader; }
	Downloader *downloader() const { return mDownloader; }
	static QImage readPNG(const QString &filename);
	Effects *effects() const;
	::Equalizer *equalizer() const { return mEqualizer; }
	::VEqualizer *vequalizer();
	KDialogBase *equalizerView() const { return mEqualizerView; }

	QCString version() const;

	virtual void commitData(QSessionManager &);
	virtual void saveState(QSessionManager &);

	/**
	 * The three startup modes how noatun should behave when it is
	 * restarted.
	 *
	 * Restore - it can restore the player's last state
	 *
	 * Play - it automatically starts playing the next file in the
	 * playlist
	 *
	 * DontPlay - it doesn't start playing
	 */
	enum StartupPlayMode { Restore = 0, Play, DontPlay };

signals:
	/**
	 * Tells you (a UI plugin) to hide
	 */
	void hideYourself();

	/**
	 * Tells you (a UI plugin) to show again
	 */
	void showYourself();

public slots:
	/**
	 * ask the UIs to hide or show
	 **/
	void toggleInterfaces();
	/**
	 * ask the UIs to show
	 **/
	void showInterfaces();
	/**
	 * ask the UIs to hide, then you have
	 * to look around for them, or you'll lose
	 **/
	void hideInterfaces();

public: //options
	/**
	 * @deprecated Use startupPlayMode() instead
	 */
	bool autoPlay() const;
	int startupPlayMode() const;
	bool loopList() const;
	bool oneInstance() const;
	QString saveDirectory() const;
	/**
	 * @deprecated
	 * now merged with clearOnOpen()
	 **/
	bool clearOnStart() const;
	/**
	 * @return true if the playlist will be cleared when opening a
	 * new file via commandline or file open dialog, false otherwise
	 **/
	bool clearOnOpen() const;
	bool hackUpPlaylist() const;
	/**
	 * @return true if hardware-mixing is being used, false in case
	 * software mixing is active
	 **/
	bool fastMixer() const;
	QString titleFormat() const;
	/**
	 * @return true if remaining time is displayed to the user, false if
	 * played time is displayed
	 **/
	bool displayRemaining() const;

	void setOneInstance(bool);
	void setLoopList(bool);
	/**
	 * @deprecated Use setStartupPlayMode() instead
	 */
	void setAutoPlay(bool);
	void setStartupPlayMode(int mode);
	void setSaveDirectory(const QString &);
	void setRememberPositions(bool);
	/**
	 * @deprecated
	 * now merged with setClearOnOpen()
	 **/
	void setClearOnStart(bool);
	/**
	 * Set if the playlist will be cleared when opening a
	 * new file via commandline or file open dialog
	 **/
	void setClearOnOpen(bool);
	void setHackUpPlaylist(bool);

	/**
	 * Set if hardware-mixing ("fast") or software-mixing ("slow") should be used
	 **/
	void setFastMixer(bool);

	void setTitleFormat(const QString &);

	/**
	 * Pass true if remaining time should be displayed to the user, false if
	 * played time should be displayed
	 **/
	void setDisplayRemaining(bool);

	/**
	 * To insert items use KActions and insert() them into pluginActionMenu().
	 * @return pointer to the actionmenu
	 */
	NoatunStdAction::PluginActionMenu *pluginActionMenu();

	/**
	 * @deprecated
	 * Adds an item to the plugin menu.
	 * You may use this value with pluginMenu() for greater control of your menu entry
	 *
	 * @return the ID associated with the menu item, for use in pluginMenuRemove()
	 **/
	int pluginMenuAdd(const QString &text, const QObject *receiver, const char *member);

	/**
	 * @deprecated
	 * Removes an item previously added to the plugin menu.
	 * @param id the ID of the "to be removed" menu item
	 **/
	void pluginMenuRemove(int id);

	/**
	 * @deprecated
	 * Use pluginActionMenu() instead
	 * @return pointer to the plugin menu
	 */
	KPopupMenu *pluginMenu();

protected:
	virtual int newInstance();

public slots:
	// slots for the contextMenu
	/**
	 * Opens the preferences dialog
	 * You can also use
	 * <pre>napp->preferencesBox()->show()</pre>
	 * @see NoatunPreferences
	 */
	void preferences();
	/**
	 * Exits Noatun
	 */
	void quit();
	/**
	 * Shows the standard file-open dialog
	 */
	void fileOpen();
	/**
	 * Shows the effects window
	 */
	void effectView();
	/**
	 * Shows the equalizer window
	 */
	void equalizerView();

private:
	void loadPlugins();
	void saveEngineState();
	void restoreEngineState();

private:
	Player *mPlayer;
	LibraryLoader *mLibraryLoader;
	KPopupMenu *mPluginMenu;
	NoatunStdAction::PluginActionMenu *mPluginActionMenu;
	Downloader *mDownloader;
	struct Private;
	Private *d;
	EffectView *mEffectView;
	NoatunPreferences *mPref;
	::Equalizer *mEqualizer;
	KDialogBase *mEqualizerView;
	bool showingInterfaces;
};

#define napp (static_cast<NoatunApp*>(kapp))

// version info for the plugins
// this is MAJOR.MINOR.PATCHLEVEL
// and you developers better ignore patchlevel :)
#define NOATUN_MAJOR 2
#define NOATUN_MINOR 10
#define NOATUN_PATCHLEVEL 0

#define NOATUN_VERSION "2.10.0"

#endif
