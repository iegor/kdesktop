/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>

	this file is X11 source
 */

#ifndef SYNABLEH_H
#define SYNABLEH_H

#include "syna.h"
#include <kprocess.h>

class SynaePrefs;
class KProcess;

class SynaeScope : public QObject, public Plugin
{
Q_OBJECT

public:
	SynaeScope();
	virtual ~SynaeScope();

	void init();
	void runScope();

private slots:
	void processExited(KProcess *);
	void readConfig();
	void receivedStdout(KProcess *, char *buf, int len);
	void receivedStderr(KProcess *, char *buf, int len);

private:
	KProcess process;
	QString scopeExePath;
	bool restarting;
	SynaePrefs *mPrefs;
};

#endif
