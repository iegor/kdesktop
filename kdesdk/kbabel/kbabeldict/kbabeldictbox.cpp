/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2003-2005 by Stanislav Visnovsky
			    <visnovsky@kde.org>

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


#include <dcopclient.h>
#include <kaboutapplication.h>

#include "aboutmoduledlg.h"
#include "kbabeldictbox.h"
#include <version.h>
#include <resources.h>

#include <kaboutdata.h>
#include <kaboutdialog.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kcmenumngr.h>
#include <kdialogbase.h>
#include <klibloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kdebug.h>
#include <ktrader.h>
#include <kwin.h>

#include <qclipboard.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qstylesheet.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qtimer.h>

#define KBABELDICT 5321

using namespace KBabel;

class ResultListItem : public QListViewItem
{
public:
    ResultListItem(QListView *parent, const SearchResult& result,bool richText);

    virtual QString key(int column, bool ascending) const;
    const SearchResult* result() const;
    bool richText() const { return _richText; }

private:
    SearchResult _result;
    bool _richText;
};

ResultListItem::ResultListItem(QListView *parent, const SearchResult& result
                , bool richText)
        : QListViewItem(parent)
        , _result(result)
        , _richText(richText)
{
    int score=_result.score;
    if(score<0)
        score=0;
    else if(score>100)
        score=100;
    setText(0,QString::number(score));
    
    QString tmp;
    if(richText)
        tmp=_result.plainFound;
    else
	// FIXME: what about plural forms?
        tmp=result.found.first();
    
    bool cutted=false;
    int index=tmp.find('\n');
    if(index > 0)
    {
        tmp=tmp.left(index);
        cutted=true;
    }
    if(tmp.length() > 30)
    {
        tmp=tmp.left(30);
        cutted=true;
    }
    tmp=tmp.stripWhiteSpace();
    if(cutted)
        tmp+="...";

    setText(1,tmp);
    
    if(richText)
        tmp=_result.plainTranslation;
    else
        tmp=result.translation;
    
    cutted=false;
    index=tmp.find('\n');
    if(index > 0)
    {
        tmp=tmp.left(index);
        cutted=true;
    }
    if(tmp.length() > 30)
    {
        tmp=tmp.left(30);
        cutted=true;
    }
    tmp=tmp.stripWhiteSpace();
    if(cutted)
        tmp+="...";
    setText(2,tmp);


    if(!_result.descriptions.isEmpty())
    {
        TranslationInfo *ti = _result.descriptions.first();
        if(ti)
        {
            setText(3,ti->location);
        }
    }

}

QString ResultListItem::key(int column, bool ascending) const
{
    if(column==0)
    {
        QString result=QString::number(_result.score);
        result=result.rightJustify(10,'0');

        return result;
    }
    
    return QListViewItem::key(column,ascending);
}

const SearchResult *ResultListItem::result() const
{
    return &_result;
}

