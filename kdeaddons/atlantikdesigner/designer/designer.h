#ifndef ATLANTK_DESIGNER_H
#define ATLANTK_DESIGNER_H

#include <qcolor.h>
#include <qguardedptr.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kmainwindow.h>

#include "boardinfo.h"
#include "group.h"
#include "editor.h"

class EstateEdit;
class QCloseEvent;
class KListAction;
class KRecentFilesAction;
class Estate;
class Player;
class ConfigEstate;
class AtlantikBoard;

class AtlanticDesigner : public KMainWindow
{
	Q_OBJECT

	public:
	AtlanticDesigner(QWidget *parent = 0, const char *name = 0);
	~AtlanticDesigner();

	protected:
	void closeEvent(QCloseEvent *);

	public slots:
	void modified();

	private slots:
	void open();
	void openRecent(const KURL &);
	void openNew();
	void save();
	void saveAs();
	void copy();
	void paste();
	void changeEstate(int);
	void changeEstate(Estate *);
	void movePlayer(Estate *);
	void setPlayerAtBeginning();
	void info();
	void editGroups();
	void groupsChanged();
	void goChanged(int);
	void updateBackground();

	void up();
	void down();
	void left();
	void right();

	void smaller();
	void larger();

	void updateJumpMenu();

	private:
	void openFile(const QString &);
	bool warnClose();
	void initBoard();
	void initMembers();
	void initToken();
	bool firstBoard;
	ConfigEstate *newEstate(int);

	QGuardedPtr<EstateEdit> editor;
	QGuardedPtr<QVBoxLayout> layout;
	QGuardedPtr<AtlantikBoard> board;
	QGuardedPtr<GroupEditor> groupEditor;
	QGuardedPtr<BoardInfoDlg> boardInfoDlg;
	EstateList estates;
	KListAction *estateAct;
	KRecentFilesAction *recentAct;

	QValueList<CardStack> cards;
	BoardInfo boardInfo;
	ConfigEstateGroupList groups;

	QString filename;

	ConfigEstate *copiedEstate;

	void doCaption(bool);

	int max;
	QStringList types;

	Player *m_player;

	bool isMod;

	QColor defaultFg;
	QColor defaultBg;
};

#endif
