/*
 * inputmanager.cpp
 *
 * Copyright (C) 2005-2007 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kurl.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <kkeydialog.h>
#include <klocale.h>

#include "kmultitabbar.h"
#include "kaffeine.h"
#include "kaffeineinput.h"
#include "inputmanager.h"
#include "mrl.h"
#include "startwindow.h"
#include "inputmanager.moc"


InputPlugin::InputPlugin( KaffeineInput *p, int ident, QString na )
{
	plug = p;
	id = ident;
	name = na;
}



InputPlugin::~InputPlugin()
{
}



InputManager::InputManager( Kaffeine *parent, QWidgetStack *ws, KMultiTabBar *mt )
	: QObject(parent)
{
	kaffeine = parent;
	stack = ws;
	mtBar = mt;
	plugs.setAutoDelete( true );
	playerContainer = NULL;
	currentPlugin = NULL;
	nextId = 3;
}



InputManager::~InputManager()
{
	plugs.clear();
}



QSize InputManager::stackSize()
{
	return stack->size();
}



QWidget* InputManager::visibleWidget() const
{
	return stack->visibleWidget();
}



void InputManager::play( const MRL &mrl, KaffeineInput *p )
{
	if ( currentPlugin!=p ) {
		currentPlugin->playerStopped();
		currentPlugin = p;
	}
	kaffeine->slotPlay( mrl );
}



void InputManager::stop()
{
	kaffeine->slotStop();
	currentPlugin->playerStopped();
}



void InputManager::pause()
{
	kaffeine->slotPlayUnPause();
	currentPlugin->playerPaused();
}



void InputManager::mergeMeta( const MRL &mrl )
{
	currentPlugin->mergeMeta( mrl );
}



void InputManager::setCurrentPlugin( KaffeineInput *p )
{
	if ( currentPlugin!=p ) {
		currentPlugin->playerStopped();
		currentPlugin = p;
	}
}



QString InputManager::activePlugin()
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( it.current()->plug==currentPlugin )
			return it.current()->name;
	}
	return QString();
}



void InputManager::showMe( KaffeineInput *p )
{
	int id=0;
	int p_id=0;
	QWidget *widg;

	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( it.current()->plug->inputMainWidget()==currentMainWidget )
			id = it.current()->id;
		if ( it.current()->plug==p ) {
			widg = it.current()->plug->inputMainWidget();
			p_id = it.current()->id;
		}
	}
	if ( currentMainWidget==playerWidget )
		id = 2;
	else if ( currentMainWidget==startWindow )
		id = 1;

	if ( id && p_id && (id!=p_id) ) {
		if ( mtBar->isHidden() )
			currentMainWidget = widg;
		else {
			mtBar->setTab( id, false );
			mtBar->setTab( p_id, true );
			show( p_id );
		}
	}
}



void InputManager::setActivePlugin( QString name )
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( it.current()->name==name ) {
			currentPlugin = it.current()->plug;
			mtBar->setTab( 1, false );
			mtBar->setTab( it.current()->id, true );
			show( it.current()->id );
			break;
		}
	}
}



void InputManager::showPlayer()
{
	int id=0;

	if ( currentMainWidget==playerWidget )
		return;
	if ( currentMainWidget==startWindow )
		id = 1;
	else {
		for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
			if ( it.current()->plug->inputMainWidget()==currentMainWidget ) {
				if ( !it.current()->plug->showPlayer() )
					return;
				id = it.current()->id;
				break;
			}
		}
	}
	mtBar->setTab( id, false );
	mtBar->setTab( 2, true );
	show( 2 );
}



void InputManager::togglePlaylist()
{
	int currentPluginId=0, playlistPluginId=0;

	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( it.current()->name == i18n("Playlist") ) {
			playlistPluginId = it.current()->id;
			break;
		}
	}
	for ( QPtrListIterator<InputPlugin> it2(plugs); it2.current(); ++it2 ) {
		if ( it2.current()->plug->inputMainWidget() == currentMainWidget ) {
			currentPluginId = it2.current()->id;
			break;
		}
	}

	if ( currentMainWidget == playerWidget ) {
		mtBar->setTab( 2, false );
		mtBar->setTab( playlistPluginId, true );
		show( playlistPluginId );
	}
	else if ( currentMainWidget == startWindow ) {
		mtBar->setTab( 1, false );
		mtBar->setTab( playlistPluginId, true );
		show( playlistPluginId );
	}
	else if ( currentPluginId == playlistPluginId ) {
		showPlayer();
	}
	else {
		mtBar->setTab( currentPluginId, false );
		mtBar->setTab( playlistPluginId, true );
		show( playlistPluginId );
	}
}



void InputManager::addConfigKeys( KKeyDialog* kd )
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it )
		kd->insert( it.current()->plug->actionCollection(), it.current()->plug->name());
}



bool InputManager::isPlaylistCurrent()
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( it.current()->name == i18n("Playlist") ) {
			if ( it.current()->plug==currentPlugin )
				return true;
		}
	}
	return false;
}



void InputManager::playCurrentTrack()
{
	MRL mrl;

	if ( currentPlugin->currentTrack( mrl ) )
		kaffeine->slotPlay( mrl );
}



void InputManager::playNextTrack()
{
	MRL mrl;

	if ( currentPlugin->nextTrack( mrl ) )
		kaffeine->slotPlay( mrl );
}



void InputManager::playPreviousTrack()
{
	MRL mrl;

	if ( currentPlugin->previousTrack( mrl ) )
		kaffeine->slotPlay( mrl );
}



void InputManager::playTrackNumber( int num )
{
	MRL mrl;

	if ( currentPlugin->trackNumber( num, mrl ) )
		kaffeine->slotPlay( mrl );
}



bool InputManager::playbackFinished( MRL &mrl )
{
	return currentPlugin->playbackFinished( mrl );
}



void InputManager::statusBarMessage( const QString &msg )
{
	kaffeine->slotChangePlaylistStatus( msg );
}



void InputManager::saveConfig()
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it )
		it.current()->plug->saveConfig();
}



bool InputManager::close()
{
	for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
		if ( !it.current()->plug->close() )
			return false;
	}
	return true;
}



void InputManager::add( KaffeineInput *p, const QPixmap &icon, const QString &name )
{
	mtBar->appendTab( icon, nextId, name );
	connect( mtBar->tab( nextId ), SIGNAL(clicked(int)), this, SLOT(show(int)) );
	stack->addWidget( p->inputMainWidget(), nextId );
	plugs.append( new InputPlugin( p, nextId, name ) );
	connect( p, SIGNAL(play(const MRL&,KaffeineInput*)), this, SLOT(play(const MRL&,KaffeineInput*)));
	connect( p, SIGNAL(setCurrentPlugin(KaffeineInput*)), this, SLOT(setCurrentPlugin(KaffeineInput*)));
	connect( p, SIGNAL(statusBarMessage(const QString&)), this, SLOT(statusBarMessage(const QString&)));
	connect( p, SIGNAL(stop()), this, SLOT(stop()));
	connect( p, SIGNAL(pause()), this, SLOT(pause()));
	connect( p, SIGNAL(showMe(KaffeineInput*)), this, SLOT(showMe(KaffeineInput*)));
	if ( !currentPlugin )
		currentPlugin = plugs.current()->plug;
	++nextId;
	makeTargets( p, true );
}



void InputManager::addPlayerWidget( QWidget *w, const QPixmap &icon, const QString &name )
{
	playerWidget = w;
	mtBar->appendTab( icon, 2, name );
	connect( mtBar->tab(2), SIGNAL(clicked(int)), this, SLOT(show(int)) );
	stack->addWidget( playerWidget, 2 );
}



void InputManager::addStartWindow( QWidget *w, const QPixmap &icon, const QString &name )
{
	startWindow = w;
	mtBar->appendTab( icon, 1, name );
	connect( mtBar->tab(1), SIGNAL(clicked(int)), this, SLOT(show(int)) );
	stack->addWidget( startWindow, 1 );
	mtBar->setTab( 1, true );
	stack->raiseWidget( 1 );
	oldMainWidget = currentMainWidget = startWindow;
	connect( (StartWindow*)w, SIGNAL(execTarget(const QString&)), this, SLOT(execTarget(const QString&)) );
}



void InputManager::execTarget( const QString &target )
{
	QPtrListIterator<InputPlugin> it(plugs);
	for ( it.toFirst(); it.current(); ++it ) {
		if ( it.current()->plug->execTarget( target ) )
			return;
	}
}



void InputManager::makeTargets( KaffeineInput *p, bool make )
{
	int i;
	QStringList uiNames, iconNames, targetNames;
	StartWindow *sw = (StartWindow*)(startWindow);

	p->getTargets( uiNames, iconNames, targetNames );
	for ( i=0; i<(int)uiNames.count(); i++ ) {
		if ( make )
			sw->registerTarget( uiNames[i], iconNames[i], targetNames[i] );
		else
			sw->unregisterTarget( targetNames[i] );
	}
}



void InputManager::remove( KaffeineInput *p )
{
	InputPlugin *i=NULL;
	QPtrListIterator<InputPlugin> it(plugs);

	for ( it.toFirst(); it.current(); ++it ) {
		if ( it.current()->plug==p ) {
			i = it.current();
			break;
		}
	}

	if ( i ) {
		if ( i->plug==currentPlugin ) {
			if ( i->plug->inputMainWidget()==currentMainWidget ) {
				mtBar->setTab( 1, true );
				show( 1 );
				currentPlugin = plugs.getFirst()->plug;
			}
			else {
				for ( it.toFirst(); it.current(); ++it ) {
					if ( it.current()->plug->inputMainWidget()==currentMainWidget ) {
						currentPlugin = it.current()->plug;
						break;
					}
				}
			}
		}
		else {
			if ( i->plug->inputMainWidget()==currentMainWidget ) {
				for ( it.toFirst(); it.current(); ++it ) {
					if ( it.current()->plug==currentPlugin ) {
						show( it.current()->id );
						break;
					}
				}
			}
		}
		mtBar->removeTab( i->id );
		stack->removeWidget( i->plug->inputMainWidget() );
		makeTargets( i->plug, false );
		delete i->plug;
		plugs.remove( i );
	}
}



void InputManager::setPlayerContainer( PlayerContainer *pc )
{
	playerContainer = pc;
	if ( playerContainer ) {
		for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
			if ( it.current()->plug->inputMainWidget()==currentMainWidget ) {
				playerContainer->reparent( it.current()->plug->wantPlayerWindow(), QPoint(0,0), true);
				playerContainer->show();
				break;
			}
		}
	}
}



void InputManager::show( int id )
{
	QWidget *widg = NULL;
	QWidget *main = NULL;

	if ( id==2 ) {
		if ( currentMainWidget==playerWidget )
			return;
		if ( playerContainer ) {
			playerContainer->reparent(playerWidget, QPoint(0,0), true);
			currentMainWidget = playerWidget;
		}
		currentMainWidget = playerWidget;
	}
	else if ( id==1 ) {
		if ( currentMainWidget==startWindow )
			return;
		currentMainWidget = startWindow;
	}
	else {
		for ( QPtrListIterator<InputPlugin> it(plugs); it.current(); ++it ) {
			if ( it.current()->id==id ) {
				main = it.current()->plug->inputMainWidget();
				if ( main==currentMainWidget ) {
					it.current()->plug->togglePanel();
					return;
				}
				currentMainWidget = main;
				widg = it.current()->plug->wantPlayerWindow();
				if ( widg && playerContainer )
					playerContainer->reparent(widg, QPoint(0,0), true);
				break;
			}
		}
	}

	stack->raiseWidget( id );
}



void InputManager::fullscreen( bool b )
{
	QPtrListIterator<InputPlugin> it(plugs);
	int id=0;

	if ( b ) {
		stack->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		oldMainWidget = currentMainWidget;
		show( 2 );
		mtBar->hide();
	}
	else {
		stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		mtBar->show();
		if ( oldMainWidget==startWindow )
			id = 1;
		else {
			for ( it.toFirst(); it.current(); ++it ) {
				if ( it.current()->plug->inputMainWidget()==oldMainWidget ) {
					id = it.current()->id;
					break;
				}
			}
		}
		show( id );
	}
}



/********/
PlayerContainer::PlayerContainer(QWidget* parent, const char* name) : QVBox(parent, name)
{
	setAcceptDrops(true);
	setMouseTracking( true );
}


