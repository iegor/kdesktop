// Copyright (C) 2001 Charles Samuels <charles@kde.org>
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>

#ifndef TYLER_H
#define TYLER_H

#include <noatun/conversion.h>
#include <noatun/plugin.h>
#include <kprocess.h>
#include <qwidget.h>

class Tyler : public QObject, public StereoScope, public Plugin
{
Q_OBJECT

public:
	Tyler();
	virtual ~Tyler();

	void init();

private slots:
	void processExited(KProcess *);

protected:
	virtual void scopeEvent(float *left, float *right, int bands);

private:
	char *mBuffer;
	static const int bufferSize;
	KProcess process;
};

#endif