/* 
 *  Constructs a KBabelDictBox which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KBabelDictBox::KBabelDictBox( QWidget* parent,  const char* name, WFlags fl )
    : DCOPObject("KBabelDict"), QWidget( parent, name, fl )
{
   active=-1;
   currentResult=0;
   currentInfo=0;
   moduleList.setAutoDelete(false);


   QVBoxLayout *mainLayout = new QVBoxLayout(this);
   mainLayout->setMargin(KDialog::marginHint());
   mainLayout->setSpacing(KDialog::spacingHint());
   
   QGridLayout *grid = new QGridLayout(mainLayout);

   QHBoxLayout *hbox = new QHBoxLayout;
   QLabel *label = new QLabel(i18n("Total:"),this);
   hbox->addWidget(label);
   totalResultsLabel = new QLabel("0",this);
   hbox->addWidget(totalResultsLabel);
   grid->addLayout(hbox,0,0);  
    
   hbox = new QHBoxLayout;
   label = new QLabel(i18n("Current:"), this);
   hbox->addWidget(label);
   currentLabel = new QLabel("0",this);
   hbox->addWidget(currentLabel);
   grid->addLayout(hbox,1,0);  


   hbox = new QHBoxLayout;
   label = new QLabel(i18n("Found in:"), this);
   hbox->addWidget(label);
   locationLabel = new QLabel(this);
   hbox->addWidget(locationLabel);
   hbox->setStretchFactor(locationLabel,2);
   grid->addLayout(hbox,0,1);  

   hbox = new QHBoxLayout;
   label = new QLabel(i18n("Translator:"), this);
   hbox->addWidget(label);
   translatorLabel = new QLabel(this);
   translatorLabel->setMinimumSize(50,0);
   hbox->addWidget(translatorLabel);
   hbox->setStretchFactor(translatorLabel,2);
   grid->addLayout(hbox,1,1);
   
   grid->setColStretch(1,2);


   hbox = new QHBoxLayout;
   label = new QLabel(i18n("Date:"),this);
   hbox->addWidget(label);
   dateLabel = new QLabel(this);
   dateLabel->setMinimumSize(50,0);
   hbox->addWidget(dateLabel);
   hbox->setStretchFactor(dateLabel,2);

   moreButton = new QPushButton(this,"moreButton");
   moreButton->setText(i18n("&More"));
   moreButton->setEnabled(false);
   moreButton->setAutoRepeat(true);
   hbox->addWidget(moreButton);

   mainLayout->addLayout(hbox);

   
   hbox = new QHBoxLayout;
   hbox->addStretch(1);
   listButton = new QToolButton(Qt::UpArrow,this);
   listButton->setFixedSize(20,15);
   listButton->setAutoRepeat(false);
   connect(listButton,SIGNAL(clicked()),this,SLOT(showListOnly()));
   hbox->addWidget(listButton);
   detailButton = new QToolButton(Qt::DownArrow,this);
   detailButton->setFixedSize(20,15);
   detailButton->setAutoRepeat(false);
   connect(detailButton,SIGNAL(clicked()),this,SLOT(showDetailsOnly()));
   hbox->addWidget(detailButton);

   mainLayout->addLayout(hbox);


   resultSplitter = new QSplitter(Qt::Vertical,this
           ,"resultsplitter");
   mainLayout->addWidget(resultSplitter);
   
   viewContainer = new QSplitter(Qt::Vertical,resultSplitter, "singleEntrySplitter");
   QVBoxLayout *vbox = new QVBoxLayout(viewContainer);
   vbox->setResizeMode(QLayout::FreeResize);
   origView = new QTextView(viewContainer,"origView");
   origView->setWordWrap( QTextEdit::WidgetWidth );
   origView->setMinimumSize(1,1);
   vbox->addWidget(origView);
   translationView = new QTextView(viewContainer,"translationView");
   origView->setWordWrap( QTextEdit::WidgetWidth );
   translationView->setMinimumSize(1,1);
   vbox->addWidget(translationView);
   viewContainer->setMinimumSize(1,1);
   
   resultListView = new KListView( resultSplitter, "resultListView" );
   resultListView->setMinimumSize(1,1);
   resultListView->addColumn( i18n( "Score" ) );
   resultListView->addColumn( i18n( "Original" ) );
   resultListView->addColumn( i18n( "Translation" ) );
   resultListView->addColumn( i18n( "Location" ) );

   resultListView->installEventFilter(this);
   connect(resultListView
           , SIGNAL(doubleClicked(QListViewItem *,const QPoint&,int))
           , this, SLOT(editFile()));
   connect(resultListView, SIGNAL(returnPressed(QListViewItem *))
           , this, SLOT(editFile()));
   connect(resultListView
           , SIGNAL(contextMenu(KListView *,QListViewItem *,const QPoint&))
           , this
           , SLOT(showContextMenu(KListView *,QListViewItem *,const QPoint&)));
   
   resultSplitter->setResizeMode(viewContainer,QSplitter::KeepSize);
   QValueList<int> sizes;
   sizes.append(50);
   sizes.append(50);
   resultSplitter->setSizes(sizes);


   hbox = new QHBoxLayout;
   hbox->addStretch(1);
   
   prevButton = new QPushButton(i18n("< &Previous"),this);
   prevButton->setEnabled(false);
   prevButton->setAutoRepeat(true);
   hbox->addWidget(prevButton);
   
   nextButton = new QPushButton(i18n("&Next >"),this);
   nextButton->setEnabled(false);
   nextButton->setAutoRepeat(true);
   hbox->addWidget(nextButton);
   
   hbox->addStretch(1);
   mainLayout->addLayout(hbox);
   
   totalResultsLabel->setNum(100000);
   totalResultsLabel->setFixedSize(totalResultsLabel->sizeHint());
   totalResultsLabel->setNum(0);
   currentLabel->setNum(100000);
   currentLabel->setFixedSize(currentLabel->sizeHint());
   currentLabel->setNum(0);

   setRMBMenu(new QPopupMenu(this));
   QStringList fileList;
#if 0
   // try to find installed modules by looking into directories
   // kbabeldict/modules and getting all files *.rc
   QStringList dirList = KGlobal::dirs()->findDirs("data"
                                    ,"kbabeldict/modules");

   for ( QStringList::Iterator it = dirList.begin(); it != dirList.end()
                                             ; ++it )
   {
      QDir dir((*it),"*.rc");
      QStringList list = dir.entryList(QDir::Files|QDir::Readable);
      
      for ( QStringList::Iterator fit = list.begin(); fit != list.end()
                                             ; ++fit )
      {
         if(!fileList.contains((*fit)))
         {
            fileList.append((*fit));
         }
      }
   }
#endif

   // use locate to locate the actual file, because rcfiles in the users
   // directory is prefered for systemwide rc files
   QStringList rcList;
   for( QStringList::Iterator fit = fileList.begin(); fit != fileList.end();
                                             ++fit)
   {
      rcList.append(locate("data","kbabeldict/modules/"+(*fit)));
   }
   
   for( QStringList::Iterator rit = rcList.begin(); rit != rcList.end();
                                             ++rit)
   {
      KConfig rcConfig((*rit),true,false);
      
      rcConfig.setGroup("SearchEngine");

      QStringList appList = rcConfig.readListEntry("Applications");
      KInstance *inst = KGlobal::instance();
      if(inst && !appList.isEmpty() && !appList.contains(inst->instanceName()))
      {
          continue;
      }

      QString libName = rcConfig.readEntry("Lib");

      if(!libName.isNull())
      {
         kdDebug(KBABELDICT) << "loading library " << libName << endl;

         KLibFactory *factory=KLibLoader::self()->factory(libName.latin1());
   
         if(factory)
         {
            SearchEngine *e = (SearchEngine *)factory->create(this
                  , "searchengine", "SearchEngine");
            if(!e)
            {
               kdError() << "searchengine not initialized" << endl;
            }
            else
            {
		registerModule(e);
            }
         }
         else
         {
            kdError() << "wasn't able to load library" << endl;
         }

      }
   }
   
   kdDebug(KBABEL_SEARCH) << "Now using trader for " << KGlobal::instance()->instanceName() << endl;

   // try to find installed modules by KTrader
   KTrader::OfferList offers = KTrader::self()->query("KBabelDictModule",
	    "('"+KGlobal::instance()->instanceName()+"' in [Applications])");
	    
   for(KTrader::OfferList::ConstIterator it = offers.begin(); it != offers.end(); ++it )
   {
      KLibFactory *factory = KLibLoader::self()->factory( (*it)->library().local8Bit() );
   
      if(factory)
      {
            SearchEngine *e = (SearchEngine *)factory->create(this
                  , "searchengine", "SearchEngine");
            if(!e)
            {
               kdError() << "searchengine not initialized" << endl;
            }
            else
            {
		registerModule(e);
            }
      }
      else
      {
            kdError() << "wasn't able to load library" << endl;
      }
   }
   
   kdDebug(KBABEL_SEARCH) << "Now for any application" << endl;

   offers = KTrader::self()->query("KBabelDictModule",
	    "not ( exist Applications)");
	    
   for(KTrader::OfferList::ConstIterator it = offers.begin(); it != offers.end(); ++it )
   {
      KLibFactory *factory = KLibLoader::self()->factory( (*it)->library().local8Bit() );
   
      if(factory)
      {
            SearchEngine *e = (SearchEngine *)factory->create(this
                  , "searchengine", "SearchEngine");
            if(!e)
            {
               kdError() << "searchengine not initialized" << endl;
            }
            else
            {
		registerModule(e);
            }
      }
      else
      {
            kdError() << "wasn't able to load library" << endl;
      }
   }

   connect(nextButton,SIGNAL(clicked()),this,SLOT(slotNextResult()));
   connect(prevButton,SIGNAL(clicked()),this,SLOT(slotPrevResult()));
   connect(moreButton,SIGNAL(clicked()),this,SLOT(nextInfo()));
   

   origView->installEventFilter(this);
   translationView->installEventFilter(this);

   resultListView->setSorting(0,false);
   resultListView->setAllColumnsShowFocus(true);

   connect(resultListView,SIGNAL(selectionChanged(QListViewItem*))
           , this, SLOT(showResult(QListViewItem*)));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KBabelDictBox::~KBabelDictBox()
{
}

void KBabelDictBox::registerModule( SearchEngine* e )
{
    active = 0;
    moduleList.append(e);
    connect(e, SIGNAL(started()),this,SIGNAL(searchStarted()));
    connect(e, SIGNAL(finished()),this,SIGNAL(searchStopped()));
    connect(e, SIGNAL(finished()),this
        ,SLOT(clearModuleResults()));
    connect(e, SIGNAL(progress(int)),this,SIGNAL(progressed(int)));
    connect(e, SIGNAL(progressStarts(const QString&)),this
        , SIGNAL(progressStarts(const QString&)));
    connect(e, SIGNAL(progressEnds()),this,SIGNAL(progressEnds()));
    connect(e, SIGNAL(resultFound(const SearchResult*)), this
        , SLOT(addResult(const SearchResult*)));
    connect(e, SIGNAL(hasError(const QString&)), this
        , SIGNAL(errorInModule(const QString&)));
}

void KBabelDictBox::saveSettings(KConfigBase *config)
{
   KConfigGroupSaver cs(config,"KBabelDict");

   config->writeEntry("ResultSplitter",resultSplitter->sizes());
   
   SearchEngine *e;

   e = moduleList.at(active);
   if(e)
   {
      config->writeEntry("ActiveModule",e->id());
   }

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      config->setGroup(e->id());
      e->saveSettings(config);
   }


}

void KBabelDictBox::saveSettings(const QString& moduleId, KConfigBase *config)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == moduleId)
      {
        e->saveSettings(config);
	break;
      }
   }
}

void KBabelDictBox::readSettings(KConfigBase *config)
{
   SearchEngine *e;
   KConfigGroupSaver cs(config,"KBabelDict");
   QValueList<int> sizes=config->readIntListEntry("ResultSplitter");
   if(!sizes.isEmpty())
       resultSplitter->setSizes(sizes);
   
   QString m = config->readEntry("ActiveModule");
   if(!m.isEmpty())
   {
      setActiveModule(m);
   }

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      config->setGroup(e->id());
      e->readSettings(config);
   }
}

void KBabelDictBox::readSettings(const QString& moduleId, KConfigBase *config)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == moduleId)
      {
        e->readSettings(config);
	break;
      }
   }
}

void KBabelDictBox::setAutoUpdateOptions(bool on)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      e->setAutoUpdateOptions(on);
   }
}


int KBabelDictBox::activeModule()
{
   return active;
}

/* 
 * public slot
 */
