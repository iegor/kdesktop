/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2004 by Stanislav Visnovsky
                            <visnovsky@kde.org>
  Copyright (C) 2005, 2006 by Nicolas GOUTTE <goutte@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#include "catmanresource.h"
#include "catalogmanager.h"
#include "catmanlistitem.h"
#include "catalog.h"
#include "kbabeldictbox.h"

#include "resources.h"
#include "multiroughtransdlg.h"
#include "msgfmt.h"
#include "kbmailer.h"
#include "validateprogress.h"
#include "cvshandler.h"
#include "svnhandler.h"
#include "markpatterndialog.h"
#include "validationoptions.h"

#include <qcheckbox.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qpainter.h>

#include <kcmenumngr.h>
#include <kcursor.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kdatatool.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kprogress.h>
#include <kwin.h>
#include <kdeversion.h>
#include <ktempfile.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qwhatsthis.h>
#include <qheader.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtextedit.h>

using namespace KBabel;

const char* columnNames[] = {
    I18N_NOOP("Name"),
    I18N_NOOP("M"),
    I18N_NOOP("Fuzzy"),
    I18N_NOOP("Untranslated"),
    I18N_NOOP("Total"),
    I18N_NOOP("CVS/SVN Status"),
    I18N_NOOP("Last Revision"),
    I18N_NOOP("Last Translator")
};

#define COLTEXT(a)  (i18n(columnNames[a]))

CatalogManagerView::CatalogManagerView(KBabel::Project::Ptr project, QWidget* parent,const char* name)
              : QListView(parent,name)
	      , _dirWatch(0)
	      , _readInfoCount(0)
	      , _active(false)
	      , _stop(false)
	      , _stopSearch(false)
	      , _updateNesting(0)
	      , _logView(0)
	      , _logWindow(0)
              , m_validPOCVSRepository( false )
              , m_validPOTCVSRepository( false )
              , m_validPOSVNRepository( false )
              , m_validPOTSVNRepository( false )
	      , markPatternDialog(0)
	      , _validateDialog(0)
	      , _validateOptions(0)
	      , _validateOptionsDlg(0)
	      , _markAsFuzzy(false)
	      , _ignoreFuzzy(false)
	      , _project(project)
{
   _dirList.resize(200);
   _fileList.resize(500);
   _readInfoFileList.clear();

   _pendingProcesses.setAutoDelete(true);

   setSelectionMode(Single);

   _dictBox = new KBabelDictBox(this, "dictbox");
   _dictBox->hide();

   _updateTimer = new QTimer(this);
   connect(_updateTimer,SIGNAL(timeout()),this,SLOT(checkUpdate()));

   addColumn(COLTEXT(COL_NAME));
   addColumn(COLTEXT(COL_MARKER),25);
   setColumnAlignment(1,AlignCenter);
   addColumn(COLTEXT(COL_FUZZY));
   setColumnAlignment(1,AlignCenter);
   addColumn(COLTEXT(COL_UNTRANS));
   setColumnAlignment(2,AlignCenter);
   addColumn(COLTEXT(COL_TOTAL));
   setColumnAlignment(3,AlignCenter);
   addColumn(QString::null); // CVS/SVN column, header is set later
   addColumn(COLTEXT(COL_REVISION));
   addColumn(COLTEXT(COL_TRANSLATOR));


   header()->setMovingEnabled(false);
   setAllColumnsShowFocus(true);
   setSorting(0);

   if(KContextMenuManager::showOnButtonPress())
   {
      connect(this,SIGNAL(rightButtonPressed(QListViewItem*,const QPoint &, int))
           ,this, SLOT(showContentsMenu(QListViewItem*,const QPoint &, int)));
   }
   else
   {
      connect(this,SIGNAL(rightButtonClicked(QListViewItem*,const QPoint &, int))
           ,this, SLOT(showContentsMenu(QListViewItem*,const QPoint &, int)));
   }
   connect(this, SIGNAL(returnPressed(QListViewItem*))
           ,this, SLOT(activateItem(QListViewItem*)));
   connect(this, SIGNAL(doubleClicked(QListViewItem*))
           ,this, SLOT(activateItem(QListViewItem*)));
   connect(this,SIGNAL(selectionChanged()),this,SLOT(checkSelected()));
   connect( this, SIGNAL( clicked(QListViewItem *, const QPoint &, int)),
    	   this, SLOT( columnClicked(QListViewItem *, const QPoint &, int)));

   _dirCommandsMenu = 0;
   _fileCommandsMenu = 0;
   _dirContentsMenu = 0;
   _fileContentsMenu = 0;

   _logWindow = new KDialogBase(0,"log window",false,i18n("Log Window")
                      ,KDialogBase::Close | KDialogBase::User1,KDialogBase::Close);
   _logWindow->setButtonText(KDialogBase::User1,i18n("C&lear"));
   _logWindow->setInitialSize(QSize(300,200));

   QWhatsThis::add(_logWindow,i18n("<qt><p><b>Log window</b></p>\n"
				   "<p>In this window the output of "
				   "the executed commands are shown.</p></qt>"));

   _logView = new QTextEdit(_logWindow);
   _logView->setReadOnly(true);

   _logWindow->setMainWidget(_logView);

   connect(_logWindow,SIGNAL(user1Clicked()),_logView,SLOT(clear()));

   QWhatsThis::add(this,i18n("<qt><p><b>Catalog Manager</b></p>\n"
"<p>The Catalog Manager merges two folders into one tree and displays all\n"
"PO and POT files in these folders. This way you can easily see if a\n"
"template has been added or removed. Also some information about the files\n"
"is displayed.</p>"
"<p>For more information see section <b>The Catalog Manager</b>"
" in the online help.</p></qt>"));

   setAcceptDrops(true); // just to get the drag displayed immediately

   mailer = new KBabelMailer( this, _project );

   // CVS
   cvshandler = new CVSHandler( );
   connect( cvshandler, SIGNAL( signalIsPORepository( bool ) ),
            this, SLOT( slotValidPOCVSRepository( bool ) ) );
   connect( cvshandler, SIGNAL( signalIsPOTRepository( bool ) ),
            this, SLOT( slotValidPOTCVSRepository( bool ) ) );
   connect( cvshandler, SIGNAL( signalFilesCommitted( const QStringList& ) ),
            this, SLOT( updateFiles( const QStringList& ) ) );

   // SVN
   svnhandler = new SVNHandler( );
   connect( svnhandler, SIGNAL( signalIsPORepository( bool ) ),
            this, SLOT( slotValidPOSVNRepository( bool ) ) );
   connect( svnhandler, SIGNAL( signalIsPOTRepository( bool ) ),
            this, SLOT( slotValidPOTSVNRepository( bool ) ) );
   connect( svnhandler, SIGNAL( signalFilesCommitted( const QStringList& ) ),
            this, SLOT( updateFiles( const QStringList& ) ) );

   KConfig *config = KGlobal::config();
   restoreView(config);

   _dictBox->readSettings(_project->config());
}

CatalogManagerView::~CatalogManagerView()
{
  if(_active)
     stop();

  if(_dirWatch)
      delete _dirWatch;

  if(_settings.killCmdOnExit)
  {
     KProcess* proc;
     for ( proc=_pendingProcesses.first(); proc != 0; proc=_pendingProcesses.next() )
     {
        proc->kill(SIGKILL);
     }
  }

  delete _logWindow;
  delete mailer;
  delete cvshandler;
  delete svnhandler;
  if (markPatternDialog) delete markPatternDialog;
}


void CatalogManagerView::saveView( KConfig *config) const
{
    saveMarker(config);

    KConfigGroupSaver( config, "CatalogManager" );

    config->writeEntry( "ValidateMarkAsFuzzy", _markAsFuzzy );
    config->writeEntry( "ValidateIgnoreFuzzy", _ignoreFuzzy );
}


void CatalogManagerView::restoreView( KConfig *config)
{
    readMarker(config);

    _markAsFuzzy = config->readBoolEntry( "ValidateMarkAsFuzzy", false );
    _ignoreFuzzy = config->readBoolEntry( "ValidateIgnoreFuzzy", false );
}

void CatalogManagerView::setRMBMenuFile( QPopupMenu *m )
{
    _fileContentsMenu = m;
}

void CatalogManagerView::setRMBMenuDir( QPopupMenu *m )
{
    _dirContentsMenu = m;
}

void CatalogManagerView::setDirCommandsMenu( QPopupMenu *m )
{
    _dirCommandsMenu = m;
    connect(_dirCommandsMenu,SIGNAL(activated(int)),this,SLOT(slotDirCommand(int)));
}

void CatalogManagerView::setFileCommandsMenu( QPopupMenu *m )
{
    _fileCommandsMenu = m;
    connect(_fileCommandsMenu,SIGNAL(activated(int)),this,SLOT(slotFileCommand(int)));
}

void CatalogManagerView::checkUpdate()
{
   _updateNesting++;
   pause(true);

   QDictIterator<CatManListItem> it( _fileList ); // iterator for dict

   while ( it.current() && !_stop)
   {
      CatManListItem* item=it.current();

      item->checkUpdate();
      ++it;
   }

   pause(false);
   --_updateNesting;
   if( _updateNesting == 0 )
   {
	emit updateFinished();
   }
}



void CatalogManagerView::pause(bool flag)
{
   if(flag)
   {
      _updateTimer->stop();
   }
   else
   {
      _updateTimer->start(10000);
   }
}


void CatalogManagerView::stop(bool s)
{
   kdDebug(KBABEL_CATMAN) << "Stopping " << s << endl;
   pause(s);
   _stop=s;
   PoInfo::stopStaticRead = true;
}

void CatalogManagerView::stopSearch()
{
    _stopSearch = true;
}

void CatalogManagerView::clear()
{
   pause(true);

   // first clear up
   if(_dirWatch)
      delete _dirWatch;

   _dirWatch= new KDirWatch();
   connect(_dirWatch,SIGNAL(deleted(const QString&)),this
				   ,SLOT(directoryDeleted(const QString&)));
   connect(_dirWatch,SIGNAL(dirty(const QString&)),this
				   ,SLOT(directoryChanged(const QString&)));
   connect(_dirWatch,SIGNAL(created(const QString&)),this
				   ,SLOT(directoryChanged(const QString&)));

   _dirList.clear();
   _fileList.clear();

   QListView::clear();
}

