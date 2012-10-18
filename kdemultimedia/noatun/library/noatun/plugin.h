#ifndef NPLUGIN_H
#define NPLUGIN_H

#include <noatun/pluginloader.h>
#include <qmemarray.h>
#include <vector>
#include <kdemacros.h>

namespace Noatun { class FFTScopeStereo; class FFTScope; class RawScope;
                   class RawScopeStereo; class StereoEffectStack;
                   class Listener;
                 }
namespace Arts { class SoundServerV2; class Dispatcher; }

//backwards compatibility

#define NOATUNPLUGIND
#define NOATUNPLUGINC(classname)  { }

class Playlist;
class Player;
class ExitNotifier;
class NoatunApp;
/**
 * @short Base class for all plugins
 **/
class KDE_EXPORT Plugin
{
public:
	Plugin();
	virtual ~Plugin();

	/**
	 * called directly after the plugin, just in case
	 * you want Noatun to be "ready" with your class
	 **/
	virtual void init();

	/**
	 * unload the plugin
	 * if it returns true, return from your function
	 * immediately and don't access members of this
	 * TODO
	 **/
	virtual bool unload();

	/**
	 * make multiple inheritence work
	 * only needed with playlist plugins
	 * generally "return this" in here
	 **/
	virtual Playlist *playlist() { return 0; }
};

/**
 * @short Base class for user-interface plugins
 *
 * Inherit from this one instead of Plugin if you are
 * a user-interface
 **/
class KDE_EXPORT UserInterface : public Plugin
{
public:
	UserInterface();
	virtual ~UserInterface();
};


class TimerThingy;


/**
 * @short Base class for all sorts of visualizations
 *
 * all Noatun Visualizations can be in
 * separate processes!  You must fork, and
 * then exec() to be able to use this.
 * Its perfectly safe to create
 * a visualization in a binary executed by
 * KProcess.
 **/
class Visualization
{
friend class TimerThingy;
friend class ThingThatTellsAVisualizationThatNoatunIsGone;
friend class ExitNotifier;
friend class NoatunApp;
public:
	/**
	 * interval is how frequently the rescope
	 * will occur
	 * 0 means disabled
	 *
	 * Pid, if not zero, can force the visualizaton
	 * to use another noatun's process's visualization stack,
	 * if it is zero, it'l try to guess what to use
	 **/
	Visualization(int interval=125, int pid=0);
	virtual ~Visualization();

	/**
	 * start the timer, if it's 0, then this
	 * will do nothing.
	 **/
	virtual void start();
	/**
	 * stop the timer
	 **/
	virtual void stop();

	/**
	 * how long between each rescoping in milliseconds
	 **/
	int interval() const;

	/**
	 * set how long to wait
	 * the change takes effect immediately,
	 * unless it hadn't been start()ed beforehand
	 **/
	virtual void setInterval(int msecs);

	/**
	 * cause a rescoping to take effect immediately
	 **/
	virtual void timeout()=0;

	Noatun::StereoEffectStack visualizationStack();
	Arts::SoundServerV2 *server();

	/**
	 * return noatun's pid, useful if you're doing remote-visualization
	 *
	 * It returns the pid to use, or -1, if there was an error, or
	 * there isn't a noatun running. If there isn't a noatun running, the
	 * computer will crash, the trains will take the wrong tracks, time
	 * will start moving in the wrong direction, and the whole universe will melt
	 * down.
	 **/
	static int noatunPid();

	/**
	 * test if the server is still there
	 **/
	bool connected();

	/**
	 * create a dispatcher object , does nothing if
	 * this already has been called
	 **/
	static void initDispatcher();

private:
	int mTimeout;
	TimerThingy *mTimerThingy;
	QCString mVisualizationStack;
	Arts::SoundServerV2 *mServer;
	static Arts::Dispatcher *mDispatcher;
	static bool internalVis;
};

/**
 * Base class for all kinds of FFT scopes
 **/
class KDE_EXPORT FFTScope : public Visualization
{
public:
	FFTScope(int interval, int pid=0);

	/**
	 * the "width" of each scopeEvent
	 **/
	virtual int bands() const=0;

	/**
	 * returns the magic number that you can
	 * give to "setBands".  For example:
	 * <pre>
	 * setBands(magic(50));
	 * bands()==50
	 * </pre>
	 **/
	static float magic(int bands);

	/**
	 * set the width combiner value.  in theory,
	 * a value of 1.0 should give you exactly
	 * 1024 samples, greater values will decrease
	 * the amount quite rapidly.  by default,
	 * 1.1 is used, which gives 50.  You'll have
	 * to tweak this manually.
	 *
	 * This will only return valid responses
	 * for values between 10 and 1024
	 *
	 * This function is a terrible hack, and we apologize
	 * for it. The values of these magic numbers
	 * do occasionally change, so you must use
	 * @ref magic
	 **/
	virtual void setBands(float n)=0;
};