void KBabelDictBox::setActiveModule(int a)
{
   if( a == active)
      return;
      
   if( a < (int)moduleList.count())
   {
      SearchEngine *engine = moduleList.at(active);
   
      if(!engine)
      {
         kdDebug(KBABELDICT) << "no module available" << endl;
      }
      else if(engine->isSearching())
      {
         engine->stopSearch();
         engine->clearResults();
      }
      
      engine = moduleList.at(a);
      if(engine)
      {
          active =  a;
          emit activeModuleChanged(active);   
          emit activeModuleChanged(engine->isEditable());   
      }

      
   }
}

void KBabelDictBox::setActiveModule(QString id)
{   
   SearchEngine *e;   

   int i=0;
   
   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == id)
      {
         setActiveModule(i);
         break;
      }

      i++;
   }
}

/* 
 * public slot
 */
void KBabelDictBox::startSearch(const QString text)
{
   clear();
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      if(engine->isSearching())
      {
         engine->stopSearch();
         connect(this, SIGNAL(searchStopped()), this
                     , SLOT(startDelayedSearch()));

         searchText=text;
      }
      else engine->startSearch(text);
   }
}

/* 
 * public slot
 */
void KBabelDictBox::startTranslationSearch(const QString text)
{
   clear();
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      if(engine->isSearching())
      {
         engine->stopSearch();
         connect(this, SIGNAL(searchStopped()), this
                     , SLOT(startDelayedTranslationSearch()));

         searchText=text;
      }
      else engine->startSearchInTranslation(text);
   }
}

