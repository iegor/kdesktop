//
// Generated in ../server/ via dcopidl -- needs to be in the lib tho.
// Regenerate when necessary by uncommenting the watcher.stub in
// ../server/Makefile.am
//

#ifndef __WATCHER_STUB__
#define __WATCHER_STUB__

#include <dcopstub.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <kdedmodule.h>
#include <kprocess.h>

namespace KMrml {

class Watcher_stub : public DCOPStub
{
public:
    Watcher_stub( const QCString& app, const QCString& id );
    Watcher_stub( DCOPClient* client, const QCString& app, const QCString& id );
    virtual bool requireDaemon( const QCString& clientAppId, const QString& daemonKey, const QString& commandline, uint timeout, int numRestarts );
    virtual void unrequireDaemon( const QCString& clientAppId, const QString& daemonKey );
    virtual QStringList runningDaemons();
protected:
    Watcher_stub() : DCOPStub( never_use ) {};
};

} // namespace

#endif
