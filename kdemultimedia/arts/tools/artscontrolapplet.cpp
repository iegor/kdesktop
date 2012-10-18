/***************************************************************************
                          artscontrolapplet.cpp  -  description
                             -------------------
    begin                : Don Jan 30 20:42:53 CET 2003
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <qcursor.h>
#include <qtimer.h>

#include "artscontrolapplet.h"
#include "artscontrolapplet_private.h"

extern "C"
{
  KDE_EXPORT KPanelApplet* init( QWidget *parent, const QString configFile)
  {
    KGlobal::locale()->insertCatalogue("artscontrol");
    return new ArtsControlApplet(configFile, KPanelApplet::Normal,
                      KPanelApplet::About /*| KPanelApplet::Help | KPanelApplet::Preferences*/,
                      parent, "artscontrolapplet");
  }
}

ArtsControlApplet::ArtsControlApplet(const QString& configFile, Type type, int actions, QWidget *parent, const char *name) : KPanelApplet(configFile, type, actions, parent, name)
{
	//kdDebug()<<"ArtsControlApplet::ArtsControlApplet( const QString& "<<configFile<<", Type "<<type<<", int "<<actions<<", QWidget* "<<parent<<", const char* "<<name<<" )"<<endl;
	// Get the current application configuration handle
	ksConfig = config();

	p = new ArtsControlAppletPrivate( this );
	if( !p->barts ) KMessageBox::information(0, i18n("Something with the ArtsServer went wrong. You probably need to restart aRts and then reload this applet."));
	setCustomMenu(p->menu);

	p->layout = new QBoxLayout( this, QBoxLayout::LeftToRight );
	p->layout->setSpacing( 2 );

	if( p->barts ) {
		p->vu = Arts::StereoVolumeControlGui( p->volume );
		p->vu.label().fontsize( 8 );
		p->vuw = new KArtsWidget( p->vu, this );
		p->vuw->setMinimumSize( 16,16 );
		p->layout->addWidget( p->vuw );
	}

	p->layout->activate();

	QTimer::singleShot( 100, this, SLOT( supdatelayout() ) );

kdDebug()<<"ArtsControlApplet::ArtsControlApplet() finished."<<endl;
}

ArtsControlApplet::~ArtsControlApplet() {
kdDebug()<<k_funcinfo<<endl;
}

#include <kaboutdata.h>
#include <kaboutapplication.h>

void ArtsControlApplet::about() {
	KAboutData about( "artscontrolapplet", I18N_NOOP( "aRts Control Applet" ), "0.5",
		I18N_NOOP( "A kickerapplet to control aRts." ),
		KAboutData::License_GPL, I18N_NOOP( "(c) 2003 by Arnold Krille" ) );
	about.addAuthor( "Arnold Krille", I18N_NOOP( "Author of the Applet" ), "arnold@arnoldarts.de" );
	about.addCredit( "Stefan Westerfeld", I18N_NOOP( "Thanks for creating aRts!" ) );
	KAboutApplication a( &about, this );
	a.exec();
}

void ArtsControlApplet::help() {
kdDebug()<<k_funcinfo<<endl;
}

void ArtsControlApplet::preferences() {
kdDebug()<<k_funcinfo<<endl;
}

int ArtsControlApplet::widthForHeight( int h ) const {
kdDebug()<<"ArtsControlApplet::widthForHeight( int "<<h<<" )"<<endl;
	return p->layout->sizeHint().width();
}

int ArtsControlApplet::heightForWidth( int w ) const {
kdDebug()<<"ArtsControlApplet::heightForWidth( int "<<w<<" )"<<endl;
	return p->layout->sizeHint().height();
}

void ArtsControlApplet::resizeEvent( QResizeEvent * /*_Event*/ ) {
	kdDebug()<<"ArtsControlApplet::resizeEvent( QResizeEvent * )"<<endl;
}

void ArtsControlApplet::mousePressEvent( QMouseEvent* ev ) {
	//kdDebug()<<"ArtsControlApplet::mousePressEvent( QMouseEvent* "<<ev<<" )"<<endl;
	if ( Qt::RightButton == ev->button() /*|| Qt::LeftButton == ev->button()*/ )
		p->menu->exec( QCursor::pos() );
}

void ArtsControlApplet::positionChange( Position ) {
	kdDebug() << k_funcinfo << endl;
	resetLayout();
}

void ArtsControlApplet::resetLayout() {
kdDebug()<<k_funcinfo<<" position()="<<position()<<endl;
	switch ( position() )
	{
		case pTop:
		case pBottom:
			p->layout->setDirection( QBoxLayout::LeftToRight );
			if ( p->barts ) p->vu.direction( Arts::LeftToRight );
			break;
		case pRight:
		case pLeft:
			p->layout->setDirection( QBoxLayout::TopToBottom );
			if ( p->barts ) p->vu.direction( Arts::TopToBottom );
			break;
		default: break;
	}
}

void ArtsControlAppletPrivate::SVinline() {
kdDebug() << k_funcinfo << endl;
	if ( !svinline ) {
		svinline = new FFTScopeView( arts->server(), _parent );
		svinline->setMargin( 2 ); svinline->setLineWidth( 2 ); svinline->setFrameStyle( QFrame::Panel|QFrame::Sunken );
		connect( svinline, SIGNAL( closed() ), this, SLOT( SVinline() ) );
		layout->addWidget( svinline );
	} else {
		delete svinline;
		svinline = 0;
	}
	_parent->supdatelayout();
}

void ArtsControlAppletPrivate::moreBars() { vu.left().count( vu.left().count()+10 ); vu.right().count( vu.right().count()+10 ); }
void ArtsControlAppletPrivate::lessBars() { vu.left().count( vu.left().count()-10 ); vu.right().count( vu.right().count()-10 ); }

void ArtsControlAppletPrivate::styleNormalBars() { vu.left().style( Arts::lmNormalBars ); vu.right().style( Arts::lmNormalBars ); _parent->supdatelayout(); }
void ArtsControlAppletPrivate::styleFireBars()   { vu.left().style( Arts::lmFireBars   ); vu.right().style( Arts::lmFireBars   ); _parent->supdatelayout(); }
void ArtsControlAppletPrivate::styleLineBars()   { vu.left().style( Arts::lmLineBars   ); vu.right().style( Arts::lmLineBars   ); _parent->supdatelayout(); }
void ArtsControlAppletPrivate::styleLEDs()       { vu.left().style( Arts::lmLEDs       ); vu.right().style( Arts::lmLEDs       ); _parent->supdatelayout(); }
void ArtsControlAppletPrivate::styleAnalog()     { vu.left().style( Arts::lmAnalog     ); vu.right().style( Arts::lmAnalog     ); _parent->supdatelayout(); }
void ArtsControlAppletPrivate::styleSmall()      { vu.left().style( Arts::lmSmall      ); vu.right().style( Arts::lmSmall      ); _parent->supdatelayout(); }

// vim: sw=4 ts=4
#include "artscontrolapplet.moc"
#include "artscontrolapplet_private.moc"
