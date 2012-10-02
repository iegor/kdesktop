/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
                2001-2003 by Stanislav Visnovsky
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

#include "catmanresource.h"
#include "catmanlistitem.h"
#include "catalogmanagerview.h"
#include "catalog.h"

#include "resources.h"

#include <klocale.h>

#include <qfileinfo.h>
#include <qdir.h>
#include <qbitmap.h>
#include <qlabel.h>
#include <qpainter.h>

#include <cvshandler.h>
#include <svnhandler.h>

using namespace KBabel;

CatManListItem::CatManListItem(CatalogManagerView *view, QListViewItem* parent,QString fullPath,QString fullPotPath,QString package)
     : QListViewItem(parent)
{
   _view = view;
   init(fullPath,fullPotPath,package);
}

CatManListItem::CatManListItem(CatalogManagerView *view, QListView* parent,QString fullPath,QString fullPotPath)
     : QListViewItem(parent)
{
   _primary=QFileInfo(fullPath);
   _template=QFileInfo(fullPotPath);
   _package="/";
   _type=Dir;
   _marked=false;
   _view = view;

   _hasPo=false;
   _hasPot=false;
   _hasErrors=false;

   _primary.setCaching(false);
   _template.setCaching(false);

   setText(COL_NAME,i18n("Message Catalogs"));
   setPixmap(COL_NAME,ICON_FOLDER_CLOSED_OK);
}


void CatManListItem::init(const QString& fullPath, const QString& fullPotPath, const QString& package)
{
   _primary=QFileInfo(fullPath);
   _template=QFileInfo(fullPotPath);
   _package=package;
   _marked=false;

   _hasPo=false;
   _hasPot=false;
   _hasErrors=false;

   _primary.setCaching(false);
   _template.setCaching(false);

    // set
    _lastUpdated=QDate(1900,1,1);

    _wordList.clear();
    _wordListUpdated = false;

    update(parent()->isOpen(),false,true);

    if( !isDir() )  setPixmap(COL_MARKER,ICON_NOFLAG);
}

void CatManListItem::setMarked(bool on)
{
   if(on)
   {
      setPixmap(COL_MARKER,ICON_FLAG);
   }
   else
   {
      setPixmap(COL_MARKER,ICON_NOFLAG);
   }

   _marked=on;
}

bool CatManListItem::marked() const
{
    if( isFile() ) return _marked;
    else if( isDir() )
    {
	CatManListItem * myChild = (CatManListItem*)firstChild();
	while( myChild )
	{
    	    if(myChild->isFile() && myChild->marked() ) return true;
    	    else if(myChild->isDir() && myChild->marked() ) return true;
	    myChild = (CatManListItem*)myChild->nextSibling();
	}

    }
    return false;
}

void CatManListItem::setOpen(bool open)
{
  bool needWork = needsWork();
	QListViewItem::setOpen(open);

    if(open && _type==Dir)
    {
       QPixmap icon;
       icon = needWork ? ICON_FOLDER_OPEN_WORK : ICON_FOLDER_OPEN_OK;

       if(!_template.exists())
       {
          icon=paintExclamation(&icon);
       }

       setPixmap(COL_NAME,icon);

       CatManListItem * myChild = (CatManListItem*)firstChild();
       while( myChild )
       {
         myChild->checkUpdate();
         myChild = (CatManListItem*)myChild->nextSibling();
       }
    }
    else
    {
       QPixmap icon;

	   if(needsWork())
		  icon = ICON_FOLDER_CLOSED_WORK;
	   else
		  icon = ICON_FOLDER_CLOSED_OK;

       if(!_template.exists())
       {
          icon=paintExclamation(&icon);
       }

       setPixmap(COL_NAME,icon);
    }

}

QStringList CatManListItem::allChildrenList(bool onlyFiles) const
{
   QStringList childrenList;

   CatManListItem * myChild = (CatManListItem*)firstChild();
   while( myChild )
   {
      QString name=myChild->package();

      if(myChild->isFile())
      {
         childrenList.append(name);
      }
      else if(myChild->isDir())
      {
         if(!onlyFiles)
            childrenList.append(name);

         childrenList+=myChild->allChildrenList(onlyFiles);
      }

      myChild = (CatManListItem*)myChild->nextSibling();
   }

   return childrenList;
}


