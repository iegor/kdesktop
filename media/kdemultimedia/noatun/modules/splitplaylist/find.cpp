#include "find.h"
#include <qlayout.h>
#include <kcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <klocale.h>

Finder::Finder(QWidget *parent) : KDialogBase(parent, 0, false, i18n("Find"), Close | User1, User1, false, KGuiItem(i18n("&Find"),"find"))
{
	QWidget *mainWidget = new QWidget(this);
	mainWidget->setMinimumWidth(320);
	setMainWidget(mainWidget);

	QGridLayout *layout=new QGridLayout(mainWidget);
	layout->setSpacing(KDialog::spacingHint());

	mText=new KHistoryCombo(mainWidget);
	mText->setMaxCount(10);

	mText->setFocus();

	mRegexp=new QCheckBox(i18n("&Regular expression"), mainWidget);
	mBackwards=new QCheckBox(i18n("Find &backwards"), mainWidget);

	layout->addMultiCellWidget(mText, 0, 0, 0, 1);
	layout->addWidget(mRegexp, 1, 0);
	layout->addWidget(mBackwards, 1, 1);

	connect(this, SIGNAL(user1Clicked()), SLOT(clicked()));

	connect(mText, SIGNAL(activated(int)), SLOT(clicked()));
	connect(mText, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));

	enableButton(User1, false);
}

void Finder::textChanged(const QString &text) {
	enableButton(User1, !text.isEmpty());
}

bool Finder::regexp() const
{
	return mRegexp->isChecked();
}

bool Finder::isForward() const
{
	return !mBackwards->isChecked();
}

void Finder::clicked()
{
	mText->addToHistory( mText->currentText() );
	emit search(this);
}

QString Finder::string() const
{
	return mText->currentText();
}


#include "find.moc"