void PlayerContainer::dragEnterEvent (QDragEnterEvent* dev)
{
	kdDebug() << "PlayerContainer: drag enter event" << endl;
	dev->accept(QUriDrag::canDecode(dev) || QTextDrag::canDecode(dev));
}

void PlayerContainer::dropEvent(QDropEvent* dev)
{
	QStringList urls,newurls;

	if (QUriDrag::decodeLocalFiles(dev, urls))
	{
		if (urls.count())
		{
			for (uint i=0; i < urls.count() ;i++)
			{
				//KURL url(QUriDrag::unicodeUriToUri(urls[i]));
				//newurls << url.path(-1);
				//kdDebug() << "Kaffeine: Dropped " << url.path() << endl;
				newurls << urls[i];
				kdDebug() << "Kaffeine: Dropped " << urls[i] << endl;
			}
			emit signalURLDropEvent(newurls);
		}
		else
		{
			QUriDrag::decodeToUnicodeUris(dev, urls);
			if (urls.count())
				emit signalURLDropEvent(urls);
		}
	}
	else
		if (strcmp(dev->format(), "text/x-moz-url") == 0)    // for mozilla drops
		{
			QByteArray data = dev->encodedData("text/plain");
			QString md(data);
			emit signalURLDropEvent(md);
		}
}
