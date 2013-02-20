/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// $Id: kviewscanner.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "kviewscanner.h"

#include <qimage.h>
#include <qobjectlist.h>

#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kscan.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kimageviewer/viewer.h>

typedef KGenericFactory<KViewScanner> KViewScannerFactory;
K_EXPORT_COMPONENT_FACTORY( kview_scannerplugin, KViewScannerFactory( "kviewscannerplugin" ) )

KViewScanner::KViewScanner( QObject* parent, const char* name,
	                                  const QStringList & )
  : Plugin( parent, name ),
  m_pScandlg( 0 ),
  m_pViewer( 0 )
{
	QObjectList * viewerList = parent->queryList( 0, "KImageViewer Part", false, false );
	m_pViewer = static_cast<KImageViewer::Viewer *>( viewerList->getFirst() );
	delete viewerList;
	if( m_pViewer )
	{
		(void) new KAction( i18n( "&Scan Image..." ), "scanner", 0,
							this, SLOT( slotScan() ),
							actionCollection(), "plugin_scan" );
	}
	else
		kdWarning( 4630 ) << "no KImageViewer interface found - the scanner plugin won't work" << endl;
}

KViewScanner::~KViewScanner()
{
}

void KViewScanner::slotScan()
{
	if( ! m_pScandlg )
	{
		m_pScandlg = KScanDialog::getScanDialog();
		if( m_pScandlg )
		{
			m_pScandlg->setMinimumSize( 300, 300 );

			connect( m_pScandlg, SIGNAL( finalImage( const QImage &, int ) ),
					this, SLOT( slotImgScanned( const QImage & ) ) );
		}
		else
		{
			KMessageBox::sorry( 0L,
					i18n( "You do not appear to have SANE support, or your scanner "
						"is not attached properly. Please check these items before "
						"scanning again." ),
					i18n( "No Scan-Service Available" ) );
			kdDebug( 4630 ) << "*** No Scan-service available, aborting!" << endl;
			return;
		}
	}

	if( m_pScandlg->setup() )
		m_pScandlg->show();
}

void KViewScanner::slotImgScanned( const QImage & img )
{
	kdDebug( 4630 ) << "received an image from the scanner" << endl;
	m_pViewer->newImage( img );
}

// vim:sw=4:ts=4:cindent
#include "kviewscanner.moc"