QStringList CatManListItem::allChildrenFileList(bool onlyFiles, bool emptyDirs, bool onlyModified) const
{
   QStringList childrenList;

   CatManListItem * myChild = (CatManListItem*)firstChild();
   while( myChild )
   {
      if(myChild->isFile() && myChild->hasPo() &&
       !(!myChild->isModified() && onlyModified))
      {
         childrenList.append(myChild->poFile());
      }
      else if(myChild->isDir())
      {
         if(!onlyFiles && (emptyDirs || myChild->_primary.exists() ))
	 {
            childrenList.append(myChild->poFile());
	 }

         childrenList+=myChild->allChildrenFileList(onlyFiles,false,onlyModified);
      }

      myChild = (CatManListItem*)myChild->nextSibling();
   }

   return childrenList;
}


QStringList CatManListItem::contentsList(bool onlyFiles) const
{
   QStringList childList;

   CatManListItem * myChild = (CatManListItem*)firstChild();
   while( myChild )
   {
      QString name=myChild->package();

      if(onlyFiles)
      {
         if(myChild->isFile())
         {
            childList.append(name);
         }
      }
      else
      {
         childList.append(name);
      }

      myChild = (CatManListItem*)myChild->nextSibling();
   }

   return childList;
}


void CatManListItem::forceUpdate()
{
    update(true,true,false);
}

void CatManListItem::checkUpdate(bool noParents)
{
	// if a file has disappeared or is new
	if(_hasPo != hasPo() || _hasPot != hasPot())
	{
		update(true,false,noParents);
	}
	else if(!isFile())
	{
		update(true,false,noParents);
	}
	else if(_hasPo && _lastUpdated < _primary.lastModified())
	{
		update(true,false,noParents);
	}
	else if(_hasPot && _lastUpdated < _template.lastModified())
	{
		update(true,false,noParents);
	}
}

QString CatManListItem::key(int col, bool) const
{
   // show directories first
   QString key=text(col);

   if(col==COL_NAME)
   {
      if(_type==Dir)
      {
         key="a"+key;
      }
      else
      {
         key="b"+key;
      }
   }
   // fuzzy, untranslated, total
   else if(col==COL_FUZZY || col ==COL_TOTAL || col==COL_UNTRANS)
   {
      key=key.rightJustify(10,'0');
   }
   // marked po's
   else if(col==COL_MARKER)
   {
     if(_marked)
     {
       key="1";
     }
     else
     {
       key="0";
     }
   }

   return key;
}

