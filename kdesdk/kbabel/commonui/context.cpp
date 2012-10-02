/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2005  by Stanislav Visnovsky
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
#include "context.h"
#include "klisteditor.h"
#include "kbprojectsettings.h"

#include <qcombobox.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qvgroupbox.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kdeversion.h>
#include <kio/netaccess.h>

#include <klibloader.h>
#include <ktrader.h>
#include <ktexteditor/document.h>
#include <ktexteditor/editinterface.h>
#include <ktexteditor/selectioninterface.h>
#include <ktexteditor/viewcursorinterface.h>

SourceContext::SourceContext(QWidget *parent, KBabel::Project::Ptr project): QWidget(parent)
    , m_parent( parent )
    , _part(0)
    , _view(0)
    , _referenceCombo(0)
    , _layout(0)
    , _project(project)
{
    _referenceList.clear();
    _referenceCombo = new QComboBox( this );
    connect( _referenceCombo, SIGNAL(activated(int)), this, SLOT(updateToSelected(int)));
    
    _layout= new QVBoxLayout(this);
    _layout->addWidget(_referenceCombo);
}

void SourceContext::setContext( const QString& packageDir, const QString& packageName, const QString& gettextComment, const KURL& urlPoFile )
{
    if( !_part && !loadPart() ) return;
    _referenceCombo->clear();
    _referenceList = resolvePath( packageDir, packageName, gettextComment, urlPoFile );
    
    for( QValueList<ContextInfo>::const_iterator it = _referenceList.constBegin(); it != _referenceList.constEnd(); ++it )
	_referenceCombo->insertItem((*it).path);

    _referenceCombo->setEnabled( !_referenceList.isEmpty() );
    if( _referenceList.isEmpty() )
    {
        _part->setReadWrite( true );
        // We have to simulate a new document (like with File/New) by using openStream and closeStream
        _part->openStream("text/plain","kbabel:error"); // KBabel does not show the URL kbabel:error
        _part->closeStream();
        (dynamic_cast<KTextEditor::EditInterface *>(_part))->setText(i18n("Corresponding source file not found"));
        _part->setReadWrite(false);
	_part->setModified(false);
    } 
    else 
    {
	_referenceCombo->setCurrentItem(0);
	updateToSelected(0);
    }
}

void SourceContext::updateToSelected(int index)
{
    if( !_part ) return;
    ContextInfo ci = *(_referenceList.at(index));
    KURL newUrl( KURL::fromPathOrURL( ci.path ) );
    if( _part->url() != newUrl )
    {
        _part->setReadWrite( true );
        _part->openURL( newUrl );
    }
    _part->setReadWrite( false );
    (dynamic_cast<KTextEditor::ViewCursorInterface *>(_view))->setCursorPosition(ci.line,0);
    (dynamic_cast<KTextEditor::SelectionInterface *>(_part))->setSelection(ci.line-1,0,ci.line,0);
}

