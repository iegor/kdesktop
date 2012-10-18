#ifndef _NOATUNSTDACTION_H_
#define _NOATUNSTDACTION_H_

#include <kaction.h>
#include <kactionclasses.h>
#include <kdemacros.h>
class KPopupMenu;

/**
 * Holds all noatun related actions
 * @short noatun specific actions
 * @author Charles Samuels
 **/
namespace NoatunStdAction
{

/**
 * An action starting noatun playback
 **/
class PlayAction : public KAction
{
Q_OBJECT
public:
	PlayAction(QObject *parent, const char *name);
private slots:
	void playing();
	void notplaying();
};

/**
 * An action starting/stopping noatun playback
 **/
class PlaylistAction : public KToggleAction
{
Q_OBJECT
public:
	PlaylistAction(QObject *parent, const char *name);
private slots:
	void shown();
	void hidden();
};

/**
 * actionmenu that holds all plugin defined actions
 * @author Stefan Gehn
 */
class PluginActionMenu : public KActionMenu
{
Q_OBJECT
public:
	PluginActionMenu(QObject *parent, const char *name);
	/**
	 * inserts the given @p action into the action-menu
	 * @param action the action to insert
	 * @param index defines the place where the action gets displayed in
	 */
	virtual void insert (KAction *action, int index=-1);
	/**
	 * removes the given @p action into the action-menu
	 */
	virtual void remove(KAction *action);
	/**
	 * Wrapper method for old Noatun API
	 * <b>DON'T USE</b>
	 **/
	int menuAdd(const QString &text, const QObject *receiver, const char *member);
	/**
	 * Wrapper method for old Noatun API
	 * <b>DON'T USE</b>
	 **/
	void menuRemove(int id);
private:
	int mCount;
};

/**
 * actionmenu that holds all vis-plugins for easier enabling/disabling
 * @author Stefan Gehn
 */
class VisActionMenu : public KActionMenu
{
Q_OBJECT
public:
	VisActionMenu(QObject *parent, const char *name);
private slots:
	void fillPopup();
	void toggleVisPlugin(int);
private:
	QMap<int, QString>mSpecMap;
};


/**
 * actionmenu that holds all looping modes
 * @author Stefan Gehn
 */
class LoopActionMenu : public KActionMenu
{
Q_OBJECT
public:
	LoopActionMenu(QObject *parent, const char *name);
private slots:
	void updateLooping(int);
	void loopNoneSelected();
	void loopSongSelected();
	void loopPlaylistSelected();
	void loopRandomSelected();
private:
	KRadioAction *mLoopNone;
	KRadioAction *mLoopSong;
	KRadioAction *mLoopPlaylist;
	KRadioAction *mLoopRandom;
};


/**
 * @return pointer to a KAction which opens the effects dialog on activation
 */
KDE_EXPORT KAction *effects(QObject *parent = 0, const char *name = 0);

/**
 * @return pointer to a KAction which opens the equalizer dialog on activation
 */
KDE_EXPORT KAction *equalizer(QObject *parent = 0, const char *name = 0);

/**
 * @return pointer to a KAction which goes back one track on activation
 */
KDE_EXPORT KAction *back(QObject *parent = 0, const char *name = 0);
/**
 * @return pointer to a KAction which stops playback on activation
 */
KDE_EXPORT KAction *stop(QObject *parent = 0, const char *name = 0);
/**
 * @return pointer to a KAction which starts/pauses playback on activation
 */
KDE_EXPORT KAction *playpause(QObject *parent = 0, const char *name = 0);
/**
 * @return pointer to a KAction which advances one track on activation
 */
KDE_EXPORT KAction *forward(QObject *parent = 0, const char *name = 0);
/**
 * @return pointer to a KToggleAction which shows/hides the playlist
 */
KDE_EXPORT KToggleAction *playlist(QObject *parent = 0, const char *name = 0);

/**
 * loop action
 **/
KDE_EXPORT LoopActionMenu *loop(QObject *parent, const char *name);

/**
 * play action
 */
KDE_EXPORT KAction *play(QObject *parent = 0, const char *name = 0);

/**
 * pause action
 */
KDE_EXPORT KAction *pause(QObject *parent = 0, const char *name = 0);

/**
 * @return pointer to the global PluginActionMenu object (there is only one instance)
 */
KDE_EXPORT PluginActionMenu *actions();

/**
 * @return pointer to a VisActionMenu object
 */
KDE_EXPORT VisActionMenu *visualizations(QObject *parent = 0, const char *name = 0);

/**
 * The global popupmenu of noatun, there's not two or three but only one of these :)
 * @author Charles Samuels
 **/
class KDE_EXPORT ContextMenu
{
public:
	static KPopupMenu *createContextMenu(QWidget *p);

	/**
	 * One menu to show them all, One menu to find them
	 * One menu to bring them all and in the darkness bind them
	 *
	 * In the land of Noatun where the oceans die
	 */
	static KPopupMenu *contextMenu();

	/**
	 * Show the context menu at point
	 **/
	static void showContextMenu(const QPoint &);

	/**
	 * show the context menu at the mouse's current position
	 **/
	static void showContextMenu();
private:
	static KPopupMenu *mContextMenu;
};

}

#endif
