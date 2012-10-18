/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <klocale.h>
#include "kaboodle_factory.h"
#include "player.h"

K_EXPORT_COMPONENT_FACTORY( libkaboodlepart, Kaboodle::KaboodleFactory)

KInstance *Kaboodle::KaboodleFactory::s_instance = 0;
KAboutData *Kaboodle::KaboodleFactory::s_aboutData = 0;

Kaboodle::KaboodleFactory::KaboodleFactory() : KParts::Factory()
{
}

Kaboodle::KaboodleFactory::~KaboodleFactory()
{
	delete s_aboutData;
	s_aboutData = 0;
	delete s_instance;
	s_instance = 0;
}

KParts::Part *Kaboodle::KaboodleFactory::createPartObject(QWidget *widgetParent, const char *widgetName, QObject *parent, const char *name, const char *className, const QStringList &)
{
	if(className == QString("KMediaPlayer/Engine"))
	{
		return new Player(parent, name);
	}
	else
	{
		return new Player(widgetParent, widgetName, parent, name);
	}
}

KInstance *Kaboodle::KaboodleFactory::instance()
{
	if(!s_instance)
		s_instance = new KInstance(aboutData());

	return s_instance;
}

const KAboutData *Kaboodle::KaboodleFactory::aboutData()
{
	if(!s_aboutData)
	{
		s_aboutData = new KAboutData("kaboodle", I18N_NOOP("Kaboodle"), "1.7",
		                             I18N_NOOP("The Lean KDE Media Player"),
		                             KAboutData::License_BSD,
		                             "(c) 2001-2003 Kaboodle developers", 0,
		                             "http://www.freekde.org/neil/kaboodle/");
		s_aboutData->addCredit("Carsten Pfeiffer", I18N_NOOP("Maintainer"), "pfeiffer@kde.org");
		s_aboutData->addAuthor("Neil Stevens", I18N_NOOP("Previous Maintainer"), "neil@qualityassistant.com");
		s_aboutData->addCredit("Elhay Achiam", I18N_NOOP("Application icon"));
		s_aboutData->addCredit("Charles Samuels", I18N_NOOP("Original Noatun Developer"), "charles@kde.org");
		s_aboutData->addCredit("Nikolas Zimmermann", I18N_NOOP("Konqueror Embedding"), "wildfox@kde.org");
	}
	return s_aboutData;
}

#include "kaboodle_factory.moc"
