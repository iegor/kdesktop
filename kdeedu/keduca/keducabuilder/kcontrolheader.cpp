/***************************************************************************
                          kcontrolheader.cpp  -  description
                             -------------------
    begin                : Tue May 29 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kcontrolheader.h"
#include "kcontrolheader.moc"

#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <klineedit.h>
#include <kcombobox.h>

#include <qlayout.h>
#include <qlabel.h>

KControlHeader::KControlHeader(FileRead *keducaFile, QWidget *parent, const char *name ) : KDialogBase(Swallow, i18n("Document Information"), Ok|Cancel, Ok, parent, name, true, true)
{
    _keducaFile = keducaFile;
    init();
    configRead();
}

KControlHeader::~KControlHeader(){}

/** Init graphical interface */
void KControlHeader::init()
{
    QWidget *mainView = new QWidget(this);

    QVBoxLayout *Form2Layout = new QVBoxLayout( mainView );
    Form2Layout->setSpacing( 6 );
    Form2Layout->setMargin( 0 );

    QFrame *Frame16 = new QFrame( mainView, "Frame16" );
    Frame16->setMinimumSize( QSize( 0, 50 ) );
    Frame16->setMaximumSize( QSize( 32767, 50 ) );
    Frame16->setFrameShape( QFrame::Box );
    Frame16->setFrameShadow( QFrame::Plain );
    Frame16->setPalette( QPalette( QColor(255, 255, 255) ) );
    QHBoxLayout *Frame16Layout = new QHBoxLayout( Frame16 );
    Frame16Layout->setSpacing( 6 );
    Frame16Layout->setMargin( 1 );

    QFrame *Frame17 = new QFrame( Frame16, "Frame17" );
    Frame17->setMinimumSize( QSize( 125, 0 ) );
    Frame17->setMaximumSize( QSize( 125, 32767 ) );
    Frame17->setFrameShape( QFrame::NoFrame );
    Frame17->setFrameShadow( QFrame::Raised );
    Frame17->setBackgroundPixmap( QPixmap(locate("appdata","pics/bk_frame.png") ) );
    QVBoxLayout *Frame17Layout = new QVBoxLayout( Frame17 );
    Frame17Layout->setSpacing( 6 );
    Frame17Layout->setMargin( 11 );

    QLabel *TextLabel1 = new QLabel( Frame17, "TextLabel1" );
    TextLabel1->setText( "<b style=\"text-decoration:underline;\">"+i18n("Information")+"</b>"  );
    TextLabel1->setBackgroundPixmap( QPixmap(locate("appdata","pics/bk_frame.png") ) );
    Frame17Layout->addWidget( TextLabel1 );
    Frame16Layout->addWidget( Frame17 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Frame16Layout->addItem( spacer );

    QLabel *TextLabel2 = new QLabel( Frame16, "TextLabel2" );
    TextLabel2->setText( i18n( "Description and rules of the project." ) );
    TextLabel2->setPalette( QPalette( QColor(255, 255, 255) ) );
    Frame16Layout->addWidget( TextLabel2 );
    Form2Layout->addWidget( Frame16 );

    QGroupBox *GroupBox7 = new QGroupBox( mainView, "GroupBox7" );
    GroupBox7->setTitle( i18n( "Description" ) );
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 0 );
    GroupBox7->layout()->setMargin( 0 );
    QVBoxLayout *GroupBox7Layout = new QVBoxLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );
    GroupBox7Layout->setSpacing( 6 );
    GroupBox7Layout->setMargin( 11 );

    QGridLayout *Layout13 = new QGridLayout;
    Layout13->setSpacing( 6 );
    Layout13->setMargin( 0 );

    QLabel *TextLabel10 = new QLabel( GroupBox7, "TextLabel10" );
    TextLabel10->setText( i18n( "Type:" ));
    TextLabel10->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout13->addWidget( TextLabel10, 2, 0 );

    QHBoxLayout *Layout12 = new QHBoxLayout;
    Layout12->setSpacing( 6 );
    Layout12->setMargin( 0 );

    _headerType = new KComboBox( FALSE, GroupBox7, "_headerType" );
    Layout12->addWidget( _headerType );

    QLabel *TextLabel11 = new QLabel( GroupBox7, "TextLabel11" );
    TextLabel11->setText( i18n( "Level:" ));
    TextLabel11->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout12->addWidget( TextLabel11 );

    _headerLevel = new KComboBox( FALSE, GroupBox7, "_headerLevel" );
    Layout12->addWidget( _headerLevel );
    Layout13->addLayout( Layout12, 2, 1 );

    QLabel *TextLabel12 = new QLabel( GroupBox7, "TextLabel12" );
    TextLabel12->setText( i18n( "Language:" ));
    TextLabel12->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout13->addWidget( TextLabel12, 3, 0 );

    _headerLang = new KLanguageCombo( GroupBox7, "_headerLang" );
    loadCountryList( _headerLang );
    Layout13->addWidget( _headerLang, 3, 1 );

    QLabel *TextLabel9 = new QLabel( GroupBox7, "TextLabel9" );
    TextLabel9->setText( i18n( "Category:" ));
    TextLabel9->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout13->addWidget( TextLabel9, 1, 0 );

    _headerCategory = new KComboBox( true, GroupBox7, "_headerCategory" );
    Layout13->addWidget( _headerCategory, 1, 1 );

    _headerTitle = new QLineEdit( GroupBox7, "_headerTitle" );
    Layout13->addWidget( _headerTitle, 0, 1 );

    QLabel *TextLabel8 = new QLabel( GroupBox7, "TextLabel8" );
    TextLabel8->setText( i18n( "Title:" ));
    TextLabel8->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout13->addWidget( TextLabel8, 0, 0 );
    GroupBox7Layout->addLayout( Layout13 );
    Form2Layout->addWidget( GroupBox7 );

    QGroupBox *GroupBox8 = new QGroupBox( mainView, "GroupBox8" );
    GroupBox8->setTitle( i18n( "Picture" ) );
    GroupBox8->setColumnLayout(0, Qt::Vertical );
    GroupBox8->layout()->setSpacing( 0 );
    GroupBox8->layout()->setMargin( 0 );
    QHBoxLayout *GroupBox8Layout = new QHBoxLayout( GroupBox8->layout() );
    GroupBox8Layout->setAlignment( Qt::AlignTop );
    GroupBox8Layout->setSpacing( 6 );
    GroupBox8Layout->setMargin( 11 );

    QLabel *TextLabel13 = new QLabel( GroupBox8, "TextLabel13" );
    TextLabel13->setText( i18n( "Default picture:" ));
    GroupBox8Layout->addWidget( TextLabel13 );

    _headerPicture = new KURLRequester( GroupBox8, "_headerPicture" );
    GroupBox8Layout->addWidget( _headerPicture );
    Form2Layout->addWidget( GroupBox8 );

    QGroupBox *GroupBox6 = new QGroupBox( mainView, "GroupBox6" );
    GroupBox6->setTitle( i18n( "Author" ) );
    GroupBox6->setColumnLayout(0, Qt::Vertical );
    GroupBox6->layout()->setSpacing( 0 );
    GroupBox6->layout()->setMargin( 0 );
    QGridLayout *GroupBox6Layout = new QGridLayout( GroupBox6->layout() );
    GroupBox6Layout->setAlignment( Qt::AlignTop );
    GroupBox6Layout->setSpacing( 6 );
    GroupBox6Layout->setMargin( 11 );

    _headerName = new QLineEdit( GroupBox6, "_headerName" );

    GroupBox6Layout->addWidget( _headerName, 0, 1 );

    _headerEmail = new QLineEdit( GroupBox6, "_headerEmail" );

    GroupBox6Layout->addWidget( _headerEmail, 1, 1 );

    QLabel *TextLabel7 = new QLabel( GroupBox6, "TextLabel7" );
    TextLabel7->setText( i18n( "Web page:" ));
    TextLabel7->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    GroupBox6Layout->addWidget( TextLabel7, 2, 0 );

    QLabel *TextLabel6 = new QLabel( GroupBox6, "TextLabel6" );
    TextLabel6->setText( i18n( "Email:" ));
    TextLabel6->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    GroupBox6Layout->addWidget( TextLabel6, 1, 0 );

    QLabel *TextLabel5 = new QLabel( GroupBox6, "TextLabel5" );
    TextLabel5->setText( i18n( "Name:" ));
    TextLabel5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    GroupBox6Layout->addWidget( TextLabel5, 0, 0 );

    _headerWWW = new QLineEdit( GroupBox6, "_headerWWW" );

    GroupBox6Layout->addWidget( _headerWWW, 2, 1 );
    Form2Layout->addWidget( GroupBox6 );

    // tab order
    setTabOrder( _headerTitle, _headerCategory );
    setTabOrder( _headerCategory, _headerType );
    setTabOrder( _headerType, _headerLevel );
    setTabOrder( _headerLevel, _headerLang );
    setTabOrder( _headerLang, _headerPicture->lineEdit() );
    setTabOrder( _headerPicture->lineEdit(), _headerPicture->comboBox() );
    setTabOrder( _headerPicture->comboBox(), _headerName );
    setTabOrder( _headerName, _headerEmail );
    setTabOrder( _headerEmail, _headerWWW );

    // set a sensible focus
    _headerTitle->setFocus();

    setMainWidget(mainView);
}