void KBabelDictBox::startDelayedSearch(const QString text)
{
   clear();
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      searchText=text;
      
      if(engine->isSearching())
      {
         engine->stopSearch();
   
         connect(this, SIGNAL(searchStopped()), this
            , SLOT(startDelayedSearch()));

      }
      else
      {
         QTimer::singleShot(5,this,SLOT(startDelayedSearch()));
      }
   }
}

void KBabelDictBox::startDelayedTranslationSearch(const QString text)
{
   clear();
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      searchText=text;
      
      if(engine->isSearching())
      {
         engine->stopSearch();
   
         connect(this, SIGNAL(searchStopped()), this
            , SLOT(startDelayedTranslationSearch()));

      }
      else
      {
         QTimer::singleShot(5,this,SLOT(startDelayedTranslationSearch()));
      }
   }
}

QString KBabelDictBox::translate(const QString text)
{
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
      return QString::null;
   }
   else 
   {
      if(engine->isSearching())
      {
         engine->stopSearch();
      }

      return engine->translate(text);
   }
}

QString KBabelDictBox::fuzzyTranslation(const QString text, int &score)
{
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
      return QString::null;
   }
   else 
   {
      if(engine->isSearching())
      {
         engine->stopSearch();
      }

      return engine->fuzzyTranslation(text, score);
   }
}

QString KBabelDictBox::searchTranslation(const QString text, int &score)
{
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
      return QString::null;
   }
   else 
   {
      if(engine->isSearching())
      {
         engine->stopSearch();
      }

      return engine->searchTranslation(text, score);
   }
}

void KBabelDictBox::startDelayedSearch()
{
   clear();

   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      disconnect(this, SIGNAL(searchStopped()), this
                     , SLOT(startDelayedSearch()));


      engine->startSearch(searchText);
   }
}

void KBabelDictBox::startDelayedTranslationSearch()
{
   clear();

   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      disconnect(this, SIGNAL(searchStopped()), this
                     , SLOT(startDelayedTranslationSearch()));


      engine->startSearchInTranslation(searchText);
   }
}

/* 
 * public slot
 */
void KBabelDictBox::stopSearch()
{
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
   }
   else 
   {
      engine->stopSearch();
   }

}

bool KBabelDictBox::isSearching() 
{
   SearchEngine *engine = moduleList.at(active);
   
   if(!engine)
   {
      kdDebug(KBABELDICT) << "no module available" << endl;
      return false;
   }

   return engine->isSearching();
}


