/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2003 by Stanislav Visnovsky
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
#include "validateprogress.h"
#include "validateprogresswidget.h"
#include "catmanlistitem.h"
#include "catmanresource.h"

#include <kdebug.h>
#include <kdatatool.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprogress.h>
#include <ksqueezedtextlabel.h>

#include <qlistbox.h>
#include <qtimer.h>

#define ID_ERROR_OPEN   1
#define ID_ERROR_IGNORE 2

// version identification for validation ignores
#define IGNOREFILE_VERSION 0x00

ValidateProgressDialog::ValidateProgressDialog(const QString& ignoreURL, QWidget *parent,const char *name)
		: KDialogBase(parent,name,true,i18n("Caption of dialog","Validation")
						, Close, Close)
		, _ignoreURL(ignoreURL), _tool(0), _stopped(false)
		, _ignoreFuzzy(false), _setAsFuzzy(false)

{
    _mainWidget = new ValidateProgressWidget(this);
    setMainWidget(_mainWidget);
    setInitialSize( QSize(400, 300) );

    _errors.clear();
    _ignores.clear();

    readIgnores();

    _errorMenu = new KPopupMenu(this);
    _errorMenu->insertItem(i18n("&Open"),ID_ERROR_OPEN);
    _errorMenu->insertItem(i18n("&Ignore"),ID_ERROR_IGNORE);

    connect( this, SIGNAL(closeClicked()), this, SLOT(stop()));
    connect( _mainWidget->_errorList, SIGNAL( doubleClicked(QListBoxItem *)),
	this, SLOT( errorItemDoubleClicked(QListBoxItem *)));

    connect( _mainWidget->_errorList, SIGNAL( contextMenuRequested(QListBoxItem *, const QPoint &)),
	this, SLOT( showContextMenu(QListBoxItem *, const QPoint &)));
}

ValidateProgressDialog::~ValidateProgressDialog()
{
    writeIgnores();
}

void ValidateProgressDialog::validate( const KDataToolInfo &tool, const QPtrList<CatManListItem> files )
{
    if( files.isEmpty() ) return;

    _errors.clear();

    KDataTool* t = tool.createTool();

    if( !t )
    {
	KMessageBox::error( this, i18n("Cannot instantiate a validation tool.\n"
	"Please check your installation."), i18n("Validation Tool Error") );
	return;
    }

    _tool = t;
    _toolID = tool.service()->library();
    _files = files;

    _mainWidget->_errorList->clear();
    _mainWidget->_currentTool->setText(*(tool.userCommands().at(0)));
    _mainWidget->_overallProgress->setTotalSteps(files.count());
    _mainWidget->_overallProgress->setValue(0);

    _stopped = false;

    QTimer::singleShot( 0, this, SLOT(validate_internal()) );

    exec();

    _stopped = true;
}

void ValidateProgressDialog::validate_internal()
{
    uint checked=0;
    uint errors=0;
    uint ignorederrors=0;

    for( CatManListItem* it=_files.first() ; it && !_stopped ; it = _files.next() )
    {
	_mainWidget->_currentFile->setText( it->poFile() );

	checked++;

	_mainWidget->_currentFileProgress->setTotalSteps(100);
	_mainWidget->_currentFileProgress->setValue(0);

	it->checkErrors(_tool,_mainWidget, _ignoreFuzzy, _setAsFuzzy);

	bool noHeader = true;

	if( it->hasErrors() )
	{
	    QValueList<IgnoreItem> err = it->errors();

	    for( QValueList<IgnoreItem>::Iterator errit = err.begin();  errit!=err.end() ; ++errit )
	    {
		IgnoreItem item = (*errit);

		QValueList<IgnoreItem>::Iterator ig;
		for( ig = _ignores.begin() ; ig != _ignores.end() ; ++ig )
		{
		    if( (*ig).validationTool == _toolID &&
		      (*ig).msgid == item.msgid &&
		      (*ig).msgstr == item.msgstr &&
		      (*ig).fileURL == item.fileURL ) break;
		}

		if( ig != _ignores.end() )
		{
		    ++ignorederrors;
		    continue;
		} ++errors;

		if( noHeader )
		{
		    _mainWidget->_errorList->insertItem( ICON_ERROR, it->package() );
		    _errors.insert( it->package(), err.first() );
		    noHeader = false;
		}

		QString errortext=QString::number(item.index+1)+": " + item.msgid.first().left(50);
	        errortext.replace("\n"," ");
		if( item.msgid.first().length() > 50 ) errortext+="...";
		_mainWidget->_errorList->insertItem( errortext);

		_errors.insert( errortext, item );
	    }
	}

	_mainWidget->_currentFileProgress->setValue(100);

	_mainWidget->_overallProgress->advance(1);
    }

    if( !_stopped )
    {
	KMessageBox::information(this, i18n("Validation done.\n"
	"\n"
	"Checked files: %1\n"
	"Number of errors: %2\n"
	"Number of ignored errors: %3").arg(checked).arg(errors).arg(ignorederrors),i18n("Validation Done"));
    }

    delete _tool;
    _tool = 0;
    _files.clear();
}

