
#ifndef _INFRARED_H_
#define _INFRARED_H_

#include <noatun/player.h>
#include <noatun/plugin.h>

class NoatunPreferences;
class Lirc;

class InfraRed : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND
public:
	InfraRed();
	~InfraRed();

private slots:
	void slotCommand(const QString &, const QString &, int);

	void start();

private:
	Lirc *m_lirc;
	int volume;
};

#endif

