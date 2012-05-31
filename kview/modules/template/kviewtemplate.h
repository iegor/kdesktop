/* This file is in the public domain */

// $Id: kviewtemplate.h 163139 2002-06-25 20:06:33Z mkretz $

#ifndef __kviewtemplate_h
#define __kviewtemplate_h

#include <kparts/plugin.h>

namespace KImageViewer { class Viewer; }

class KViewTemplate : public KParts::Plugin
{
	Q_OBJECT
public:
	KViewTemplate( QObject* parent, const char* name, const QStringList & );
	virtual ~KViewTemplate();

private slots:
	void yourSlot();

private:
	KImageViewer::Viewer * m_pViewer;
};

// vim:sw=4:ts=4:cindent
#endif
