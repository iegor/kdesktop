#ifndef PLUGIN_DEPS_H
#define PLUGIN_DEPS_H

#include <qtimer.h>
#include <qobject.h>
#include <noatunarts.h>

class Visualization;
class NoatunListener;


class TimerThingy : public QObject
{
Q_OBJECT
public:
	TimerThingy(Visualization*);

	void setInterval(int msec);
	void stop();
	void start();

public:
	virtual void timerEvent(QTimerEvent *);

private:
	Visualization *mVis;
	int id;
	int ms;
};

class NoatunListenerNotif : public Noatun::Listener_skel
{
public:
	NoatunListenerNotif(NoatunListener *);

	virtual void message();
	
	operator Noatun::Listener() { return Noatun::Listener::_from_base(_copy()); }
private:
	NoatunListener *mListener;
};

#endif