/** Insert header */
void KControlHeader::insertHeader()
{
    QStringList tmpResult = _defaultCategories.grep( _headerCategory->currentText() );
    if( tmpResult.count() == 0 )
    {
        KConfig *appconfig = KGlobal::config();
        _defaultCategories.append( _headerCategory->currentText() );
        appconfig->setGroup("General");
        appconfig->writeEntry( "Categories", _defaultCategories);
        appconfig->sync();
    }
    _keducaFile->setHeader( "category", 	_headerCategory->currentText() );
    _keducaFile->setHeader( "title",			_headerTitle->text() );
    _keducaFile->setHeader( "image",			_headerPicture->url() );
    _keducaFile->setHeader( "type", 			QString().setNum( _headerType->currentItem() + 1 ) );
    _keducaFile->setHeader( "level",			QString().setNum( _headerLevel->currentItem() + 1 ) );
    _keducaFile->setHeader( "language",	_headerLang->currentTag() );
    _keducaFile->setHeader( "name",	 		_headerName->text() );
    _keducaFile->setHeader( "email", 		_headerEmail->text() );
    _keducaFile->setHeader( "www", 			_headerWWW->text() );
}

/** Load country list */
void KControlHeader::loadCountryList(KLanguageCombo *combo)
{
    QString sub = QString::fromLatin1("l10n/");

    // clear the list
    combo->clear();

    QStringList regionlist = KGlobal::dirs()->findAllResources("locale", sub + QString::fromLatin1("*.desktop"));
    regionlist.sort();

    for ( QStringList::ConstIterator it = regionlist.begin();
          it != regionlist.end();
          ++it )
    {
        QString tag = *it;
        int index;

        index = tag.findRev('/');
        if (index != -1) tag = tag.mid(index + 1);

        index = tag.findRev('.');
        if (index != -1) tag.truncate(index);

        KSimpleConfig entry(*it);
        entry.setGroup(QString::fromLatin1("KCM Locale"));
        QString name = entry.readEntry(QString::fromLatin1("Name"));

        combo->insertSubmenu( name, '-' + tag, sub );
    }

    // add all languages to the list
    QStringList countrylist = KGlobal::dirs()->findAllResources("locale",  sub + QString::fromLatin1("*/entry.desktop"));
    countrylist.sort();

    for ( QStringList::ConstIterator it = countrylist.begin();
          it != countrylist.end(); ++it )
    {
        KSimpleConfig entry(*it);
        entry.setGroup(QString::fromLatin1("KCM Locale"));
        QString name = entry.readEntry(QString::fromLatin1("Name"));
        QString submenu = '-' + entry.readEntry(QString::fromLatin1("Region"));

        QString tag = *it;
        int index = tag.findRev('/');
        tag.truncate(index);
        index = tag.findRev('/');
        tag = tag.mid(index+1);
        combo->insertLanguage(tag, name, sub, submenu);
    }
}