void CatalogManagerView::toggleAllMarks()
{
   _markerList.clear();

   QListViewItemIterator it( this );
   CatManListItem* item;

   for ( ; it.current(); ++it )
   {
      item = (CatManListItem*) it.current();
      if(item->isFile())
      {
         bool wasMarked=item->marked();
         item->setMarked(!wasMarked);
         if(!wasMarked)
         {
            _markerList.append(item->package());
         }
         else
         {
            _markerList.remove(item->package());
         }
      }
   }

   checkSelected();
}

void CatalogManagerView::clearAllMarks()
{
   _markerList.clear();
   QDictIterator<CatManListItem> it( _fileList ); // iterator for dict

   while ( it.current() )
   {
      CatManListItem* item=it.current();

      if(item->marked())
         _markerList.remove(item->package());

      item->setMarked(false);
      ++it;
   }

   checkSelected();
}

void CatalogManagerView::markModifiedFiles()
{
    QDictIterator<CatManListItem> it( _fileList );
    while ( it.current() )
   {
      CatManListItem* item=it.current();
      /*if(item->marked())
         _markerList.remove(item->package());
      */
      if(item->isModified() && ! item->marked() ) {
          item->setMarked(true);
	  _markerList.append(item->package( ));
      }
      ++it;
   }

   checkSelected();
}

void CatalogManagerView::loadMarks()
{
    const KURL url = KFileDialog::getOpenURL( QString(),"*.marklist", this );
    if( url.isEmpty() ) return;

    QString filename;
#if KDE_IS_VERSION( 3, 2, 90 )
    if (!KIO::NetAccess::download( url, filename, this ) )
#else
    if( !KIO::NetAccess::download( url, filename ) )
#endif
    {
	KMessageBox::error(this,i18n(
	    "Error while trying to open file:\n %1").arg(url.prettyURL()));
	return;
    }

    // now load from file
    QStringList newMarkerList; // better create new list in case of problems
    QFile f( filename );
    if( f.open( IO_ReadOnly) )
    {
	QTextStream s(&f);

	QString input;

	s >> input ;
	if( input == "[Markers]" )
	{
	    while( !s.atEnd() )
	    {
		s >> input;
		newMarkerList.append(input);
	    }
	}
	else
	{
    	    KMessageBox::error(this
		,i18n("Error while trying to read file:\n %1\n"
		"Maybe it is not a valid file with list of markings.").arg(url.prettyURL()));
	    f.close();
	    return;
	}
	f.close();
    }
    else
    {
	KMessageBox::error(this,i18n(
	    "Error while trying to open file:\n %1").arg(url.prettyURL()));
    }

    KIO::NetAccess::removeTempFile( filename );

    // test validity of list items
    QStringList testedList;
    QStringList::const_iterator it;
    for( it=newMarkerList.constBegin() ; it!=newMarkerList.constEnd() ; ++it )
	if( _fileList[(*it)] != 0 ) testedList.append( (*it) );

    // apply new list
    for( it=_markerList.constBegin() ; it!=_markerList.constEnd() ; ++it )
    {
	CatManListItem* item = _fileList[(*it)];
	if( item ) item->setMarked(false);
    }

    _markerList = testedList;
    for( it=_markerList.constBegin() ; it!=_markerList.constEnd() ; ++it )
    {
	CatManListItem* item = _fileList[(*it)];
	if( item ) item->setMarked(true);
    }

   checkSelected();
}

void CatalogManagerView::saveMarks()
{
    const KURL url2 = KFileDialog::getSaveURL( QString(), "*.marklist", this );
    if( url2.isEmpty() ) return;

    // ### FIXME: why is the file dialog not doing this?
    if ( KIO::NetAccess::exists( url2, false, this ) )
    {
        if(KMessageBox::warningContinueCancel(this,QString("<qt>%1</qt>").arg(i18n("The file %1 already exists. "
	            "Do you want to overwrite it?").arg(url2.prettyURL())),i18n("Warning"),i18n("&Overwrite"))==KMessageBox::Cancel)
	{
	    return;
	}
    }

#if KDE_IS_VERSION( 3, 4, 92 )
    // Support for partially remote KIO slave like media:
    const KURL url ( KIO::NetAccess::mostLocalURL( url2, this ) );
#else
    const KURL url ( url2 );
#endif
    kdDebug() << "Saving marks: " << url2.prettyURL() << " most-local: " << url.prettyURL() << endl;
    
    QFile* file = 0;
    KTempFile* tempFile = 0;
    QTextStream* stream = 0;
    bool error = false;

    const bool localFile = url.isLocalFile();
    if ( localFile )
    {
        // We have a local file
        file = new QFile( url.path() );
        if ( file->open (IO_WriteOnly) )
        {
            stream = new QTextStream( file );
        }
        else
        {
            error = true;
        }
    }
    else
    {
        tempFile = new KTempFile();
        tempFile->setAutoDelete(true);
        stream = tempFile->textStream();
        error = !stream;
    }
    if ( !error )
    {
        // ### TODO: try to get a better file format for KDE4 (XML?), one working with real relative paths (no / at start) and working with UTF-8
        *stream << "[Markers]" << endl;
        for( QStringList::const_iterator it = _markerList.constBegin(); it!=_markerList.constEnd() ; ++it )
            *stream << (*it) << endl;
    }
    if ( error )
    {
        // ### KDE4 FIXME: strip the final \n of the message
        KMessageBox::error( this,
            i18n( "An error occurred while trying to write to file:\n%1\n" ).arg( url.prettyURL()) );
    }
    else if ( !localFile )
    {
        tempFile->close();
        if( !KIO::NetAccess::upload( tempFile->name(), url, this ) )
        {
            // ### KDE4 FIXME: strip the final \n of the message
            KMessageBox::error(this,
                i18n("An error occurred while trying to upload the file:\n%1\n").arg(url.prettyURL()));
        }
    }

    // We have finished so clean up
    if ( localFile )
    {
        delete stream;
        file->close();
        delete file;
    }
    else
    {
        delete tempFile;
    }
   
    checkSelected();
}

void CatalogManagerView::slotMarkPattern( )
{
  setPatternMarks(true);
}

void CatalogManagerView::slotUnmarkPattern( )
{
  setPatternMarks(false);
}

void CatalogManagerView::setPatternMarks(bool mark)
{
  CatManListItem * item = (CatManListItem*)currentItem( );
  if (!item)
    item = (CatManListItem*)_dirList["/"];
  if (!item->isDir( ))
    return;

  if (!markPatternDialog)
    markPatternDialog = new MarkPatternDialog(this);

  markPatternDialog->setMode(mark);

  if (markPatternDialog->exec( ) != KDialog::Accepted)
    return;

  QRegExp rx(markPatternDialog->pattern( ));
  rx.setWildcard(!markPatternDialog->useRegExp( ));
  rx.setCaseSensitive(markPatternDialog->isCaseSensitive( ));

  QStringList fileList = item->allChildrenList(true);
  for (QStringList::const_iterator it = fileList.constBegin( ); it != fileList.constEnd( ); ++it) {
    CatManListItem * i = _fileList[*it];

    QString matchName;
    if (i->hasPo( ))
      matchName = i->poFile( );
    else if (i->hasPot( ) && markPatternDialog->includeTemplates( ))
      matchName = i->potFile( );
    matchName = QFileInfo(matchName).baseName( );

    if (mark) {
      if (!matchName.isEmpty( ) && rx.exactMatch(matchName) && !i->marked( )) {
        i->setMarked(true);
        _markerList.append(i->package( ));
      }
    } else {
      if (!matchName.isEmpty( ) && rx.exactMatch(matchName) && i->marked( )) {
        i->setMarked(false);
        _markerList.remove(i->package( ));
      }
    }
  }
}

void CatalogManagerView::statistics()
{
   CatManListItem* i=(CatManListItem*) currentItem();

   if(!i)
     i=(CatManListItem*)_dirList["/"];

   if(isActive() && i->isDir())
   {
      if(KMessageBox::warningContinueCancel(this
         ,i18n("The Catalog Manager is still updating information about the files.\n"
"If you continue, it will try to update all necessary files, however this can take "
"a long time and may lead to wrong results. Please wait until all files are updated."),i18n("Warning")
         ,KStdGuiItem::cont()) == KMessageBox::Cancel)
      {
        return;
      }
   }

   QStringList doList;

   if( i->isFile() ) doList.append(i->package());
   else doList = i->allChildrenList(true);

   showStatistics( i, doList );
}

void CatalogManagerView::markedStatistics()
{
   CatManListItem* i=(CatManListItem*) currentItem();

   if(!i)
     i=(CatManListItem*)_dirList["/"];

   if(isActive() && i->isDir())
   {
      if(KMessageBox::warningContinueCancel(this
         ,i18n("The Catalog Manager is still updating information about the files.\n"
"If you continue, it will try to update all necessary files, however this can take "
"a long time and may lead to wrong results. Please wait until all files are updated."),i18n("Warning")
         ,KStdGuiItem::cont()) == KMessageBox::Cancel)
      {
        return;
      }
   }

   QStringList doList;

   if( i->isFile() ) doList.append(i->package());
   else doList = i->allChildrenList(true);

   QStringList markedDoList;
   QStringList::const_iterator it;
   for( it = doList.constBegin(); it != doList.constEnd(); ++it )
   {
         CatManListItem* item = _fileList[(*it)];
	 if( item->marked() ) markedDoList.append(item->package());
   }

   showStatistics( i, markedDoList );
}

