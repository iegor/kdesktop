#ifndef NEXPLUG_H
#define NEXPLUG_H

#include <kprocess.h>
#include <noatun/plugin.h>

class NexPlugin : public QObject, public Plugin
{
Q_OBJECT
	
public:
	NexPlugin();
	virtual ~NexPlugin();

	void init();

private slots:
	void processExited(KProcess *);

private:
	KProcess process;
};


#endif 