void ValidateProgressDialog::stop()
{
    _stopped = true;
}

void ValidateProgressDialog::errorItemDoubleClicked(QListBoxItem * item)
{
    QString it = item->text();

    bool ok =false;
    int offset = it.find(":");

    int num;
    if( offset < -1 ) num = 0;
    else
    {
	num = it.left(offset).toInt(&ok);
	if( !ok ) num = 0;
    }

    QListBoxItem* package=item;

    while( package && !package->text().startsWith("/") ) package=package->prev();

    if( !package )
    {
	kdWarning() << "Unable to find the package for the error" << endl;
	return;
    }

    emit errorDoubleClicked(package->text(), num-1 );
}

void ValidateProgressDialog::showContextMenu(QListBoxItem * item, const QPoint & pos)
{
    // disable ignore for whole package
    _errorMenu->setItemEnabled(ID_ERROR_IGNORE, item->pixmap()==0 );
    int result = _errorMenu->exec(pos);
    switch( result )
    {
	case ID_ERROR_OPEN:
	    errorItemDoubleClicked( item );
	    break;
	case ID_ERROR_IGNORE:
	    IgnoreItem it = _errors.find(item->text()).data();

	    // if there is no pixmap, it's the whole file
	    if( !item->pixmap() )
	    {
		it.validationTool = _toolID;
		_ignores.append( it );
	    }
	    break;
    }
}

void ValidateProgressDialog::readIgnores()
{
    IgnoreItem item;

    QFile ignoreFile( _ignoreURL );
    if( ignoreFile.open( IO_ReadOnly ) ) {
        QDataStream s( &ignoreFile );
	QString url;
	int version;

	s >> version;
	if( version == IGNOREFILE_VERSION ) // Only read if correct versi
	{
	    _ignores.clear();

	    while( !s.atEnd() ) {
		s >> item;
                _ignores.append(item);
            }
	}
	ignoreFile.close();
    }
}

void ValidateProgressDialog::writeIgnores()
{
    QFile ignoreFile( _ignoreURL );
    if( ignoreFile.open( IO_WriteOnly ) ) {
        QDataStream s( &ignoreFile );
	QString url;
	int version = IGNOREFILE_VERSION;

	s << version;

	for( QValueList<IgnoreItem>::Iterator it = _ignores.begin(); it!=_ignores.end(); ++it)
	{
	    s << (*it);
	}
	ignoreFile.close();
    }
}

QDataStream & operator<<( QDataStream & stream, const IgnoreItem & i )
{
    return stream << i.fileURL
                  << i.msgid
		  << i.msgstr
		  << i.index
		  << i.validationTool;
}

QDataStream & operator>>( QDataStream & stream, IgnoreItem & i ) {
    return stream >> i.fileURL
		    >> i.msgid
		    >> i.msgstr
		    >> i.index
		    >> i.validationTool;
}

#include "validateprogress.moc"
