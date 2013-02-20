/*
    This file is part of KBugBuster.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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

#include "htmlparser.h"
#include "bugimpl.h"
#include "packageimpl.h"

#include <kdebug.h>

#include <qbuffer.h>
#include <qregexp.h>
#include <qtextstream.h>

KBB::Error HtmlParser::parseBugList( const QByteArray &data, Bug::List &bugs )
{
  QBuffer buffer( data );
  if ( !buffer.open( IO_ReadOnly ) ) {
    return KBB::Error( "Can't open buffer" );
  }

  QTextStream ts( &buffer );

  mState = Idle;

  QString line;
  while ( !( line = ts.readLine() ).isNull() ) {
    KBB::Error err = parseLine( line, bugs );
    if ( err ) return err;
  }

  return KBB::Error();
}

KBB::Error HtmlParser::parsePackageList( const QByteArray &data,
                                         Package::List &packages )
{
  init();

  QBuffer buffer( data );
  if ( !buffer.open( IO_ReadOnly ) ) {
    return KBB::Error( "Can't open buffer" );
  }

  QTextStream ts( &buffer );

  QString line;
  while ( !( line = ts.readLine() ).isNull() ) {
    KBB::Error err = parseLine( line, packages );
    if ( err ) return err;
  }

  processResult( packages );

  return KBB::Error();
}

void HtmlParser::init()
{
}

void HtmlParser::setPackageListQuery( KURL &url )
{
  url.setFileName( "query.cgi" );
}

KBB::Error HtmlParser::parseLine( const QString &, Bug::List & )
{
  return KBB::Error();
}

KBB::Error HtmlParser::parseLine( const QString &, Package::List & )
{
  return KBB::Error();
}

void HtmlParser::processResult( Package::List & )
{
}

QString HtmlParser::getAttribute( const QString &line, const QString &name )
{
  int pos1 = line.find( name + "=\"" );
  if ( pos1 < 1 ) return QString::null;
  pos1 += name.length() + 2;
  int pos2 = line.find( "\"", pos1 );
  if ( pos2 < 1 ) return QString::null;
  return line.mid( pos1, pos2 - pos1 );
}

bool HtmlParser::getCpts( const QString &line, QString &key,
                          QStringList &values )
{
  if ( !line.contains( QRegExp( "\\s*cpts" ) ) ) return false;

//  kdDebug() << "LINE: " << line << endl;
  int pos1 = line.find( "[" );
  if ( pos1 < 0 ) return false;
  int pos2 = line.find( "]", ++pos1 );
  if ( pos2 < 0 ) return false;

  key = line.mid( pos1, pos2 - pos1 );
  int pos3 = key.find( "'" );
  if ( pos3 >= 0 ) {
    int pos4 = key.find( "'", ++pos3 );
    if ( pos4 >= 0 ) key = key.mid( pos3, pos4 - pos3 );
  }
//  kdDebug() << " KEY: " << key << endl;

  pos1 = line.find( "'", ++pos2 );
  if ( pos1 >= 0 ) pos2 = line.find( "'", ++pos1 );

  while ( pos1 >= 0 && pos2 >= 0 ) {
    QString value = line.mid( pos1, pos2 - pos1 );
//      kdDebug() << " VALUE: " << value << endl;

    values.append( value );

    pos1 = line.find( "'", ++pos2 );
    if ( pos1 >= 0 ) pos2 = line.find( "'", ++pos1 );
  }

  return true;
}

KBB::Error HtmlParser_2_10::parseLine( const QString &line, Bug::List &bugs )
{
  if ( line.startsWith( "<TR VALIGN" ) ) {
//  kdDebug() << "LINE: " << line << endl;
    QRegExp re( "show_bug\\.cgi\\?id=(\\d+)" );
    re.search( line );
    QString number = re.cap( 1 );
//  kdDebug() << " NUMBER: " << number << endl;

    QString summary;
    int pos = line.findRev( "summary>" );
    if ( pos >= 0 ) summary = line.mid( pos + 8 );

    Bug bug( new BugImpl( summary, Person(), number, 0xFFFFFFFF, Bug::SeverityUndefined,
                          Person(), Bug::StatusUndefined,
                          Bug::BugMergeList() ) );

    if ( !bug.isNull() ) {
      bugs.append( bug );
    }
  }

  return KBB::Error();
}

KBB::Error HtmlParser_2_10::parseLine( const QString &line,
                                       Package::List &packages )
{
  QString package;
  QStringList components;

  if ( getCpts( line, package, components ) ) {
    packages.append( Package( new PackageImpl( package, "", 0, Person(),
                                               components ) ) );
  }

  return KBB::Error();
}


void HtmlParser_2_14_2::init()
{
  mComponentsMap.clear();
  
  mState = Idle;
}

KBB::Error HtmlParser_2_14_2::parseLine( const QString &line,
                                         Package::List & )
{
  switch ( mState ) {
    case Idle:
      if ( line.startsWith( "tms[" ) ) mState = Components;
      break;
    case Components: {
      if ( line.startsWith( "function" ) ) mState = Finished;
      QString key;
      QStringList values;
      if ( getCpts( line, key, values ) ) {
//        kdDebug() << "KEY: " << key << "  VALUES: " << values.join(",") << endl;
        if ( values.count() == 2 ) {
          mComponentsMap[ values.last() ].append( key );
        }
      }
    }
    default:
      break;
  }

  return KBB::Error();
}

void HtmlParser_2_14_2::processResult( Package::List &packages )
{
  QMap<QString,QStringList>::ConstIterator it;
  for ( it = mComponentsMap.begin(); it != mComponentsMap.end(); ++it ) {
    packages.append( Package( new PackageImpl( it.key(), "", 0, Person(),
                                               it.data() ) ) );    
  }
}


void HtmlParser_2_17_1::init()
{
  mProducts.clear();
  mComponents.clear();
  
  mState = Idle;
}

KBB::Error HtmlParser_2_17_1::parseBugList( const QByteArray &data, Bug::List &bugs )
{
  return RdfProcessor::parseBugList( data, bugs );
}

KBB::Error HtmlParser_2_17_1::parseLine( const QString & /*line*/, Bug::List &/*bugs*/ )
{
  return KBB::Error( "Not implemented" );
}

