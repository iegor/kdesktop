//-----------------------------------------------------------------------------
//
// kbanner - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// clock function and color cycling added 2000/01/09 by Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
// 2001/03/04 Converted to use libkscreensaver by Martin R. Jones
// 2002/04/07 Added random vertical position of text,
//		changed horizontal step size to reduce jerkyness,
//		text will return to right margin immediately (and not be drawn half a screen width off-screen)
//		Harald H.-J. Bongartz <harald@bongartz.org>
// 2003/09/06 Converted to use KDialogBase - Nadeem Hasan <nhasan@kde.org>
#include <stdlib.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qfontdatabase.h>
#include <qpainter.h>

#include <kapplication.h>
#include <krandomsequence.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kcolorbutton.h>
#include <kfontcombo.h>

#include "banner.h"
#include "banner.moc"

// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kbanner.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "KBanner" );
    KDE_EXPORT const char *kss_version = "2.2.0";

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new KBannerSaver( id );
    }

    KDE_EXPORT QDialog *kss_setup()
    {
        return new KBannerSetup();
    }
}

//-----------------------------------------------------------------------------

KBannerSetup::KBannerSetup( QWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n( "Setup Banner Screen Saver" ),
	  Ok|Cancel|Help, Ok, true ), saver( 0 ), ed(0), speed( 50 )
{
	setButtonText( Help, i18n( "A&bout" ) );
	readSettings();

    QWidget *main = makeMainWidget();

	QLabel *label;

	QVBoxLayout *tl = new QVBoxLayout(main, 0, spacingHint());
	QHBoxLayout *tl1 = new QHBoxLayout( 0, 0, spacingHint() );
	tl->addLayout(tl1);
	QVBoxLayout *tl11 = new QVBoxLayout( 0, 0, spacingHint() );
	tl1->addLayout(tl11);

	QGroupBox *group = new QGroupBox( 0, Vertical, i18n("Font"), main );
	QGridLayout *gl = new QGridLayout(group->layout(), 6, 2, spacingHint() );

	label = new QLabel( i18n("Family:"), group );
	gl->addWidget(label, 1, 0);

	KFontCombo* comboFonts = new KFontCombo( QFontDatabase().families(), group );
	comboFonts->setCurrentFont( fontFamily );
	gl->addWidget(comboFonts, 1, 1);
	connect( comboFonts, SIGNAL( activated( const QString& ) ),
			SLOT( slotFamily( const QString& ) ) );

	label = new QLabel( i18n("Size:"), group );
	gl->addWidget(label, 2, 0);

	comboSizes = new QComboBox( TRUE, group );
        fillFontSizes();
	gl->addWidget(comboSizes, 2, 1);
	connect( comboSizes, SIGNAL( activated( int ) ), SLOT( slotSize( int ) ) );
	connect( comboSizes, SIGNAL( textChanged( const QString & ) ),
			SLOT( slotSizeEdit( const QString &  ) ) );

	QCheckBox *cb = new QCheckBox( i18n("Bold"),
				       group );
	cb->setChecked( bold );
	connect( cb, SIGNAL( toggled( bool ) ), SLOT( slotBold( bool ) ) );
	gl->addWidget(cb, 3, 0);

	cb = new QCheckBox( i18n("Italic"), group );
	cb->setChecked( italic );
	gl->addWidget(cb, 3, 1);
	connect( cb, SIGNAL( toggled( bool ) ), SLOT( slotItalic( bool ) ) );

	label = new QLabel( i18n("Color:"), group );
	gl->addWidget(label, 4, 0);

	colorPush = new KColorButton( fontColor, group );
	gl->addWidget(colorPush, 4, 1);
	connect( colorPush, SIGNAL( changed(const QColor &) ),
		 SLOT( slotColor(const QColor &) ) );

   QCheckBox *cyclingColorCb=new QCheckBox(i18n("Cycling color"),group);
   cyclingColorCb->setMinimumSize(cyclingColorCb->sizeHint());
   gl->addMultiCellWidget(cyclingColorCb,5,5,0,1);
   connect(cyclingColorCb,SIGNAL(toggled(bool)),this,SLOT(slotCyclingColor(bool)));
   cyclingColorCb->setChecked(cyclingColor);

	preview = new QWidget( main );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new KBannerSaver( preview->winId() );
	tl1->addWidget(preview);

	tl11->addWidget(group);

	label = new QLabel( i18n("Speed:"), main );
	tl11->addStretch(1);
	tl11->addWidget(label);

	QSlider *sb = new QSlider(0, 100, 10, speed, QSlider::Horizontal, main );
	sb->setMinimumWidth( 180);
	sb->setFixedHeight(20);
    sb->setTickmarks(QSlider::Below);
    sb->setTickInterval(10);
	tl11->addWidget(sb);
	connect( sb, SIGNAL( valueChanged( int ) ), SLOT( slotSpeed( int ) ) );

	QHBoxLayout *tl2 = new QHBoxLayout;
	tl->addLayout(tl2);

	label = new QLabel( i18n("Message:"), main );
	tl2->addWidget(label);

	ed = new QLineEdit( main );
	tl2->addWidget(ed);
	ed->setText( message );
	connect( ed, SIGNAL( textChanged( const QString & ) ),
			SLOT( slotMessage( const QString &  ) ) );

   QCheckBox *timeCb=new QCheckBox( i18n("Show current time"), main);
   timeCb->setFixedSize(timeCb->sizeHint());
   tl->addWidget(timeCb,0,Qt::AlignLeft);
   connect(timeCb,SIGNAL(toggled(bool)),this,SLOT(slotTimeToggled(bool)));
   timeCb->setChecked(showTime);

   tl->addStretch();
}

