/***************************************************************************
                          keducabuilder.cpp  -  description
                             -------------------
    begin                : Sat May 26 2001
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

#include "keducabuilder.h"
#include "keducabuilder.moc"

#include "kcontroladdedit.h"
#include "kcontrolheader.h"
#include "keducaeditorstartdialog.h"
#include "../libkeduca/kgallerydialog.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kedittoolbar.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kprinter.h>

#include <qlayout.h>
#include <qcheckbox.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>

static const char* const keducabuilder_data[] = {
    "15 16 4 1",
    "a c #0000ff",
    "# c #008183",
    "b c #c5c2c5",
    ". c #ffffff",
    "...............",
    ".#############.",
    ".#...........#.",
    ".#...aaaa....#.",
    ".#..aa.baa...#.",
    ".#......aa...#.",
    ".#.....aab...#.",
    ".#....aab....#.",
    ".#....aab....#.",
    ".#.....b.....#.",
    ".#....aa.....#.",
    ".#....aa.....#.",
    ".#...........#.",
    ".#...........#.",
    ".#############.",
    "..............."};

KEducaBuilder::KEducaBuilder(QWidget* parent, const char *name, WFlags f )
    : KMainWindow(parent,name,f),
      _nativeFormat("application/x-edu")
{
    _keducaFile = new FileRead();
    initMenuBar();
    init();
    configRead();
    setIcon( KGlobal::iconLoader()->loadIcon("control.png",
                                                   KIcon::Small) );
    _isOpenFile = false;
}

KEducaBuilder::~KEducaBuilder()
{
    delete _keducaFile;
}

/** Init graphical interface */
void KEducaBuilder::init()
{
    QWidget *mainView = new QWidget(this, "mainview");

    QVBoxLayout *form2Layout = new QVBoxLayout( mainView );
    form2Layout->setSpacing( 2 );
    form2Layout->setMargin( 0 );

    _split = new QSplitter( mainView );
    _split->setOrientation( QSplitter::Vertical );

    _listAnswer = new KListBox( _split, "_listAnswer" );
    _listAnswer->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, _listAnswer->sizePolicy().hasHeightForWidth() ) );
    connect( _listAnswer,  SIGNAL( highlighted(QListBoxItem *) ), this, SLOT( slotPreview(QListBoxItem *) ) );
    connect( _listAnswer,  SIGNAL( doubleClicked(QListBoxItem *) ), this, SLOT( slotEditbyList(QListBoxItem *) ) );

    _preview = new QTextView( _split, "_preview" );
    _preview->setFrameShape( QTextView::NoFrame );
    _preview->setLineWidth( 1 );

    form2Layout->addWidget( _split );

    setCentralWidget( mainView );
    if (!initialGeometrySet())
       resize( QSize(500, 400).expandedTo(minimumSizeHint()));
    setupGUI( ToolBar | Keys | StatusBar | Create, "keducabuilderui.rc" );
    setAutoSaveSettings();
}

/** Init menu bar settings */
void KEducaBuilder::initMenuBar()
{
    KStdAction::openNew(this, SLOT( slotFileOpenNew() ), actionCollection());
    KStdAction::open   (this, SLOT( slotFileOpen() ), actionCollection());
    (void)new KAction(i18n("Open &Gallery..."), 0, 0, this, SLOT( slotGallery() ), actionCollection(), "open_gallery");
    _recentFiles = KStdAction::openRecent(this, SLOT ( slotFileOpenURL(const KURL &)), actionCollection());
    KStdAction::save   (this, SLOT( slotFileSave() ), actionCollection());
    KStdAction::saveAs (this, SLOT( slotFileSaveAs() ), actionCollection());
    KStdAction::print  (this, SLOT( slotFilePrint() ), actionCollection());
    (void)new KAction(i18n("Document Info"), "info", 0, this, SLOT( slotHeader() ), actionCollection(), "info_doc");
    KStdAction::quit(this, SLOT( close() ), actionCollection());

    (void)new KAction(i18n("&Add..."), "addquestion", 0, this, SLOT( slotAdd() ), actionCollection(), "question_add");
    (void)new KAction(i18n("&Edit..."), "editquestion", 0, this, SLOT( slotEdit() ), actionCollection(), "question_edit");
    (void)new KAction(i18n("&Remove"), "delquestion", 0, this, SLOT( slotRemove() ), actionCollection(), "question_remove");
    (void)new KAction(i18n("&Up"), "up", 0, this, SLOT( slotUp() ), actionCollection(), "question_up");
    (void)new KAction(i18n("&Down"), "down", 0, this, SLOT( slotDown() ), actionCollection(), "question_down");
}