void CatManListItem::update(bool showPoInfo,bool includeChildren
				, bool noParents)
{
	if(  _view->isStopped() ) return; // if parent view is stopped, we should stop as well

	bool updateWordList = _view->settings().indexWords;

	// flag, if something has changed and parent has to be updated
	bool updateParent=false;

	// update flags for files...
	const bool hadPo=_hasPo;
	_hasPo = hasPo();
	const bool hadPot = _hasPot;
	_hasPot = hasPot();

	// and check if something changed
	if(hadPo != _hasPo || hadPot != _hasPot)
		updateParent=true;


    if(_package!="/") // don't update root item
    {
       if(_primary.exists())
       {
          if(_primary.isDir())
          {
             QDir dir=_primary.dir();
             setText(COL_NAME,dir.dirName());

	     // count the childen numbers
	     int fuzzy = 0;
	     int untrans = 0;
	     int total = 0;

	     CatManListItem* ch = static_cast<CatManListItem*>(firstChild ());

	     while (ch)
	     {
	        fuzzy += ch->fuzzy ();
		untrans += ch->untranslated ();
		total += ch->total ();
		ch = static_cast<CatManListItem*>(ch->nextSibling());
	     }

	     setText(COL_FUZZY,QString::number(fuzzy));
             setText(COL_UNTRANS,QString::number(untrans));
             setText(COL_TOTAL,QString::number(total));

             //setSelectable(false);
             _type=Dir;

       bool needWork = needsWork();
			 QPixmap icon;
			 if(!isOpen())
			 {
			 	if( needWork )
					icon = ICON_FOLDER_CLOSED_WORK;
				 else
				    icon = ICON_FOLDER_CLOSED_OK;
			 }
			 else
			 {
         icon = needWork ? ICON_FOLDER_OPEN_WORK : ICON_FOLDER_OPEN_OK;
			 }

             // check if the same directory exists also in the
			 // template directory
             if(_template.isDir())
             {
                setPixmap( COL_NAME, icon );
             }
             else
             {
                QPixmap folder = icon;
                icon=paintExclamation(&folder);

                setPixmap(COL_NAME,folder);
             }
          }
          else // primary is file
          {
             _type=File;
             QString name=_primary.fileName();
             setText(COL_NAME,name.left(name.length()-3));

             if(showPoInfo)
             {
                _lastUpdated=QDateTime::currentDateTime();

			    bool neededWork=needsWork();
				bool needWork=false;

                PoInfo poInfo;
                QPixmap icon = ICON_UPDATING;
		setPixmap(COL_NAME,icon);
                if ( PoInfo::info( _primary.absFilePath(), poInfo, _wordList, updateWordList, true, true ) == OK )
                {
		    if( _view->isStopped() ) return;
		    if( updateWordList) _wordListUpdated = true;

		    _hasErrors=false;

                    const CVSHandler* cvsHandler = _view->cvsHandler();
                    const SVNHandler* svnHandler = _view->svnHandler();
                    
                    const CVSHandler::FileStatus cvsFileStatus = cvsHandler->fstatus( poFile() );
                    const SVNHandler::FileStatus svnFileStatus = svnHandler->fstatus( poFile() );

                    _isModified = cvsHandler->isConsideredModified( cvsFileStatus )
                        || svnHandler->isConsideredModified( svnFileStatus );

                    QString versionControl;
                    if ( cvsFileStatus != CVSHandler::NO_REPOSITORY )
                        versionControl = cvsHandler->fileStatus( cvsFileStatus );
                    else if  ( svnFileStatus != SVNHandler::NO_REPOSITORY )
                        versionControl = svnHandler->fileStatus( svnFileStatus );
                    else
                        versionControl = i18n("No version control");

		    setText(COL_FUZZY,QString::number(poInfo.fuzzy));
                    setText(COL_UNTRANS,QString::number(poInfo.untranslated));
                    setText(COL_TOTAL,QString::number(poInfo.total));
		    setText( COL_CVS_OR_SVN, versionControl );
                    setText(COL_REVISION,poInfo.revision);
                    setText(COL_TRANSLATOR,poInfo.lastTranslator);

                    if(needsWork())
                    {
                        icon=ICON_NEEDWORK;
			needWork = true;
                    }
                    else
                    {
                        icon = ICON_OK;
			needWork=false;
                    }
                }
                else
                {
		    kdDebug(KBABEL_CATMAN) << "This file is broken" << endl;
		    if( _view->isStopped() ) return;
		    _hasErrors=true;
                    icon = ICON_BROKEN;
		    needWork=true;
                }

                if(!_template.exists())
                {
                   icon=paintExclamation(&icon);
                }

                setPixmap(COL_NAME,icon);

		updateParent=true;
             }
          }
       }
	   // only the template exists
       else if(_template.exists())
       {
          if(_template.isDir())
          {
             QDir dir=_template.dir();
             setText(COL_NAME,dir.dirName());
             //setSelectable(false);
             _type=Dir;

	     // count the childen numbers
	     int total = 0;

	     CatManListItem* ch = static_cast<CatManListItem*>(firstChild ());

	     while (ch)
	     {
		total += ch->total ();
		ch = static_cast<CatManListItem*>(ch->nextSibling());
	     }

             setText(COL_TOTAL,QString::number(total));


	     QPixmap icon;
	     if(!isOpen())
	     {
           icon = ICON_FOLDER_CLOSED_WORK;
	     }
	     else
	     {
         icon = needsWork() ? ICON_FOLDER_OPEN_WORK : ICON_FOLDER_OPEN_OK;
	     }

             setPixmap(COL_NAME, icon );
          }
	  // item is file
          else
          {
             _type=File;
             QString name=_primary.fileName();
             setText(COL_NAME,name.left(name.length()-3));

             if(showPoInfo)
             {
                _lastUpdated=QDateTime::currentDateTime();

		// clean previous state information
		setText(COL_FUZZY,QString::null);
                setText(COL_UNTRANS,QString::null);
                setText(COL_TOTAL,QString::null);
		setText(COL_CVS_OR_SVN, QString::null);
                setText(COL_REVISION, QString::null);
                setText(COL_TRANSLATOR, QString::null);

		setPixmap(COL_NAME,ICON_UPDATING);

                PoInfo poInfo;
                if ( PoInfo::info( _template.absFilePath(), poInfo, _wordList, false, true, true ) == OK )
                {
        	    if( _view->isStopped() ) return;
		    setText(COL_TOTAL,QString::number(poInfo.total));
                }
        	if( _view->isStopped() ) return;
             }
	     setPixmap(COL_NAME,ICON_MISSING);

	     updateParent = true;
          }
       }
       else
       {
          kdWarning(KBABEL_CATMAN) << "whether po nor pot exists: " << _package << endl;
       }
    }

    _view->fileInfoRead( package() );

    if( _view->isStopped() ) return;

    if(updateParent && !noParents)
	{
		updateParents();
	}

    if( _view->isStopped() ) return;

    if(includeChildren)
    {
       CatManListItem *myChild = (CatManListItem*)firstChild();
       while( myChild )
       {
          myChild->update(showPoInfo,includeChildren);
          myChild = (CatManListItem*)myChild->nextSibling();
       }
    }

    // HACK to get the signal emitted
    if (isSelected( )) {
      listView( )->setSelected(this, false);
      listView( )->setSelected(this, true);
    }
}

