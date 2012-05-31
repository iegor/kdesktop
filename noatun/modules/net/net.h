#ifndef DCOPIFACE_H
#define DCOPIFACE_H

#include <noatun/player.h>
#include <noatun/plugin.h>
#include <qserversocket.h>
#include <qsocket.h>
#include <unistd.h>

class Net : public QServerSocket, public Plugin
{
Q_OBJECT

public:
	Net();
	~Net();

public slots:
	void newSong();

private slots:
	void closed();
protected:
	virtual void newConnection(int socket);

private:
	QPtrList<QSocket> mFDs;
};

#endif

