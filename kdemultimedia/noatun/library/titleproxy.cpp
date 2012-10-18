/***************************************************************************
                     Proxy.cpp  -  description
                        -------------------
begin                : Nov 20 14:35:18 CEST 2003
copyright            : (C) 2003 by Mark Kretschmann
email                : markey@web.de
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "titleproxy.h"

#include <kdebug.h>
#include <kprotocolmanager.h>
#include <kmdcodec.h>

#include <qstring.h>
#include <qtimer.h>
#include "noatun/app.h"

using namespace TitleProxy;

static const uint MIN_PROXYPORT = 6700;
static const uint MAX_PROXYPORT = 7777;
static const int BUFSIZE = 32768;

Proxy::Proxy( KURL url )
	: QObject()
	, m_url( url )
	, m_initSuccess( true )
	, m_metaInt( 0 )
	, m_byteCount( 0 )
	, m_metaLen( 0 )
	, m_usedPort( 0 )
	, m_pBuf( 0 )
{
	kdDebug(66666) << k_funcinfo << endl;

	m_pBuf = new char[ BUFSIZE ];
	// Don't try to get metdata for ogg streams (different protocol)
	m_icyMode = url.path().endsWith( ".ogg" ) ? false : true;
	// If no port is specified, use default shoutcast port
	if ( m_url.port() < 1 )
		m_url.setPort( 80 );

	connect( &m_sockRemote, SIGNAL( error( int ) ), this, SLOT( connectError() ) );
	connect( &m_sockRemote, SIGNAL( connected() ), this, SLOT( sendRequest() ) );
	connect( &m_sockRemote, SIGNAL( readyRead() ), this, SLOT( readRemote() ) );

	uint i = 0;
	Server* server = 0;
	for ( i = MIN_PROXYPORT; i <= MAX_PROXYPORT; i++ )
	{
		server = new Server( i, this );
		kdDebug(66666) << k_funcinfo <<
			"Trying to bind to port: " << i << endl;
		if ( server->ok() )     // found a free port
			break;
		delete server;
	}

	if ( i > MAX_PROXYPORT )
	{
		kdWarning(66666) << k_funcinfo <<
			"Unable to find a free local port. Aborting." << endl;
		m_initSuccess = false;
		return;
	}
	m_usedPort = i;
	connect( server, SIGNAL( connected( int ) ), this, SLOT( accept( int ) ) );
}


Proxy::~Proxy()
{
    kdDebug(66666) << k_funcinfo << endl;
    delete[] m_pBuf;
}


//////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC
//////////////////////////////////////////////////////////////////////////////////////////

KURL Proxy::proxyUrl()
{
	if ( m_initSuccess )
	{
		KURL url;
		url.setPort( m_usedPort );
		url.setHost( "localhost" );
		url.setProtocol( "http" );
		return url;
	}
	else
		return m_url;
}


//////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE SLOTS
//////////////////////////////////////////////////////////////////////////////////////////

void Proxy::accept( int socket ) //SLOT
{
	//kdDebug(66666) << "BEGIN " << k_funcinfo << endl;
	m_sockProxy.setSocket( socket );
	m_sockProxy.waitForMore( KProtocolManager::readTimeout() * 1000 );
	connectToHost();
	//kdDebug(66666) << "END " << k_funcinfo << endl;
}


void Proxy::connectToHost() //SLOT
{
	//kdDebug(66666) << "BEGIN " << k_funcinfo << endl;

	{ //initialisations
		m_connectSuccess = false;
		m_headerFinished = false;
		m_headerStr = "";
	}

	{ //connect to server
		QTimer::singleShot( KProtocolManager::connectTimeout() * 1000,
			this, SLOT( connectError() ) );

		kdDebug(66666) << k_funcinfo << "Connecting to " <<
			m_url.host() << ":" << m_url.port() << endl;

		m_sockRemote.connectToHost( m_url.host(), m_url.port() );
	}

	//kdDebug(66666) << "END " << k_funcinfo << endl;
}


void Proxy::sendRequest() //SLOT
{
	//kdDebug(66666) << "BEGIN " << k_funcinfo << endl;

	QCString username = m_url.user().utf8();
	QCString password = m_url.pass().utf8();
	QCString authString = KCodecs::base64Encode( username + ":" + password );
	bool auth = !( username.isEmpty() && password.isEmpty() );

	QString request = QString( "GET %1 HTTP/1.0\r\n"
		"Host: %2\r\n"
		"User-Agent: Noatun/%5\r\n"
		"%3"
		"%4"
		"\r\n" )
		.arg( m_url.path( -1 ).isEmpty() ? "/" : m_url.path( -1 ) )
		.arg( m_url.host() )
		.arg( m_icyMode ? QString( "Icy-MetaData:1\r\n" ) : QString::null )
		.arg( auth ? QString( "Authorization: Basic " ).append( authString ) : QString::null )
		.arg( NOATUN_VERSION );

	m_sockRemote.writeBlock( request.latin1(), request.length() );

	//kdDebug(66666) << "END " << k_funcinfo << endl;
}


void Proxy::readRemote() //SLOT
{
	m_connectSuccess = true;
	Q_LONG index = 0;
	Q_LONG bytesWrite = 0;
	Q_LONG bytesRead = m_sockRemote.readBlock( m_pBuf, BUFSIZE );
	if ( bytesRead == -1 )
	{
		kdDebug(66666) << k_funcinfo << "Could not read remote data from socket, aborting" << endl;
		m_sockRemote.close();
		emit proxyError();
		return;
	}

	if ( !m_headerFinished )
	{
		if ( !processHeader( index, bytesRead ) )
			return;
	}

	//This is the main loop which processes the stream data
	while ( index < bytesRead )
	{
		if ( m_icyMode && m_metaInt && ( m_byteCount == m_metaInt ) )
		{
			m_byteCount = 0;
			m_metaLen = m_pBuf[ index++ ] << 4;
		}
		else if ( m_icyMode && m_metaLen )
		{
			m_metaData.append( m_pBuf[ index++ ] );
			--m_metaLen;

			if ( !m_metaLen )
			{
				transmitData( m_metaData );
				m_metaData = "";
			}
		}
		else
		{
			bytesWrite = bytesRead - index;

			if ( m_icyMode && bytesWrite > m_metaInt - m_byteCount )
				bytesWrite = m_metaInt - m_byteCount;
			bytesWrite = m_sockProxy.writeBlock( m_pBuf + index, bytesWrite );

			if ( bytesWrite == -1 )
			{
				error();
				return;
			}

			index += bytesWrite;
			m_byteCount += bytesWrite;
		}
	}
}


void Proxy::connectError() //SLOT
{
	if ( !m_connectSuccess )
	{
		kdWarning(66666) <<
			"TitleProxy error: Unable to connect to this stream " <<
			"server. Can't play the stream!" << endl;

		emit proxyError();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
//////////////////////////////////////////////////////////////////////////////////////////

bool Proxy::processHeader( Q_LONG &index, Q_LONG bytesRead )
{
	while ( index < bytesRead )
	{
		m_headerStr.append( m_pBuf[ index++ ] );
		if ( m_headerStr.endsWith( "\r\n\r\n" ) )
		{
			/*kdDebug(66666) << k_funcinfo <<
				"Got shoutcast header: '" << m_headerStr << "'" << endl;*/

			// Handle redirection
			QString loc( "Location: " );
			int index = m_headerStr.find( loc );
			if ( index >= 0 )
			{
				int start = index + loc.length();
				int end = m_headerStr.find( "\n", index );
				m_url = m_headerStr.mid( start, end - start - 1 );

				kdDebug(66666) << k_funcinfo <<
					"Stream redirected to: " << m_url << endl;

				m_sockRemote.close();
				connectToHost();
				return false;
			}


			if (m_headerStr.startsWith("ICY"))
			{
				m_metaInt = m_headerStr.section( "icy-metaint:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 ).toInt();
				m_bitRate = m_headerStr.section( "icy-br:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				m_streamName = m_headerStr.section( "icy-name:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				m_streamGenre = m_headerStr.section( "icy-genre:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				m_streamUrl = m_headerStr.section( "icy-url:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
			}
			else // not ShoutCast
			{
				QString serverName = m_headerStr.section( "Server:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				kdDebug(66666) << k_funcinfo << "Server name: " << serverName << endl;

				if (serverName == "Icecast")
				{
					m_metaInt = 0;
					m_streamName = m_headerStr.section( "ice-name:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
					m_streamGenre = m_headerStr.section( "ice-genre:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
					m_streamUrl = m_headerStr.section( "ice-url:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				}
				else if (serverName.startsWith("icecast/1."))
				{
					m_metaInt = 0;
					m_bitRate = m_headerStr.section( "x-audiocast-bitrate:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
					m_streamName = m_headerStr.section( "x-audiocast-name:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
					m_streamGenre = m_headerStr.section( "x-audiocast-genre:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
					m_streamUrl = m_headerStr.section( "x-audiocast-url:", 1, 1, QString::SectionCaseInsensitiveSeps ).section( "\r", 0, 0 );
				}
			}

			if ( m_streamUrl.startsWith( "www.", true ) )
				m_streamUrl.prepend( "http://" );

			m_sockProxy.writeBlock( m_headerStr.latin1(), m_headerStr.length() );
			m_headerFinished = true;

			if ( m_icyMode && !m_metaInt )
			{
				error();
				return false;
			}

			connect( &m_sockRemote, SIGNAL( connectionClosed() ),
				this, SLOT( connectError() ) );
			return true;
		}
	}
	return false;
}


void Proxy::transmitData( const QString &data )
{
	/*kdDebug(66666) << k_funcinfo <<
		" received new metadata: '" << data << "'" << endl;*/

	//prevent metadata spam by ignoring repeated identical data
	//(some servers repeat it every 10 seconds)
	if ( data == m_lastMetadata )
		return;

	m_lastMetadata = data;

	emit metaData(
		m_streamName, m_streamGenre, m_streamUrl, m_bitRate,
		extractStr(data, QString::fromLatin1("StreamTitle")),
		extractStr(data, QString::fromLatin1("StreamUrl")));
}


void Proxy::error()
{
	kdDebug(66666) <<
		"TitleProxy error: Stream does not support shoutcast metadata. " <<
		"Restarting in non-metadata mode." << endl;

	m_sockRemote.close();
	m_icyMode = false;

	//open stream again,  but this time without metadata, please
	connectToHost();
}


QString Proxy::extractStr( const QString &str, const QString &key )
{
	int index = str.find( key, 0, true );
	if ( index == -1 )
	{
		return QString::null;
	}
	else
	{
		index = str.find( "'", index ) + 1;
		int indexEnd = str.find( "'", index );
		return str.mid( index, indexEnd - index );
	}
}

#include "titleproxy.moc"