void CatalogManagerView::showStatistics( CatManListItem  *i, QStringList &childrenList )
{
   KLocale *locale = KGlobal::locale();

      QString msg;
      int totalPackages=0;
      int totalPo=0;
      int totalNoPot=0;
      int needworkPo=0;
      int totalMsgid=0;
      int totalFuzzy=0;
      int totalUntranslated=0;

      QStringList::const_iterator it;
      for( it = childrenList.constBegin(); it != childrenList.constEnd(); ++it )
      {
         CatManListItem* item = _fileList[(*it)];

         /*
         KASSERT1(item,KDEBUG_FATAL,KBABEL_CATMAN,"CatalogManagerView::statistics: item not in list %s"
                  ,(*it).ascii());
           */
         // be sure, that the information is updated
         _updateNesting++;
         item->checkUpdate();
         _updateNesting--;
	 if( _stop ) return;

         totalPackages++;

         int fuzzy=item->fuzzy();
         int total=item->total();
         int untrans=item->untranslated();

         if(item->hasPo())
            totalPo++;

         if(!item->hasPot())
            totalNoPot++;


         if(fuzzy || untrans)
            needworkPo++;

         totalMsgid+=total;
         totalFuzzy+=fuzzy;
         totalUntranslated+=untrans;
      }

      double percent;

      const QString name=i->package(false);
      if(name.isEmpty())
          msg = i18n("Statistics for all:\n");
      else
          msg = i18n("Statistics for %1:\n").arg(name);

      msg+=i18n("Number of packages: %1\n").arg(locale->formatNumber(totalPackages, 0));

      percent=100.0-((double)needworkPo*100.0)/totalPackages;
      msg+=i18n("Complete translated: %1 % (%2)\n").arg(locale->formatNumber(percent,2)).arg(locale->formatNumber(totalPackages-needworkPo, 0));

      percent=100.0-((double)totalPo*100.0)/totalPackages;
      msg+=i18n("Only template available: %1 % (%2)\n").arg(locale->formatNumber(percent,2)).arg(locale->formatNumber(totalPackages-totalPo,0));
      percent=((double)totalNoPot*100.0)/totalPackages;
      msg+=i18n("Only PO file available: %1 % (%2)\n").arg(locale->formatNumber(percent,02)).arg(locale->formatNumber(totalNoPot, 0));

      msg+=i18n("Number of messages: %1\n").arg(locale->formatNumber(totalMsgid, 0));

      long int totalTranslated = totalMsgid - totalFuzzy - totalUntranslated;
      percent=((double)totalTranslated*100.0)/totalMsgid;
      msg+=i18n("Translated: %1 % (%2)\n").arg(locale->formatNumber(percent,2)).arg(locale->formatNumber(totalTranslated, 0));

      percent=((double)totalFuzzy*100.0)/totalMsgid;
      msg+=i18n("Fuzzy: %1 % (%2)\n").arg(locale->formatNumber(percent,2)).arg(locale->formatNumber(totalFuzzy, 0));

      percent=((double)totalUntranslated*100.0)/totalMsgid;
      msg+=i18n("Untranslated: %1 % (%2)\n").arg(locale->formatNumber(percent,2)).arg(locale->formatNumber(totalUntranslated, 0));

   KMessageBox::information(this,msg,i18n("Statistics"));
}

void CatalogManagerView::checkSyntax()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(!item)
      item=(CatManListItem*) _dirList["/"];

   if(item->isFile())
   {
      if(!item->hasPo())
         return;

      Msgfmt::Status status;
      QString output;
      Msgfmt msgfmt;

      status=msgfmt.checkSyntax(item->poFile(),output);

      switch(status)
      {
         case Msgfmt::Ok:
         {
            KMessageBox::information(this,i18n("The file is syntactically correct.\nOutput of \"msgfmt --statistics\":")+"\n"+output);
            break;
         }
         case Msgfmt::SyntaxError:
         {
            KMessageBox::information(this,i18n("The file has syntax errors.\nOutput of \"msgfmt --statistics\":")+"\n"+output);
            break;
         }
         case Msgfmt::HeaderError:
         {
            KMessageBox::information(this,i18n("The file has header syntax error.\nOutput of \"msgfmt --statistics\":")+"\n"+output);
            break;
         }
         case Msgfmt::Error:
         {
            KMessageBox::error(this,i18n("An error occurred while processing \"msgfmt --statistics\""));
            break;
         }
         case Msgfmt::NoExecutable:
         {
            KMessageBox::sorry(this,i18n("Cannot execute msgfmt. Please make sure that you have msgfmt in your PATH."));
            break;
         }
	 case Msgfmt::Unsupported:
	 {
    	    KMessageBox::sorry(this,i18n("You can use gettext tools only for checking PO files."));
	    break;
	 }
      }

   }
   else
   {
      Msgfmt::Status status;
      QString output;
      Msgfmt msgfmt;

      status=msgfmt.checkSyntaxInDir(item->poFile(), "*.po", output);

      QString name=item->package(false);

      switch(status)
      {
         case Msgfmt::Ok:
         {
            QString msg;
            if(!name.isEmpty())
            {
               msg=i18n("All files in folder %1 are syntactically correct.\n"
"Output of \"msgfmt --statistics\":\n").arg(name)+output;
            }
            else
            {
               msg=i18n("All files in the base folder are syntactically correct.\n"
"Output of \"msgfmt --statistics\":\n")+output;
            }
            KMessageBox::information(this,msg);
            break;
         }
         case Msgfmt::SyntaxError:
         {
            QString msg;
            if(!name.isEmpty())
            {
               msg=i18n("At least one file in folder %1 has syntax errors.\n"
"Output of \"msgfmt --statistics\":\n").arg(name)+output;
            }
            else
            {
               msg=i18n("At least one file in the base folder has syntax errors.\n"
"Output of \"msgfmt --statistics\":\n")+output;
            }
            KMessageBox::information(this,msg);
            break;
         }
         case Msgfmt::HeaderError:
         {
            QString msg;
            if(!name.isEmpty())
            {
               msg=i18n("At least one file in folder %1 has header syntax errors.\n"
"Output of \"msgfmt --statistics\":\n").arg(name)+output;
            }
            else
            {
               msg=i18n("At least one file in the base folder has header syntax errors.\n"
"Output of \"msgfmt --statistics\":\n")+output;
            }
            KMessageBox::information(this,msg);
            break;
         }
         case Msgfmt::Error:
         {
            QString msg;
            if(!name.isEmpty())
            {
              msg=i18n("An error occurred while processing \"msgfmt --statistics *.po\" "
"in folder %1").arg(name);
            }
            else
            {
              msg=i18n("An error occurred while processing \"msgfmt --statistics *.po\" "
"in the base folder");
            }
            KMessageBox::error(this,msg);
            break;
         }
         case Msgfmt::NoExecutable:
         {
            KMessageBox::sorry(this,i18n("Cannot execute msgfmt. Please make sure that you have msgfmt in your PATH."));
            break;
         }
	 case Msgfmt::Unsupported:
	 {
    	    KMessageBox::sorry(this,i18n("You can use gettext tools only for checking PO files."));
	    break;
	 }
      }
   }
}

void CatalogManagerView::roughTranslation()
{
    QPtrList<CatManListItem> result;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    if( current->isDir() )
    {
	QStringList s = current->allChildrenList(true);
	QStringList::const_iterator it;
	for( it = s.constBegin() ; it != s.constEnd(); ++it )
	{
	    CatManListItem *item = _fileList[(*it)];
	    if( item ) result.append( item );
	}
    }
    else
    {
	result.append( current );
    }

    MultiRoughTransDlg* dialog=new MultiRoughTransDlg( _dictBox, result, this );
    dialog->exec();
    delete dialog;
}

void CatalogManagerView::markedRoughTranslation()
{
    if( _markerList.count() == 0 ) return;

    QPtrList<CatManListItem> result;

    QStringList::const_iterator it;
    for( it = _markerList.constBegin() ; it != _markerList.constEnd(); ++it )
    {
	CatManListItem *item = _fileList[(*it)];
	result.append( item );
    }

    MultiRoughTransDlg* dialog=new MultiRoughTransDlg( _dictBox, result, this );
    dialog->exec();
    delete dialog;
}

void CatalogManagerView::mailFiles()
{
  CatManListItem* item = (CatManListItem*)currentItem();
  if(item->isDir()) {
    QStringList filesToSend;
    QStringList childrenList = item->allChildrenList(true);

    QStringList::const_iterator it;
    for (it = childrenList.constBegin(); it != childrenList.constEnd(); ++it) {
      CatManListItem* i = _fileList[(*it)];
      if (i->hasPo()) {
	filesToSend << i->poFile();
      }
    }
    mailer->sendFiles(filesToSend, item->text(0));
  } else {
    if (item->hasPo()) {
      mailer->sendOneFile(item->poFile());
    }
  }
}

void CatalogManagerView::mailMarkedFiles()
{
  if (_markerList.count() == 0)
    return;

  QStringList filesToSend;
  QStringList::const_iterator it;
  for (it = _markerList.constBegin(); it != _markerList.constEnd(); ++it) {
    CatManListItem* i = _fileList[(*it)];
    if (i->hasPo()) {
      filesToSend << i->poFile();
    }
  }
  mailer->sendFiles(filesToSend);
}

void CatalogManagerView::packageFiles( )
{
    CatManListItem* item = (CatManListItem*)currentItem();
    if(item->isDir()) {
        QStringList filesToPackage;
        QStringList childrenList = item->allChildrenList(true);

        QStringList::const_iterator it;
        for (it = childrenList.constBegin(); it != childrenList.constEnd(); ++it) {
          CatManListItem* i = _fileList[(*it)];
          if (i->hasPo()) {
	    filesToPackage << i->poFile();
          }
        }
	QString packageFileName = KFileDialog::getSaveFileName(QString::null,"*.tar.bz2\n*.tar.gz",this);
	mailer->buildArchive( filesToPackage, packageFileName, QString::null, false );
    }
    else {
        if (item->hasPo()) {
          QStringList fileToPackage(item->poFile());
	  QString packageFileName = KFileDialog::getSaveFileName(QString::null,"*.tar.bz2\n*.tar.gz",this);
	  mailer->buildArchive( fileToPackage, packageFileName, QString::null, false );
        }
    }
}
void CatalogManagerView::packageMarkedFiles( )
{
    if (_markerList.count() == 0)
      return;

    QStringList filesToPackage;
    QStringList::const_iterator it;
    for (it = _markerList.constBegin(); it != _markerList.constEnd(); ++it) {
      CatManListItem* i = _fileList[(*it)];
      if (i->hasPo()) {
        filesToPackage << i->poFile();
      }
    }

    QString packageFileName = KFileDialog::getSaveFileName(QString::null,"*.tar.bz2\n*.tar.gz",this);
    mailer->buildArchive( filesToPackage, packageFileName, QString::null, false );
}

// CVS
void CatalogManagerView::cvsUpdate( )
{
  doCVSCommand( CVS::Update );
}

