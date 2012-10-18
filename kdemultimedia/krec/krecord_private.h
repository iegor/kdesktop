/***************************************************************************
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef KRECORD_PRIVATE_H
#define KRECORD_PRIVATE_H

#include <qobject.h>

#include <kartsserver.h>
#include <kartsdispatcher.h>
#include <artsflow.h>
#include <artsgui.h>
#include <kartswidget.h>
#include <kurl.h>
#include <qlayout.h>

#include "krecfileview.h"

class KRecord;
class KRecMainWidget;
class KRecFile;
class KRecExportItem;

class KAudioRecordStream;
class KAudioPlayStream;
class KConfig;
class ArtsActions;
class KAction;
class KActionMenu;
class KToggleAction;
class KRecentFilesAction;

namespace KSettings { class Dialog; }

class KRecPrivate : public QObject  {
   Q_OBJECT
public:
	/// Constructor
	KRecPrivate( KRecord*, const char* =0 );
	/// Destructor
	~KRecPrivate();
public slots:
/*	/// Does nothing.
	void nothing() { }
	/// Starts Recording.
	void startRec();
	/// Starts Playback.
	void startPlay();
	/// ByteStreamSender informs of his playing.
	void playIsRunning() { playing=true; checkActions(); }
	/// ByteStreamSender informs of his playing.
	void playStops() { playing=false; checkActions(); }
	/// Stops Recording.
	void stopRec();

	/// If we can play or not.
	void canPlay( bool b ) { _canplay = b; checkActions(); }
	/// If we can record or not.
	void canRecord( bool b ) { _canrecord = b; checkActions(); }
*/

//	void startUpWindow( bool on );

	void showConfDialog();

	/// Checks all the Actions and sets Enabled/Disabled.
	void checkActions();

	/// Enables Play-thru from the AMAN_RECORD to the AMAN_PLAY.
	void playthru( bool );

	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();
	bool closeFile();
	void exportFile();
	void endExportFile();
	void endExportFile2();

	void toBegin();
	void toEnd();

	void forceTipOfDay();
	void execaRtsControl();
	void execKMix();
private:
	void pNewFile( KRecFile* );
	void pSaveFile( const QString &);
public:
	KAction *aRecord, *aPlay, *aStop, *aExportFile;
	KAction *aBegin, *aEnd;
	KToggleAction *aThru, *aStartUpWindow;
	KAction *aExecaRtsControl, *aExecKMix;

	KSettings::Dialog *_confdlg;

	KArtsServer *server;
	KArtsDispatcher *dispatcher;
	KAudioPlayStream *m_playStream;
	KAudioRecordStream * m_recStream;

	Arts::StereoVolumeControl volumecontrol;
	Arts::StereoEffect comp;
	KArtsWidget *w;
	long vc_id, comp_id;
	ArtsActions *artsactions;

	bool b_arts, b_comp;

	KRecord *_impl;
	KRecMainWidget *mainwidget;

	KRecFile *_currentFile;

	KRecExportItem *_exportitem;
};

class KRecMainWidget : public QWidget {
   Q_OBJECT
public:
	KRecMainWidget( QWidget* p, const char* n=0 ) : QWidget( p,n ) {
		_layout = new QBoxLayout( this, QBoxLayout::LeftToRight, 2 );
		_kaw_volumecontrol = new KArtsWidget( this );
		_layout->addWidget( _kaw_volumecontrol, 0 );
		_fileview = new KRecFileView( this );
		_layout->addWidget( _fileview, 500 );
	}
	QBoxLayout *_layout;
	KArtsWidget *_kaw_volumecontrol;
	Arts::Widget _artswidget;
	KRecFileView *_fileview;
};

#endif

// vim:sw=4:ts=4