// read settings from config file
void KBannerSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   speed=config->readNumEntry("Speed",50);
/*	if ( speed > 100 )
		speed = 100;
	else if ( speed < 50 )
		speed = 50;*/

   message=config->readEntry("Message","KDE");
   showTime=config->readBoolEntry("ShowTime",FALSE);
   fontFamily=config->readEntry("FontFamily",(QApplication::font()).family());
   fontSize=config->readNumEntry("FontSize",48);
   fontColor.setNamedColor(config->readEntry("FontColor","red"));
   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);
   bold=config->readBoolEntry("FontBold",FALSE);
   italic=config->readBoolEntry("FontItalic",FALSE);
}

void KBannerSetup::fillFontSizes()
{
    bool block = comboSizes->signalsBlocked();
    comboSizes->blockSignals( true );
    comboSizes->clear();
    int i = 0;
    sizes = QFontDatabase().pointSizes( fontFamily );
    sizes << 96 << 128 << 156 << 0;
    int current = 0;
    while ( sizes[i] )
	{
	QString num;
	num.setNum( sizes[i] );
	comboSizes->insertItem( num, i );
	if ( fontSize == sizes[i] ) // fontsize equals one of the defined ones
	    {
            current = i;
	    comboSizes->setCurrentItem( current );
	    slotSize( current );
	    }
	i++;
	}
    if ( current == 0 ) // fontsize seems to be entered by hand
        {
	QString fsize;
	fsize.setNum( fontSize );
	comboSizes->setEditText(fsize);
	slotSizeEdit( fsize );
	}
    comboSizes->blockSignals( block );
}

