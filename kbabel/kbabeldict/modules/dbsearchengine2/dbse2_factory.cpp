
#include <klocale.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "dbse2_factory.h"
#include "KDBSearchEngine2.h"


extern "C"
{
	KDE_EXPORT void *init_kbabeldict_dbsearchengine2()
//	void *init_libdbsearchengine2()
	{
		return new DbSe2Factory;
	}
};


KInstance *DbSe2Factory::s_instance = 0;
KAboutData *DbSe2Factory::s_about = 0;


DbSe2Factory::DbSe2Factory( QObject *parent, const char *name)
		: KLibFactory(parent,name)
{
}

DbSe2Factory::~DbSe2Factory()
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


QObject *DbSe2Factory::createObject( QObject *parent, const char *name,
				    const char *classname, const QStringList &)
{
	if(QCString(classname) != "SearchEngine")
	{
		kdError() << "not a SearchEngine requested" << endl;
		return 0;
	}
	
	KDBSearchEngine2 *se = new KDBSearchEngine2(parent,name);

	emit objectCreated(se);
	return se;
}


KInstance *DbSe2Factory::instance()
{
	if(!s_instance)
	{

		s_about = new KAboutData( "kdbsearchengine2",
			      I18N_NOOP("Translation Database")
				, "1.99"	,
I18N_NOOP("A fast translation search engine based on databases")
						, KAboutData::License_GPL
						, I18N_NOOP("Copyright 2000-2003 by Andrea Rizzi")
						,0,0, "rizzi@kde.org");

		s_about->addAuthor("Andrea Rizzi",0,"rizzi@kde.org");

		s_instance = new KInstance(s_about);
	}

	return s_instance;
}

#include "dbse2_factory.moc"
