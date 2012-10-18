#ifndef _WASKINMANAGER_H
#define _WASKINMANAGER_H

#include <dcopobject.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qmap.h>

class WaSkinManager : public QObject, public DCOPObject {
Q_OBJECT
K_DCOP

public:
	WaSkinManager();
	~WaSkinManager();

k_dcop:
	QStringList availableSkins();
	QString currentSkin();
	bool loadSkin(QString skinName);

	QString defaultSkin();

	bool installSkin(QString url);

	bool skinRemovable(QString skinName);
	bool removeSkin(QString skinName);

	QStringList skinMimeTypes();

signals:
	void updateSkinList();

private:
	QString mCurrentSkin;
};

#endif