QValueList<ContextInfo> SourceContext::resolvePath( const QString& packageDir, const QString& packageName, const QString& gettextComment, const KURL& urlPoFile )
{
    //kdDebug() << "GETTEXTCOMMENT:" << gettextComment << endl;

    // Find the directory name of the PO file, if the PO file is local
    // ### TODO: find a way to allow remote files too
    QString poDir;
#if KDE_IS_VERSION( 3, 5, 0 )
    const KURL localUrl( KIO::NetAccess::mostLocalURL( urlPoFile, m_parent ) );
    if ( localUrl.isLocalFile() )
    {
        const QFileInfo fi( localUrl.path() );
        poDir = fi.dirPath( true );
    }
#else
    if ( urlPoFile.isLocalFile() )
    {
        const QFileInfo fi( urlPoFile.path() );
        poDir = fi.dirPath( true );
    }
#endif

#if 0
    kdDebug() << "CONTEXT VARIABLE START" << endl;
    kdDebug() << "@CODEROOT@: " << _project->settings()->codeRoot() << endl;
    kdDebug() << "@PACKAGEDIR@: " << packageDir  << endl;
    kdDebug() << "@PACKAGE@: " << packageName << endl;
    kdDebug() << "@POFILEDIR@: " << poDir  << endl;
    kdDebug() << "CONTEXT VARIABLE END" << endl;
#endif
    
    QStringList prefixes;
    const QStringList paths = _project->settings()->paths();
    
    for( QStringList::const_iterator it = paths.constBegin(); it!=paths.constEnd() ; ++it )
    {
	QString pref = (*it);

        if ( !poDir.isEmpty() )
        {
            pref.replace( "@POFILEDIR@", poDir );
        }
        else if ( pref.find( "@POFILEDIR@ " ) != -1 )
            continue; // No need to keep this path pattern, as we have no PO file dir
        
        pref.replace( "@PACKAGEDIR@", packageDir);
        pref.replace( "@PACKAGE@", packageName);
	pref.replace( "@CODEROOT@", _project->settings()->codeRoot());
	prefixes.append(pref);
    }

    QValueList<ContextInfo> rawRefList; // raw references
    QRegExp re("^\\s*(.+):(\\d+)\\s*$"); // Reg. exp. for Gettext references
    QRegExp rex( "^#. i18n: file (.+) line (\\d+)\\s*$" ); //Reg. exp. for KDE extractrc/extractattr references
    QRegExp res( "^# [Ff]ile: (.+), line(?: number)?: (\\d+)\\s*$"); // Reg. exp. for "strict" PO format
    const QStringList lines = QStringList::split( "\n", gettextComment );
    for ( QStringList::const_iterator it = lines.constBegin() ; it != lines.constEnd() ; ++it)
    {
	const QString curLine = (*it).stripWhiteSpace();
	if( curLine.startsWith( "#:" ) )
        {
            // We have a Gettext line with references
            const QStringList references( QStringList::split( " ", curLine.mid( 2 ), false ) );
            for ( QStringList::const_iterator it = references.constBegin(); it != references.constEnd(); ++it )
            {
                if ( re.exactMatch( (*it) ) )
                {
                    ContextInfo ref;
                    ref.line = re.cap(2).toInt();
                    ref.path = re.cap(1);
                    // ### TODO KDE4: perhaps we should not do the replace if compiled for Windows
                    ref.path.replace( QChar( '\\' ), QChar( '/' ) );
                    rawRefList.append( ref );
                }
            }
            
        }
        else if ( curLine.startsWith( "#," ) )
        {
            // We have a Gettext option line. There is no source reference here.
            continue;
        }
        else if ( curLine.startsWith( "#. i18n:") )
        {
            // We might have a KDE reference from extractrc/extractattr
            if ( rex.exactMatch( (*it) ) )
            {
                ContextInfo ref;
                ref.line = rex.cap(2).toInt();
                ref.path = rex.cap(1);
                // KDE is not extracted on Windows, so no backslash conversion is needed.
                rawRefList.append( ref );
            }
        }
        else if ( curLine.startsWith( "# F" ) || curLine.startsWith( "# f" ) )
        {
            // We might have a "strict PO" reference
            if ( res.exactMatch( (*it) ) )
            {
                ContextInfo ref;
                ref.line = res.cap(2).toInt();
                ref.path = res.cap(1);
                // ### TODO KDE4: perhaps we should not do the replace if compiled for Windows
                ref.path.replace( QChar( '\\' ), QChar( '/' ) );
                rawRefList.append( ref );
            }
        }
        else
            continue; 
    }
    
    // Now that we have gathered the references, we need to convert them to absolute paths
    QValueList<ContextInfo> results;
    for ( QValueList<ContextInfo>::const_iterator it = rawRefList.constBegin(); it != rawRefList.constEnd(); ++it )
    {
        const int lineNum = (*it).line;
        const QString fileName = (*it).path;
        for ( QStringList::const_iterator it1 = prefixes.constBegin(); it1 != prefixes.constEnd(); ++it1 )
        {
            QString path = (*it1);
            path.replace( "@COMMENTPATH@", fileName);

            //kdDebug() << "CONTEXT PATH: " << path << endl; // DEBUG
            QFileInfo pathInfo( path );
            if( pathInfo.exists() )
            {
                ContextInfo ref;
                ref.path = pathInfo.absFilePath();
                ref.line = lineNum;
                results.append(ref);
            }
        }
    }
    
    return results;
}

bool SourceContext::loadPart()
{
    KTrader::OfferList offers = KTrader::self()->query( "KTextEditor/Document" );
    if( offers.count() < 1 )
    {
	KMessageBox::error(this,i18n("KBabel cannot start a text editor component.\n"
	"Please check your KDE installation."));
	_part=0;
	_view=0;
	return false;
    }
    KService::Ptr service = *offers.begin();
    KLibFactory *factory = KLibLoader::self()->factory( service->library().latin1() );
    if( !factory )
    {
	KMessageBox::error(this,i18n("KBabel cannot start a text editor component.\n"
	"Please check your KDE installation."));
	_part=0;
	_view=0;
	return false;
    }
    _part = static_cast<KTextEditor::Document *>( factory->create( this, 0, "KTextEditor::Document" ) );

    if( !_part )
    {
	KMessageBox::error(this,i18n("KBabel cannot start a text editor component.\n"
	"Please check your KDE installation."));
	_part=0;
	_view=0;
	return false;
    }
    _view = _part->createView( this, 0 );
    _layout->addWidget(static_cast<QWidget *>(_view), 1);
    static_cast<QWidget *>(_view)->show();

    return true;
}

void SourceContext::setProject( KBabel::Project::Ptr project )
{
    _project = project;
}

#include "context.moc"

// kate: space-indent on; indent-width 4; replace-tabs on;
