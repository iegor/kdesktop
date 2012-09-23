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

#include "krecord.h"
#include "krecord_private.h"

#include "artsactions.h"
// VolumeControl
#include "artsmoduleseffects.h"

#include "krecfile.h"
#include "krecglobal.h"
#include "krecexport_template.h"

#include <arts/kaudioplaystream.h>
#include <arts/kaudiorecordstream.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kaction.h>
#include <kconfig.h>
#include <qlayout.h>
#include <qtimer.h>
#include <ksettings/dialog.h>
#include <ktip.h>

KRecPrivate::KRecPrivate( KRecord* p, const char* n )
  : QObject( p,n )
  , _confdlg( 0 )
  , server( new KArtsServer( 0 ) )
  , dispatcher( new KArtsDispatcher( 0 ) )
  , m_playStream( new KAudioPlayStream( server, "KRec::Out", server ) )
  , m_recStream( new KAudioRecordStream( server, "KRec::In", server ) )
  , w( 0 )
  , b_arts( true ), b_comp( true )
  , _impl( p )
  , mainwidget( new KRecMainWidget( p ) )
  , _currentFile( 0 )
  , _exportitem( 0 )
{
	//kdDebug( 60005 ) << k_funcinfo << endl;
	m_recStream->usePolling( false );
	if( m_recStream->effectStack().isNull() )
		kdFatal() << "Couldn't get Object 'StereoEffectStack' from KAudioRecordStream!!!" << endl;
	volumecontrol = Arts::DynamicCast( server->server().createObject( "Arts::StereoVolumeControl" ) );
	if( volumecontrol.isNull() )
		kdFatal()<<"Couldn't create Object 'StereoVolumeControl' !!!"<<endl;
	comp = Arts::DynamicCast( server->server().createObject( "Arts::Synth_STEREO_COMPRESSOR" ) );
	if( comp.isNull() )
		b_comp = false;

	volumecontrol.start();
	if( b_comp ) comp.start();
	vc_id = m_recStream->effectStack().insertBottom( volumecontrol, "VolumeControl" );
	if( b_comp ) comp_id = m_recStream->effectStack().insertTop( comp, "Compressor" );

	Arts::StereoVolumeControlGui gui = Arts::StereoVolumeControlGui( volumecontrol );
	gui.title( i18n( "Recording level" ).utf8().data() );
	mainwidget->_artswidget = gui;
	mainwidget->_kaw_volumecontrol->setContent( mainwidget->_artswidget );

}

KRecPrivate::~KRecPrivate() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	mainwidget->_artswidget = Arts::StereoVolumeControlGui::null();
	delete _currentFile;
	_currentFile = 0;
	//kdDebug( 60005 ) << k_funcinfo << "done." << endl;
}

void KRecPrivate::showConfDialog() {
	if ( !_confdlg )
		_confdlg = new KSettings::Dialog( _impl );
	_confdlg->show();
}

void KRecPrivate::newFile() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) closeFile();
	if ( !_currentFile ) {
		pNewFile( new KRecFile( this ) );
	}
}
void KRecPrivate::openFile() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) closeFile();
	if ( !_currentFile ) {
		QString filename = KFileDialog::getOpenFileName( "", "*.krec", _impl );
		//kdDebug( 60005 ) << k_funcinfo << filename << endl;
		if ( !filename.isNull() )
			pNewFile( new KRecFile( filename, this ) );
	}
}
void KRecPrivate::pNewFile( KRecFile* file ) {
	_currentFile = file;
	connect( m_recStream, SIGNAL( data( QByteArray& ) ), _currentFile, SLOT( writeData( QByteArray& ) ) );
	connect( m_playStream, SIGNAL( requestData( QByteArray& ) ), _currentFile, SLOT( getData( QByteArray& ) ) );
	mainwidget->_fileview->setFile( _currentFile );
	checkActions();
}

void KRecPrivate::saveFile() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) pSaveFile( _currentFile->filename() );
}
void KRecPrivate::saveAsFile() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) pSaveFile( QString::null );
}
void KRecPrivate::pSaveFile( const QString &filename ) {
	if ( !_currentFile )
		return;

	if ( !filename.isNull() )
		_currentFile->save( filename );
	else {
		QString userFilename = KFileDialog::getSaveFileName( "", "*.krec", _impl, i18n( "Save File As" ) );
		if ( !userFilename.isNull() )
			_currentFile->save( userFilename );
	}
}

