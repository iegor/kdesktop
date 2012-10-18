#include "stdaction.h"
#include "app.h"
#include "player.h"
#include "stereobuttonaction.h"
#include "pluginloader.h"

#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kstdaction.h>
#include <qcursor.h>
#include <qmap.h>
#include <kdebug.h>
#include <kstdguiitem.h>

/**
 * A namespace to have all of noatun's standard actions
 * This is treated like KStdAction
 **/
namespace NoatunStdAction
{
/////////////////////////////////////////////////////
PlayAction::PlayAction(QObject *parent, const char *name)
	: KAction(i18n("Play"), 0, napp->player(), SLOT(playpause()), parent, name)
{
	connect(napp->player(), SIGNAL(playing()), SLOT(playing()));
	connect(napp->player(), SIGNAL(paused()), SLOT(notplaying()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(notplaying()));
	if (napp->player()->isPlaying())
		playing();
	else if (napp->player()->isPaused() || napp->player()->isStopped())
		notplaying();
}

void PlayAction::playing()
{
	setIconSet(QIconSet(SmallIcon("player_pause")));
	setText(i18n("Pause"));
}

void PlayAction::notplaying()
{
	setIconSet(QIconSet(SmallIcon("player_play")));
	setText(i18n("Play"));
}
/////////////////////////////////////////////////////

PlaylistAction::PlaylistAction(QObject *parent, const char *name)
	: KToggleAction(i18n("Show Playlist"), "playlist", 0, napp->player(), SLOT(toggleListView()), parent, name)
{
        setCheckedState(i18n("Hide Playlist"));
	connect(napp->player(), SIGNAL(playlistShown()),  SLOT(shown()));
	connect(napp->player(), SIGNAL(playlistHidden()), SLOT(hidden()));
	setChecked(napp->playlist()->listVisible());
}

void PlaylistAction::shown()
{
	setChecked(true);
}

void PlaylistAction::hidden()
{
	setChecked(false);
}

////////////////////////////////////////////////////

PluginActionMenu::PluginActionMenu(QObject *parent, const char *name)
	: KActionMenu(i18n("&Actions"), parent, name)
{
// 	kdDebug(66666) << k_funcinfo << "called" << endl;
	setEnabled(false);
	mCount=0;
}

void PluginActionMenu::insert (KAction *action, int index)
{
// 	kdDebug(66666) << k_funcinfo << "called" << endl;
	KActionMenu::insert(action,index);
	setEnabled(true);
	mCount++;
}

void PluginActionMenu::remove(KAction *action)
{
// 	kdDebug(66666) << k_funcinfo << "called" << endl;
	KActionMenu::remove(action);
	mCount--;
	if(mCount==0)
		setEnabled(false);
}

int PluginActionMenu::menuAdd(const QString &text, const QObject *receiver, const char *member)
{
// 	kdDebug(66666) << k_funcinfo << "called, mCount is currently at " << mCount << endl;
	setEnabled(true);
	mCount++;
	return popupMenu()->insertItem(text, receiver, member);
}

void PluginActionMenu::menuRemove(int id)
{
// 	kdDebug(66666) << k_funcinfo << "called, mCount is currently at " << mCount << endl;
	popupMenu()->removeItem(id);
	mCount--;
	if(mCount==0)
		setEnabled(false);
}

////////////////////////////////////////////////////

VisActionMenu::VisActionMenu(QObject *parent, const char *name)
	: KActionMenu(i18n("&Visualizations"), parent, name)
{
	connect(popupMenu(), SIGNAL(aboutToShow()), this, SLOT(fillPopup()));
	connect(popupMenu(), SIGNAL(activated(int)), this, SLOT(toggleVisPlugin(int)));
}

void VisActionMenu::fillPopup()
{
	int id;
	popupMenu()->clear();
	mSpecMap.clear();

	QValueList<NoatunLibraryInfo> available = napp->libraryLoader()->available();
	QValueList<NoatunLibraryInfo> loaded = napp->libraryLoader()->loaded();

	for(QValueList<NoatunLibraryInfo>::Iterator i = available.begin(); i != available.end(); ++i)
	{
		if ((*i).type == "visualization")
		{
			id = popupMenu()->insertItem((*i).name);
			mSpecMap[id] = (*i).specfile;
			popupMenu()->setItemChecked(id, loaded.contains(*i));
		}
	}
}

void VisActionMenu::toggleVisPlugin(int id)
{
	if(!mSpecMap.contains(id))
		return;

	QString specfile = mSpecMap[id];

	if(popupMenu()->isItemChecked(id))
	{
		napp->libraryLoader()->remove(specfile);
		popupMenu()->setItemChecked(id, false);
	}
	else
	{
		napp->libraryLoader()->add(specfile);
		popupMenu()->setItemChecked(id, true);
	}
}

////////////////////////////////////////////////////

LoopActionMenu::LoopActionMenu(QObject *parent, const char *name)
	: KActionMenu(i18n("&Loop"), parent, name)
{
	mLoopNone = new KRadioAction(i18n("&None"), QString::fromLocal8Bit("noatunloopnone"),
		0, this, SLOT(loopNoneSelected()), this, "loop_none");
	mLoopNone->setExclusiveGroup("loopType");
	insert(mLoopNone);

	mLoopSong = new KRadioAction(i18n("&Song"), QString::fromLocal8Bit("noatunloopsong"),
		0, this, SLOT(loopSongSelected()), this, "loop_song");
	mLoopSong->setExclusiveGroup("loopType");
	insert(mLoopSong);

	mLoopPlaylist = new KRadioAction(i18n("&Playlist"), QString::fromLocal8Bit("noatunloopplaylist"),
		0, this, SLOT(loopPlaylistSelected()), this, "loop_playlist");
	mLoopPlaylist->setExclusiveGroup("loopType");
	insert(mLoopPlaylist);

	mLoopRandom = new KRadioAction(i18n("&Random"), QString::fromLocal8Bit("noatunlooprandom"),
		0, this, SLOT(loopRandomSelected()), this, "loop_random");
	mLoopRandom->setExclusiveGroup("loopType");
	insert(mLoopRandom);

	connect(napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(updateLooping(int)));

	updateLooping(static_cast<int>(napp->player()->loopStyle()));
}

void LoopActionMenu::updateLooping(int loopType)
{
	switch(loopType)
	{
		case Player::None:
			mLoopNone->setChecked(true);
			setIcon("noatunloopnone");
			break;
		case Player::Song:
			mLoopSong->setChecked(true);
			setIcon("noatunloopsong");
			break;
		case Player::Playlist:
			mLoopPlaylist->setChecked(true);
			setIcon("noatunloopplaylist");
			break;
		case Player::Random:
			mLoopRandom->setChecked(true);
			setIcon("noatunlooprandom");
			break;
	}
}

void LoopActionMenu::loopNoneSelected()
{
	napp->player()->loop(Player::None);
}

void LoopActionMenu::loopSongSelected()
{
	napp->player()->loop(Player::Song);
}

void LoopActionMenu::loopPlaylistSelected()
{
	napp->player()->loop(Player::Playlist);
}

void LoopActionMenu::loopRandomSelected()
{
	napp->player()->loop(Player::Random);
}

////////////////////////////////////////////////////

KAction *playpause(QObject *parent, const char *name)
{
	return new PlayAction(parent, name);
}

KAction *effects(QObject *parent, const char *name)
{
	return new KAction(i18n("&Effects..."), "effect", 0, napp, SLOT(effectView()), parent, name);
}

KAction *equalizer(QObject *parent, const char *name)
{
	return new KAction(i18n("E&qualizer..."), "equalizer", 0, napp, SLOT(equalizerView()), parent, name);
}

KAction *back(QObject *parent, const char *name)
{
	return new KAction(i18n("&Back"), "player_start", 0, napp->player(), SLOT(back()), parent, name);
}

KAction *stop(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("Stop"), "player_stop", 0, napp->player(), SLOT(stop()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(disable()));
	if(napp->player()->isStopped())
		action->disable();
	else
		action->enable();
	return action;
}

KAction *forward(QObject *parent, const char *name)
{
	return new KAction(i18n("&Forward"), "player_end", 0, napp->player(), SLOT(forward()), parent, name);
}

KAction *play(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("&Play"), "player_play", 0, napp->player(), SLOT(playpause()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(disable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(enable()));
	if(napp->player()->isPlaying())
		action->disable();
	else
		action->enable();
	return action;
}

KAction *pause(QObject *parent, const char *name)
{
	StereoButtonAction *action = new StereoButtonAction(i18n("&Pause"), "player_pause", 0, napp->player(), SLOT(playpause()), parent, name);
	QObject::connect(napp->player(), SIGNAL(playing()), action, SLOT(enable()));
	QObject::connect(napp->player(), SIGNAL(paused()), action, SLOT(disable()));
	QObject::connect(napp->player(), SIGNAL(stopped()), action, SLOT(disable()));
	if(napp->player()->isPlaying())
		action->enable();
	else
		action->disable();
	return action;
}

LoopActionMenu *loop(QObject *parent, const char *name)
{
	return new LoopActionMenu(parent, name);
}

PluginActionMenu *actions()
{
	// NoatunApp makes sure that we only have one ActionMenu around
	return napp->pluginActionMenu();
}

VisActionMenu *visualizations(QObject *parent, const char *name)
{
	return new VisActionMenu(parent, name);
}

KToggleAction *playlist(QObject *parent, const char *name)
{
	return new PlaylistAction(parent, name);
}

KPopupMenu *ContextMenu::mContextMenu = 0;

KPopupMenu *ContextMenu::contextMenu()
{
	if(!mContextMenu) mContextMenu = createContextMenu(0);

	return mContextMenu;
}

KPopupMenu *ContextMenu::createContextMenu(QWidget *p)
{
	KPopupMenu *contextMenu = new KPopupMenu(p, "NoatunContextMenu");

	KHelpMenu *helpmenu = new KHelpMenu(contextMenu, kapp->aboutData(), false);
	KActionCollection* actions = new KActionCollection(helpmenu);

	KStdAction::open(napp, SLOT(fileOpen()), actions)->plug(contextMenu);
	KStdAction::quit(napp, SLOT(quit()), actions)->plug(contextMenu);
	contextMenu->insertItem(SmallIcon("help"), KStdGuiItem::help().text(), helpmenu->menu());
	contextMenu->insertSeparator();
	KStdAction::preferences(napp, SLOT(preferences()), actions)->plug(contextMenu);
	NoatunStdAction::playlist(contextMenu)->plug(contextMenu);
	NoatunStdAction::effects(contextMenu)->plug(contextMenu);
	NoatunStdAction::equalizer(napp)->plug(contextMenu);
	NoatunStdAction::visualizations(napp)->plug(contextMenu);
	napp->pluginActionMenu()->plug(contextMenu);

	return contextMenu;
}

void ContextMenu::showContextMenu(const QPoint &p)
{
	contextMenu()->exec(p);
}

void ContextMenu::showContextMenu()
{
	showContextMenu(QCursor::pos());
}

} // END namespace NoatunStdAction

#include "stdaction.moc"