QStringList KBabelDictBox::moduleNames()
{
   QStringList list;
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      list.append(e->name());
   }

   return list;
}

QStringList KBabelDictBox::modules()
{
   QStringList list;
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      list.append(e->id());
   }

   return list;
}

QPtrList<ModuleInfo> KBabelDictBox::moduleInfos()
{
   QPtrList<ModuleInfo> list;
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      ModuleInfo *info = new ModuleInfo;
      info->id=e->id();
      info->name=e->name();
      info->editable=e->isEditable();

      list.append(info);
   }

   return list;
}


QPtrList<PrefWidget> KBabelDictBox::modPrefWidgets(QWidget *parent)
{
   QPtrList<PrefWidget> list;
   list.setAutoDelete(false);
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      list.append(e->preferencesWidget(parent));
   }

   return list;

}


void KBabelDictBox::showResult(QListViewItem *item)
{  
   ResultListItem *resultItem = static_cast<ResultListItem*>(item);
   
   if(!item)
   {
      kdError(KBABELDICT) << "no item" << endl;
      if(rmbPopup)
      {
        rmbPopup->changeItem(editFileIndex, i18n("Edit File"));
        rmbPopup->setItemEnabled(editFileIndex,false);
      }
   }
   else
   {
      const SearchResult *result= resultItem->result();
      if(!result)
         return;

      resultListView->ensureItemVisible(item);

      currentResult = resultListView->itemIndex(item);
      currentInfo = 0;

      bool richText=resultItem->richText();
      if(richText)
      {
         // FIXME: what about plural forms?
         origView->setText(result->found.first());
         translationView->setText(result->translation);
      }
      else
      {
         // FIXME: what about plural forms?
         origView->setText(QStyleSheet::convertFromPlainText(result->found.first()));
         translationView->setText(
               QStyleSheet::convertFromPlainText(result->translation));
      }

      if(result->descriptions.count() > 0)
      {
         QPtrListIterator<TranslationInfo> it(result->descriptions);
         TranslationInfo *info=it.current();
         if(info)
         {
            if(info->lastChange.isValid())
            {
               dateLabel->setText(KGlobal::locale()->formatDate(
                    info->lastChange.date(),true));
            }
            else
            {
                dateLabel->setText("");
            }
            locationLabel->setText(info->location);
            translatorLabel->setText(info->translator); 
       
            if(rmbPopup)
            {
                if(!info->filePath.isEmpty())
                {
                    rmbPopup->changeItem(editFileIndex
                        ,i18n("Edit File %1").arg(info->location));
                    rmbPopup->setItemEnabled(editFileIndex,true);
                }
                else
                {
                    rmbPopup->changeItem(editFileIndex, i18n("Edit File"));
                    rmbPopup->setItemEnabled(editFileIndex,false);
                }
             }
         }
      }
      else
      {
         dateLabel->setText("");
         locationLabel->setText("");
         translatorLabel->setText("");

         rmbPopup->changeItem(editFileIndex, i18n("Edit File"));
         rmbPopup->setItemEnabled(editFileIndex,false);
      }

      moreButton->setEnabled((result->descriptions.count() > 1));

      currentLabel->setText(QString::number(currentResult+1));

      prevButton->setEnabled(currentResult > 0);
      nextButton->setEnabled(currentResult+1 < total);
   }

}

void KBabelDictBox::nextResult()
{
    QListViewItem *item=resultListView->selectedItem();
    if(item)
    {
        item=item->itemBelow();
        if(item)
        {
            resultListView->setSelected(item,true);
        }
    }
}


void KBabelDictBox::prevResult()
{    
    QListViewItem *item=resultListView->selectedItem();
    if(item)
    {
        item=item->itemAbove();
        if(item)
        {
            resultListView->setSelected(item,true);
        }
    }

}

void KBabelDictBox::addResult(const SearchResult* result)
{
   SearchEngine *e;

   e = moduleList.at(active);
   if(!e)
   {
       kdError(KBABELDICT) << "no module available" << endl;
       return;
   }

   QListViewItem *item=resultListView->selectedItem();
   int index=0;
   if(item)
   {
       index=resultListView->itemIndex(item);
   }
  
   new ResultListItem(resultListView, *result,e->usesRichTextResults());
   total++;
   totalResultsLabel->setText(QString::number(total));

   if(total==1)
   {
      resultListView->setSelected(resultListView->firstChild(),true);
   }
   else
   {
      nextButton->setEnabled((currentResult+1) < total);
      item=resultListView->itemAtIndex(index);
      if(item)
      {
          resultListView->setSelected(item,true);
      }
   }
}

