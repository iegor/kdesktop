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

#include "domprocessor.h"

#include <qregexp.h>
#include <qstylesheet.h>

#include <kdebug.h>
#include <kmdcodec.h>

#include "bugserver.h"
#include "packageimpl.h"
#include "bugimpl.h"
#include "bugdetailsimpl.h"
#include "kbbprefs.h"

DomProcessor::DomProcessor( BugServer *server )
  : Processor( server )
{
}

DomProcessor::~DomProcessor()
{
}

KBB::Error DomProcessor::parsePackageList( const QByteArray &data,
                             Package::List &packages )
{
    QDomDocument doc;
    if ( !doc.setContent( data ) ) {
      return KBB::Error( "Error parsing xml response for package list request." );
    }
    
    QDomElement bugzilla = doc.documentElement();

    if ( bugzilla.isNull() ) {
      return KBB::Error( "No document in xml response." );
    }

    KBB::Error err = parseDomPackageList( bugzilla, packages );

    return err;
}

KBB::Error DomProcessor::parseBugList( const QByteArray &data, Bug::List &bugs )
{
    QDomDocument doc;
    if ( !doc.setContent( data ) ) {
      return KBB::Error( "Error parsing xml response for bug list request" );
    }

    QDomElement bugzilla = doc.documentElement();

    if ( bugzilla.isNull() ) {
      return KBB::Error( "No document in xml response." );
    }

    KBB::Error err = parseDomBugList( bugzilla, bugs );

    return err;
}

KBB::Error DomProcessor::parseBugDetails( const QByteArray &data,
                                          BugDetails &bugDetails )
{
    QDomDocument doc;
    if ( !doc.setContent( data ) ) {
      return KBB::Error( "Error parsing xml response for bug details request." );
    }
    
    QDomElement bugzilla = doc.documentElement();

    if ( bugzilla.isNull() ) {
      return KBB::Error( "No document in xml response." );
    }
    
    QDomNode p;
    for ( p = bugzilla.firstChild(); !p.isNull(); p = p.nextSibling() ) {
        QDomElement bug = p.toElement();
        if ( bug.tagName() != "bug" ) continue;

        KBB::Error err = parseDomBugDetails( bug, bugDetails );

        if ( err ) return err;
    }
    
    return KBB::Error();
}


KBB::Error DomProcessor::parseDomPackageList( const QDomElement &element,
                                              Package::List &packages )
{
  QDomNode p;
  for ( p = element.firstChild(); !p.isNull(); p = p.nextSibling() ) {
    QDomElement bug = p.toElement();

    if ( bug.tagName() != "product" ) continue;

    QString pkgName = bug.attribute( "name" );
    uint bugCount = 999;
    Person maintainer;
    QString description;
    QStringList components;

    QDomNode n;
    for( n = bug.firstChild(); !n.isNull(); n = n.nextSibling() ) {
      QDomElement e = n.toElement();
      if ( e.tagName() == "descr" ) description= e.text().stripWhiteSpace();
      if ( e.tagName() == "component" ) components += e.text().stripWhiteSpace();
    }

    Package pkg( new PackageImpl( pkgName, description, bugCount, maintainer, components ) );

    if ( !pkg.isNull() ) {
        packages.append( pkg );
    }
  }

  return KBB::Error();
}