void CatalogManagerView::cvsUpdateMarked( )
{
  doCVSCommand( CVS::Update, true );
}

void CatalogManagerView::cvsCommit( )
{
  doCVSCommand( CVS::Commit );
}

void CatalogManagerView::cvsCommitMarked( )
{
  doCVSCommand( CVS::Commit, true );
}

void CatalogManagerView::cvsStatus( )
{
  doCVSCommand( CVS::Status );
}

void CatalogManagerView::cvsStatusMarked( )
{
  doCVSCommand( CVS::Status, true );
}

void CatalogManagerView::cvsUpdateTemplate( )
{
  doCVSCommand( CVS::Update, false, true );
}

void CatalogManagerView::cvsUpdateMarkedTemplate( )
{
  doCVSCommand( CVS::Update, true, true );
}

void CatalogManagerView::cvsCommitTemplate( )
{
  doCVSCommand( CVS::Commit, false, true );
}

void CatalogManagerView::cvsCommitMarkedTemplate( )
{
  doCVSCommand( CVS::Commit, true, true );
}

void CatalogManagerView::cvsDiff( )
{
  doCVSCommand( CVS::Diff, false, false );
}

void CatalogManagerView::doCVSCommand( CVS::Command cmd, bool marked, bool templates )
{
  KSharedConfig* config = _project->sharedConfig();
  if ( marked ) {
    if ( _markerList.isEmpty() ) return;
    QStringList fileList;
    QStringList::const_iterator it;
    for ( it = _markerList.constBegin( ); it != _markerList.constEnd( ); ++it ) {
      CatManListItem * i = _fileList[(*it)];
      if ( templates && i->hasPot( ) )
        fileList << i->potFile( );
      else if ( !templates && i->hasPo( ) )
        fileList << i->poFile( );
    }
    cvshandler->execCVSCommand( this, cmd, fileList, templates, config );
  } else {
    const QString basedir = ( templates ? _settings.potBaseDir : _settings.poBaseDir );
    QString cvsItem;
    CatManListItem * item = (CatManListItem*)currentItem( );
    if ( ( cmd == CVS::Commit || cmd == CVS::Diff ) && item->isDir( ) ) {
      // all children including directories
      QStringList cvsItems = item->allChildrenFileList (true, false, true);
      if ( !cvsItems.isEmpty( ) )
        cvshandler->execCVSCommand( this, cmd, cvsItems, templates, config );
    } else {
      if ( templates && item->hasPot( ) )
        cvsItem = item->potFile( );
      else if ( !templates && item->hasPo( ) )
        cvsItem = item->poFile( );

      if ( !cvsItem.isEmpty( ) )
        cvshandler->execCVSCommand( this, cmd, cvsItem, templates, config );
    }
  }
}

//SVN
void CatalogManagerView::svnUpdate( )
{
    doSVNCommand( SVN::Update );
}

void CatalogManagerView::svnUpdateMarked( )
{
    doSVNCommand( SVN::Update, true );
}

void CatalogManagerView::svnCommit( )
{
    doSVNCommand( SVN::Commit );
}

void CatalogManagerView::svnCommitMarked( )
{
    doSVNCommand( SVN::Commit, true );
}

void CatalogManagerView::svnStatusRemote( )
{
    doSVNCommand( SVN::StatusRemote );
}

void CatalogManagerView::svnStatusRemoteMarked( )
{
    doSVNCommand( SVN::StatusRemote, true );
}

void CatalogManagerView::svnStatusLocal( )
{
    doSVNCommand( SVN::StatusLocal );
}

void CatalogManagerView::svnStatusLocalMarked( )
{
    doSVNCommand( SVN::StatusLocal, true );
}

void CatalogManagerView::svnInfo()
{
    doSVNCommand( SVN::Info );
}

void CatalogManagerView::svnInfoMarked()
{
    doSVNCommand( SVN::Info, true );
}

void CatalogManagerView::svnUpdateTemplate( )
{
    doSVNCommand( SVN::Update, false, true );
}

void CatalogManagerView::svnUpdateMarkedTemplate( )
{
    doSVNCommand( SVN::Update, true, true );
}

void CatalogManagerView::svnCommitTemplate( )
{
    doSVNCommand( SVN::Commit, false, true );
}

void CatalogManagerView::svnCommitMarkedTemplate( )
{
    doSVNCommand( SVN::Commit, true, true );
}

void CatalogManagerView::svnDiff( )
{
    doSVNCommand( SVN::Diff, false, false );
}

void CatalogManagerView::doSVNCommand( SVN::Command cmd, bool marked, bool templates )
{
    KSharedConfig* config = _project->sharedConfig();
    if ( marked ) {
        if ( _markerList.isEmpty() ) return;
        QStringList fileList;
        QStringList::const_iterator it;
        for ( it = _markerList.constBegin( ); it != _markerList.constEnd( ); ++it ) {
            CatManListItem * i = _fileList[(*it)];
            if ( templates && i->hasPot( ) )
                fileList << i->potFile( );
            else if ( !templates && i->hasPo( ) )
                fileList << i->poFile( );
        }
        svnhandler->execSVNCommand( this, cmd, fileList, templates, config );
    } else {
        const QString basedir = ( templates ? _settings.potBaseDir : _settings.poBaseDir );
        QString svnItem;
        CatManListItem * item = (CatManListItem*)currentItem( );
        if ( ( cmd == SVN::Commit || cmd == SVN::Diff ) && item->isDir( ) ) {
      // all children including directories
            QStringList svnItems = item->allChildrenFileList (true, false, true);
            if ( !svnItems.isEmpty( ) )
                svnhandler->execSVNCommand( this, cmd, svnItems, templates, config );
        } else {
            if ( templates && item->hasPot( ) )
                svnItem = item->potFile( );
            else if ( !templates && item->hasPo( ) )
                svnItem = item->poFile( );

            if ( !svnItem.isEmpty( ) )
                svnhandler->execSVNCommand( this, cmd, svnItem, templates, config );
    }
  }
}

void CatalogManagerView::showLog()
{
   _logWindow->show();
}

QString CatalogManagerView::find( FindOptions &options, QStringList &rest )
{
   CatManListItem* i=(CatManListItem*) currentItem();

   if(!i || options.inAllFiles)
     i=(CatManListItem*)_dirList["/"];

   QValueList<QString> foundFiles;
   _stopSearch = false;

   const QString search = options.findStr.lower().simplifyWhiteSpace();
   QStringList searchWords = QStringList::split(' ', search);

   QStringList childrenList;
   if( i->isFile() ) childrenList.append(i->name());
   else childrenList =i->allChildrenList(true);

   emit prepareFindProgressBar(childrenList.size());

   QStringList::const_iterator it;
   for( it = childrenList.constBegin(); it != childrenList.constEnd(); ++it )
   {
	 CatManListItem* item = _fileList[(*it)];

	 if( !item )
	 {
	    kdWarning(KBABEL_CATMAN) << "The file information not found, skipping" << endl;
	    continue;
	 }

	 // skip if not marked and we lookup in marked
	 if( options.inMarkedFiles && !item->marked() )
	 {
	    kdDebug(KBABEL_CATMAN) << "Skipping due marking " << item->name() << endl;
    	    emit signalSearchedFile(1);
	    continue;
	 }

	 bool doSearch = options.isRegExp || options.inTemplates; // for regexp and templates we do not support index search
	 if( item->wordsUpdated() )
	    doSearch = doSearch || hasMatchingWords(item->wordList(), searchWords);
	 else doSearch = true; // we do not have index, we need to search
	 if( doSearch )
	 {
	    QString itemFile;
	    if( options.inTemplates )
	    {
		if( item->hasPot() ) itemFile=item->potFile();
	    } else {
		if( item->hasPo() ) itemFile=item->poFile();
	    }

	    if( itemFile.isNull() )
	    {
		emit signalSearchedFile(1);
		continue;
	    }

    	    if( PoInfo::findInFile( itemFile , options ) )
	    {
    		emit signalSearchedFile(1);
		rest.clear();
		if( _stopSearch )
		{
		    // if we are stopped, return what we found and clear the rest
		    _stopSearch = false;
		    return itemFile;
		}
		const QString foundItemFile = itemFile;

		it++;
		while( it != childrenList.constEnd() )
		{
        	    CatManListItem *item = _fileList[(*it)];

		    itemFile = QString::null;
		    if( options.inTemplates )
		    {
			if( item->hasPot() ) itemFile=item->potFile();
		    } else {
			if( item->hasPo() )itemFile=item->poFile();
		    }
		    if( options.inMarkedFiles && !item->marked() )
			itemFile=QString::null;

		    if( !itemFile.isNull())
		    {
			if( item->wordsUpdated() &&  !options.inTemplates )
			{
			    if( options.isRegExp || hasMatchingWords(item->wordList(), searchWords) )
			    {
				rest.append( itemFile );
			    }
			    else kdDebug(KBABEL_CATMAN) << "Don't try to lookup in " << itemFile << endl;
			} else {
			    rest.append( itemFile ); // there is no word index, add the file
			}
		    } else emit signalSearchedFile(1);

		    it++;
		}
    		return foundItemFile;

	    }
	} else kdDebug(KBABEL_CATMAN) << "Skipping " << item->poFile() << endl;
	emit signalSearchedFile(1);
	if( _stop || _stopSearch ) {
	    _stopSearch = false;
	    rest.clear();
	    if( _updateNesting == 0 && !_stop ) emit updateFinished();
	    return QString::null;
	}
   }
   return QString::null;
}

bool CatalogManagerView::hasMatchingWords( QStringList &itemWords, QStringList &searchWords)
{
    for( QStringList::const_iterator it1 = searchWords.constBegin() ; it1 != searchWords.constEnd() ; ++it1 )
	for( QStringList::const_iterator it2 = itemWords.constBegin() ; it2 != itemWords.constEnd() ; ++it2 )
	    if( *it1 == *it2
		|| (*it1).contains(*it2)
		|| (*it2).contains(*it1) ) return true;
    return false;
}

void CatalogManagerView::showContentsMenu(QListViewItem *i, const QPoint &point, int)
{
   CatManListItem* item = (CatManListItem*) i;

   if(!item)
      return;

  if(item->isDir())
   {
      _dirContentsMenu->exec(point);
   }
   else
   {
      _fileContentsMenu->exec(point);
   }
}