/** Delete current document and start new */
void KEducaBuilder::slotFileOpenNew()
{
    if (!currentFileMayBeReplaced())
        return;

    if( _keducaFile ) delete _keducaFile;
    _listAnswer->clear();
    _preview->setText("<qt><br><center><h1>"+i18n("Welcome To KEduca!")+"</h1>"+
                      i18n("Add a new Question by using the Edit Menu or by choosing one of the icons above.")+
                      "</center></qt>");
    _keducaFile = new FileRead();
    _isOpenFile = false;
    // ask for meta information
    slotHeader();
}

/** Open new document. */
void KEducaBuilder::slotFileOpen()
{
    KURL url = KFileDialog::getOpenURL( QString::null, _nativeFormat, this,  i18n("Open Educa File") );

    if( !url.isEmpty() )
        slotFileOpenURL( url );
}

/** Open new file */
void KEducaBuilder::slotFileOpenURL( const KURL &url )
{
    if (!currentFileMayBeReplaced())
        return;

    QPixmap image0( ( const char** ) keducabuilder_data );
    delete _keducaFile;
    _keducaFile = new FileRead();
    if( !_keducaFile->openFile(url) ) { return; }
    _recentFiles->addURL( url );
    _listAnswer->clear();
    _isOpenFile = true;

    _keducaFile->recordFirst();
    while( !_keducaFile->recordEOF() )
    {
        _listAnswer->insertItem( image0, _keducaFile->getQuestion( FileRead::QF_TEXT ) );
        _keducaFile->recordNext();
    }
}

/** Item select */
void KEducaBuilder::slotPreview( QListBoxItem *item )
{
//    bool match = false;
    QString tmpListanswer;

    // FIND THE SELECT RECORD
    if( !item ) return;

//    kdDebug() << "Item : " << _listAnswer->currentItem() <<endl;
    _keducaFile->recordAt( _listAnswer->currentItem() );

    // START THE PREVIEW VALUES
    tmpListanswer = "<p><b>" + i18n("Answers") + "</b></p><hr>";
    tmpListanswer += "<p>";

    tmpListanswer +="<TABLE WIDTH=98%>";
    _keducaFile->recordAnswerFirst();
    while( !_keducaFile->recordAnswerEOF() )
    {
        tmpListanswer += "<TR>";
        tmpListanswer += "<TD WIDTH=3% BGCOLOR=";
        _keducaFile->getAnswerValue() ? tmpListanswer += "#006b6b>" : tmpListanswer += "#b84747>";
        if( _keducaFile->getQuestionInt( FileRead::QF_TYPE ) == 3 )
        {
            tmpListanswer += "<FONT COLOR=#ffffff><b>"
                          + _keducaFile->getAnswer( FileRead::AF_POINT )
                          + "</b></FONT>";
        }
        tmpListanswer += "</TD>";
        tmpListanswer += "<TD WIDTH=95% BGCOLOR=";
        _keducaFile->getAnswerValue() ? tmpListanswer += "#daeaea>" : tmpListanswer += "#eadada>";
        tmpListanswer += "<span style=\"color:black;\">"+_keducaFile->getAnswer( FileRead::AF_TEXT )+"</span>";
        tmpListanswer += "</TD></TR>";
        _keducaFile->recordAnswerNext();
    };
    tmpListanswer += "</TABLE>";

    if( !_keducaFile->getQuestion( FileRead::QF_PICTURE ).isEmpty() )
    {
        tmpListanswer += "<table><tr><td><img src=";
        tmpListanswer += _keducaFile->getPicture();
        tmpListanswer += "></td>";
        tmpListanswer += "<td align=right><b>" + i18n("Question Image") + "</b><hr>";
        tmpListanswer += _keducaFile->getQuestion( FileRead::QF_PICTURE ) + "</td></tr></table>";
    };

    if( _keducaFile->getQuestionInt( FileRead::QF_POINTS ) > 0 )
    {
        tmpListanswer += "<p><b>" + i18n("Question point") + ":</b> ";
        tmpListanswer += _keducaFile->getQuestion( FileRead::QF_POINTS );
    }

    if( _keducaFile->getQuestionInt( FileRead::QF_TIME ) > 0 )
    {
        tmpListanswer += "<p><b>"+i18n("Time")+":</b> ";
        tmpListanswer += _keducaFile->getQuestion( FileRead::QF_TIME );
        tmpListanswer += i18n(" seconds</p>");
    }

    if( !_keducaFile->getQuestion( FileRead::QF_TIP ).isEmpty() )
    {
        tmpListanswer += "<p><b>" + i18n("Tip") +":</b> ";
        tmpListanswer += _keducaFile->getQuestion( FileRead::QF_TIP );
        tmpListanswer += "</p>";
    }

    if( !_keducaFile->getQuestion( FileRead::QF_EXPLAIN ).isEmpty() )
    {
        tmpListanswer += "<p><b>"
                      + i18n("Explain") + ":</b>"
                      + _keducaFile->getQuestion( FileRead::QF_EXPLAIN )
                      + "</p>";
    }

    _preview->setText( tmpListanswer );
}