KBB::Error DomProcessor::parseDomBugList( const QDomElement &topElement,
                                          Bug::List &bugs )
{
  QDomElement element;

  if ( topElement.tagName() != "querybugids" ) {
    QDomNode buglist = topElement.namedItem( "querybugids" );
    element = buglist.toElement();
    if ( element.isNull() ) {
      return KBB::Error( "No querybugids element found." );
    }
  } else {
    element = topElement;
  }

  QDomNode p;
  for ( p = element.firstChild(); !p.isNull(); p = p.nextSibling() ) {
    QDomElement hit = p.toElement();

    kdDebug() << "DomProcessor::parseDomBugList(): tag: " << hit.tagName() << endl;

    if ( hit.tagName() == "error" ) {
      return KBB::Error( "Error: " + hit.text() );
    } else if ( hit.tagName() != "hit" ) continue;

    QString title;
    QString submitterName;
    QString submitterEmail;
    QString bugNr;
    Bug::Status status = Bug::StatusUndefined;
    Bug::Severity severity = Bug::SeverityUndefined;
    Person developerTodo;
    Bug::BugMergeList mergedList;
    uint age = 0xFFFFFFFF;

    QDomNode n;
    for ( n = hit.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
      QDomElement e = n.toElement();

      if ( e.tagName() == "bugid" )
        bugNr = e.text();
      else if ( e.tagName() == "status" )
        status = server()->bugStatus( e.text() );
      else if ( e.tagName() == "descr" )
        title = e.text();
      else if ( e.tagName() == "reporter" )
        submitterEmail = e.text();
      else if ( e.tagName() == "reporterName" )
        submitterName = e.text();
      else if ( e.tagName() == "severity" )
        severity = Bug::stringToSeverity( e.text() );
      else if ( e.tagName() == "creationdate" )
        age = ( QDateTime::fromString( e.text(), Qt::ISODate ) ).daysTo( QDateTime::currentDateTime() );
    }

    Person submitter( submitterName, submitterEmail );

    Bug bug( new BugImpl( title, submitter, bugNr, age, severity,
                          developerTodo, status, mergedList ) );

    if ( !bug.isNull() ) {
      bugs.append( bug );
    }
  }

  return KBB::Error();
}

KBB::Error DomProcessor::parseDomBugDetails( const QDomElement &element,
                                             BugDetails &bugDetails )
{
  if ( element.tagName() != "bug" ) return KBB::Error( "No <bug> tag found" );

  BugDetailsPart::List parts;
  QValueList<BugDetailsImpl::AttachmentDetails> attachments;

  QString versionXml;
  QString osXml;

  QString version;
  QString source;
  QString compiler;
  QString os;

  QDomNode n;
  for( n = element.firstChild(); !n.isNull(); n = n.nextSibling() ) {
    QDomElement e = n.toElement();
    if ( e.tagName() == "version" ) versionXml = e.text().stripWhiteSpace();
    if ( e.tagName() == "op_sys" ) osXml = e.text().stripWhiteSpace();

    if ( e.tagName() == "long_desc" ) {

      QString encoding = e.attribute( "encoding" );

      Person sender;
      QDateTime date;
      QString text;

      QDomNode n2;
      for( n2 = e.firstChild(); !n2.isNull(); n2 = n2.nextSibling() ) {
        QDomElement e2 = n2.toElement();
        if ( e2.tagName() == "who" ) {
          sender = Person::parseFromString( e2.text() );
        } else if ( e2.tagName() == "bug_when" ) {
          date = parseDate( e2.text().stripWhiteSpace() );
        } else if ( e2.tagName() == "thetext" ) {
          QString in;
          if ( encoding == "base64" ) {
            in = KCodecs::base64Decode( e2.text().latin1() );
          } else {
            in = e2.text();
          }

          QString raw = QStyleSheet::escape( in );

          if ( parts.isEmpty() )
          {
            QTextStream ts( &raw, IO_ReadOnly );
            QString line;
            while( !( line = ts.readLine() ).isNull() ) {
              if ( parseAttributeLine( line, "Version", version ) ) continue;
              if ( parseAttributeLine( line, "Installed from", source ) ) continue;
              if ( parseAttributeLine( line, "Compiler", compiler ) ) continue;
              if ( parseAttributeLine( line, "OS", os ) ) continue;

              text += line + "\n";
            }
          } else {
            text += raw;
          }
          QString bugBaseURL = server()->serverConfig().baseUrl().htmlURL();
          text = "<pre>" + wrapLines( text ).replace( QRegExp( "(Created an attachment \\(id=([0-9]+)\\))" ),
              "<a href=\"" + bugBaseURL + "/attachment.cgi?id=\\2&action=view\">\\1</a>" ) + "\n</pre>";
        }
      }

      parts.prepend( BugDetailsPart( sender, date, text ) );
    }

    if ( e.tagName() == "attachment" ) {
        QString attachid, date, desc;
        for( QDomNode node = e.firstChild(); !node.isNull(); node = node.nextSibling() ) {
            QDomElement e2 = node.toElement();
            if ( e2.tagName() == "attachid" ) {
                attachid = e2.text();
            } else if ( e2.tagName() == "date" ) {
                date = e2.text().stripWhiteSpace();
            } else if ( e2.tagName() == "desc" ) {
                desc = "<pre>" + wrapLines( QStyleSheet::escape(e2.text()) ) + "\n</pre>";
            }
        }
        attachments.append( BugDetailsImpl::AttachmentDetails( desc, date, attachid ) );
    }
  }

  if ( version.isEmpty() ) version = versionXml;
  if ( os.isEmpty() ) os = osXml;

  bugDetails = BugDetails( new BugDetailsImpl( version, source, compiler, os,
                                               parts ) );
  bugDetails.addAttachmentDetails( attachments );

  return KBB::Error();
}

