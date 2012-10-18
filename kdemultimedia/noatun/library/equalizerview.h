#ifndef EQUALIZERVIEW_H
#define EQUALIZERVIEW_H

#include <qwidget.h>
#include <kdialogbase.h>
#include <klistview.h>

class VBand;
class QSlider;
class QLabel;
class QListViewItem;
class VPreset;
class QHBoxLayout;
class EqualizerWidget;


class EqualizerLevel : public QWidget
{
Q_OBJECT
public:
	EqualizerLevel(QWidget *parent, VBand band);

public slots:
	void changed();
	void changed(int);

	void setBand(VBand band);

private:
	VBand mBand;
	QSlider *mSlider;
	QLabel *mLabel;
};


class PresetList : public KListView
{
Q_OBJECT
public:
	PresetList(QWidget *parent, const char *name=0);

public:
	void rename(QListViewItem *item, int c);
};


class EqualizerView : public KDialogBase
{
Q_OBJECT
	QPtrList<EqualizerLevel> mBands;

public:
	EqualizerView();
	virtual void show();

	QListViewItem *itemFor(const QString &filename);
	QListViewItem *itemFor(const VPreset &preset);

public slots:
	void setPreamp(int);
	void changedPreamp(int);

private slots:
	void changedBands();
	void changedEq();

	void removed(VPreset p);
	void created(VPreset p);
	void renamed(VPreset p);

	void remove();
	void create();
	void reset();
	void rename(QListViewItem *);
	void select(QListViewItem*);

private:
	bool first;
	EqualizerWidget *mWidget;
	QHBoxLayout *bandsLayout;
// 	QCheckBox *mOn;
// 	QSlider *mPreamp;
	PresetList *mPresets;
	bool mGoingPreset;
// 	QPushButton *mRemovePreset, *mAddPreset;
// 	QFrame *mSliders;
// 	KIntNumInput *mBandCount;
};

#endif