bool KRecPrivate::closeFile() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) {
		if ( !_currentFile->saved() ) {
			int choice = KMessageBox::questionYesNoCancel( _impl, i18n( "The document \"%1\" has been modified.\nDo you want to save it?" ).arg( _currentFile->filename() ), QString::null, KStdGuiItem::save(), KStdGuiItem::discard() );
			if ( choice == KMessageBox::Yes ) saveFile();
			if ( choice == KMessageBox::Cancel ) return false;
			// go on if KMessageBox::No
		}
		delete _currentFile;
		_currentFile = 0;
		mainwidget->_fileview->setFile( _currentFile );
	}
	checkActions();
	return true;
}

void KRecPrivate::exportFile() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _currentFile ) {
		QString filename = KFileDialog::getSaveFileName( "", KRecGlobal::the()->exportFormatEndings(), _impl, "Export File As" );
		if ( !filename.isNull() ) {
			int lastdot = filename.find( '.', -5 );
			QString ending = filename.right( filename.length()-lastdot-1 );
			_exportitem = KRecGlobal::the()->getExportItemForEnding( ending );
			if ( _exportitem ) {
				_exportitem->initialize( _currentFile->samplerate(), _currentFile->bits(), _currentFile->channels() );
				if ( _exportitem->initialize( filename ) ) {
					connect( _exportitem, SIGNAL( getData( QByteArray& ) ), _currentFile, SLOT( getData( QByteArray& ) ) );
					connect( _currentFile, SIGNAL( endReached() ), _exportitem, SLOT( stop() ) );
					connect( _currentFile, SIGNAL( endReached() ), this, SLOT( endExportFile() ) );
					_exportitem->start();
				}
			} else
				KMessageBox::detailedSorry( _impl,
				i18n( "Sorry, an encoding method could not be determined." ),
				i18n( "<qt>This can have several reasons:<ul>" \
				      "<li>You did not specify an ending.</li>" \
				      "<li>You specified an ending but there is no plugin available " \
				      "for this ending. In both cases be sure to choose an ending of " \
				      "the list presented in the previous dialog.</li>" \
				      "<li>The plugin loading mechanism isn't working. If you are sure " \
				      "you did everything right, please file a bugreport saying what " \
				      "you where about to do and please quote the following line:<br />" \
				      "%1</li>" \
				      "</ul></qt>" ).arg( KRecGlobal::the()->exportFormatEndings() ),
				i18n( "Could not determine encodingmethod" ) );
		}
	} else KRecGlobal::the()->message( i18n( "There is nothing to export." ) );
	checkActions();
}
void KRecPrivate::endExportFile() { QTimer::singleShot( 20, this, SLOT( endExportFile2() ) ); }
void KRecPrivate::endExportFile2() {
	_exportitem->finalize();
	disconnect( _currentFile, 0, _exportitem, 0 );
	disconnect( _exportitem, 0, 0, 0 );
	checkActions();
}

void KRecPrivate::playthru( bool yes ){
	kdDebug( 60005 ) << k_funcinfo << yes << endl;
	if(yes) {
		Arts::connect( m_recStream->effectStack(), m_playStream->effectStack() );
		kdDebug( 60005 ) << "Making a connection between recStream and playStream :-)" << endl;
	} else {
		Arts::disconnect( m_recStream->effectStack(), m_playStream->effectStack() );
		kdDebug( 60005 ) << "Disconnecting recStream and playStream :-(" << endl;
	}
}

