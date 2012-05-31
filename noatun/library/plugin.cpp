#include <qtimer.h>
#include <qfile.h>
#include <artsflow.h>
#include <vector>
#include <artsflow.h>
#include <soundserver.h>
#include <noatunarts.h>
#include <dcopclient.h>
#include <dispatcher.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include <cmath>

#include "plugin_deps.h"
#include "player.h"
#include "plugin.h"
#include "pluginloader.h"
#include "app.h"
#include "engine.h"

using std::vector;

Arts::Dispatcher *Visualization::mDispatcher=0;
bool Visualization::internalVis=false;

Plugin::Plugin()
{
}

Plugin::~Plugin()
{
}

void Plugin::init()
{
}

bool Plugin::unload()
{
	return napp->libraryLoader()->remove(this);
}

TimerThingy::TimerThingy(Visualization *vis)
	: mVis(vis), id(-1)
{
}

void TimerThingy::timerEvent(QTimerEvent *)
{
	mVis->timeout();
}

void TimerThingy::setInterval(int msec)
{
	ms=msec;
	if (id!=-1)
	{
		killTimer(id);
		id=startTimer(msec);
	}
}

void TimerThingy::start()
{
	if (id==-1)
		id=startTimer(ms);
}

void TimerThingy::stop()
{
	if (id==-1)
	{
		killTimer(id);
		id=-1;
	}
}


Visualization::Visualization(int timeout, int pid)
{
	mTimerThingy=new TimerThingy(this);
	setInterval(timeout);

	// if this is a fork, do a cutesy arts thingy to get a remote
	// stack, otherwise, get it from localhost :)
	{
		int parent=pid ? pid : getppid();

		if (getenv("NOATUN_PID"))
			parent = QString::fromLatin1(getenv("NOATUN_PID")).toInt();

		DCOPClient c;
		c.attach();

		QCString appids[2];
		appids[0]=QString("noatun-%1").arg(parent).local8Bit();
		appids[1]="noatun";
		QCString &appid=appids[0];

		if (!internalVis && c.isApplicationRegistered(appids[0]))
		{
			appid=appids[0];
		}
		else if (!internalVis && c.isApplicationRegistered(appids[1]))
		{
			appid=appids[1];
		}
		else
		{
			kdDebug(66666) << "Internal Vis" <<endl;
			mVisualizationStack=napp->player()->engine()->visualizationStack()->toString().c_str();
			mServer=new Arts::SoundServerV2(*(napp->player()->engine()->server()));
			return;
		}

		{
			QByteArray replyData;
			QCString replyType;

			if (!c.call(appid, "Noatun", "visStack()", QByteArray(), replyType, replyData))
			{
				kdDebug(66666) << "Error communicating to parent noatun" << endl;
			}
			else
			{
				initDispatcher();
				mServer=new Arts::SoundServerV2;
				*mServer = Arts::Reference("global:Arts_SoundServerV2");
				QDataStream reply(replyData, IO_ReadOnly);
				QCString result;
				reply >> result;
				mVisualizationStack=result;
			}
		}
	}
}

Visualization::~Visualization()
{
//	napp->player()->engine()->visualizationStack()->remove(mId);

	delete mServer;
	delete mTimerThingy;
}

void Visualization::start()
{
	mTimerThingy->start();
}

void Visualization::stop()
{
	mTimerThingy->stop();
}

void Visualization::setInterval(int msec)
{
	mTimeout=msec;
	if (!msec)
		stop();
	mTimerThingy->setInterval(msec);
}

int Visualization::interval() const
{
	return mTimeout;
}

Noatun::StereoEffectStack Visualization::visualizationStack()
{
	return Arts::Reference(mVisualizationStack);
}

Arts::SoundServerV2 *Visualization::server()
{
	return mServer;
}

int Visualization::noatunPid()
{
	DCOPClient dcop;
	dcop.attach();
	QCStringList apps = dcop.registeredApplications();
	for (QCStringList::Iterator i = apps.begin(); i != apps.end(); ++i )
		if ((*i).left(9) != "anonymous" )
		{
			if ((*i).left(6) != "noatun")
				continue;
			int pid=(*i).mid((*i).find('-')+1).toInt();
			return pid;
		}
	return -1;
}