void CatalogManagerView::checkSelected()
{
   CatManListItem* item=(CatManListItem*)selectedItem();
   if(!item) return;

   const uint actionValue =
      NEEDS_PO * item->hasPo() + NEEDS_POT * item->hasPot() +
      NEEDS_MARK * item->marked() + NEEDS_DIR * item->isDir() +
      NEEDS_PO_CVS * m_validPOCVSRepository + NEEDS_POT_CVS * m_validPOTCVSRepository +
      NEEDS_PO_SVN * m_validPOSVNRepository + NEEDS_POT_SVN * m_validPOTSVNRepository;

   emit selectedChanged(actionValue);
}

void CatalogManagerView::activateItem(QListViewItem *)
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(!item)
      return;

   if(item->isDir())
   {
      item->setOpen(!item->isOpen());
      return;
   }

   if(item->hasPo())
   {
      emit openFile(item->poFile(),item->package());
   }
   else if(item->hasPot())
   {
      emit openTemplate(item->potFile(),item->poFile(),item->package());
   }
   else
   {
      kdError(KBABEL_CATMAN) << "CatalogManagerView::activateItem: item has no file?" << endl;
   }

}

void CatalogManagerView::slotOpenFile()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(item && item->isFile())
   {
      activateItem(item);
   }
}

void CatalogManagerView::slotOpenFileInNewWindow()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(item && item->isFile())
   {
	QString filename;
	if(item->hasPo())
	{
    	    emit openFileInNewWindow(item->poFile(),item->package());
	}
	else if(item->hasPot())
	{
    	    emit openTemplateInNewWindow(item->potFile(),item->poFile(),item->package());
	}
    }
}

void CatalogManagerView::slotOpenTemplate()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(item && item->isFile())
   {
      emit openFile(item->potFile(),item->package());
   }
}

void CatalogManagerView::slotDeleteFile()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(item && item->isFile() && item->hasPo() && !item->hasPot())
   {
      const QString msg=i18n("Do you really want to delete the file %1?").arg(item->poFile());
      if(KMessageBox::warningContinueCancel(this,msg,i18n("Warning"),KGuiItem( i18n("Delete"), "editdelete"))== KMessageBox::Continue)
      {
         if(!QFile::remove(item->poFile()))
         {
             KMessageBox::sorry(this,i18n("Was not able to delete the file %1!").arg(item->poFile()));
         }
      }
   }
}

void CatalogManagerView::toggleMark()
{
    CatManListItem* i = (CatManListItem*) currentItem();
    if( i && i->isDir() ) slotToggleMarksInDir();
    else slotToggleMark();
}

void CatalogManagerView::slotToggleMark()
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(item && item->isFile())
   {
      bool wasMarked=item->marked();
      item->setMarked(!wasMarked);

      if(wasMarked)
      {
         _markerList.remove(item->package());
      }
      else
      {
         _markerList.append(item->package());
      }
   }

    checkSelected();
}

void CatalogManagerView::slotToggleMarksInDir()
{
   CatManListItem* i=(CatManListItem*) currentItem();

   if(i && i->isDir())
   {
      const QStringList contentList = i->allChildrenList(true);

      QStringList::const_iterator it;
      for( it = contentList.constBegin(); it != contentList.constEnd(); ++it )
      {
         CatManListItem* item = _fileList[(*it)];

         if ( item == 0 )
             kdFatal(KBABEL_CATMAN) << "CatalogManagerView::slotToggleMarkInDir: item not in list" << endl;

         const bool wasMarked=item->marked();
         item->setMarked(!wasMarked);

         if(wasMarked)
         {
            _markerList.remove(item->package());
         }
         else
         {
            _markerList.append(item->package());
         }
      }
   }

    checkSelected();
}


void CatalogManagerView::slotClearMarksInDir()
{
   CatManListItem* i=(CatManListItem*) currentItem();

   if(i && i->isDir())
   {
      const QStringList contentList=i->contentsList(true);

      QStringList::const_iterator it;
      for( it = contentList.constBegin(); it != contentList.constEnd(); ++it )
      {
         CatManListItem* item = _fileList[(*it)];

         if ( item == 0 )
           kdFatal(KBABEL_CATMAN) << "CatalogManagerView::slotClearMarkInDir: item not in list" << endl;

         if(item->marked())
         {
            _markerList.remove(item->package());
         }
         item->setMarked(false);
      }
   }

    checkSelected();
}


void CatalogManagerView::slotDirCommand(int index)
{
   CatManListItem* item=(CatManListItem*) currentItem();


   if(index>=0 && item && item->isDir())
   {
      QString cmd=*(_settings.dirCommands).at(index);
      cmd.replace("@PACKAGE@",item->name());
      cmd.replace("@PODIR@",item->poFile());
      cmd.replace("@POTDIR@",item->potFile());
      cmd.replace("@POFILES@",current().join(" "));
      cmd.replace("@MARKEDPOFILES@",marked().join(" "));

      kdDebug(KBABEL_CATMAN) << cmd << endl;

      KProcess* proc = new KShellProcess();
      _pendingProcesses.append(proc);

      connect( proc,SIGNAL( processExited(KProcess *) ), this
               ,SLOT( processEnded(KProcess*) ) );
      connect( proc,SIGNAL( receivedStdout(KProcess*,char*,int) ), this
               ,SLOT( showOutput(KProcess*,char*,int) ) );
      connect( proc,SIGNAL( receivedStderr(KProcess*,char*,int) ), this
               ,SLOT( showOutput(KProcess*,char*,int) ) );

      *proc << "cd" << item->poFile() << ";" << cmd;
      proc->start(KProcess::NotifyOnExit,KProcess::AllOutput);
   }
}

void CatalogManagerView::slotFileCommand(int index)
{
   CatManListItem* item=(CatManListItem*) currentItem();

   if(index>=0 && item && item->isFile())
   {
      CatManListItem* parent = (CatManListItem*)item->parent();

      QString cmd=*(_settings.fileCommands).at(index);
      cmd.replace("@PACKAGE@",item->name());
      cmd.replace("@POFILE@",item->poFile());
      cmd.replace("@POTFILE@",item->potFile());
      cmd.replace("@PODIR@",parent->poFile());
      cmd.replace("@POTDIR@",parent->potFile());
      cmd.replace("@POEMAIL@",item->text(COL_TRANSLATOR));

      kdDebug(KBABEL_CATMAN) << cmd << endl;

      KProcess* proc = new KShellProcess();
      _pendingProcesses.append(proc);

      connect( proc,SIGNAL( processExited(KProcess *) ), this
               ,SLOT( processEnded(KProcess*) ) );
      connect( proc,SIGNAL( receivedStdout(KProcess*,char*,int) ), this
               ,SLOT( showOutput(KProcess*,char*,int) ) );
      connect( proc,SIGNAL( receivedStderr(KProcess*,char*,int) ), this
               ,SLOT( showOutput(KProcess*,char*,int) ) );

      *proc << "cd" << parent->poFile() << ";" << cmd;
      proc->start(KProcess::NotifyOnExit,KProcess::AllOutput);
   }

}


void CatalogManagerView::updateFile(QString fileWithPath, bool force)
{
   QString relFile;
   if(fileWithPath.startsWith(_settings.poBaseDir))
   {
      relFile=fileWithPath.mid(_settings.poBaseDir.length());
   }
   else if(fileWithPath.startsWith(_settings.potBaseDir))
   {
      relFile=fileWithPath.mid(_settings.potBaseDir.length());
   }
   else
   {
      return;
   }

   if(relFile.endsWith(".pot"))
   {
      relFile.truncate(relFile.length()-4);
   }
   else if(relFile.endsWith(".po"))
   {
      relFile.truncate(relFile.length()-3);
   }

   CatManListItem* item=_fileList[relFile];

   if(item)
   {
	_updateNesting++;
	if( force ) item->forceUpdate();
        else item->checkUpdate();
        _updateNesting--;
   }

}

void CatalogManagerView::updateAfterSave(QString fileWithPath, PoInfo &newInfo)
{
   QString relFile;
   if(fileWithPath.startsWith(_settings.poBaseDir))
   {
      relFile=fileWithPath.mid(_settings.poBaseDir.length());
   }
   else if(fileWithPath.startsWith(_settings.potBaseDir))
   {
      relFile=fileWithPath.mid(_settings.potBaseDir.length());
   }
   else
   {
      return;
   }

   if(relFile.endsWith(".pot"))
   {
      relFile.truncate(relFile.length()-4);
   }
   else if(relFile.endsWith(".po"))
   {
      relFile.truncate(relFile.length()-3);
   }

   CatManListItem* item=_fileList[relFile];

   if(item)
   {
      item->updateAfterSave(newInfo);
   }

}

