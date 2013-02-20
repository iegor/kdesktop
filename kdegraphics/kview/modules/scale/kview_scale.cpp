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

/* $Id: kview_scale.cpp 465369 2005-09-29 14:33:08Z mueller $ */

#include "kview_scale.h"
#include "scaledlg.h"

#include <qimage.h>
#include <qvbox.h>

#include <kaction.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kimageviewer/viewer.h>
#include <kimageviewer/canvas.h>
#include <kdialogbase.h>

typedef KGenericFactory<KViewScale> KViewScaleFactory;
K_EXPORT_COMPONENT_FACTORY( kview_scale, KViewScaleFactory( "kview_scale" ) )

KViewScale::KViewScale( QObject* parent, const char* name, const QStringList & )
	: Plugin( parent, name )
	, m_pViewer( 0 )
	, m_pCanvas( 0 )
{
	m_pViewer = static_cast<KImageViewer::Viewer *>( parent );
	if( m_pViewer )
	{
		kdDebug( 4630 ) << "m_pViewer->canvas() = " << m_pViewer->canvas() << endl;
		m_pCanvas = m_pViewer->canvas();

		(void) new KAction( i18n( "&Scale Image..." ), 0, 0,
							this, SLOT( slotScaleDlg() ),
							actionCollection(), "plugin_scale" );
	}
	else
		kdWarning( 4630 ) << "no KImageViewer interface found - the scale plugin won't work" << endl;
}

KViewScale::~KViewScale()
{
}

void KViewScale::slotScaleDlg()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	KDialogBase dlg( m_pViewer->widget(), "KView scale dialog", true, i18n( "Scale Image" ), KDialogBase::Ok|KDialogBase::Cancel );
	ScaleDlg widget( m_pCanvas->imageSize(), dlg.makeVBoxMainWidget() );
