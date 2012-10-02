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
#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "package.h"
#include "bug.h"
#include "error.h"
#include "rdfprocessor.h"

#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>

class HtmlParser : public RdfProcessor
{
  protected:
    enum State { Idle, SearchComponents, SearchProducts, Components, Products,
                 Finished };
    State mState;

  public:
    HtmlParser( BugServer *s ) : RdfProcessor( s ), mState( Idle ) {}
    virtual ~HtmlParser() {}

    KBB::Error parseBugList( const QByteArray &data, Bug::List &bugs );
    KBB::Error parsePackageList( const QByteArray &data,
                                 Package::List &packages );

    void setPackageListQuery( KURL & );

  protected:
    virtual void init();

    virtual KBB::Error parseLine( const QString &line, Bug::List &bugs );
    virtual KBB::Error parseLine( const QString &line,
                                  Package::List &packages );

    virtual void processResult( Package::List &packages );

    QString getAttribute( const QString &line, const QString &name );
    bool getCpts( const QString &line, QString &key, QStringList &values );
};


class HtmlParser_2_10 : public HtmlParser
{
  public:
    HtmlParser_2_10( BugServer *s ) : HtmlParser( s ) {}

  protected:  
    KBB::Error parseLine( const QString &line, Bug::List &bugs );
    KBB::Error parseLine( const QString &line, Package::List &packages );
};


class HtmlParser_2_14_2 : public HtmlParser_2_10
{
  public:
    HtmlParser_2_14_2( BugServer *s ) : HtmlParser_2_10( s ) {}

  protected:
    void init();

    KBB::Error parseLine( const QString &line, Package::List &packages );

    void processResult( Package::List &packages );

  private:
    QMap<QString, QStringList> mComponentsMap;
};



class HtmlParser_2_17_1 : public HtmlParser
{
  public:
    HtmlParser_2_17_1( BugServer *s ) : HtmlParser( s ) {}
    
    KBB::Error parseBugList( const QByteArray &data, Bug::List &bugs );

  protected:
    void init();

    KBB::Error parseLine( const QString &line, Bug::List &bugs );
    KBB::Error parseLine( const QString &line, Package::List &packages );

    void processResult( Package::List &packages );

  private:
    QStringList mProducts;
    QValueList<QStringList> mComponents;
};

#endif