void KRecPrivate::checkActions(){
	_impl->actionCollection()->action( "player_record" )->setEnabled( false );
	_impl->actionCollection()->action( "player_play" )->setEnabled( false );
	_impl->actionCollection()->action( "player_stop" )->setEnabled( false );
	_impl->actionCollection()->action( "player_gobegin" )->setEnabled( false );
	_impl->actionCollection()->action( "player_goend" )->setEnabled( false );
	_impl->actionCollection()->action( "export_file" )->setEnabled( false );
	_impl->actionCollection()->action( "file_save" )->setEnabled( false );
	_impl->actionCollection()->action( "file_save_as" )->setEnabled( false );
	_impl->actionCollection()->action( "file_close" )->setEnabled( false );
	if ( _currentFile ) {
		if ( !_exportitem || ( _exportitem && !_exportitem->running() ) ) {
			if ( !m_recStream->running() && !m_playStream->running() ) {
				_impl->actionCollection()->action( "player_record" )->setEnabled( true );
				_impl->actionCollection()->action( "player_play" )->setEnabled( true );
			}
			if ( m_playStream->running() || m_recStream->running() )
				_impl->actionCollection()->action( "player_stop" )->setEnabled( true );
			if ( _currentFile->position() != 0 )
				_impl->actionCollection()->action( "player_gobegin" )->setEnabled( true );
			if ( _currentFile->position() != _currentFile->size() )
				_impl->actionCollection()->action( "player_goend" )->setEnabled( true );
		}
		_impl->actionCollection()->action( "export_file" )->setEnabled( true );
		_impl->actionCollection()->action( "file_close" )->setEnabled( true );
		//if ( !_currentFile->saved() ) {
			_impl->actionCollection()->action( "file_save" )->setEnabled( true );
			_impl->actionCollection()->action( "file_save_as" )->setEnabled( true );
		//}
	}
}

void KRecPrivate::toBegin() {
	if ( _currentFile )
		_currentFile->newPos( 0 );
	checkActions();
}
void KRecPrivate::toEnd() {
	if ( _currentFile )
		_currentFile->newPos( _currentFile->size() );
	checkActions();
}

void KRecPrivate::forceTipOfDay() {
	KTipDialog::showTip( _impl, QString::null, true );
}

void KRecPrivate::execaRtsControl() {
	kapp->kdeinitExec( "artscontrol" );
}
void KRecPrivate::execKMix() {
	kapp->kdeinitExec( "kmix" );
}

/**

	KRecord:

*/

KRecord::KRecord(QWidget *parent, const char *name )
	: KMainWindow(parent,name)
	, d( new KRecPrivate( this ) )
{
	kdDebug( 60005 ) << k_funcinfo << endl;
	//kdDebug( 60005 )<<"KRecord::KRecord( QWidget *"<<parent<<", const char* "<<name<<" )"<<endl; // (DEBUG)

	KRecGlobal::the()->setMainWidget( this );
	KRecGlobal::the()->setStatusBar( statusBar() );

	// * * * Startup-Window * * *
	KTipDialog::showTip( this );

	// * * * Mainwidget * * *
	setCentralWidget( d->mainwidget );

	// * * * Actions * * *
	d->artsactions = new ArtsActions( d->server, actionCollection(), this );

	KStdAction::preferences( d, SLOT( showConfDialog() ), actionCollection() );

	KStdAction::openNew( d, SLOT( newFile() ), actionCollection() );
	KStdAction::open( d, SLOT( openFile() ), actionCollection() );
	KStdAction::save( d, SLOT( saveFile() ), actionCollection() );
	KStdAction::saveAs( d, SLOT( saveAsFile() ), actionCollection() );
	KStdAction::close( d, SLOT( closeFile() ), actionCollection() );
	KStdAction::quit( this, SLOT( close() ), actionCollection() );

	KStdAction::tipOfDay( d, SLOT( forceTipOfDay() ), actionCollection() );

	d->aExportFile = new KAction( i18n( "Export..." ), KShortcut(),
	                              d, SLOT( exportFile() ), actionCollection(), "export_file" );

	d->aRecord = new KAction( i18n( "&Record" ), KShortcut( Key_R ),
	                          this, SLOT( startRec() ), actionCollection(), "player_record" );
	d->aPlay = new KAction( i18n( "&Play" ), KShortcut( Key_P ),
	                        this, SLOT( startPlay() ), actionCollection(), "player_play" );
	d->aStop = new KAction( i18n( "&Stop" ), KShortcut( Key_S ),
	                        this, SLOT( stopRec() ), actionCollection(), "player_stop" );
	d->aThru = new KToggleAction( i18n( "Play Through" ), KShortcut( CTRL + Key_P), actionCollection(), "play_thru" );
	connect( d->aThru, SIGNAL( toggled( bool ) ), d, SLOT( playthru( bool ) ) );

	d->aBegin = new KAction( i18n( "Go to &Beginning" ), KShortcut( SHIFT + Key_Left ),
	                         d, SLOT( toBegin() ), actionCollection(), "player_gobegin" );
	d->aEnd = new KAction( i18n( "Go to &End" ), KShortcut( SHIFT + Key_Right ),
	                       d, SLOT( toEnd() ), actionCollection(), "player_goend" );

	( void* ) d->artsactions->actionAudioManager();

	d->aExecaRtsControl = new KAction( i18n( "Start aRts Control Tool" ), KShortcut(),
	                                   d, SLOT( execaRtsControl() ), actionCollection(), "exec_artscontrol" );
	d->aExecKMix = new KAction( i18n( "Start KMix" ), KShortcut(),
	                            d, SLOT( execKMix() ), actionCollection(), "exec_kmix" );

	// * * * GUI * * *
	// TODO Fix toolbar config so this line can just be setupGUI()
	setupGUI(KMainWindow::Keys | StatusBar | Save | Create );
	setStandardToolBarMenuEnabled( true );
	// TODO Fix the arts toolbar to know it own minium size, this app shouldn't care!
	setMinimumWidth( 400 );

	if( d->b_comp )
	{
		Arts::GenericGuiFactory factory;
		Arts::Widget aw = factory.createGui( d->comp );
		d->w = new KArtsWidget( aw, toolBar("compressor") );
		d->w->setName( "kde toolbar widget" );

		toolBar( "compressor" )->insertWidget( 1, 400, d->w );
		toolBar( "compressor" )->setBarPos( KToolBar::Bottom );
	}
	else
	{
		toolBar( "compressor" )->close();
		KMessageBox::detailedSorry( this,
			i18n( "Your system is missing the Synth_STEREO_COMPRESSOR aRts module.\nYou will be able to use KRec but without the great functions of the compressor." ),
			i18n( "Possible reasons are:\n- You installed KRec on its own without the rest of kdemultimedia.\n- You installed everything correctly, but did not restart the aRts daemon\n and therefore it is not aware of the new effects.\n- This is a bug." ),
			i18n( "Unable to Find Compressor" ) );
	}

	d->checkActions();
}