void KBabelDictBox::clear()
{     
    dateLabel->setText("");
    locationLabel->setText("");
    translatorLabel->setText("");
    currentLabel->setText(QString::number(0));
    totalResultsLabel->setText(QString::number(0));
    origView->setText("");
    translationView->setText("");
    currentResult=0;
    currentInfo=0;
    total=0;

    resultListView->clear();
    clearModuleResults();

    moreButton->setEnabled(false);
    prevButton->setEnabled(false);
    nextButton->setEnabled(false);

    if(rmbPopup)
    {
        rmbPopup->changeItem(editFileIndex,i18n("Edit File"));
        rmbPopup->setItemEnabled(editFileIndex,false);
    }
}

void KBabelDictBox::nextInfo()
{   
   ResultListItem *item = static_cast<ResultListItem*>(resultListView->selectedItem());
   
   if(!item)
   {
      kdDebug(KBABELDICT) << "no item available" << endl;
   }
   else 
   {
      const SearchResult *result = item->result();
      if(!result)
         return;

      if(result->descriptions.count() > 0)
      {
         currentInfo++;
         TranslationInfo *info;
         if(currentInfo == (int)result->descriptions.count())
         {
            QPtrListIterator<TranslationInfo> it(result->descriptions);
            info = it.current();
            currentInfo = 0;
         }
         else
         {
            QPtrListIterator<TranslationInfo> it(result->descriptions);
            for(int i=0; i < currentInfo; i++)
            {
                ++it;
            }
            info=*it;
         }

         if(info->lastChange.isValid())
         {
             dateLabel->setText(KGlobal::locale()->formatDate(
                           info->lastChange.date(),true));
         }
         else
         {
             dateLabel->setText("");
         }

         locationLabel->setText(info->location);
         translatorLabel->setText(info->translator);
         
         if(rmbPopup)
         {
            if(!info->filePath.isEmpty())
            {
                rmbPopup->changeItem(editFileIndex
                        ,i18n("Edit File %1").arg(info->location));
                rmbPopup->setItemEnabled(editFileIndex,true);
            }
            else
            {
                rmbPopup->changeItem(editFileIndex, i18n("Edit File"));
                rmbPopup->setItemEnabled(editFileIndex,false);
            }
         }
      }
   }

}

void KBabelDictBox::showListOnly()
{
   int h=resultSplitter->height();
   QValueList<int> sizes;
   sizes.append(1);
   sizes.append(h-1);
   resultSplitter->setSizes(sizes);
}

void KBabelDictBox::showDetailsOnly()
{
   int h=resultSplitter->height();
   QValueList<int> sizes;
   sizes.append(h-1);
   sizes.append(h);
   resultSplitter->setSizes(sizes);
}

void KBabelDictBox::clearModuleResults()
{
    SearchEngine *engine = moduleList.at(active);
    if(engine)
        engine->clearResults();
}

void KBabelDictBox::about()
{
   KAboutApplication *aboutDlg = new KAboutApplication(this);

   SearchEngine *e;
   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      KAboutContainer *page = aboutDlg->addScrolledContainerPage(e->name());

      const KAboutData *aboutData = e->about();
      if(aboutData)
      {

         QString text = aboutData->programName() + " " +
                      aboutData->version() + "\n";
               
         text += "\n"+aboutData->shortDescription()+"\n";

         if(!aboutData->homepage().isEmpty())
         {
            text += "\n" + aboutData->homepage() + "\n";
         }
         if(!aboutData->otherText().isEmpty())
         {
            text += "\n" + aboutData->otherText() + "\n";
         }
         if(!aboutData->copyrightStatement().isEmpty())
         {
            text += "\n" + aboutData->copyrightStatement() + "\n";
         }
      
         if(aboutData->bugAddress() != "submit@bugs.kde.org")
         {
            text += "\n" + i18n("Send bugs to %1")
                  .arg(aboutData->bugAddress()) +"\n";
         }

         QLabel *label = new QLabel(text,0);
         page->addWidget(label);

         int authorCount = aboutData->authors().count();
         if(authorCount)
         {
            if(authorCount==1)
                  text=i18n("Author:");
            else
                  text=i18n("Authors:");

            label = new QLabel(text,0);
            page->addWidget(label);

            QValueList<KAboutPerson>::ConstIterator it;
            for(it = aboutData->authors().begin(); 
                        it != aboutData->authors().end(); ++it)
            {
               page->addPerson( (*it).name(), (*it).emailAddress(),
                        (*it).webAddress(), (*it).task() ); 
            }
         }
         int creditsCount = aboutData->credits().count();
         if(creditsCount)
         {
            text = i18n("Thanks to:");
            label = new QLabel(text,0);
            page->addWidget(label);

            QValueList<KAboutPerson>::ConstIterator it;
            for(it = aboutData->credits().begin(); 
                        it != aboutData->credits().end(); ++it)
            {
               page->addPerson( (*it).name(), (*it).emailAddress(),
                        (*it).webAddress(), (*it).task() ); 
            }

         }
      }
      else
      {
         QString text = i18n("No information available.");
         QLabel *label = new QLabel(text,0);
         page->addWidget(label);
      }
         
   }

   aboutDlg->setInitialSize(QSize(400,1));
   aboutDlg->exec();

   delete aboutDlg;
}

