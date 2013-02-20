/*
    This file is part of KBugBuster.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "rdfprocessor.h"

#include "bugserver.h"
#include "packageimpl.h"
#include "bugimpl.h"
#include "bugdetailsimpl.h"
#include "kbbprefs.h"

#include "kdebug.h"

RdfProcessor::RdfProcessor( BugServer *server )
  : DomProcessor( server )
{
}

RdfProcessor::~RdfProcessor()
{
}

KBB::Error RdfProcessor::parseDomBugList( const QDomElement &element,
                                                Bug::List &bugs )
{
  if ( element.tagName() != "RDF" ) {
    kdDebug() << "RdfProcessor::parseBugList(): no RDF element." << endl;
    return KBB::Error( "No RDF element found" );
  }

  QDomNodeList bugNodes = element.elementsByTagName( "bz:bug" );

  for( uint i = 0; i < bugNodes.count(); ++i ) {
    QString title;
    Person submitter;
    QString bugNr;
    Bug::Status status = Bug::StatusUndefined;
    Bug::Severity severity = Bug::SeverityUndefined;
    Person developerTodo;
    Bug::BugMergeList mergedList;

    QDomNode hit = bugNodes.item( i );

    QDomNode n;
    for( n = hit.firstChild(); !n.isNull(); n = n.nextSibling() ) {
      QDomElement e = n.toElement();

      if ( e.tagName() == "bz:id" ) {
        bugNr = e.text();
      } else if ( e.tagName() == "bz:status" ) {
        status = server()->bugStatus( e.text() );
      } else if ( e.tagName() == "bz:severity" ) {
        severity = server()->bugSeverity( e.text() );
      } else if ( e.tagName() == "bz:summary" ) {
        title = e.text();
      }
    }

    Bug bug( new BugImpl( title, submitter, bugNr, 0xFFFFFFFF, severity,
                          developerTodo, status, mergedList ) );

    if ( !bug.isNull() ) {
      bugs.append( bug );
    }
  }

  return KBB::Error();
}

void RdfProcessor::setBugListQuery( KURL &url, const Package &product, const QString &component )
{
  url.setFileName( "buglist.cgi" );
  if ( component.isEmpty() )
    url.setQuery( "?format=rdf&product=" + product.name() );
  else
    url.setQuery( "?format=rdf&product=" + product.name() + "&component=" + component );
  if ( KBBPrefs::instance()->mShowVoted ) {
    url.addQueryItem( "field0-0-0", "votes" );
    url.addQueryItem( "type0-0-0", "greaterthan" );
    QString num = QString::number( KBBPrefs::instance()->mMinVotes );;
    url.addQueryItem( "value0-0-0",  num );
  }
}

/*
 * vim:sw=4:ts=4:et
 */