bool Visualization::connected()
{
	server()->_interfaceName(); // makes error() work
	return !(server()->error() || server()->isNull());
}

void Visualization::initDispatcher()
{
	if (!mDispatcher)
	{
		mDispatcher=new Arts::Dispatcher;
	}
}

FFTScope::FFTScope(int interval, int pid) : Visualization(interval, pid)
{
}

float FFTScope::magic(int bands)
{
/*	QString path=locate("data", "noatun/magictable");
	QFile magic(path);
	if (!magic.open(IO_ReadOnly | IO_Raw))
		return 0;
	if (!magic.at(bands*sizeof(float)))
		return 0;

	float value;
	if (magic.readBlock((char*)&value, sizeof(float))==-1)
		value=0;
*/
	bands += 20-1; // index-1 from FFTScopes.cpp
	float value = std::log(std::pow(2046.0, 1.0/bands));

	return value;
}


StereoFFTScope::StereoFFTScope(int timeout, int pid) : FFTScope(timeout, pid)
{
	mScope=new Noatun::FFTScopeStereo;
	*mScope=Arts::DynamicCast(server()->createObject("Noatun::FFTScopeStereo"));

	if ((*mScope).isNull())
	{
		delete mScope;
		mScope=0;
	}
	else
	{
		mScope->start();
		mId=visualizationStack().insertBottom(*mScope, "Noatun FFT");
	}
}

StereoFFTScope::~StereoFFTScope()
{
	if (mScope)
	{
		if (connected())
			visualizationStack().remove(mId);
		mScope->stop();
		delete mScope;
	}
}

void StereoFFTScope::scopeData(vector<float> *&left, vector<float> *&right)
{
	left=mScope->scopeLeft();
	right=mScope->scopeRight();
}

void StereoFFTScope::timeout()
{
	vector<float> *left, *right;
	scopeData(left, right);

	if (left->size())
		scopeEvent(&left->front(), &right->front(), left->size());
	delete left;
	delete right;

}

int StereoFFTScope::bands() const
{
	vector<float> *d=mScope->scopeLeft();
	int size=d->size();
	delete d;
	return size;
}

void StereoFFTScope::setBands(float f)
{
	mScope->bandResolution(f);
}



MonoFFTScope::MonoFFTScope(int timeout, int pid) : FFTScope(timeout, pid)
{
	mScope=new Noatun::FFTScope;
	*mScope=Arts::DynamicCast(server()->createObject("Noatun::FFTScope"));

	if ((*mScope).isNull())
	{
		delete mScope;
		mScope=0;
	}
	else
	{
		mScope->start();
		mId=visualizationStack().insertBottom(*mScope, "Noatun FFT");
	}
}

MonoFFTScope::~MonoFFTScope()
{
	if (mScope)
	{
		if (connected())
			visualizationStack().remove(mId);
		mScope->stop();
		delete mScope;
	}
}

void MonoFFTScope::scopeData(vector<float> *&data)
{
	data=mScope->scope();
}

void MonoFFTScope::timeout()
{
	vector<float> *data(mScope->scope());

	float *f=&data->front();
	if (data->size()) scopeEvent(f, data->size());
	delete data;
}

int MonoFFTScope::bands() const
{
	vector<float> *d=mScope->scope();
	int size=d->size();
	delete d;
	return size;

}

void MonoFFTScope::setBands(float f)
{
	mScope->bandResolution(f);
}






Scope::Scope(int interval, int pid) : Visualization(interval, pid)
{
}



MonoScope::MonoScope(int timeout, int pid) : Scope(timeout, pid)
{
	mScope=new Noatun::RawScope;
	*mScope=Arts::DynamicCast(server()->createObject("Noatun::RawScope"));

	if ((*mScope).isNull())
	{
		delete mScope;
		mScope=0;
	}
	else
	{
		mScope->start();
		mId=visualizationStack().insertBottom(*mScope, "Noatun Scope");
	}
}

