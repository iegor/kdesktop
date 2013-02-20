//
// Generated in ../server/ via dcopidl -- needs to be in the lib tho.
// Regenerate when necessary by uncommenting the watcher.stub in
// ../server/Makefile.am
//

#include "watcher_stub.h"
#include <dcopclient.h>

#include <kdatastream.h>

namespace KMrml {

Watcher_stub::Watcher_stub( const QCString& app, const QCString& obj )
  : DCOPStub( app, obj )
{
}

Watcher_stub::Watcher_stub( DCOPClient* client, const QCString& app, const QCString& obj )
  : DCOPStub( client, app, obj )
{
}

bool Watcher_stub::requireDaemon( const QCString& arg0, const QString& arg1, const QString& arg2, uint arg3, int arg4 )
{
    bool result;
    if ( !dcopClient()  ) {
	setStatus( CallFailed );
	return false;
    }
    QByteArray data, replyData;
    QCString replyType;
    QDataStream arg( data, IO_WriteOnly );
    arg << arg0;
    arg << arg1;
    arg << arg2;
    arg << arg3;
    arg << arg4;
    if ( dcopClient()->call( app(), obj(), "requireDaemon(QCString,QString,QString,uint,int)", data, replyType, replyData ) ) {
	if ( replyType == "bool" ) {
	    QDataStream _reply_stream( replyData, IO_ReadOnly );
	    _reply_stream >> result;
	    setStatus( CallSucceeded );
	} else {
	    callFailed();
	}
    } else { 
	callFailed();
    }
    return result;
}

void Watcher_stub::unrequireDaemon( const QCString& arg0, const QString& arg1 )
{
    if ( !dcopClient()  ) {
	setStatus( CallFailed );
	return;
    }
    QByteArray data, replyData;
    QCString replyType;
    QDataStream arg( data, IO_WriteOnly );
    arg << arg0;
    arg << arg1;
    if ( dcopClient()->call( app(), obj(), "unrequireDaemon(QCString,QString)", data, replyType, replyData ) ) {
	setStatus( CallSucceeded );
    } else { 
	callFailed();
    }
}

QStringList Watcher_stub::runningDaemons()
{
    QStringList result;
    if ( !dcopClient()  ) {
	setStatus( CallFailed );
	return result;
    }
    QByteArray data, replyData;
    QCString replyType;
    if ( dcopClient()->call( app(), obj(), "runningDaemons()", data, replyType, replyData ) ) {
	if ( replyType == "QStringList" ) {
	    QDataStream _reply_stream( replyData, IO_ReadOnly );
	    _reply_stream >> result;
	    setStatus( CallSucceeded );
	} else {
	    callFailed();
	}
    } else { 
	callFailed();
    }
    return result;
}

} // namespace

