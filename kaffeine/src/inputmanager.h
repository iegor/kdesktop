/*
 * inputmanager.h
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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qptrlist.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qstring.h>

class KaffeineInput;
class KMultiTabBar;
class Kaffeine;
class KKeyDialog;
class MRL;



class PlayerContainer : public QVBox
{
	Q_OBJECT

public:
	PlayerContainer(QWidget* parent = 0, const char* name = 0);
	virtual ~PlayerContainer() {}

signals:
	void signalURLDropEvent(const QStringList&);

protected:
	void dragEnterEvent (QDragEnterEvent*);
	void dropEvent(QDropEvent*);

};



class InputPlugin
{
public:
	InputPlugin( KaffeineInput *p, int ident, QString na );
	~InputPlugin();

	KaffeineInput *plug;
	int id;
	QString name;
};



class InputManager : public QObject
{
	Q_OBJECT
public:
	InputManager( Kaffeine *parent, QWidgetStack *ws, KMultiTabBar *mt );
	~InputManager();

	void add( KaffeineInput *p, const QPixmap &icon, const QString &name );
	void addPlayerWidget( QWidget *w, const QPixmap &icon, const QString &name );
	void addStartWindow( QWidget *w, const QPixmap &icon, const QString &name );
	void remove( KaffeineInput *p );
	void setPlayerContainer( PlayerContainer *pc );
	void fullscreen( bool b );
	QString activePlugin();
	void setActivePlugin( QString name );
	void addConfigKeys( KKeyDialog* kd );
	void showPlayer();
	void togglePlaylist();
	void saveConfig();

	void playCurrentTrack();
	void playNextTrack();
	void playPreviousTrack();
	void playTrackNumber( int num );
	bool playbackFinished( MRL &mrl );
	void mergeMeta(const MRL&);
	bool close();
	QSize stackSize();
	QWidget* visibleWidget() const;
	bool isPlaylistCurrent();

public slots:
	void execTarget( const QString& );

private slots:
	void show( int id );
	void play( const MRL&, KaffeineInput* );
	void statusBarMessage( const QString& );
	void setCurrentPlugin( KaffeineInput* );
	void stop();
	void pause();
	void showMe( KaffeineInput* );

private:
	void makeTargets( KaffeineInput*, bool );

	QWidgetStack *stack;
	KMultiTabBar *mtBar;
	QPtrList<InputPlugin> plugs;
	QWidget *playerWidget, *startWindow;
	QWidget *currentMainWidget;
	PlayerContainer *playerContainer;
	QWidget *oldMainWidget;
	KaffeineInput *currentPlugin;
	int nextId;
	Kaffeine *kaffeine;
};

#endif /* INPUTMANAGER_H */