/** Read config data */
void KEducaBuilder::configRead()
{
    KConfig *config = KGlobal::config();

    config->setGroup( "keducacontrol" );
    applyMainWindowSettings( config, "keducacontrol" );
    _split->setSizes( config->readIntListEntry("Splitter_size") );
    _recentFiles->loadEntries(config);
}

/** Write settings */
void KEducaBuilder::configWrite()
{
    KConfig *config = KGlobal::config();

    config->setGroup( "keducacontrol" );
    saveMainWindowSettings( config, "keducacontrol" );
    config->writeEntry("Splitter_size", _split->sizes() );
    _recentFiles->saveEntries(config);
    config->sync();
}

bool KEducaBuilder::currentFileMayBeReplaced()
{
    if (_keducaFile->changed())
    {
        int res = KMessageBox::warningYesNoCancel( widget(),
            // ask the user to save
            i18n( "The document \"%1\" has been modified.\n"
                  "Do you want to save it?" ).arg( _keducaFile->getCurrentURL().fileName() ),
            i18n( "Save Document?" ), KStdGuiItem::save(), KStdGuiItem::discard() );
        switch (res)
        {
        case KMessageBox::Yes:
            // If the saving was successful, then the file may be replaced
            return fileSave();
            break;
        case KMessageBox::Cancel:
            // User cancels operation, do not replace the file
            return false;
            break;
        default: ;
        }
    }

    // file was not changed, it may be replaced
    return true;
}

/** Close the window */
bool KEducaBuilder::queryClose ()
{
    if (_keducaFile) {
        if (!currentFileMayBeReplaced())
            return false;
    }
    configWrite();
    return( TRUE );
}

/** Add question */
void KEducaBuilder::slotAdd()
{
    QPixmap image0( ( const char** ) keducabuilder_data );

    KControlAddEdit edit(this);
    edit.init( _keducaFile, false );
    if( !edit.exec() ) return;

    _listAnswer->insertItem( image0, _keducaFile->getQuestion( FileRead::QF_TEXT ) );
    _listAnswer->setCurrentItem( _listAnswer->count()-1 );
    slotPreview( _listAnswer->item(_listAnswer->currentItem()) );
}

/** Edit question */
void KEducaBuilder::slotEdit()
{
    if( _listAnswer->currentItem() == -1 ) return;

    QPixmap image0( ( const char** ) keducabuilder_data );

    KControlAddEdit edit(this);
    edit.init( _keducaFile, true );
    if( edit.exec() )
    {
        _listAnswer->changeItem( image0, _keducaFile->getQuestion( FileRead::QF_TEXT ), _listAnswer->currentItem() );
        slotPreview( _listAnswer->item(_listAnswer->currentItem()) );
    }
}