// we know that this item was saved and PoInfo contains new information
// about this item, the item is file
// however, is can be saved template or translation!!! - only translation is handled???
void CatManListItem::updateAfterSave( PoInfo &poInfo )
{
    // flag, if something has changed and parent has to be updated
    bool updateParent=false;

    // update flags for files...
    const bool hadPo=_hasPo;
    _hasPo = hasPo();
    const bool hadPot = _hasPot;
    _hasPot = hasPot();

    // and check if something changed
    if(hadPo != _hasPo || hadPot != _hasPot)
    	updateParent=true;

    if(_primary.exists())
    {
	// primary is existent file

        _type=File;
        QString name=_primary.fileName();
        setText(COL_NAME,name.left(name.length()-3));

        _lastUpdated=QDateTime::currentDateTime();

	bool neededWork=needsWork();
	bool needWork=false;

        QPixmap icon;
	_hasErrors=false;

        const CVSHandler::FileStatus cvsFileStatus = _view->cvsHandler()->fstatus(poFile());
        const SVNHandler::FileStatus svnFileStatus = _view->svnHandler()->fstatus(poFile());

        QString versionControl;
        if ( cvsFileStatus != CVSHandler::NO_REPOSITORY )
            versionControl = _view->cvsHandler()->fileStatus( cvsFileStatus );
        else if  ( svnFileStatus != SVNHandler::NO_REPOSITORY )
            versionControl = _view->svnHandler()->fileStatus( svnFileStatus );
        else
            versionControl = i18n("No version control");
 
	setText(COL_FUZZY,QString::number(poInfo.fuzzy));
        setText(COL_UNTRANS,QString::number(poInfo.untranslated));
        setText(COL_TOTAL,QString::number(poInfo.total));
	setText( COL_CVS_OR_SVN, versionControl );
        setText(COL_REVISION,poInfo.revision);
        setText(COL_TRANSLATOR,poInfo.lastTranslator);

        if(needsWork())
        {
    	    icon=ICON_NEEDWORK;
	    needWork = true;
        }
        else
        {
    	    icon = ICON_OK;
	    needWork=false;
        }

        if(!_template.exists())
        {
    	    icon=paintExclamation(&icon);
        }

        setPixmap(COL_NAME,icon);

	// if the status changed, update the parent item
	if(needWork != neededWork)
	{
	    updateParent=true;
	}
    }

    if(updateParent)
    {
	updateParents();
    }
}


void CatManListItem::updateParents()
{
	CatManListItem *item = (CatManListItem*)parent();
	while( item && !_view->isStopped())
	{
		item->update(false,false);
		item = (CatManListItem*)item->parent();
	}
}

bool CatManListItem::hasPo() const
{
   return _primary.exists();
}

bool CatManListItem::hasPot() const
{
   return _template.exists();
}

bool CatManListItem::isModified() const
{
   return _isModified;
}

int CatManListItem::fuzzy() const
{
   bool success;
   int number=text(COL_FUZZY).toInt(&success);
   if(!success)
      number=0;

   return number;
}

int CatManListItem::untranslated() const
{
   bool success;
   int number;
   if( !hasPo() )
   {
      number=total();
   }
   else
   {
      number=text(COL_UNTRANS).toInt(&success);
      if(!success)
         number=0;
   }

   return number;
}

int CatManListItem::total() const
{
   bool success;
   int number=text(COL_TOTAL).toInt(&success);
   if(!success)
      number=0;

   return number;
}

bool CatManListItem::needsWork() const
{
	bool flag=false;

	if(isFile())
	{
		if(!hasPo() || fuzzy() > 0 || untranslated() > 0 || _hasErrors)
			flag=true;
	}
	else
	{
		CatManListItem *myChild = (CatManListItem*)firstChild();
		while( myChild )
		{
			if( myChild->needsWork() )
			{
				flag=true;
				myChild=0;
			}
			else
			{
				myChild = (CatManListItem*)myChild->nextSibling();
			}
		}
	}

	return flag;
}

