#ifndef FIND_H
#define FIND_H

#include <kdialogbase.h>

class KHistoryCombo;
class QCheckBox;
class QPushButton;

class Finder : public KDialogBase
{
Q_OBJECT
public:
	Finder(QWidget *parent);

	bool regexp() const;
	bool isForward() const;

	QString string() const;
signals:
	void search(Finder *);

public slots:
	void textChanged(const QString &);
	void clicked();
	
private:
	KHistoryCombo *mText;
	QCheckBox *mRegexp, *mBackwards;
};

#endif