/** Edit question */
void KEducaBuilder::slotRemove()
{
    if( _listAnswer->currentItem() == -1 ) return;
    _keducaFile->recordDelete();
    _listAnswer->removeItem( _listAnswer->currentItem() );
}

/** Up record */
void KEducaBuilder::slotUp()
{
    int current = _listAnswer->currentItem();

    if( current < 1 ) return;
    QPixmap image0( ( const char** ) keducabuilder_data );

    _keducaFile->recordSwap( true );
    QString tmpString = _listAnswer->text( current-1 );
    _listAnswer->changeItem( image0, _listAnswer->text( current ), current-1 );
    _listAnswer->changeItem( image0, tmpString, current );
    _listAnswer->setCurrentItem( current-1 );
}

/** Down record */
void KEducaBuilder::slotDown()
{
    int current = _listAnswer->currentItem();

    if( current == -1 || current == (signed)(_listAnswer->count()-1) ) return;
    QPixmap image0( ( const char** ) keducabuilder_data );

    _keducaFile->recordSwap( true );
    QString tmpString = _listAnswer->text( current+1 );
    _listAnswer->changeItem( image0, _listAnswer->text( current ), current+1 );
    _listAnswer->changeItem( image0, tmpString, current );
    _listAnswer->setCurrentItem( current+1 );
}

/** Save file */
void KEducaBuilder::slotFileSave()
{
    fileSave();
}
bool KEducaBuilder::fileSave()
{
    if( !_isOpenFile )	{ return fileSaveAs();  }

    if( checkSave() ) return _keducaFile->saveFile();
    return false;
}

/** Save As... */
void KEducaBuilder::slotFileSaveAs()
{
    fileSaveAs();
}

bool KEducaBuilder::fileSaveAs()
{
    if( !checkSave() ) return false;

    QCheckBox *saveCompressedCheckBox = new QCheckBox( i18n( "Compress the file" ), 0 );
    saveCompressedCheckBox->setChecked( true );

    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, this, "file dialog", true, saveCompressedCheckBox);
    dialog->setCaption( i18n("Save Document As") );
    dialog->setKeepLocation( true );
    dialog->setOperationMode( KFileDialog::Saving );
    QStringList mimeFilter(_nativeFormat);
    dialog->setMimeFilter( mimeFilter, _nativeFormat );
    KURL newURL;
    QString outputFormat (_nativeFormat);

    bool bOk;
    do {
        bOk=true;
        if(dialog->exec()==QDialog::Accepted) {
            newURL=dialog->selectedURL();
            outputFormat=dialog->currentMimeFilter();
        }
        else
        {
            bOk = false;
            break;
        }
        kdDebug()<<"Requested saving to file "<<newURL.prettyURL() <<endl;
        if ( newURL.isEmpty() )
        {
            bOk = false;
            break;
        }

        if ( QFileInfo( newURL.path() ).extension().isEmpty() ) {
            // No more extensions in filters. We need to get it from the mimetype.
            KMimeType::Ptr mime = KMimeType::mimeType( outputFormat );
            QString extension = mime->property( "X-KDE-NativeExtension" ).toString();
            kdDebug() << "KEducaBuilder::slotFileSaveAs outputFormat=" << outputFormat << " extension=" << extension << endl;
            newURL.setPath( newURL.path() + extension );
        }

        if ( KIO::NetAccess::exists( newURL, false, this ) ) { // this file exists => ask for confirmation
            bOk = KMessageBox::warningContinueCancel( this,
                                              i18n("A document with this name already exists.\n"\
                                                   "Do you want to overwrite it?"),
                                              i18n("Warning"), i18n("Overwrite") ) == KMessageBox::Continue;
        }
    } while ( !bOk );

    bool saveCompressed = saveCompressedCheckBox->isChecked();

    delete dialog;

    if (bOk) {
        bool copyImages = KMessageBox::questionYesNo( this,
                            i18n("If you want to share this document, it is better to copy the images to the same folder as the document.\n"\
                            "Do you want to copy images?"),
                            i18n("Warning"), i18n("Copy Images"), i18n("Do Not Copy") ) == KMessageBox::Yes;
        if (_keducaFile->saveFile( newURL, copyImages, saveCompressed ) ) {
            _recentFiles->addURL(newURL);
            // we saved the file, so the internal representation is now present as a file
            _isOpenFile=true;
            return true;
        }else {
            kdDebug()<< "saving of file failed" <<endl;
            return false;
        }
    }else {
        kdDebug()<< "KEducaBuilder: no valid filename selected" <<endl;
        return false;
    }
}