/** Ok push */
void KControlHeader::slotOk()
{
    insertHeader();
    configWrite();
    accept();
}

/** Read settings */
void KControlHeader::configRead()
{
    KConfig *config = new KConfig("emaildefaults");
    KConfig *appconfig = KGlobal::config();

    config->setGroup("UserInfo");

    if( _keducaFile->getHeader( "name" ).isEmpty() )
        _headerName->setText(config->readEntry("FullName", QString::null ));
    else
        _headerName->setText( _keducaFile->getHeader("name") );

    if( _keducaFile->getHeader( "email" ).isEmpty() )
        _headerEmail->setText(config->readEntry("EmailAddress", QString::null ));
    else
        _headerEmail->setText( _keducaFile->getHeader("email") );

    _headerWWW->setText( _keducaFile->getHeader("www") );

    if( !_keducaFile->getHeader("language").isEmpty() )
    {
        _headerLang->setCurrentItem( _keducaFile->getHeader("language") );
    }
    else
    {
        config->setGroup("Locale");
        _headerLang->setCurrentItem( config->readEntry("Country") );
    }

    appconfig->setGroup("General");
    _defaultCategories = appconfig->readListEntry("Categories");
    if( _defaultCategories.count() == 0 )
    {
        _defaultCategories.append(i18n("Computers"));
    }
    _defaultCategories.sort();
    _headerCategory->insertStringList( _defaultCategories );
    if( !_keducaFile->getHeader( "category" ).isEmpty() ) _headerCategory->setEditText( _keducaFile->getHeader("category") );

    _headerType->insertItem( i18n("Test") );
    _headerType->insertItem( i18n("Test with Question Points") );
    _headerType->insertItem( i18n("Test with Answers Points") );
    _headerType->insertItem( i18n("Slidershow") );
    _headerType->insertItem( i18n("Exam") );
    _headerType->insertItem( i18n("Psychotechnic Test") );
    if( QString(_keducaFile->getHeader("type")).toInt() > 0 ) _headerType->setCurrentItem( QString(_keducaFile->getHeader("type")).toInt() -1 );

    _headerLevel->insertItem( i18n("Easy") );
    _headerLevel->insertItem( i18n("Normal") );
    _headerLevel->insertItem( i18n("Expert") );
    _headerLevel->insertItem( i18n("Supreme") );
    if( QString(_keducaFile->getHeader("level")).toInt() >0 ) _headerLevel->setCurrentItem( QString(_keducaFile->getHeader("level")).toInt() -1 );

    _headerPicture->setURL( _keducaFile->getHeader("image") );
    _headerTitle->setText( _keducaFile->getHeader("title") );

    appconfig->setGroup("kcontrolheader");
    QSize *defaultSize = new QSize(500,400);
    resize( appconfig->readSizeEntry("Geometry", defaultSize ) );
    delete config;
}

/** Write settings */
void KControlHeader::configWrite()
{
    KConfig *config = KGlobal::config();
    config->setGroup("kcontrolheader");
    config->writeEntry("Geometry", size() );
    config->sync();
}