void KBabelDictBox::aboutActiveModule()
{
    SearchEngine *engine = moduleList.at(active);
    if(!engine)
        return;

    aboutModule(engine->id());
}

void KBabelDictBox::aboutModule(const QString& id)
{
   SearchEngine *e;   

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == id)
      {
          break;
      }
   }
 
   if(!e)
       return;

   AboutModuleDlg *aboutDlg = new AboutModuleDlg(e->about(),this);
   aboutDlg->exec();

   delete aboutDlg;

}


void KBabelDictBox::slotNextResult()
{
   nextResult();
}

void KBabelDictBox::slotPrevResult()
{
   prevResult();
}


void KBabelDictBox::slotStopSearch()
{
   stopSearch();
}

void KBabelDictBox::slotStartSearch(const QString& text)
{
   startSearch(text);
}

void KBabelDictBox::setEditedPackage(const QString& name)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      e->setEditedPackage(name);
   }
}


void KBabelDictBox::setEditedFile(const QString& path)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      e->setEditedFile(path);
   }
}

void KBabelDictBox::setLanguage(const QString& languageCode,
        const QString& languageName)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      e->setLanguage(languageCode,languageName);
      e->setLanguageCode(languageCode);
   }
}

void KBabelDictBox::copy()
{ 
   if(origView->hasSelectedText())
   {
      origView->copy();
   }
   else if(translationView->hasSelectedText())
   {
      translationView->copy();
   }
   else
   {
      QClipboard *cb = KApplication::clipboard();
      cb->setText(translation());
   }
}

QString KBabelDictBox::translation()
{
   QString trans;
   
   QListViewItem *item=resultListView->selectedItem();
   if(item)
   {
       ResultListItem *r=static_cast<ResultListItem*>(item);
       if(r)
       {
           const SearchResult *sr=r->result();
           if(sr)
           {
               if(r->richText())
                   trans=sr->plainTranslation;
               else
                   trans=sr->translation;
           }
       }
   }

   return trans;
}

void KBabelDictBox::setRMBMenu(QPopupMenu *popup)
{    
   if(popup)
   {
      if(popup->count() > 0)
          popup->insertSeparator();

      editFileIndex = popup->insertItem(i18n("Edit File")
              , this, SLOT(editFile()));
      popup->setItemEnabled(editFileIndex,false);
       
      KContextMenuManager::insert(origView,popup);
      KContextMenuManager::insert(origView->viewport(),popup);
      KContextMenuManager::insert(translationView,popup);
      KContextMenuManager::insert(translationView->viewport(),popup);
      KContextMenuManager::insert(this,popup);
      //KContextMenuManager::insert(resultListView->viewport(),popup);

      rmbPopup = popup;
   }
}

bool KBabelDictBox::hasSelectedText() const
{
   bool have=false;
   if(origView->hasSelectedText())
      have=true;
   else if(translationView->hasSelectedText())
      have=true;
   else if(resultListView->selectedItem() )
      have=true;
  
   return have;
}

QString KBabelDictBox::selectedText() const
{
   QString text;
   if(origView->hasSelectedText())
      text=origView->selectedText();
   else if(translationView->hasSelectedText())
      translationView->selectedText();

   return text;
}



void KBabelDictBox::configure(const QString& id, bool modal)
{

   QWidget* w = prefDialogs[id];
   if(w)
   {
       KWin::setActiveWindow(w->winId());
       return;
   }

   SearchEngine *e;
   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == id)
      {
         QString caption = i18n("Configure Dictionary %1").arg(e->name());
         KDialogBase *dialog = new KDialogBase(this,"prefDialog"
                   , modal, caption
                   , KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel|
                   KDialogBase::Default);

         QWhatsThis::add(dialog,"");

         PrefWidget *prefWidget = e->preferencesWidget(dialog);
         dialog->setMainWidget(prefWidget);
         
         connect(dialog, SIGNAL(okClicked()),prefWidget,SLOT(apply()));
         connect(dialog, SIGNAL(applyClicked()),prefWidget,SLOT(apply()));
         connect(dialog, SIGNAL(defaultClicked()),prefWidget,SLOT(standard()));
         connect(dialog, SIGNAL(cancelClicked()),prefWidget,SLOT(cancel()));
         
         connect(dialog, SIGNAL(finished()),this,SLOT(destroyConfigDialog()));
         
         prefDialogs.insert(id,dialog);

         if( modal ) dialog->exec();
	 else dialog->show();

         break;
      }
   }

}


void KBabelDictBox::destroyConfigDialog()
{
    const QObject *obj = sender();
    if(obj && obj->inherits("KDialogBase"))
    {
       KDialogBase *dialog = (KDialogBase*)obj;
       if(dialog)
       {
           dialog->delayedDestruct();

           QDictIterator<QWidget> it(prefDialogs);
           while(it.current() != dialog)
           {
               ++it;
           }

           prefDialogs.remove(it.currentKey());
       }
    }
}

