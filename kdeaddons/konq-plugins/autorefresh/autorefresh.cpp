// -*- c++ -*-

/*
 * Copyright 2003 by Richard J. Moore, rich@kde.org
 */


#include <khtml_part.h> // this plugin applies to a khtml part
#include <kdebug.h>
#include "autorefresh.h"
#include <kaction.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <qmessagebox.h>
#include <klocale.h>
#include <qtimer.h>
#include <kgenericfactory.h>
 
AutoRefresh::AutoRefresh( QObject* parent, const char* name, const QStringList & /*args*/ )
    : Plugin( parent, name )
{
   timer = new QTimer( this );
   connect( timer, SIGNAL( timeout() ), this, SLOT( slotRefresh() ) );
   
    refresher = new KSelectAction( i18n("&Auto Refresh"),
                                   "reload", 0,
                                   this, SLOT(slotIntervalChanged()),
                                   actionCollection(), "autorefresh" );
    QStringList sl;
    sl << i18n("None");
    sl << i18n("Every 15 Seconds");
    sl << i18n("Every 30 Seconds");
    sl << i18n("Every Minute");
    sl << i18n("Every 5 Minutes");
    sl << i18n("Every 10 Minutes");
    sl << i18n("Every 15 Minutes");
    sl << i18n("Every 30 Minutes");
    sl << i18n("Every 60 Minutes");

    refresher->setItems( sl );
    refresher->setCurrentItem( 0 );
}
 
AutoRefresh::~AutoRefresh()
{
}

void AutoRefresh::slotIntervalChanged()
{
   int idx = refresher->currentItem();
   int timeout = 0;
   switch (idx) {
     case 1:
         timeout = ( 15*1000 );
         break;
     case 2:
         timeout = ( 30*1000 );
         break;
     case 3:
         timeout = ( 60*1000 );
         break;
     case 4:
         timeout = ( 5*60*1000 );
         break;
     case 5:
         timeout = ( 10*60*1000 );
         break;
     case 6:
         timeout = ( 15*60*1000 );
         break;
     case 7:
         timeout = ( 30*60*1000 );
         break;
     case 8:
         timeout = ( 60*60*1000 );
         break;
     default:
         break;
   }

   timer->stop();
   if ( timeout )
      timer->start( timeout );
}

void AutoRefresh::slotRefresh()
{
    if ( !parent()->inherits("KParts::ReadOnlyPart") ) {
        QString title = i18n( "Cannot Refresh Source" );
        QString text = i18n( "<qt>This plugin cannot auto-refresh the current part.</qt>" );
 
        QMessageBox::warning( 0, title, text );
    }
    else
    {
        KParts::ReadOnlyPart *part = (KParts::ReadOnlyPart *) parent();

        // Get URL
        KURL url = part->url();
        part->openURL( url );
    }
}

K_EXPORT_COMPONENT_FACTORY( libautorefresh, KGenericFactory<AutoRefresh>( "autorefresh" ) )

#include "autorefresh.moc"