void CatalogManagerView::buildTree()
{
   // in case we were called after settings update
   disconnect( this, SIGNAL( updateFinished() ), this, SLOT(buildTree() ) );

   emit signalBuildTree(false); // announce start of building

   clear();

   if(isActive())
      return;

   _updateNesting++;

   _active=true;
   _stop=false;


   CatManListItem* root = new CatManListItem(this, this,_settings.poBaseDir,_settings.potBaseDir);
   _dirList.insert("/",root);
   //root->setSelectable(false);

   QFileInfo fileInfo(_settings.poBaseDir);
   if(!fileInfo.isDir())
   {
      KMessageBox::error(this,i18n("You have not specified a valid folder "
"for the base folder of the PO files:\n%1\n"
"Please check your settings in the project settings dialog.").arg(_settings.poBaseDir));

      _active=false;
      _updateNesting--;
      if( _updateNesting == 0 ) emit updateFinished();
      return;
   }

   cvshandler->setPOBaseDir( _settings.poBaseDir );
   svnhandler->setPOBaseDir( _settings.poBaseDir );
   mailer->setPOBaseDir(_settings.poBaseDir);

   fileInfo.setFile(_settings.potBaseDir);
   if(!fileInfo.isDir() && !_settings.potBaseDir.isEmpty())
   {
      KMessageBox::error(this,i18n("You have not specified a valid folder "
"for the base folder of the PO template files:\n%1\n"
"Please check your settings in the project settings dialog.").arg(_settings.potBaseDir));
   }

   cvshandler->setPOTBaseDir( _settings.potBaseDir );
   svnhandler->setPOTBaseDir( _settings.potBaseDir );

   setCursor(KCursor::waitCursor());

   //"/" is the root item
   buildDir("/",true);   // build dir without updating the items...

    if( _stop ) {
        _active = false;
	_updateNesting--;
	if( _updateNesting == 0 ) emit updateFinished();
	return;
    }

   _dirWatch->addDir(_settings.poBaseDir);
   if(!_settings.potBaseDir.isEmpty())
	   _dirWatch->addDir(_settings.potBaseDir);

   emit signalBuildTree(true); // announce beginning of tree building

   unsetCursor();

    if( _stop ) {
        _active = false;
	_updateNesting--;
	if( _updateNesting == 0 ) emit updateFinished();
	return;
    }

    updateMarkerList();

   const int files=_fileList.count()+_dirList.count();

   _readInfoCount = 0;

   emit prepareProgressBar(i18n("Reading file information"),files);

   root->setOpen(true);

    if( _stop ) {
        _active = false;
	_updateNesting--;
	if( _updateNesting == 0 ) emit updateFinished();
	return;
    }

   // first read information about the files...
   QDictIterator<CatManListItem> it( _fileList ); // iterator for dict

   int i=0;
   while ( it.current() && !_stop)
   {
      it.current()->checkUpdate(true);
      ++i;
      ++it;
   }

   // ...then update directories
   QDictIterator<CatManListItem> dit( _dirList ); // iterator for dict

   while ( dit.current() && !_stop)
   {
      dit.current()->checkUpdate();
      ++i;
      ++dit;
   }

   emit clearProgressBar();

   _dirWatch->startScan();
   pause(false);

   _active=false;

   _updateNesting--;

   if( _updateNesting == 0 )
   {
	emit updateFinished();
   }
}

bool CatalogManagerView::buildDir(QString relDir,bool fast)
{
   if( _stop ) return false;

   bool haveTemplateDir=true;
   QFileInfo fileInfo;

   fileInfo.setFile(_settings.potBaseDir);
   if(!fileInfo.isDir())
   {
      haveTemplateDir=false;
   }

   bool potHasFiles=false;
   if(haveTemplateDir)
   		potHasFiles=buildDir(_settings.potBaseDir,relDir,".pot",fast);

   bool poHasFiles=buildDir(_settings.poBaseDir,relDir,".po",fast);

   return (poHasFiles | potHasFiles);
}


bool CatalogManagerView::buildDir(const QString& baseDir,const QString& relDir
                                                , const QString extension , bool fast)
{
      if( _stop ) return false;

      bool havePoFiles=false;

      CatManListItem* thisItem=_dirList[relDir];
      if(!thisItem)
      {
         kdFatal(KBABEL_CATMAN) << "null pointer to this item" << endl;
         return false;
      }

      const QString poBaseDir=_settings.poBaseDir;
      const QString potBaseDir=_settings.potBaseDir;

      // traverse directory in poBaseDir
      QDir dir(baseDir+relDir);
      QStringList entryList=dir.entryList("*"+extension,QDir::Files,QDir::Name);

      QStringList::const_iterator it;

      for (  it = entryList.constBegin(); it != entryList.constEnd() && !_stop ; ++it )
      {
	  if( _stop ) return false;

          havePoFiles=true;

          QString file=relDir+(*it);
          file.remove(QRegExp(extension+"$"));
          CatManListItem* item = _fileList[file];
          if(!item)
          {
             item = new CatManListItem(this,thisItem,poBaseDir+file+".po",potBaseDir+file+".pot",file);
             _fileList.insert(file,item);
             _readInfoFileList.prepend(file);

             if(_markerList.contains(file))
             {
                item->setMarked(true);
             }

             if(!fast)
             {
                item->checkUpdate();
             }
          }
      }

      entryList=dir.entryList(QDir::Dirs,QDir::Name);

      for (  it = entryList.constBegin(); it != entryList.constEnd() && !_stop ; ++it )
      {
	  if( _stop ) return false;

          if((*it)=="." || (*it)=="..")
          {
             continue;
          }

          QString subDir=relDir+(*it)+"/";
          if(!_dirWatch->contains(baseDir+subDir))
          {
              _dirWatch->addDir(baseDir+subDir);
          }

          bool otherHasFiles=true;

          CatManListItem* item = _dirList[subDir];
          if(!item && !_stop)
          {
             item = new CatManListItem(this, thisItem,poBaseDir+subDir,potBaseDir+subDir,subDir);
             _dirList.insert(subDir,item);

             otherHasFiles=false;
          }

	  if( _stop ) return false;

          // recursive call
          if(!buildDir(baseDir,subDir,extension,fast) && !otherHasFiles)
          {
             kdDebug(KBABEL_CATMAN) << "skipping " << subDir << endl;
             deleteDirItem(subDir);
			 item=0;
          }
          else
             havePoFiles=true;

      } // end looking up directories in po base dir

      return havePoFiles;
}


void CatalogManagerView::updateDir(QString relDir)
{
      if( _stop ) return;

      kdDebug(KBABEL_CATMAN) << "updating dir " << relDir << endl;

      bool havePoFiles=false;

      CatManListItem* thisItem=_dirList[relDir];
      if(!thisItem)
      {
         kdFatal(KBABEL_CATMAN) << "null pointer to this item" << endl;
         return;
      }

      QStringList contentList = thisItem->contentsList(true);

      const QString poBaseDir=_settings.poBaseDir;
      const QString potBaseDir=_settings.potBaseDir;

      // first lookup template directory
      QDir dir(potBaseDir+relDir);
      QStringList entryList=dir.entryList("*.pot",QDir::Files,QDir::Name);

      QStringList::const_iterator it;

      for (  it = entryList.constBegin(); it != entryList.constEnd(); ++it )
      {
          if( _stop ) return;

          havePoFiles=true;

          QString file=relDir+(*it);
          file.remove(QRegExp(".pot$"));
          CatManListItem* item = _fileList[file];
          if(!item)
          {
             item = new CatManListItem(this, thisItem,poBaseDir+file+".po",potBaseDir+file+".pot",file);
             _fileList.insert(file,item);

             if(_markerList.contains(file))
             {
                item->setMarked(true);
             }

             item->checkUpdate();
          }
		  else
		  {
			 item->checkUpdate();
		  }

          contentList.remove(file);
      }

      entryList=dir.entryList(QDir::Dirs,QDir::Name);

      for (  it = entryList.constBegin(); it != entryList.constEnd(); ++it )
      {
          if( _stop ) return;

          if((*it)=="." || (*it)=="..")
          {
             continue;
          }

          bool newDirAdded=false;

          QString subDir=relDir+(*it)+"/";
          if(!_dirWatch->contains(potBaseDir+subDir))
          {
              _dirWatch->addDir(potBaseDir+subDir);

              newDirAdded=true;
          }

          CatManListItem* item = _dirList[subDir];
          if(!item && newDirAdded)
          {
             item = new CatManListItem(this, thisItem,poBaseDir+subDir,potBaseDir+subDir,subDir);
             _dirList.insert(subDir,item);

              if(!buildDir(subDir,false))
              {
                   kdDebug(KBABEL_CATMAN) << "skipping " << subDir << endl;
                   deleteDirItem(subDir);
                   item=0;
              }
          }
		  else if(newDirAdded)
		  {
			  updateDir(subDir);
		  }


		  // if directory was already here, but no item
		  // -> directory contains no files
          if(item && !newDirAdded)
          {
             havePoFiles=true;
          }

      } // end looking up directories in template dir

      // now traverse directory in poBaseDir
      dir.setPath(poBaseDir+relDir);
      entryList=dir.entryList("*.po",QDir::Files,QDir::Name);

      for (  it = entryList.constBegin(); it != entryList.constEnd(); ++it )
      {
          havePoFiles=true;

          if( _stop ) return;

          QString file=relDir+(*it);
          file.remove(QRegExp(".po$"));
          CatManListItem* item = _fileList[file];
          if(!item)
          {
             item = new CatManListItem(this, thisItem,poBaseDir+file+".po",potBaseDir+file+".pot",file);
             _fileList.insert(file,item);

             if(_markerList.contains(file))
             {
                item->setMarked(true);
             }

             item->checkUpdate();
          }
		  else
		  {
			 item->checkUpdate();
		  }

          contentList.remove(file);
      }

      entryList=dir.entryList(QDir::Dirs,QDir::Name);

      for (  it = entryList.constBegin(); it != entryList.constEnd(); ++it )
      {
          if( _stop ) return;

          if((*it)=="." || (*it)=="..")
          {
             continue;
          }

          bool newDirAdded=false;

          QString subDir=relDir+(*it)+"/";
          if(!_dirWatch->contains(poBaseDir+subDir))
          {
               _dirWatch->addDir(poBaseDir+subDir);
               newDirAdded=true;
          }

          CatManListItem* item = _dirList[subDir];

          bool templateHasFiles=(bool)item;

          if(!item && newDirAdded)
          {
             item = new CatManListItem(this, thisItem,poBaseDir+subDir,potBaseDir+subDir,subDir);
             _dirList.insert(subDir,item);

              if(!buildDir(subDir,false) && !templateHasFiles)
              {
                   kdDebug(KBABEL_CATMAN) << "skipping " << subDir << endl;
                   deleteDirItem(subDir);
				   item=0;
              }
          }
		  else if(newDirAdded)
		  {
			  updateDir(subDir);
		  }

		  // if directory was already here, but no item
		  // -> directory contains no files
          if(item && !newDirAdded)
          {
             havePoFiles=true;
          }


      } // end looking up directories in po base dir


      // check, if something in the directory has been deleted
      // but only if we traversed also the template directory
      if(contentList.count()>0)
      {
          QStringList::const_iterator it;
          for( it = contentList.constBegin(); it != contentList.constEnd(); ++it )
          {
             QFileInfo po(poBaseDir+(*it));
             QFileInfo pot(potBaseDir+(*it));

             if(!po.exists() && !pot.exists())
             {
                CatManListItem* item = _fileList[(*it)];
                if(item)
                {
                   if(item->marked())
                      _markerList.remove(item->package());

                   _fileList.remove((*it));
                   delete item;
                }
             }
          }
      }

      if(!havePoFiles)
      {
	     deleteDirItem(relDir);

          // if this directory has to be removed, check, if
		  // the parent directory has to be removed too
          const int index=relDir.findRev("/",relDir.length()-2);
          if(index<0)
          {
            relDir="/";
          }
          relDir=relDir.left(index+1);
          updateDir(relDir);
      }
}