#if 0
	QVBox * layout = dlg.makeVBoxMainWidget();

	QGroupBox * pixelgroup = new QGroupBox( i18n( "Pixel Dimensions" ), layout );
	QGridLayout * pixelgroupgrid = new QGridLayout( pixelgroup, 1, 1, 0, KDialog::spacingHint() );
	pixelgroupgrid->setSpacing( KDialog::spacingHint() );
	pixelgroupgrid->setMargin( KDialog::marginHint() );
	QLabel * label;
	QSize imagesize = m_pCanvas->imageSize();

	// show original width
	label = new QLabel( i18n( "Original width:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 0, 0 );
	pixelgroupgrid->addWidget( new QLabel( QString::number( imagesize.width() ), pixelgroup ), 0, 1 );
	label = new QLabel( i18n( "Height:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 1, 0 );
	pixelgroupgrid->addWidget( new QLabel( QString::number( imagesize.height() ), pixelgroup ), 1, 1 );

	pixelgroupgrid->addRowSpacing( 2, KDialog::spacingHint() );

	label = new QLabel( i18n( "New width:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 3, 0 );
	label = new QLabel( i18n( "Height:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 4, 0 );
	QSpinBox * newwidth = new QSpinBox( 1, 100000, 1, pixelgroup );
	pixelgroupgrid->addWidget( newwidth, 3, 1 );
	QSpinBox * newheight = new QSpinBox( 1, 100000, 1, pixelgroup );
	pixelgroupgrid->addWidget( newheight, 4, 1 );
	KComboBox * newsizeunit = new KComboBox( pixelgroup );
	newsizeunit->insertItem( i18n( "px" ) );
	newsizeunit->insertItem( i18n( "%" ) );
	pixelgroupgrid->addMultiCellWidget( newsizeunit, 3, 4, 2, 2, Qt::AlignVCenter );

	pixelgroupgrid->addRowSpacing( 5, KDialog::spacingHint() );

	label = new QLabel( i18n( "Ratio X:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 6, 0 );
	label = new QLabel( i18n( "Y:" ), pixelgroup );
	label->setAlignment( QLabel::AlignRight );
	pixelgroupgrid->addWidget( label, 7, 0 );
	QSpinBox * ratiox = new QSpinBox( pixelgroup );
	ratiox->setValidator( new QDoubleValidator( 0.0001, 10000, 4, ratiox ) );
	pixelgroupgrid->addWidget( ratiox, 6, 1 );
	QSpinBox * ratioy = new QSpinBox( pixelgroup );
	ratioy->setValidator( new QDoubleValidator( 0.0001, 10000, 4, ratioy ) );
	pixelgroupgrid->addWidget( ratioy, 7, 1 );
	pixelgroupgrid->addMultiCellWidget( new QCheckBox( i18n( "Link" ), pixelgroup ), 6, 7, 2, 2, Qt::AlignVCenter );

	QGroupBox * printgroup = new QGroupBox( i18n( "Print Size && Display Units" ), layout );
	QGridLayout * printgroupgrid = new QGridLayout( printgroup, 1, 1, 0, KDialog::spacingHint() );
	printgroupgrid->setSpacing( KDialog::spacingHint() );
	printgroupgrid->setMargin( KDialog::marginHint() );

	label = new QLabel( i18n( "New width:" ), printgroup );
	label->setAlignment( QLabel::AlignRight );
	printgroupgrid->addWidget( label, 0, 0 );
	label = new QLabel( i18n( "Height:" ), printgroup );
	label->setAlignment( QLabel::AlignRight );
	printgroupgrid->addWidget( label, 1, 0 );
	QSpinBox * newwidth2 = new QSpinBox( printgroup );
	printgroupgrid->addWidget( newwidth2, 0, 1 );
	QSpinBox * newheight2 = new QSpinBox( printgroup );
	printgroupgrid->addWidget( newheight2, 1, 1 );
	KComboBox * newsizeunit2 = new KComboBox( printgroup );
	newsizeunit2->insertItem( i18n( "in" ) );
	newsizeunit2->insertItem( i18n( "mm" ) );
	printgroupgrid->addMultiCellWidget( newsizeunit2, 0, 1, 2, 2, Qt::AlignVCenter );

	printgroupgrid->addRowSpacing( 2, KDialog::spacingHint() );

	label = new QLabel( i18n( "Resolution X:" ), printgroup );
	label->setAlignment( QLabel::AlignRight );
	printgroupgrid->addWidget( label, 3, 0 );
	label = new QLabel( i18n( "Y:" ), printgroup );
	label->setAlignment( QLabel::AlignRight );
	printgroupgrid->addWidget( label, 4, 0 );
	QSpinBox * resx = new QSpinBox( printgroup );
	printgroupgrid->addWidget( resx, 3, 1 );
	QSpinBox * resy = new QSpinBox( printgroup );
	printgroupgrid->addWidget( resy, 4, 1 );
	printgroupgrid->addMultiCellWidget( new QCheckBox( i18n( "Link" ), printgroup ), 3, 4, 2, 2, Qt::AlignVCenter );
	//KComboBox * newsizeunit2 = new KComboBox( printgroup );
	//newsizeunit2->insertItem( i18n( "in" ) );
	//newsizeunit2->insertItem( i18n( "mm" ) );
	//printgroupgrid->addMultiCellWidget( newsizeunit2, 0, 1, 2, 2, Qt::AlignVCenter );
#endif

	dlg.exec();
}

void KViewScale::slotScale()
{
	// retrieve current image
	kdDebug( 4630 ) << "m_pCanvas = " <<  m_pCanvas << endl;
	const QImage * image = m_pCanvas->image();
	kdDebug( 4630 ) << "image pointer retrieved" << endl;
	if( image )
	{
		// scale
		QImage newimage = image->smoothScale( 50, 50 );
		// put back (modified)
		m_pCanvas->setImage( newimage );
		m_pViewer->setModified( true );
	}
	else
		kdDebug( 4630 ) << "no image to scale" << endl;
}

// vim:sw=4:ts=4:cindent
#include "kview_scale.moc"
