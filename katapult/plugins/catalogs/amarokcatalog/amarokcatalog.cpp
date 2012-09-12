/***************************************************************************
 *   Copyright (C) 2005 by Bastian Holst                                   *
 *   bastianholst@gmx.de                                                   *
 *   Copyright (C) 2006 by Martin Meredith                                 *
 *   mez@thekatapult.org.uk                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kapplication.h>
#include <ksycocaentry.h>
#include <ksycocatype.h>
#include <knuminput.h>
#include <kurl.h>
#include <kapp.h>
#include <qstring.h>
#include <qcstring.h>
#include <qstringlist.h>
#include <qdatastream.h>
#include <dcopclient.h>
#include <qregexp.h>

#include "actionplaysong.h"
#include "song.h"
#include "amarokcatalog.h"
#include "actionregistry.h"
#include "status.h"
#include "settings.h"

K_EXPORT_COMPONENT_FACTORY( katapult_amarokcatalog,
                           KGenericFactory<AmarokCatalog>( "katapult_amarokcatalog" ) )

AmarokCatalog::AmarokCatalog(QObject*, const char*, const QStringList&): _result(QString::null) 
{
	_minQueryLen = 3;
	ActionRegistry::self()->registerAction(new ActionPlaySong());
	_gotCollectionStatus = false;
	_dynamicCollection = false;
	checkCollectionType();	

}
AmarokCatalog::~AmarokCatalog()
{
}

void AmarokCatalog::queryChanged()
{
	int newStatus = 0;
	QString queryString = query();
	
	if((QString(queryString).remove(':').remove('\"').remove(' ').isEmpty()) || (queryString.length() < _minQueryLen)) {
		reset();
		setBestMatch(Match());
		setStatus(0);
	} else {

		if ( _gotCollectionStatus)
		{

			if (!_dynamicCollection)
			{

			// Stuff for Amarok < 1.4.2

				QStringList queryList;
				//prepares SQL-queryQRegExp
				QString sqlQuery("SELECT a.name, t.title, t.url, i.path, album.name FROM tags t, artist a, album LEFT JOIN statistics s ON (t.url = s.url) LEFT JOIN images i ON (a.name = i.artist AND album.name = i.album) WHERE t.album = album.id AND t.artist = a.id"); // AND 
		
				queryList = QStringList::split ( QString(" "), QString(queryString).replace(QChar(':')," ").replace(QChar('\''), " ").replace(QChar('\''), "%") );	
				for(QStringList::Iterator it = queryList.begin(); it != queryList.end(); ++it) {
					sqlQuery.append(QString(" AND (t.title LIKE '\%%1\%'").arg(*it));
					sqlQuery.append(QString(" OR a.name LIKE '\%%1\%')").arg(*it));
				}
				sqlQuery.append(" ORDER BY a.name, t.title, s.percentage DESC");
				
				//sending SQL-query to ararok via dcop
				QByteArray sqlQueryData, replyData;
				QCString replyType;
				QDataStream arg(sqlQueryData, IO_WriteOnly);
				arg << sqlQuery;
				if (!kapp->dcopClient()->call("amarok", "collection", "query(QString)",
				     sqlQueryData, replyType, replyData)) {
					newStatus = 0;
				} else {
					QDataStream reply(replyData, IO_ReadOnly);
					if (replyType == "QStringList") {
						QStringList sqlResult;
						reply >> sqlResult;
					
						if(sqlResult.isEmpty()) {
							newStatus = 0;
						} else {
							reset();
							//Reads information from SQL-Query
							_result.setArtist(sqlResult[0]);
							_result.setName(sqlResult[1]);
							_result.setURL(KURL(sqlResult[2]));
							_result.setAlbum(sqlResult[4]);
						
							//_result.setIcon(QString());
							if ( !sqlResult[3].isEmpty() ) {
								_result.setIcon(sqlResult[3]);
							}
							
							//counts the matched charecters
							int i = queryString.find( ':' );
							if ( i != -1 ) {
								if ( queryString[i+1] != ' ' )
									queryString.insert(i+1, ' ');
								if ( queryString[i-1] != ' ' ) 
									queryString.insert(i, ' ');
							}
							queryList = QStringList::split ( " ", queryString );
							unsigned int matched = 0;
							for(QStringList::Iterator it = queryList.begin(); it != queryList.end(); ++it) {
								if(matched < (_result.text().find(*it, matched, false) + (*it).length()))
									matched = _result.text().find(*it, matched, false) + (*it).length();
							}
							setBestMatch(Match(&_result, 100*queryString.length()/_result.text().length(), matched)); 
					
							//Checks if there are multiple results
							if( !sqlResult[5].isEmpty() )
								newStatus = S_HasResults | S_Multiple;
							else 
								newStatus = S_HasResults;
						}
					} else {
						newStatus = 0;
					}
				}

			} else {	// Dynamic Collection 

				// Do same as above here again but with dyn collection stuff

				QStringList queryList;
				//prepares SQL-queryQRegExp
				QString sqlQuery("SELECT a.name, t.title, t.deviceid, d.lastmountpoint, t.url, i.path, album.name FROM tags t LEFT JOIN statistics s ON (t.url = s.url AND t.deviceid = s.deviceid) LEFT JOIN artist a ON (t.artist = a.id) LEFT JOIN album ON (t.album = album.id) LEFT JOIN images i ON ( a.name = i.artist AND album.name = i.album) LEFT JOIN devices d ON (t.deviceid = d.id) WHERE ");
				queryList = QStringList::split ( QString(" "), QString(queryString).replace(QChar(':')," ").replace(QChar('\''), " ").replace(QChar('\''), "%") );

        // Let's build each of these clauses
        QStringList clauses;
				for(QStringList::Iterator it = queryList.begin(); it != queryList.end(); ++it) {

					clauses += QString(" (t.title LIKE '\%%1\%'").arg(*it) +
            QString(" OR a.name LIKE '\%%1\%')").arg(*it);
				}
        sqlQuery.append(clauses.join(QString(" AND ")));
				sqlQuery.append(" ORDER BY a.name, t.title, s.percentage DESC");
				
				//sending SQL-query to ararok via dcop
				QByteArray sqlQueryData, replyData;
				QCString replyType;
				QDataStream arg(sqlQueryData, IO_WriteOnly);
				arg << sqlQuery;
				if (!kapp->dcopClient()->call("amarok", "collection", "query(QString)",
				     sqlQueryData, replyType, replyData)) {
					newStatus = 0;
				} else {
					QDataStream reply(replyData, IO_ReadOnly);
					if (replyType == "QStringList") {
						QStringList sqlResult;
						reply >> sqlResult;
					
						if(sqlResult.isEmpty()) {
							newStatus = 0;
						} else {
							reset();
							//Reads information from SQL-Query
							_result.setArtist(sqlResult[0]);
							_result.setName(sqlResult[1]);
							if (sqlResult[2]!="-1") {
								KURL absolutePath;
								absolutePath.setPath( sqlResult[3] );
								absolutePath.addPath( sqlResult[4] );
								absolutePath.cleanPath();

								_result.setURL( absolutePath );
							} else {
								KURL absolutePath;
								absolutePath.setPath( "/" );
								absolutePath.addPath( sqlResult[4] );
								absolutePath.cleanPath(); 

								_result.setURL( absolutePath );
							}

							_result.setAlbum(sqlResult[6]);
						
							//_result.setIcon(QString());
							if ( !sqlResult[3].isEmpty() ) {
								_result.setIcon(sqlResult[5]);
							}
							
							//counts the matched charecters
							int i = queryString.find( ':' );
							if ( i != -1 ) {
								if ( queryString[i+1] != ' ' )
									queryString.insert(i+1, ' ');
								if ( queryString[i-1] != ' ' ) 
									queryString.insert(i, ' ');
							}
							queryList = QStringList::split ( " ", queryString );
							unsigned int matched = 0;
							for(QStringList::Iterator it = queryList.begin(); it != queryList.end(); ++it) {
								if(matched < (_result.text().find(*it, matched, false) + (*it).length()))
									matched = _result.text().find(*it, matched, false) + (*it).length();
							}
							setBestMatch(Match(&_result, 100*queryString.length()/_result.text().length(), matched)); 
					
							//Checks if there are multiple results
						 	if( !sqlResult[7].isEmpty() )
								newStatus = S_HasResults | S_Multiple;
							else 
								newStatus = S_HasResults;
						}
					} else {
						newStatus = 0;
					}
				}


			} //end of >1.4.2 section
		setStatus(newStatus);

		} else {                //We haven't got the collection status

			checkCollectionType();	
			reset();
			setBestMatch(Match());
			setStatus(0);
		}


	} //dont go after this while fixing

}

void AmarokCatalog::reset()
{
	_result.setName(QString::null);
	_result.setArtist(QString::null);
	_result.setAlbum(QString::null);
	_result.setIcon(QString::null);
}

void AmarokCatalog::checkCollectionType()
{
	QString sqlQuery("SELECT COUNT(*) FROM admin WHERE noption = 'Database Devices Version'");

	QByteArray sqlQueryData, replyData;
	QCString replyType;
	QDataStream arg(sqlQueryData, IO_WriteOnly);
	arg << sqlQuery;
	if (!kapp->dcopClient()->call("amarok", "collection", "query(QString)", sqlQueryData, replyType, replyData))
	{
		_gotCollectionStatus = false;

	}
	else
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QStringList")
		{
			QStringList sqlResult;
			reply >> sqlResult;

			if (sqlResult[0] == "1")
			{
				_dynamicCollection = true;

			}
			else
			{
				_dynamicCollection = false;

			}
			_gotCollectionStatus = true;
		}
		else
		{
			_gotCollectionStatus = false;
		}
	}
}


/*
void AmarokCatalog::initialize()
{
}
*/

unsigned int AmarokCatalog::minQueryLen() const
{
	return _minQueryLen;
}

QWidget * AmarokCatalog::configure()
{
	AmarokCatalogSettings *settings = new AmarokCatalogSettings();
	
	settings->minQueryLen->setValue(_minQueryLen);
	connect(settings->minQueryLen, SIGNAL(valueChanged(int)), this, SLOT(minQueryLenChanged(int)));
	return settings;
}

void AmarokCatalog::minQueryLenChanged(int _minQueryLen)
{
	this->_minQueryLen = _minQueryLen;
}

void AmarokCatalog::readSettings(KConfigBase *config)
{
	_minQueryLen = config->readUnsignedNumEntry("MinQueryLen", 3);
}

void AmarokCatalog::writeSettings(KConfigBase *config)
{
	config->writeEntry("MinQueryLen", _minQueryLen);
}