void CatalogManagerView::directoryChanged(const QString& dir)
{
   pause(true);

   QString relDir, relDirPo, relDirPot;
   if(dir.startsWith(_settings.poBaseDir))
   {
      relDirPo=dir.mid(_settings.poBaseDir.length());
   }
   if(dir.startsWith(_settings.potBaseDir))
   {
      relDirPot=dir.mid(_settings.potBaseDir.length());
   }
   
   if( relDirPo.isEmpty() )
   {
    // use POT
    relDir = relDirPot;
   }
   else if( relDirPot.isEmpty() )
   {
    // use PO
    relDir = relDirPo;
   }
   else 
   {
    // both PO and POT usable, find out the correct one
    if( relDirPo.left(1) == "/" )
    {
	relDir = relDirPo;
    }
    else
    {
	relDir = relDirPot;
    }
   }

   if(relDir.right(1)!="/")
   {
      relDir+="/";
   }

   kdDebug(KBABEL_CATMAN) << "directory changed: " << relDir << endl;

   QFileInfo fileInfo(_settings.potBaseDir);

   CatManListItem* thisItem=_dirList[relDir];
   if(!thisItem)
   {
      // if this item is not in the list search for next existing parent item
      QString prevRelDir;
      do
      {
          prevRelDir=relDir;
          const int index=relDir.findRev("/",relDir.length()-2);
          if(index<0)
          {
            relDir="/";
          }
          relDir=relDir.left(index+1);

          thisItem=_dirList[relDir];
      }
      while(relDir!="/" && !thisItem);

      if(!thisItem)
      {
         kdFatal(KBABEL_CATMAN) << "null pointer to this item: " << relDir << endl;
         return;
      }
      else
      {
          // if a parent item dir is found, create the needed item in this dir
          // and build the tree from this item on
          kdDebug(KBABEL_CATMAN) << "building dir: " << prevRelDir << endl;
          CatManListItem* item = new CatManListItem(this, thisItem,_settings.poBaseDir+prevRelDir
                     ,_settings.potBaseDir+prevRelDir,prevRelDir);
          _dirList.insert(prevRelDir,item);


          if(!buildDir(prevRelDir,false))
          {
             deleteDirItem(prevRelDir);
          }
      }
   }
   else
   {
      updateDir(relDir);
   }

   pause(false);
}


void CatalogManagerView::directoryDeleted(const QString& dir)
{
   pause(true);

   QString relDir, relDirPo, relDirPot;
   if(dir.startsWith(_settings.poBaseDir))
   {
      relDirPo=dir.mid(_settings.poBaseDir.length());
   }
   if(dir.startsWith(_settings.potBaseDir))
   {
      relDirPot=dir.mid(_settings.potBaseDir.length());
   }
   
   if( relDirPo.isEmpty() )
   {
    // use POT
    relDir = relDirPot;
   }
   else if( relDirPot.isEmpty() )
   {
    // use PO
    relDir = relDirPo;
   }
   else 
   {
    // both PO and POT usable, find out the correct one
    if( relDirPo.left(1) == "/" )
    {
	relDir = relDirPo;
    }
    else
    {
	relDir = relDirPot;
    }
   }

   if(relDir.right(1)!="/")
   {
      relDir+="/";
   }

   kdDebug(KBABEL_CATMAN) << "directory deleted: " << relDir << endl;

   CatManListItem* thisItem=_dirList[relDir];
   if(thisItem)
   {
	  // we have to take care, if one directory still exists
	  const bool poDeleted=!thisItem->hasPo();
	  const bool potDeleted=!thisItem->hasPot();

	  // if neither the po- nor the pot-directory exists any more
	  // delete all sub items
	  if(poDeleted && potDeleted)
	  {
		  deleteDirItem(relDir);
	  }
	  else
	  {
      	 QStringList childList = thisItem->contentsList();

         CatManListItem* item;
         QStringList::const_iterator it;
         for( it = childList.constBegin();it !=  childList.constEnd(); ++it )
         {
            item=_fileList[(*it)];
            if(item)
            {
		       if( (poDeleted && !item->hasPot()) ||
						(potDeleted && !item->hasPo()) )
			   {
                  _fileList.remove((*it));
                  delete item;
			   }
			   else
			   {
			      item->checkUpdate();
			   }
            }
            else
            {
               item=_dirList[(*it)];
               if(item)
               {
                  if( (poDeleted && !item->hasPot()) ||
						(potDeleted && !item->hasPo()) )
				  {
					  deleteDirItem((*it));
				  }
               }
               else
               {
                  kdDebug(KBABEL_CATMAN) << "directoryDeleted: don't have item "
						  << (*it) << endl;
               }
            }
         }
      }
   }

   pause(false);
}

void CatalogManagerView::fileInfoRead( QString filename )
{
    if( _readInfoFileList.find( filename ) != _readInfoFileList.end() ) {
	emit progress( ++_readInfoCount);
	_readInfoFileList.remove( filename );
    }

    if( _readInfoFileList.isEmpty() ) emit clearProgressBar();
}

void CatalogManagerView::setSettings(CatManSettings newSettings)
{
   CatManSettings oldSettings=_settings;
   _settings=newSettings;

   if(_settings.poBaseDir.right(1)=="/")
      _settings.poBaseDir.truncate(_settings.poBaseDir.length()-1);
   if(_settings.potBaseDir.right(1)=="/")
      _settings.potBaseDir.truncate(_settings.potBaseDir.length()-1);

   _dirCommandsMenu->clear();
   int counter=0;
   for ( QStringList::const_iterator it = _settings.dirCommandNames.constBegin()
                        ; it != _settings.dirCommandNames.constEnd(); ++it )
   {
      _dirCommandsMenu->insertItem((*it),counter);
      counter++;
   }
   _dirCommandsMenu->insertSeparator();
   _dirCommandsMenu->insertItem(i18n("Log Window"),this,SLOT(showLog()));

   _fileCommandsMenu->clear();
   counter=0;
   for ( QStringList::const_iterator it = _settings.fileCommandNames.constBegin()
                        ; it != _settings.fileCommandNames.constEnd(); ++it )
   {
      _fileCommandsMenu->insertItem((*it),counter);
      counter++;
   }
   _fileCommandsMenu->insertSeparator();
   _fileCommandsMenu->insertItem(i18n("Log Window"),this,SLOT(showLog()));

   const bool pathChanged = (oldSettings.poBaseDir!=_settings.poBaseDir)
           || (oldSettings.potBaseDir!=_settings.potBaseDir);
   
   if(pathChanged)
   {
      if( !isActive() ) {
        QTimer::singleShot(100,this,SLOT(buildTree()));
      } else {
        stop();
        connect( this, SIGNAL( updateFinished() ), this, SLOT(buildTree() ) );
      }
   }

   toggleColumn( COL_MARKER, _settings.flagColumn );
   toggleColumn( COL_FUZZY, _settings.fuzzyColumn );
   toggleColumn( COL_UNTRANS, _settings.untranslatedColumn );
   toggleColumn( COL_TOTAL, _settings.totalColumn );
   // FIXME: follow CVS/SVN status
   toggleColumn( COL_CVS_OR_SVN, _settings.cvsColumn );
   toggleColumn( COL_REVISION, _settings.revisionColumn );
   toggleColumn( COL_TRANSLATOR, _settings.translatorColumn );
}


CatManSettings CatalogManagerView::settings() const
{
   return _settings;
}


void CatalogManagerView::hideEvent(QHideEvent*)
{
   pause(true);

   if(_dirWatch)
      _dirWatch->stopScan();
}

void CatalogManagerView::showEvent(QShowEvent*)
{
   QTimer::singleShot(1,this,SLOT(checkUpdate()));

   pause(false);

   if(_dirWatch)
      _dirWatch->startScan(true);
}

void CatalogManagerView::contentsMousePressEvent(QMouseEvent* event)
{
   if(event->button() == LeftButton)
      _pressPos=event->pos();

   QListView::contentsMousePressEvent( event );
}

void CatalogManagerView::contentsMouseMoveEvent(QMouseEvent* event)
{
   if(event->state() & LeftButton)
   {
      const int delay = KGlobalSettings::dndEventDelay();
      if(QABS( event->pos().x() - _pressPos.x() ) >= delay ||
         QABS( event->pos().y() - _pressPos.y() ) >= delay)
      {
         CatManListItem* item = (CatManListItem*)itemAt(contentsToViewport(_pressPos));
         if(item && item->isFile())
         {
            // always add the po-file and if existing the pot-file to the drag and
            // let the user decide what to do, when dropping into kbabel
            QStrList uri;
            uri.append(QUriDrag::localFileToUri(item->poFile()));
            if(item->hasPot())
               uri.append(QUriDrag::localFileToUri(item->potFile()));

            QUriDrag* drag = new QUriDrag(uri,this);
            QPixmap icon=KGlobal::iconLoader()->loadIcon("txt",KIcon::Desktop);
            drag->setPixmap(icon,QPoint(icon.width()/2,icon.height()/2));
            drag->drag();
         }
         else
         {
            QListView::contentsMouseMoveEvent(event);
         }
      }
      else
      {
         QListView::contentsMouseMoveEvent(event);
      }
   }
   else
   {
      QListView::contentsMouseMoveEvent(event);
   }
}

void CatalogManagerView::readMarker( KConfig* config)
{
   KConfigGroupSaver cs(config,"CatalogManager");

   _markerList = config->readListEntry("Marker");
}

void CatalogManagerView::saveMarker( KConfig* config) const
{
   KConfigGroupSaver cs(config,"CatalogManager");

   config->writeEntry("Marker",_markerList);
   config->sync();
}


void CatalogManagerView::deleteDirItem(QString relDir)
{
	CatManListItem* thisItem=_dirList[relDir];

	if(!thisItem)
		return;

	_dirList.remove(relDir);

	QStringList childList = thisItem->allChildrenList();

	QStringList::const_iterator it;
	for( it = childList.constBegin();it !=  childList.constEnd(); ++it )
    {
		if(!_fileList.remove((*it)))
			_dirList.remove((*it));
	}


	// delete the item with all sub item
	delete thisItem;
}



