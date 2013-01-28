/* Synaescope - a pretty noatun visualization (based on P. Harrison's Synaesthesia)
   Copyright (C) 1997 Paul Francis Harrison <pfh@yoyo.cc.monash.edu.au>
                 2001 Charles Samuels <charles@kde.org>
   Copyright (C) 2001 Neil Stevens<multivac@fcmail.com>

	this file is X11 source
 */

#ifndef SYNAESCOPE_H
#define SYNAESCOPE_H

#include <noatun/conversion.h>
#include "syna.h"
#include <noatun/plugin.h>
#include <kprocess.h>
#include <qwidget.h>
#include <qxembed.h>

class SynaeScope : public QWidget, public StereoScope, public Plugin
{
Q_OBJECT

public:
	SynaeScope();
	virtual ~SynaeScope();

	void init();

private slots:
	void processExited(KProcess *);
	void toggle(void);
	void read(KProcess *, char *, int);

protected:
	virtual void scopeEvent(float *left, float *right, int bands);

private:
	char *mBuffer;
	static const int bufferSize=512;
	KProcess process;
	int pluginMenuItem;
	QXEmbed *embed;
	WId id;
};

#endif
