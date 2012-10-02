
#include <klocale.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "dbse_factory.h"
#include "KDBSearchEngine.h"


extern "C"
{
	KDE_EXPORT void *init_kbabeldict_dbsearchengine()
	{
		return new DbSeFactory;
	}
}


KInstance *DbSeFactory::s_instance = 0;
KAboutData *DbSeFactory::s_about = 0;


DbSeFactory::DbSeFactory( QObject *parent, const char *name)
		: KLibFactory(parent,name)
{
}

DbSeFactory::~DbSeFactory()
{
	if(s_instance)
	{
		delete s_instance;
		s_instance=0;
	}

	if(s_about)
	{
		delete s_about;
		s_about=0;
	}
}


QObject *DbSeFactory::createObject( QObject *parent, const char *name,
				    const char *classname, const QStringList &)
{
	if(QCString(classname) != "SearchEngine")
	{
		kdError() << "not a SearchEngine requested" << endl;
		return 0;
	}
	
	KDBSearchEngine *se = new KDBSearchEngine(parent,name);

	emit objectCreated(se);
	return se;
}


KInstance *DbSeFactory::instance()
{
	if(!s_instance)
	{

		s_about = new KAboutData( "kdbsearchengine",
			      I18N_NOOP("Translation Database")
				, "0.3"	,
I18N_NOOP("A fast translation search engine based on databases")
						, KAboutData::License_GPL
						, I18N_NOOP("Copyright 2000-2001 by Andrea Rizzi")
						,0,0, "rizzi@kde.org");

		s_about->addAuthor("Andrea Rizzi",0,"rizzi@kde.org");

		s_instance = new KInstance(s_about);
	}

	return s_instance;
}

#include "dbse_factory.moc"