bool CatManListItem::isDir() const
{
   return type()==Dir;
}

bool CatManListItem::isFile() const
{
   return type()==File;
}

QString CatManListItem::poFile() const
{
   return _primary.absFilePath();
}

QString CatManListItem::potFile() const
{
   return _template.absFilePath();
}

QString CatManListItem::package(bool rootSlash) const
{
   if(rootSlash)
      return _package;
   else
   {
      return _package.right(_package.length()-1);
   }
}

QString CatManListItem::packageDir( ) const
{
  return ( _type == Dir ? _package : QString::null );
}

QString CatManListItem::name() const
{
   int index = _package.findRev("/");
   return _package.right(_package.length()-index-1);
}

QPixmap CatManListItem::paintExclamation(QPixmap* pixmap)
{
   if(!pixmap || pixmap->isNull())
      return QPixmap(0,0);

   if(_package=="/" && _template.filePath().isEmpty())
	  return *pixmap;

   if(isDir() && _package == _template.filePath())
	  return *pixmap;

   if(isFile() && _package+".pot" == _template.filePath())
	  return *pixmap;

   int width=pixmap->width();
   int height=pixmap->height();

   int diameter=QMIN(width,height);

   QBitmap mask=pixmap->createHeuristicMask();

   QPainter mp(&mask);
   mp.setPen(QPen(Qt::color1,1));
   mp.drawEllipse(width-diameter,height-diameter,diameter,diameter);

   QPixmap result(width,height);

   QPainter p(&result);
   p.drawPixmap(0,0,*pixmap);
   p.setPen( QPen(red,1) );
   p.drawEllipse(width-diameter,height-diameter,diameter,diameter);

   result.setMask(mask);

   return result;
}

QListViewItem *CatManListItem::previousSibling()
{
    QListViewItem * i = parent();
    if( !i ) return i;
    i = i->firstChild();
    if( !i ) return i;
    if( i == this ) return 0;
    while( i->nextSibling()!=this ) i = i->nextSibling();
    return i;
}

QListViewItem *CatManListItem::lastChild()
{
    QListViewItem * i = firstChild();
    if( !i ) return i;
    while( i->nextSibling() ) i = i->nextSibling();
    return i;
}

void CatManListItem::checkErrors(KDataTool* tool, QObject* progressSignalHandler, bool ignoreFuzzy, bool markAsFuzzy)
{
    bool hasError=false;
    _errors.clear();
    Catalog* cat = new Catalog();

    QObject::connect( cat, SIGNAL( signalProgress(int) ), progressSignalHandler, SIGNAL( setValidationProgressBar(int)));
    QObject::connect( cat, SIGNAL( signalResetProgressBar(QString, int) ), progressSignalHandler, SLOT( setupFileProgressBar(QString, int)));

    if( cat->openURL(KURL( poFile() )) == OK )
    {
	kdDebug(KBABEL_CATMAN) << "File opened succesfully" << endl;
	if( !cat->checkUsingTool(tool,true) )
	{
	    hasError = true;
	} else forceUpdate(); // no error, find out the new state
    } else {
    	kdDebug(KBABEL_CATMAN) << "File not opened !!!!!" << endl;
	hasError=true;
    }

    if( hasError )
    {
	QString errortext;
	_hasErrors = true;

	DocPosition dummy;
	IgnoreItem i;
	i.fileURL = poFile();

	if( cat->hasError(0,dummy) && (!ignoreFuzzy || !cat->isFuzzy(0)))
	{
	    i.msgid = cat->msgid(0);
	    i.msgstr = cat->msgstr(0);
	    i.index = 0;
	    _errors.append( i );

	    if( markAsFuzzy ) cat->setFuzzy(0, true);
	}

	int index=0;
	do
	{
	    index=cat->nextError(index,dummy);
	    if( index != -1 && (!ignoreFuzzy || !cat->isFuzzy(index) ) )
	    {
		i.msgid = cat->msgid(index);
		i.msgstr = cat->msgstr(index);
		i.index = index;
		_errors.append( i );
		if( markAsFuzzy ) cat->setFuzzy(index, true);
	    }
	} while(index>=0);

	// change icon only if there were non-ignored errors
    	if( !_errors.isEmpty() )
	{
	    setPixmap(COL_NAME, ICON_ERROR);
	}

	// if we changed fuzzy flags, save the result
	if( cat->isModified() ) cat->saveFile();
    }

    delete cat;
}
