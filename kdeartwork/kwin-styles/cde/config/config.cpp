// $Id: config.cpp 390983 2005-02-19 17:29:38Z giessl $
#include "config.h"
#include <kapplication.h>
#include <kglobal.h>
#include <qwhatsthis.h>
#include <qvbox.h>
#include <klocale.h>

extern "C" KDE_EXPORT QObject* allocate_config( KConfig* conf, QWidget* parent )
{
	return new CdeConfig(conf, parent);
}


/* NOTE: 
 * 'conf' 	is a pointer to the kwindecoration modules open kwin config,
 *			and is by default set to the "Style" group.
 *
 * 'parent'	is the parent of the QObject, which is a VBox inside the
 *			Configure tab in kwindecoration
 */

CdeConfig::CdeConfig( KConfig* conf, QWidget* parent )
	: QObject( parent )
{
	cdeConfig = new KConfig("kwincderc");
	KGlobal::locale()->insertCatalogue("kwin_art_clients");
	
	groupBox = new QVBox( parent );
	
	bgAlign = new QButtonGroup( 3, Qt::Horizontal, i18n("Text &Alignment"), groupBox );
	bgAlign->setExclusive( true );
	QWhatsThis::add( bgAlign, i18n("Use these buttons to set the alignment of the titlebar caption text.") );
	new QRadioButton( i18n("Left"), bgAlign, "AlignLeft" );
	QRadioButton *radio2 = new QRadioButton( i18n("Centered"), bgAlign, "AlignHCenter" );
	radio2->setChecked( true );
	new QRadioButton( i18n("Right"), bgAlign, "AlignRight" );
	
	cbColorBorder = new QCheckBox( i18n("Draw window frames using &titlebar colors"), groupBox );
	QWhatsThis::add( cbColorBorder, i18n("When selected, the window decoration borders "
					     "are drawn using the titlebar colors. Otherwise, they are "
					     "drawn using normal border colors instead.") );
	
//	cbTitlebarButton = new QCheckBox( i18n("Titlebar acts like a &pushbutton when clicked"), groupBox );
//	QWhatsThis::add( cbTitlebarButton, i18n("When selected, this option causes the window titlebar to behave "
//						"as if it was a pushbutton when you click it to move the window.") );
	
	(void) new QLabel( i18n("Tip: If you want the look of the original Motif(tm) Window Manager,\n"
				"click the \"Buttons\" tab above and remove the help\n"
				"and close buttons from the titlebar."), groupBox );
	
	// Load configuration options
	load( conf );

	// Ensure we track user changes properly
	connect( cbColorBorder, SIGNAL(clicked()), SLOT(slotSelectionChanged()) );
//	connect( cbTitlebarButton, SIGNAL(clicked()), SLOT(slotSelectionChanged()) );
	connect( bgAlign, SIGNAL(clicked(int)), SLOT(slotSelectionChanged(int)) );

	// Make the widgets visible in kwindecoration
	groupBox->show();
}


CdeConfig::~CdeConfig()
{
	delete bgAlign;
	delete groupBox;
	delete cdeConfig;
}


void CdeConfig::slotSelectionChanged()
{
	emit changed();
}

void CdeConfig::slotSelectionChanged( int )
{
	emit changed();
}

// Loads the configurable options from the kwinrc config file
// It is passed the open config from kwindecoration to improve efficiency
void CdeConfig::load( KConfig* /*conf*/ )
{
	cdeConfig->setGroup("General");

	QString value = cdeConfig->readEntry( "TextAlignment", "AlignHCenter" );
	QRadioButton *button = (QRadioButton*)bgAlign->child( (const char *)value.latin1() );
	if ( button )
	    button->setChecked( true );

	bool coloredFrame = cdeConfig->readBoolEntry( "UseTitleBarBorderColors", true );
	cbColorBorder->setChecked( coloredFrame );

//	bool titlebarButton = cdeConfig->readBoolEntry( "TitlebarButtonMode", true );
//	cbTitlebarButton->setChecked( titlebarButton );
}


// Saves the configurable options to the kwinrc config file
void CdeConfig::save( KConfig* /*conf*/ )
{
	cdeConfig->setGroup("General");

	QRadioButton *button = (QRadioButton*)bgAlign->selected();
	if ( button )
	    cdeConfig->writeEntry( "TextAlignment", QString(button->name()) );

	cdeConfig->writeEntry( "UseTitleBarBorderColors", cbColorBorder->isChecked() );
//	cdeConfig->writeEntry( "TitlebarButtonMode", cbTitlebarButton->isChecked() );
	
	// Ensure others trying to read this config get updated
	cdeConfig->sync();
}


// Sets UI widget defaults which must correspond to style defaults
void CdeConfig::defaults()
{
	QRadioButton *button = (QRadioButton*)bgAlign->child( "AlignHCenter" );
	if ( button )
	    button->setChecked( true );

	cbColorBorder->setChecked( true );
//	cbTitlebarButton->setChecked( true );
}

#include "config.moc"
// vim: ts=4