void KBabelDictBox::edit(const QString& id)
{
   SearchEngine *e;   

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      if(e->id() == id)
      {
         if(e->isEditable())
             e->edit();
         
         break;
      }
   }
}

void KBabelDictBox::edit()
{
    SearchEngine *engine = moduleList.at(active);
    if(!engine)
        return;

    if(engine->isEditable())
        engine->edit();
}

void KBabelDictBox::setTextChanged(const QStringList& orig, 
        const QString& translation, uint pluralForm, const QString& description)
{
   SearchEngine *e;

   for(e = moduleList.first(); e != 0; e = moduleList.next())
   {
      e->stringChanged(orig,translation,pluralForm,description);
   }
}


void KBabelDictBox::wheelEvent(QWheelEvent *we)
{
    if(we->delta() > 0)
    {
        prevResult();
    }
    else
    {
        nextResult();
    }

    we->accept();
}

bool KBabelDictBox::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Wheel)
    {
        QWheelEvent *we = static_cast<QWheelEvent*>(e);
        if(we)
        {
            wheelEvent(we);
            return true;
        }
    }
    else if(e->type() == QEvent::Resize && o == resultListView)
    {
        if(resultListView->height() < 2)
        {
           detailButton->setEnabled(false); 
           listButton->setEnabled(true); 
        }
        else if(resultListView->height() > resultSplitter->height()-10)
        {
           detailButton->setEnabled(true); 
           listButton->setEnabled(false); 
        }
        else
        {
           detailButton->setEnabled(true); 
           listButton->setEnabled(true); 
        }
    }

    return false;
}

void KBabelDictBox::editFile()
{
    ResultListItem *item = static_cast<ResultListItem*>(resultListView->currentItem());
   
   if(!item)
   {
      kdDebug(KBABELDICT) << "no item available" << endl;
   }
   else 
   {
      const SearchResult *result = item->result();
      if(!result)
         return;

      if(!result->descriptions.isEmpty())
      {
         TranslationInfo *info;
         QPtrListIterator<TranslationInfo> it(result->descriptions);
         for(int i=0; i < currentInfo; i++)
         {
             ++it;
         }
         info=*it;

         if(!info->filePath.isEmpty())
         {
             QString url = info->filePath;
             QString msgid;
             
             if( item->richText() )
             {
                msgid = result->plainFound;
             }
             else
             {
                 msgid = result->found.first();
             }

             DCOPClient *dcop = kapp->dcopClient();

             QCStringList list = dcop->registeredApplications();
             int index = list.findIndex("kbabel");
             if(index < 0)
//             if(!dcop->isApplicationRegistered("kbabel"));
             {
                 kdDebug(KBABELDICT) << "kbabel is not registered" << endl;
                 
                 QString error;
                 QStringList argList;
                 argList.append("--nosplash");
                 argList.append("--gotomsgid");
                 argList.append(msgid.local8Bit());
                 argList.append(url.local8Bit());
                 kapp->kdeinitExec("kbabel",argList,&error);
                 if(!error.isNull())
                 {
                     KMessageBox::sorry(this
                             ,i18n("There was an error starting KBabel:\n%1")
                                    .arg(error));
                     return;
                 }
             }
             else
             {             
                 QByteArray data;
                 QDataStream arg(data, IO_WriteOnly);
                 arg << url.utf8() << msgid.utf8();
                 if (!dcop->send("kbabel", "KBabelIFace"
                         , "gotoFileEntry(QCString,QCString)",data)) 
                 {
                     KMessageBox::sorry(this
                             ,i18n("There was an error using DCOP."));
                 }
             }
          }
       }
   }
}

void KBabelDictBox::showContextMenu(KListView *,QListViewItem *,const QPoint& p)
{
    if(rmbPopup)
    {
        rmbPopup->exec(p);
    }
}


bool KBabelDictBox::messagesForPackage(const QString& package
           , QValueList<DiffEntry>& resultList, QString& error)
{
   setActiveModule("dbsearchengine");
   SearchEngine *engine = moduleList.at(active);

   if(!engine)
   {
       KMessageBox::sorry(this
          ,i18n("The \"Translation Database\" module\n"
              "appears not to be installed on your system."));
       return false;
   }

   QValueList<SearchResult> rList;

   SearchFilter* filter = new SearchFilter();
   filter->setLocation(package);
   
   bool success = engine->messagesForFilter(filter,rList,error);

   if(success)
   {
       QValueList<SearchResult>::Iterator it;
       for(it=rList.begin(); it != rList.end(); ++it)
       {
    	   // FIXME: what about plural forms?
           DiffEntry e;
           e.msgid = (*it).found.first();
           e.msgstr = (*it).translation;
           resultList.append(e);
       }
   }
   
   return success;
}

#include "kbabeldictbox.moc"