void CatalogManagerView::processEnded(KProcess* proc)
{
   _pendingProcesses.removeRef(proc);
}


void CatalogManagerView::showOutput(KProcess*, char *buffer, int buflen)
{
   const QCString output(buffer,buflen+1);

   _logView->insert(output);
}

void CatalogManagerView::columnClicked(QListViewItem * item, const QPoint &, int c)
{
    if( item && c == COL_MARKER )
    {
       slotToggleMark();
    }
}

CatManListItem *CatalogManagerView::itemBelow( CatManListItem *item )
{
    if( item->isDir() )
    {
	if( item->firstChild() )
	    return static_cast<CatManListItem *>( item->firstChild() );
	else
	{
	    while( !static_cast<CatManListItem *>( item->nextSibling() ) )
	    {
		item = static_cast<CatManListItem *>( item->parent() );
		if( !item ) return item;
	    }
	    return static_cast<CatManListItem *>( item->nextSibling() );
	}
    }
    else
    {
	while( !static_cast<CatManListItem *>( item->nextSibling() ) )
	{
	    item =  static_cast<CatManListItem *>( item->parent());
	    if( !item ) return item;
	}
	return static_cast<CatManListItem *>( item->nextSibling() );
    }
}

CatManListItem *CatalogManagerView::itemAbove( CatManListItem *item )
{
    if( item->isDir() )
    {
	if( item->firstChild() )
	    return static_cast<CatManListItem *>( item->lastChild() );
	else
	{
	    while( !static_cast<CatManListItem *>( item->previousSibling() ) )
	    {
		item = static_cast<CatManListItem *>( item->parent() );
		if( !item ) return item;
	    }
	    return static_cast<CatManListItem *>( item->previousSibling() );
	}
    }
    else
    {
	while( !static_cast<CatManListItem *>( item->previousSibling() ) )
	{
	    item =  static_cast<CatManListItem *>( item->parent());
	    if( !item ) return item;
	}
	return static_cast<CatManListItem *>( item->previousSibling() );
    }
}

void CatalogManagerView::gotoNextUntranslated()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->untranslated() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousUntranslated()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->untranslated() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextFuzzy()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->fuzzy() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousFuzzy()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->fuzzy() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextFuzzyOrUntranslated()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->untranslated() > 0 || i->fuzzy() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousFuzzyOrUntranslated()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->untranslated() > 0 || i->fuzzy() > 0 )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextError()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->hasErrors() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousError()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->hasErrors() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextTemplate()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->hasPot() && !i->hasPo() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousTemplate()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->hasPot() && !i->hasPo() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextPo()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->hasPo() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousPo()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->hasPo() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoNextMarked()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( ( i = itemBelow(current)) )
    {
	if( i->marked() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

void CatalogManagerView::gotoPreviousMarked()
{
    CatManListItem *i;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    while( (i = itemAbove(current)) )
    {
	if( i->marked() )
	{
	    setCurrentItem(i);
	    ensureItemVisible(i);
	    return;
	} else current = i;
    }
}

QStringList CatalogManagerView::current()
{
    QStringList result;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    if( current->isDir() )
    {
	QStringList s = current->allChildrenList(true);
	QStringList::const_iterator it;
	for( it = s.constBegin() ; it != s.constEnd(); ++it )
	{
	    CatManListItem *item = _fileList[(*it)];
	    result.append( item->poFile() );
	}
    }
    else
    {
	if( current->hasPo() ) result.append( current->poFile() );
    }
    return result;
}

QStringList CatalogManagerView::marked()
{
    QStringList result;

    QStringList::const_iterator it;
    for( it = _markerList.constBegin() ; it != _markerList.constEnd(); ++it )
    {
	CatManListItem *item = _fileList[(*it)];
	result.append( item->poFile() );
    }
    return result;
}

void CatalogManagerView::updateCurrent()
{
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    if( !current->hasPo() && !current->hasPot() )
    {
	if( current->isFile() )
	{
	    _fileList.remove(current->package());
	    delete current;
	}
	else
	{
	    directoryDeleted(current->package());
	}
    }
    else
    {
	if( current->isDir() )
	{
	    directoryChanged(current->poFile() );
	}

	// check, if the item didn't get lost by the update in directoryChanged()
	CatManListItem *new_current = static_cast<CatManListItem *>(currentItem());
        if (new_current == current)
	{
	    current->forceUpdate();
	}
    }
}

void CatalogManagerView::updateFiles( const QStringList& files )
{
  QStringList::ConstIterator it;
  for ( it = files.constBegin( ); it != files.constEnd( ); ++it ) {
    updateFile( *it, true );
  }
}

CVSHandler * CatalogManagerView::cvsHandler( )
{
  return cvshandler;
}

SVNHandler * CatalogManagerView::svnHandler( )
{
   return svnhandler;
}

void CatalogManagerView::validateUsingTool( const KDataToolInfo &tool, const QString& command )
{
    QStringList result;
    CatManListItem *current = static_cast<CatManListItem *>(currentItem());
    if( current->isDir() )
    {
	const QStringList s = current->allChildrenList(true);
	QStringList::const_iterator it;
	for( it = s.constBegin() ; it != s.constEnd(); ++it )
	{
	    CatManListItem *item = _fileList[(*it)];
	    if( item && item->hasPo() ) result.append( item->package() );
	}
    }
    else
    {
	result.append( current->package() );
    }

    validate_internal( result, tool, command );
}

void CatalogManagerView::validateMarkedUsingTool( const KDataToolInfo &tool, const QString& command )
{
    validate_internal( _markerList, tool, command );
}

void CatalogManagerView::validate_internal( const QStringList& files, const KDataToolInfo &tool, const QString& )
{
    if( files.isEmpty() ) return;

    KDataTool* t = tool.createTool();

    if( !t )
    {
	KMessageBox::error( this, i18n("Cannot instantiate a validation tool.\n"
	"Please check your installation."), i18n("Validation Tool Error") );
	return;
    }

    // setup options
    if( !_validateOptionsDlg )
    {
	_validateOptionsDlg = new KDialogBase( this, "validation options",
	    true, i18n("Validation Options"), KDialogBase::Ok|KDialogBase::Cancel);
	_validateOptions = new ValidationOptions(_validateOptionsDlg);
	_validateOptionsDlg->setMainWidget( _validateOptions );
	_validateOptions->resize( _validateOptions->sizeHint() );

	// setup stored values
	_validateOptions->markAsFuzzy->setChecked( _markAsFuzzy );
	_validateOptions->ignoreFuzzy->setChecked( _ignoreFuzzy );
    }

    if( _validateOptionsDlg->exec() != QDialog::Accepted )
    {
	delete t;

	return;
    }

    if( !_validateDialog )
    {
	_validateDialog = new ValidateProgressDialog(_settings.ignoreURL, this);
	connect( _validateDialog, SIGNAL( errorDoubleClicked(const QString,const int)),
	    this, SLOT(showError( const QString, const int )));
    }

    _markAsFuzzy = _validateOptions->markAsFuzzy->isChecked();
    _ignoreFuzzy = _validateOptions->ignoreFuzzy->isChecked();

    _validateDialog->setMarkAsFuzzy(_markAsFuzzy);
    _validateDialog->setIgnoreFuzzy(_ignoreFuzzy);

    QPtrList<CatManListItem> fileitems;

    for( QValueListConstIterator<QString> it=files.begin() ; it!=files.end() ; ++it )
    {
	CatManListItem* item=_fileList[ (*it) ];
	if( item ) {
	    fileitems.append(item);
	} else kdDebug(KBABEL_CATMAN) << "Cannot find the file item for " << (*it) << endl;
    }

    _validateDialog->validate(tool, fileitems);

    delete t;
}

void CatalogManagerView::showError( const QString package, const int num )
{
    CatManListItem* item = _fileList[ package];
    if( !item )
    {
	kdWarning() << "Can not find error package: " << package << endl;
	return;
    }

    emit gotoFileEntry( item->poFile(), package, num );
}

void CatalogManagerView::updateMarkerList()
{
    QStringList newMarkers;

    for( QStringList::const_iterator it = _markerList.constBegin(); it != _markerList.constEnd(); ++it ) {
        if( _fileList[ (*it) ] )
	    newMarkers.append( (*it) );
    }

    _markerList = newMarkers;
}

void CatalogManagerView::slotValidPOCVSRepository( bool valid )
{
  m_validPOCVSRepository = valid;
  slotToggleCVSOrSVNColumn(valid);
  // set initial state for CVS menu entries
  emit selectedChanged(NEEDS_PO + NEEDS_PO_CVS * m_validPOCVSRepository);
}

void CatalogManagerView::slotValidPOSVNRepository( bool valid )
{
    m_validPOSVNRepository = valid;
    slotToggleCVSOrSVNColumn(valid);
  // set initial state for SVN menu entries
    emit selectedChanged(NEEDS_PO + NEEDS_PO_SVN * m_validPOSVNRepository);
}

void CatalogManagerView::slotValidPOTCVSRepository( bool valid )
{
    m_validPOTCVSRepository = valid;
  // set initial state for CVS menu entries
  // ### TODO: something missing here?
}

void CatalogManagerView::slotValidPOTSVNRepository( bool valid )
{
    m_validPOTSVNRepository = valid;
  // set initial state for SVN menu entries
  // ### TODO: something missing here?
}

void CatalogManagerView::slotToggleCVSOrSVNColumn( bool show )
{
#if 0
  toggleColumn( COL_CVS_OR_SVN, show );
#else
  // ### HACK
  toggleColumn( COL_CVS_OR_SVN, m_validPOCVSRepository || m_validPOSVNRepository );

#endif
}

void CatalogManagerView::toggleColumn( uint column, bool show )
{
  if ( show ) {
    setColumnWidthMode( column, Maximum );
    setColumnWidth( column, -1 );
    // ensure that the column heading is always fully visible
    setColumnText( column, COLTEXT(column));
  } else {
    setColumnWidthMode( column, Manual );
    setColumnWidth( column, 0 );
  }
}

#include "catalogmanagerview.moc"
// kate: space-indent on; indent-width 4; replace-tabs on;