MonoScope::~MonoScope()
{
	if (mScope)
	{
		if (connected())
			visualizationStack().remove(mId);
		mScope->stop();
		delete mScope;
	}
}

void MonoScope::scopeData(vector<float> *&data)
{
	data=mScope->scope();
}

void MonoScope::timeout()
{
	vector<float> *data(mScope->scope());

	float *f=&data->front();
	if (data->size()) scopeEvent(f, data->size());
	delete data;
}

int MonoScope::samples() const
{
	return (long)mScope->buffer();
}

void MonoScope::setSamples(int len)
{
	mScope->buffer((long)len);
}



StereoScope::StereoScope(int timeout, int pid) : Scope(timeout, pid)
{
	mScope=new Noatun::RawScopeStereo;
	*mScope=Arts::DynamicCast(server()->createObject("Noatun::RawScopeStereo"));

	if ((*mScope).isNull())
	{
		delete mScope;
		mScope=0;
	}
	else
	{
		mScope->start();
		mId=visualizationStack().insertBottom(*mScope, "Noatun Stereo Scope");
	}
}

StereoScope::~StereoScope()
{
	if (mScope)
	{
		if (connected())
			visualizationStack().remove(mId);
		mScope->stop();
		delete mScope;
	}
}

void StereoScope::scopeData(vector<float> *&left, vector<float> *&right)
{
	left=mScope->scopeLeft();
	right=mScope->scopeRight();
}

void StereoScope::timeout()
{
	vector<float> *left(mScope->scopeLeft());
	vector<float> *right(mScope->scopeRight());

	float *l=&left->front();
	float *r=&right->front();

	if (left->size()==right->size() && left->size())
		scopeEvent(l, r, left->size());
	delete left;
	delete right;
}

int StereoScope::samples() const
{
	return (long)mScope->buffer();
}

void StereoScope::setSamples(int len)
{
	mScope->buffer((long)len);
}



NoatunListener::NoatunListener(QObject *parent) : QObject(parent)
{ }

NoatunListener::~NoatunListener()
{ }

void NoatunListener::message()
{
	emit event();
}

NoatunListenerNotif::NoatunListenerNotif(NoatunListener *l)
{
	mListener=l;
}

void NoatunListenerNotif::message()
{
	mListener->message();
}


ExitNotifier::ExitNotifier(int pid, QObject *parent) : NoatunListener(parent)
{
	mNotif=new NoatunListenerNotif(this);

	DCOPClient c;
	c.attach();


	QCString appids[2];
	appids[0]=QString("noatun-%1").arg(pid).local8Bit();
	appids[1]="noatun";
	appid=appids[0];

	if (c.isApplicationRegistered(appids[0]))
	{
		appid=appids[0];
	}
	else if (c.isApplicationRegistered(appids[1]))
	{
		appid=appids[1];
	}
	else
	{
		return;
	}

	QByteArray replyData;
	QCString replyType;

	QCString sessionName;

	if (!c.call(appid, "Noatun", "session()", QByteArray(), replyType, replyData))
	{
		kdDebug(66666) << "Error communicating to parent noatun" << endl;
	}
	else
	{
		QDataStream reply(replyData, IO_ReadOnly);
		reply >> sessionName;
	}

	Visualization::initDispatcher();

	Noatun::Session session=Arts::Reference(sessionName);
	session.addListener(*mNotif);
}

ExitNotifier::~ExitNotifier()
{
	QByteArray replyData;
	QCString replyType;

	QCString sessionName;

	DCOPClient c;
	c.attach();

	if (c.call(appid, "Noatun", "session()", QByteArray(), replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		reply >> sessionName;

		Noatun::Session session=Arts::Reference(sessionName);
		session.removeListener(*mNotif);
	}
	delete mNotif;
}

BoolNotifier::BoolNotifier(bool *value, NoatunListener *listener, QObject *parent)
	: QObject(parent)
{
	connect(listener, SIGNAL(event()), SLOT(event()));
	mValue=value;
}

SessionManagement::SessionManagement() { }
SessionManagement::~SessionManagement() { }
void SessionManagement::restore() { }

#include "plugin.moc"
#include "plugin_deps.moc"
