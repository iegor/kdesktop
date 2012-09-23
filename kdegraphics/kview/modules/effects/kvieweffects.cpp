/* This file is in the public domain */

// $Id: kvieweffects.cpp 239366 2003-07-27 00:25:16Z mueller $

#include "kvieweffects.h"

#include <qobjectlist.h>

#include <kaction.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kimageviewer/viewer.h>
#include <kimageviewer/canvas.h>
#include <kdialogbase.h>
#include <knuminput.h>
#include <kiconeffect.h>
#include <qvbox.h>
#include <kcolorbutton.h>
#include <kimageeffect.h>
#include <qlabel.h>
#include <assert.h>

typedef KGenericFactory<KViewEffects> KViewEffectsFactory;
K_EXPORT_COMPONENT_FACTORY( kview_effectsplugin, KViewEffectsFactory( "kvieweffectsplugin" ) )

KViewEffects::KViewEffects( QObject* parent, const char* name, const QStringList & )
	: Plugin( parent, name )
	, m_gamma( 0.5 ), m_lastgamma( -1.0 )
	, m_opacity( 50 ), m_lastopacity( -1 )
	, m_intensity( 50 ), m_lastintensity( -1 )
	, m_color( white )
	, m_image( 0 )
{
	QObjectList * viewerList = parent->queryList( 0, "KImageViewer Part", false, false );
	m_pViewer = static_cast<KImageViewer::Viewer *>( viewerList->getFirst() );
	delete viewerList;
	if( m_pViewer )
	{
		KAction * gammaaction = new KAction( i18n( "&Gamma Correction..." ), 0, 0,
				this, SLOT( gamma() ),
				actionCollection(), "plugin_effects_gamma" );
		KAction * blendaction = new KAction( i18n( "&Blend Color..." ), 0, 0,
				this, SLOT( blend() ),
				actionCollection(), "plugin_effects_blend" );
		KAction * intensityaction = new KAction( i18n( "Change &Intensity (Brightness)..." ), 0, 0,
				this, SLOT( intensity() ),
				actionCollection(), "plugin_effects_intensity" );
		gammaaction->setEnabled( m_pViewer->canvas()->image() != 0 );
		blendaction->setEnabled( m_pViewer->canvas()->image() != 0 );
		intensityaction->setEnabled( m_pViewer->canvas()->image() != 0 );
		connect( m_pViewer->widget(), SIGNAL( hasImage( bool ) ), gammaaction, SLOT( setEnabled( bool ) ) );
		connect( m_pViewer->widget(), SIGNAL( hasImage( bool ) ), blendaction, SLOT( setEnabled( bool ) ) );
		connect( m_pViewer->widget(), SIGNAL( hasImage( bool ) ), intensityaction, SLOT( setEnabled( bool ) ) );
	}
	else
		kdWarning( 4630 ) << "no KImageViewer interface found - the effects plugin won't work" << endl;
}

KViewEffects::~KViewEffects()
{
	// no need to delete m_image here since it will always be NULL at this
	// point.
	assert( m_image == 0 );
}

void KViewEffects::intensity()
{
	KDialogBase dlg( m_pViewer->widget(), "Intensity Dialog", true /*modal*/, i18n( "Change Intensity" ), KDialogBase::Ok | KDialogBase::Try | KDialogBase::Cancel );
	connect( &dlg, SIGNAL( tryClicked() ), this, SLOT( applyIntensity() ) );

	QVBox * vbox = new QVBox( &dlg );
	vbox->setSpacing( KDialog::spacingHint() );
	dlg.setMainWidget( vbox );
	KIntNumInput * percent = new KIntNumInput( vbox, "Intensity Input" );
	percent->setRange( 0, 100, 1, true );
	percent->setValue( m_intensity );
	percent->setLabel( i18n( "&Intensity:" ) );
	percent->setSuffix( QString::fromAscii( "%" ) );
	connect( percent, SIGNAL( valueChanged( int ) ), this, SLOT( setIntensity( int ) ) );

	int result = dlg.exec();
	if( result == QDialog::Accepted )
	{
		applyIntensity();
		m_pViewer->setModified( true );
	}
	else
		if( m_image )
			m_pViewer->canvas()->setImage( *m_image );
	m_lastintensity = -1;
	delete m_image;
	m_image = 0;
}

void KViewEffects::setIntensity( int intensity )
{
	m_intensity = intensity;
}

void KViewEffects::applyIntensity()
{
	kdDebug( 4630 ) << k_funcinfo << endl;
	if( m_intensity == m_lastintensity )
		return; // nothing to do

	QImage * work = workImage();
	if( work )
	{
		KImageEffect::intensity( *work, m_intensity * 0.01 );
		m_pViewer->canvas()->setImage( *work );
		delete work;
		m_lastintensity = m_intensity;
	}
}

