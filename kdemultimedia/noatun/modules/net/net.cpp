#include "net.h"
#include <noatun/player.h>
#include <noatun/app.h>

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new Net();
	}
}


Net::Net() : QServerSocket(7539, 10), Plugin()
{
	mFDs.setAutoDelete(true);
	connect(napp->player(), SIGNAL(newSong()), SLOT(newSong()));
}

Net::~Net()
{
}


void Net::newConnection(int fd)
{
	QSocket *s=new QSocket;
	s->setSocket(fd);
	mFDs.append(s);
}

void Net::newSong()
{
	if (!napp->player()->current())
		return;
		
	for (QSocket *i=mFDs.first(); i!=0; i=mFDs.next())
	{
		QCString line;
		line=napp->player()->current().title().latin1();
		line+='\n';
		::write(i->socket(), (const void*)line.data(), line.length());
	}
}

void Net::closed()
{
	for (QSocket *i=mFDs.first(); i!=0; i=mFDs.next())
	{
		if (sender()==i)
			mFDs.removeRef(i);
	}
}



#include "net.moc"