void DomProcessor::setPackageListQuery( KURL &url )
{
  url.setFileName( "xml.cgi" );
  url.setQuery( "?data=versiontable" );
}

void DomProcessor::setBugListQuery( KURL &url, const Package &product, const QString &component )
{
  if ( server()->serverConfig().bugzillaVersion() == "Bugworld" ) {
    url.setFileName( "bugworld.cgi" );
  } else {
    url.setFileName( "xmlquery.cgi" );
  }

  QString user = server()->serverConfig().user();

  if ( component.isEmpty() )
      url.setQuery( "?user=" + user + "&product=" + product.name() );
  else
      url.setQuery( "?user=" + user + "&product=" + product.name() + "&component=" + component );

  if ( KBBPrefs::instance()->mShowClosedBugs )
      url.addQueryItem( "addClosed", "1" );
}

void DomProcessor::setBugDetailsQuery( KURL &url, const Bug &bug )
{
  url.setFileName( "xml.cgi" );
  url.setQuery( "?id=" + bug.number() );
}

QString DomProcessor::wrapLines( const QString &text )
{
  int wrap = KBBPrefs::instance()->mWrapColumn;

  QStringList lines = QStringList::split( '\n', text, true );
  //kdDebug() << lines.count() << " lines." << endl;

  QString out;
  bool removeBlankLines = true;
  for ( QStringList::Iterator it = lines.begin() ; it != lines.end() ; ++it )
  {
      QString line = *it;

      if ( removeBlankLines ) {
        if ( line.isEmpty() ) continue;
        else removeBlankLines = false;
      }

      //kdDebug() << "BugDetailsJob::processNode IN line='" << line << "'" << endl;

      QString wrappedLine;
      while ( line.length() > uint( wrap ) )
      {
          int breakPoint = line.findRev( ' ', wrap );
          //kdDebug() << "Breaking at " << breakPoint << endl;
          if( breakPoint == -1 ) {
              wrappedLine += line.left( wrap ) + '\n';
              line = line.mid( wrap );
          } else {
              wrappedLine += line.left( breakPoint ) + '\n';
              line = line.mid( breakPoint + 1 );
          }
      }
      wrappedLine += line; // the remainder
      //kdDebug() << "BugDetailsJob::processNode OUT wrappedLine='" << wrappedLine << "'" << endl;

      out += wrappedLine + "\n";
  }

  return out;
}

bool DomProcessor::parseAttributeLine( const QString &line, const QString &key,
                                       QString &result )
{
  if ( !result.isEmpty() ) return false;

  if ( !line.startsWith( key + ":" ) ) return false;
  
  QString value = line.mid( key.length() + 1 );
  value = value.stripWhiteSpace();

  result = value;

  return true;
}

QDateTime DomProcessor::parseDate( const QString &dateStr )
{
  QDateTime date = QDateTime::fromString( dateStr, Qt::ISODate );

  return date;
}

/*
 * vim:sw=4:ts=4:et
 */
