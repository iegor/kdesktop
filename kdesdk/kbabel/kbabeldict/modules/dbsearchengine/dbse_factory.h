#ifndef DBSE_FACTORY_H
#define DBSE_FACTORY_H

#include <klibloader.h>
class KInstance;
class KAboutData;

class DbSeFactory : public KLibFactory
{
	Q_OBJECT
public:
	DbSeFactory( QObject *parent=0, const char *name=0);
	~DbSeFactory();

	virtual QObject *createObject( QObject *parent=0, const char *name=0,
	                               const char *classname="QObject",
	                               const QStringList &args = QStringList());

	static KInstance *instance();

private: 
	static KInstance *s_instance;
	static KAboutData *s_about;
};

#endif