KRecord::~KRecord(){
	kdDebug( 60005 ) << k_funcinfo << endl; // (DEBUG)
	stopRec();

	d->m_recStream->effectStack().remove( d->vc_id );
	if( d->b_comp ) d->m_recStream->effectStack().remove( d->comp_id );
	d->volumecontrol.stop();
	if( d->b_comp ) d->comp.stop();

	d->volumecontrol = Arts::StereoVolumeControl::null();
	if( d->b_comp ) d->comp = Arts::StereoEffect::null();

	KRecGlobal::kconfig()->sync();
	delete d;
	kdDebug( 60005 ) << k_funcinfo << "done. Bye!" << endl;
}

void KRecord::startRec(){
	if( !d->m_recStream->running() && d->_currentFile ) {
		//kdDebug( 60005 )<<"KRecord::startRec()"<<endl; // (DEBUG)
		d->_currentFile->newBuffer();
		d->m_recStream->start( d->_currentFile->samplerate(),
		                       d->_currentFile->bits(),
		                       d->_currentFile->channels() );
	}
	d->checkActions();
	d->mainwidget->_fileview->updateGUI();
}

void KRecord::startPlay(){
	if ( !d->m_playStream->running() && d->_currentFile ) {
		d->m_playStream->start( d->_currentFile->samplerate(),
		                        d->_currentFile->bits(),
		                        d->_currentFile->channels() );
	}
	d->checkActions();
	d->mainwidget->_fileview->updateGUI();
}

void KRecord::stopRec(){
	if ( d->m_recStream->running() )
		d->m_recStream->stop();
	if ( d->m_playStream->running() )
		d->m_playStream->stop();
	d->checkActions();
	d->mainwidget->_fileview->updateGUI();
}


bool KRecord::queryClose() {
	kdDebug( 60005 ) << k_funcinfo << endl;
	return d->closeFile();
}

#include "krecord.moc"
#include "krecord_private.moc"

// vim:sw=4:ts=4
