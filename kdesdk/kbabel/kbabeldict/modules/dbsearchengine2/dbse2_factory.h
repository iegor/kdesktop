#ifndef DBSE2_FACTORY_H
#define DBSE2_FACTORY_H

#include <klibloader.h>
class KInstance;
class KAboutData;

class DbSe2Factory : public KLibFactory
{
	Q_OBJECT
public:
	DbSe2Factory( QObject *parent=0, const char *name=0);
	~DbSe2Factory();

	virtual QObject *createObject( QObject *parent=0, const char *name=0,
	                               const char *classname="QObject",
	                               const QStringList &args = QStringList());

	static KInstance *instance();

private: 
	static KInstance *s_instance;
	static KAboutData *s_about;
};

#endif