void KBannerSetup::slotFamily( const QString& fam )
{
	fontFamily = fam;
        fillFontSizes(); // different font, different sizes
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSize( int indx )
{
	fontSize = sizes[indx];
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSizeEdit( const QString& fs )
{
	bool ok;
        fontSize = fs.toInt( &ok, 10 );
	if ( ok )
		if ( saver )
			saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotColor( const QColor &col )
{
    fontColor = col;
    if ( saver )
	saver->setColor(fontColor);
}

void KBannerSetup::slotCyclingColor(bool on)
{
   colorPush->setEnabled(!on);
   cyclingColor=on;

   if ( saver )
   {
      saver->setCyclingColor( on );
      if ( !on )
         saver->setColor( fontColor );
   }
}

void KBannerSetup::slotBold( bool state )
{
	bold = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotItalic( bool state )
{
	italic = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( speed );
}

void KBannerSetup::slotMessage( const QString &msg )
{
	message = msg;
	if ( saver )
		saver->setMessage( message );
}

void KBannerSetup::slotTimeToggled( bool on )
{
   ed->setEnabled(!on);
   showTime=on;
   if (saver)
   {
      if (showTime)
         saver->setTimeDisplay();
      else
      {
         message=ed->text();
         saver->setMessage(message);
      };
   };
}

// Ok pressed - save settings and exit
void KBannerSetup::slotOk()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	config->writeEntry( "Speed", speed );
	config->writeEntry( "Message", message );
	config->writeEntry( "ShowTime", showTime );
	config->writeEntry( "FontFamily", fontFamily );

	QString fsize;
	if (fontSize == 0) // an non-number was entered in the font size combo
	{
		fontSize = 48;
	}
	fsize.setNum( fontSize );
	config->writeEntry( "FontSize", fsize );

	QString colName;
	colName.sprintf( "#%02x%02x%02x", fontColor.red(), fontColor.green(),
		fontColor.blue() );
	config->writeEntry( "FontColor", colName );
	config->writeEntry( "CyclingColor", cyclingColor );
	config->writeEntry( "FontBold", bold );
	config->writeEntry( "FontItalic", italic );

	config->sync();

	accept();
}

void KBannerSetup::slotHelp()
{
	KMessageBox::about(this,
			     i18n("Banner Version 2.2.1\n\nWritten by Martin R. Jones 1996\nmjones@kde.org\nExtended by Alexander Neundorf 2000\nalexander.neundorf@rz.tu-ilmenau.de\n"));
}

//-----------------------------------------------------------------------------

KBannerSaver::KBannerSaver( WId id ) : KScreenSaver( id )
{
	krnd = new KRandomSequence();
	readSettings();
	initialize();
	colorContext = QColor::enterAllocContext();
	needBlank = TRUE;
	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
}

KBannerSaver::~KBannerSaver()
{
	timer.stop();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
	delete krnd;
}

void KBannerSaver::setSpeed( int spd )
{
	timer.stop();
	int inv = 100 - spd;
	speed = 1 + ((inv * inv) / 100);
	timer.start( speed );
}

void KBannerSaver::setFont( const QString& family, int size, const QColor &color,
		bool b, bool i )
{
	fontFamily = family;
	fontSize = size;
	fontColor = color;
	bold = b;
	italic = i;

	initialize();
}

void KBannerSaver::setColor(QColor &color)
{
    fontColor = color;
    cyclingColor = FALSE;
    needUpdate = TRUE;
}

void KBannerSaver::setCyclingColor( bool on )
{
    cyclingColor = on;
    needUpdate = TRUE;
}

void KBannerSaver::setMessage( const QString &msg )
{
    showTime = FALSE;
    message = msg;
    pixmapSize = QSize();
    needBlank = TRUE;
}

void KBannerSaver::setTimeDisplay()
{
    showTime = TRUE;
    pixmapSize = QSize();
    needBlank = TRUE;
}

// read settings from config file
void KBannerSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   setSpeed( config->readNumEntry("Speed",50) );

   message=config->readEntry("Message","KDE");

   showTime=config->readBoolEntry("ShowTime",FALSE);

   fontFamily=config->readEntry("FontFamily",(QApplication::font()).family());

   fontSize=config->readNumEntry("FontSize",48);

   fontColor.setNamedColor(config->readEntry("FontColor","red"));

   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);

   bold=config->readBoolEntry("FontBold",FALSE);
   italic=config->readBoolEntry("FontItalic",FALSE);

    if ( cyclingColor )
    {
        currentHue=0;
        fontColor.setHsv(0,SATURATION,VALUE);
    }
}

// initialize font
void KBannerSaver::initialize()
{
	fsize = fontSize * height() / QApplication::desktop()->height();

	font = QFont( fontFamily, fsize, bold ? QFont::Bold : QFont::Normal, italic );

	pixmapSize = QSize();
	needBlank = TRUE;

	xpos = width();
	ypos = fsize + (int) ((double)(height()-fsize)*krnd->getDouble());
	step = 2 * width() / QApplication::desktop()->width(); // 6 -> 2 -hhjb-
	if ( step == 0 )
		step = 1;
}

// erase old text and draw in new position
void KBannerSaver::slotTimeout()
{
    if (cyclingColor)
    {
        int hueStep = speed/10;
        currentHue=(currentHue+hueStep)%360;
        fontColor.setHsv(currentHue,SATURATION,VALUE);
    }
    if (showTime)
    {
        QString new_message = KGlobal::locale()->formatTime(QTime::currentTime(), true);
	if( new_message != message )
	    needUpdate = TRUE;
	message = new_message;
    }
    if ( !pixmapSize.isValid() || cyclingColor || needUpdate || needBlank )
    {
        QRect rect = QFontMetrics( font ).boundingRect( message );
	rect.setWidth( rect.width() + step );
	if ( rect.width() > pixmapSize.width() )
	    pixmapSize.setWidth( rect.width() );
	if ( rect.height() > pixmapSize.height() )
	    pixmapSize.setHeight( rect.height() );
	pixmap = QPixmap( pixmapSize );
	pixmap.fill( black );
	QPainter p( &pixmap );
	p.setFont( font );
	p.setPen( fontColor );
	p.drawText( -rect.x(), -rect.y(), message );
	needUpdate = FALSE;
    }
    xpos -= step;
    if ( xpos < -pixmapSize.width() ) {
	QPainter p( this );
	p.fillRect( xpos + step, ypos, pixmapSize.width(), pixmapSize.height(), black );
	xpos = width();
	ypos = fsize + (int) ((double)(height()-2.0*fsize)*krnd->getDouble());
    }

    if ( needBlank )
    {
	setBackgroundColor( black );
	erase();
	needBlank = FALSE;
    }
    bitBlt( this, xpos, ypos, &pixmap, 0, 0, pixmapSize.width(), pixmapSize.height());
}
