#ifndef CMODULE_H
#define CMODULE_H

#include <qframe.h>
#include <klistview.h>
#include "noatun/pref.h"

class KListView;
class QSplitter;
class QListViewItem;
class NoatunLibraryInfo;
class QTextView;
class QButtonGroup;
class MimeTypeTree;
class KLineEdit;

namespace Arts {class TraderOffer;}

class QCheckBox;
class KURLRequester;

class General : public CModule
{
Q_OBJECT
public:
	General(QObject *parent=0);
	virtual void save();

private slots:
	void slotRequesterClicked( KURLRequester * );

private:
	QCheckBox *mLoopList, *mOneInstance, *mRememberPositions,
	          *mClearOnOpen, *mFastVolume, *mRemaining;
	QButtonGroup* mPlayOnStartup;
	KURLRequester *mDlSaver;
	KLineEdit *mTitleFormat;
};

// I'm too lazy to grep - Neil
#include "pluginmodule.h"

#endif