/**
 * An easy to use FFT scope, stereo version.
 * You certainly want to reimplement scopeEvent()
 **/
class KDE_EXPORT StereoFFTScope : public FFTScope
{
public:
	StereoFFTScope(int timeout=250, int pid=0);
	virtual ~StereoFFTScope();

	/**
	 * called according to the timeout
	 * the two floats contain the data, with @p len items
	 * you override this yourself
	 **/
	virtual void scopeEvent(float *left, float *right, int len)
		{ (void)left; (void)right; (void)len; }

	/**
	 * get the current data
	 * pass two vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&left, std::vector<float> *&right);
	virtual void timeout();

	virtual int bands() const;
	virtual void setBands(float f);

private:
	Noatun::FFTScopeStereo *mScope;
	long mId;
};


/**
 * An easy to use FFT scope, mono version.
 * You certainly want to reimplement scopeEvent()
 **/
class KDE_EXPORT MonoFFTScope : public FFTScope
{
public:
	MonoFFTScope(int timeout=250, int pid=0);
	virtual ~MonoFFTScope();

	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself
	 **/
	virtual void scopeEvent(float *data, int len)
		{ (void)data; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&data);

	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();

	virtual int bands() const;
	virtual void setBands(float f);

private:
	Noatun::FFTScope *mScope;
	long mId;
};


/**
 * Base class for all kinds of scope visualizations, i.e. if you want to display
 * the currently played waveform
 **/
class Scope : public Visualization
{
public:
	Scope(int interval, int pid=0);

	/**
	 * the "width" of each scopeEvent
	 **/
	virtual int samples() const=0;

	virtual void setSamples(int )=0;
};

/**
 * An easy to use scope visualization, mono version.
 * Note: Of course this one also works for audio with more than one channel
 * You certainly want to reimplement scopeEvent()
 */
class KDE_EXPORT MonoScope : public Scope
{
public:
	MonoScope(int timeout=250, int pid=0);
	virtual ~MonoScope();

	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself, and process the data
	 **/
	virtual void scopeEvent(float *data, int len)
		{(void)data; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&data);

	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();

	virtual int samples() const;
	virtual void setSamples(int);

private:
	Noatun::RawScope *mScope;
	long mId;
};

/**
 * An easy to use scope visualization, stereo version.
 * You certainly want to reimplement scopeEvent()
 **/
class StereoScope : public Scope
{
public:
	StereoScope(int timeout=250, int pid=0);
	virtual ~StereoScope();

	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself, and process the data
	 **/
	virtual void scopeEvent(float *left, float *right, int len)
		{ (void)left; (void)right; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&left, std::vector<float> *&right);

	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();

	virtual int samples() const;
	virtual void setSamples(int);


private:
	Noatun::RawScopeStereo *mScope;
	long mId;
};

/**
 * @short Base class for session manager plugins
 *
 * Inherit from this one instead of Plugin if you are
 * a session manager plugin
 **/
class SessionManagement : public Plugin
{
public:
	SessionManagement();
	virtual ~SessionManagement();

	virtual void restore();
};

class NoatunListenerNotif;
/**
 * @short Abstract base class to monitor noatun
 *
 * So far only used for ExitNotifier.
 **/
class NoatunListener : public QObject
{
Q_OBJECT
friend class NoatunListenerNotif;

public:
	NoatunListener(QObject *parent=0);
	virtual ~NoatunListener();

signals:
	void event();

protected:
	virtual void message();

	NoatunListenerNotif *mNotif;
};

/**
 * @short Notifies you as soon as Noatun exits
 *
 * Link to libnoatun, and the signal event() will
 * be emitted whenever noatun exits, and the best
 * part is how it doesn't matter if noatun exits
 * cleanly so you even get informed in case noatun
 * crashes.
 **/
class ExitNotifier : public NoatunListener
{
public:
	ExitNotifier(int pid, QObject *parent=0);
	virtual ~ExitNotifier();

private:
	QCString appid;
};

/**
 * @short Wrapper to set a bool variable as soon as Noatun exits
 * This class can even be used when you cannot use signals/slots
 * Example:
 * <pre>
 * bool noatunOk = false;
 * new BoolNotifier(&noatunOk, new ExitNotifier(this), this);
 * </pre>
 *
 * When noatunOk is false, then noatun has exited somehow.
 **/
class BoolNotifier : public QObject
{
Q_OBJECT
public:
	BoolNotifier(bool *value, NoatunListener *listener, QObject *parent=0);

private slots:
	void event() {*mValue=false;}

private:
	bool *mValue;
};

#endif