void KViewEffects::blend()
{
	KDialogBase dlg( m_pViewer->widget(), "Blend Color Dialog", true /*modal*/, i18n( "Blend Color" ), KDialogBase::Ok | KDialogBase::Try | KDialogBase::Cancel );
	connect( &dlg, SIGNAL( tryClicked() ), this, SLOT( applyBlend() ) );

	QVBox * vbox = new QVBox( &dlg );
	vbox->setSpacing( KDialog::spacingHint() );
	dlg.setMainWidget( vbox );
	KIntNumInput * opacity = new KIntNumInput( vbox, "Opacity Input" );
	opacity->setRange( 0, 100, 1, true );
	opacity->setValue( m_opacity );
	opacity->setLabel( i18n( "O&pacity:" ) );
	opacity->setSuffix( QString::fromAscii( "%" ) );
	connect( opacity, SIGNAL( valueChanged( int ) ), this, SLOT( setOpacity( int ) ) );
	QLabel * label = new QLabel( i18n( "Blend c&olor:" ), vbox );
	KColorButton * color = new KColorButton( m_color, vbox, "Color Input Button" );
	label->setBuddy( color );
	connect( color, SIGNAL( changed( const QColor & ) ), this, SLOT( setColor( const QColor & ) ) );

	int result = dlg.exec();
	if( result == QDialog::Accepted )
	{
		applyBlend();
		m_pViewer->setModified( true );
	}
	else
		if( m_image )
			m_pViewer->canvas()->setImage( *m_image );
	m_lastopacity = -1;
	delete m_image;
	m_image = 0;
}

void KViewEffects::setOpacity( int opacity )
{
	m_opacity = opacity;
}

void KViewEffects::setColor( const QColor & color )
{
	m_color = color;
}

void KViewEffects::applyBlend()
{
	if( m_opacity == m_lastopacity )
		return; // nothing to do

	QImage * work = workImage();
	if( work )
	{
		KImageEffect::blend( m_color, *work, m_opacity * 0.01 );
		m_pViewer->canvas()->setImage( *work );
		delete work;
		m_lastopacity = m_opacity;
	}
}

void KViewEffects::gamma()
{
	KDialogBase dlg( m_pViewer->widget(), "Gamma Correction Dialog", true /*modal*/, i18n( "Gamma Correction" ), KDialogBase::Ok | KDialogBase::Try | KDialogBase::Cancel );
	connect( &dlg, SIGNAL( tryClicked() ), this, SLOT( applyGammaCorrection() ) );

	// create dialog
	KDoubleNumInput * gammavalue = new KDoubleNumInput( 0.0, 1.0, 0.5, 0.01, 4, &dlg, "Gamma value input" );
	gammavalue->setRange( 0.0, 1.0, 0.01, true );
	connect( gammavalue, SIGNAL( valueChanged( double ) ), this, SLOT( setGammaValue( double ) ) );
	gammavalue->setLabel( i18n( "Gamma value:" ) );
	dlg.setMainWidget( gammavalue );

	int result = dlg.exec();
	if( result == QDialog::Accepted )
	{
		// apply gamma correction
		applyGammaCorrection();
		m_pViewer->setModified( true );
	}
	else
	{
		if( m_image )
			m_pViewer->canvas()->setImage( *m_image );
	}
	m_lastgamma = -1;
	delete m_image;
	m_image = 0;
}

void KViewEffects::setGammaValue( double gamma )
{
	m_gamma = gamma;
	kdDebug( 4630 ) << "m_gamma = " << m_gamma << endl;
	// TODO: show effect on the fly if requested
}

void KViewEffects::applyGammaCorrection()
{
	if( m_gamma == m_lastgamma )
		return; // nothing to do

	QImage * corrected = workImage();
	if( corrected )
	{
		KIconEffect::toGamma( *corrected, m_gamma );
		m_pViewer->canvas()->setImage( *corrected );
		delete corrected;
		m_lastgamma = m_gamma;
	}
}

inline QImage * KViewEffects::workImage()
{
	if( ! m_image )
	{
		const QImage * canvasimage = m_pViewer->canvas()->image();
		if( canvasimage )
			m_image = new QImage( *canvasimage );
	}
	if( m_image )
	{
		QImage * changed = new QImage( *m_image );
		changed->detach();
		return changed;
	}
	return 0;
}

// vim:sw=4:ts=4:cindent
#include "kvieweffects.moc"