KBB::Error HtmlParser_2_17_1::parseLine( const QString &line, Package::List & )
{
  switch ( mState ) {
    case Idle:
    case SearchComponents:
      if ( line.contains( "var cpts" ) ) mState = Components;
      break;
    case SearchProducts:
      if ( line.contains( "onchange=\"selectProduct" ) ) mState = Products;
      break;
    case Components: {
      if ( line.contains( QRegExp( "\\s*function" ) ) ) {
        mState = SearchProducts;
      }
      QString key;
      QStringList components;
      if ( getCpts( line, key, components ) ) {
        mComponents.append( components );
      }
    }
    case Products: {
      if ( line.contains( "</select>" ) ) mState = Finished;
      QString product = getAttribute( line, "value" );
      if ( !product.isEmpty() ) {
        kdDebug() << "PRODUCT: " << product << endl;
        mProducts.append( product );
      }
      break;
    }
    case Finished:
    default:
      break;
  }

  return KBB::Error();
}

void HtmlParser_2_17_1::processResult( Package::List &packages )
{
  QStringList::ConstIterator itProduct = mProducts.begin();
  QValueList<QStringList>::ConstIterator itComponents = mComponents.begin();
  
  while( itProduct != mProducts.end() && itComponents != mComponents.end() ) {
    packages.append( Package( new PackageImpl( *itProduct, "", 0, Person(),
                                               *itComponents ) ) );
    ++itProduct;
    ++itComponents;
  }
}