/** Print file */
void KEducaBuilder::slotFilePrint()
{
    KPrinter *printer = new KPrinter();

    QString file = _keducaFile->getCurrentURL().fileName();
    if (file.isEmpty())
	file = i18n("Test");
    if(printer->setup(this, i18n("Print %1").arg(file)))
    {
        QPaintDeviceMetrics metrics(printer);
        QPainter p;

        const int margin=20;
        int yPos=0;
        QString text;

        p.begin(printer);
        p.setFont( QFont(font().family(), 12, QFont::Bold) );
        QFontMetrics fm = p.fontMetrics();

        _keducaFile->recordFirst();
        while(!_keducaFile->recordEOF())
        {
            p.drawText( 10, yPos+margin,  metrics.width(), fm.lineSpacing(),
                        ExpandTabs | DontClip, _keducaFile->getQuestion(FileRead::QF_TEXT) );
            yPos += fm.lineSpacing();

            p.drawLine(10, yPos+margin, metrics.width(), yPos+margin);
            yPos += 2*fm.lineSpacing();

            _keducaFile->recordNext();
        }

        p.end();
    }

    delete printer;

}

/** Header info */
int KEducaBuilder::slotHeader() {
    KControlHeader infoDoc(_keducaFile);
    return infoDoc.exec();
}

/** Check if the current document can save */
bool KEducaBuilder::checkSave()
{
    if( _keducaFile->getHeader("title").isEmpty() 		||
        _keducaFile->getHeader("category").isEmpty() ||
        _keducaFile->getHeader("type").isEmpty() 		||
        _keducaFile->getHeader("level").isEmpty()		||
        _keducaFile->getHeader("language").isEmpty() )
    {
        if (KMessageBox::warningContinueCancel(this, i18n("Unable to save file.\n\nYou must complete the Document Info\n(Only the description is necessary)"), QString::null, i18n("Complete Document Info..."))
            == KMessageBox::Continue) {
            if (slotHeader()!=KControlHeader::Accepted)
                return false;
        }else
            return false;
    }

    if( _listAnswer->count() < 1 )
    {
        if (KMessageBox::warningContinueCancel(this, i18n("Unable to save file.\n\nYou must insert a question."),
                                               QString::null, i18n("Insert Question"))==KMessageBox::Continue)
            slotAdd();
        else
            return false;
    }

    return true;
}

bool KEducaBuilder::startChooser() {
    KEducaEditorStartDialog startDialog(this, "KEducaEditorStartDialog", true);

    switch (startDialog.exec()) {
    case KEducaEditorStartDialog::OpenDoc:
    case KEducaEditorStartDialog::OpenRecentDoc:
        slotFileOpenURL(startDialog.getURL());
        break;
    case KEducaEditorStartDialog::NewDoc:
        slotFileOpenNew();
        break;
    case KEducaEditorStartDialog::Gallery:
        initGallery();
        break;
    default:
        close();
        break;
    }

    return true;
}
/** Init Internet Gallery Servers */
void KEducaBuilder::initGallery(const KURL &urlFile)
{
    KGalleryDialog galleries;

    if( !urlFile.url().isEmpty() )
      galleries.putURL( urlFile );

    if( galleries.exec() == QDialog::Accepted )
      slotFileOpenURL( galleries.getURL() );
}

/** Init Gallery */
void KEducaBuilder::slotGallery()
{
  initGallery();
}

/** Double click in list, edit question */
void KEducaBuilder::slotEditbyList(QListBoxItem *item)
{
  if( item )
    slotEdit();
}
