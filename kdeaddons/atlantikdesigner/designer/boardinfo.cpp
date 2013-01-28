#include <qstring.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qframe.h>
#include <qptrlist.h>
#include <qlabel.h>

#include <kcolorbutton.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kurllabel.h>

#include "boardinfo.h"

BoardInfoDlg::BoardInfoDlg(bool editable, BoardInfo *info, QWidget *parent, const char *_name, bool modal)
	: KDialogBase(KDialogBase::Tabbed, i18n("Gameboard Information"), (editable? Ok|Apply|Cancel : Close), (editable? Ok : Close), parent, _name, modal)
{
	if (!info)
		return;
	this->info = info;

	setWFlags(WDestructiveClose);

	QFrame *about = addPage(i18n("Information"));
	QVBoxLayout *aboutLayout = new QVBoxLayout(about, spacingHint());

	if (editable)
	{
		aboutLayout->addWidget(name = new KLineEdit(about));
		name->setText(info->name);
	}
	else
	{
		aboutLayout->addWidget(new QLabel(info->name, about));
		name = 0;
	}

	QHBoxLayout *versionLayout = new QHBoxLayout(aboutLayout, spacingHint());
	versionLayout->addWidget(new QLabel(i18n("Version:"), about));
	if (editable)
	{
		versionLayout->addWidget(version = new KLineEdit(about));
		version->setText(info->version);
	}
	else
		versionLayout->addWidget(new QLabel(info->version, about));

	QHBoxLayout *urlLayout = new QHBoxLayout(aboutLayout, spacingHint());
	urlLayout->addWidget(new QLabel(i18n("URL:"), about));
	if (editable)
		urlLayout->addWidget(url = new KLineEdit(info->url, about));
	else
		urlLayout->addWidget(new KURLLabel(info->url, info->url, about));

	aboutLayout->addStretch(3);

	aboutLayout->addWidget(new QLabel(i18n("Description:"), about));
	aboutLayout->addStretch();
	aboutLayout->addWidget(description = new KLineEdit(about));
	description->setText(info->description);
	if (!editable)
	{
		description->setReadOnly(true);
	}

	if (editable)
	{
		QHBoxLayout *bgLayout = new QHBoxLayout(aboutLayout, spacingHint());
		bgLayout->addWidget(new QLabel(i18n("Background color:"), about));
		bgColor = new KColorButton(info->bgColor, about);
		bgLayout->addWidget(bgColor);
	}
	else
		bgColor = 0;

	QFrame *authorsFrame = addPage(i18n("&Authors"));
	QVBoxLayout *authorsLayout = new QVBoxLayout(authorsFrame, spacingHint());
	authorsLayout->addWidget(authors = new LotsaEdits(editable, info->authors, authorsFrame));

	QFrame *creditsFrame = addPage(i18n("&Thanks To"));
	QVBoxLayout *creditsLayout = new QVBoxLayout(creditsFrame, spacingHint());
	creditsLayout->addWidget(credits = new LotsaEdits(editable, info->credits, creditsFrame));
}

void BoardInfoDlg::slotApply()
{
	info->name = name->text();
	info->description = description->text();
	info->version = version->text();
	info->url = url->text();
	info->authors = authors->save();
	info->credits = credits->save();

	if (bgColor)
		info->bgColor = bgColor->color().name();

	emit okClicked();
	KDialogBase::slotApply();
}

void BoardInfoDlg::slotOk()
{
	slotApply();

	KDialogBase::slotOk();
}

///////////////////////////////////

LotsaEdits::LotsaEdits(bool editable, QStringList defaults, QWidget *parent, const char *name) : QWidget(parent, name)
{
	list.setAutoDelete(true);
	this->editable = editable;

	layout = new QVBoxLayout(this, KDialogBase::spacingHint());
	QHBoxLayout *hlayout = new QHBoxLayout(layout, KDialogBase::spacingHint());

	if (editable)
	{
		KPushButton *more = new KPushButton(i18n("&Add Name"), this);
		hlayout->addWidget(more);
		connect(more, SIGNAL(clicked()), this, SLOT(more()));
		hlayout->addStretch();
		KPushButton *less= new KPushButton(i18n("&Delete Name"), this);
		hlayout->addWidget(less);
		connect(less, SIGNAL(clicked()), this, SLOT(less()));
	}

	layout->addStretch();

	for (QStringList::Iterator it = defaults.begin(); it != defaults.end(); ++it)
	{
		more();
		if (editable)
			static_cast<KLineEdit *>(list.last())->setText(*it);
		else
			static_cast<QLabel *>(list.last())->setText(*it);
	}
}

void LotsaEdits::more()
{
	QWidget *edit;
	if (editable)
		edit = new KLineEdit(this);
	else
		edit = new QLabel(this);
	layout->addWidget(edit);
	list.append(edit);
	edit->show();
}

void LotsaEdits::less()
{
	list.removeLast();
	   /*
	QWidget *edit = 0;
	for (edit = list.first(); edit; edit = list.next())
	{
		if (edit->hasFocus())
		{
			list.remove();
			break;
		}
	}
	*/
}

QStringList LotsaEdits::save()
{
	QStringList ret;

	QWidget *edit = 0;
	for (edit = list.first(); edit; edit = list.next())
		if (editable)
			ret.append(static_cast<KLineEdit *>(edit)->text());
		else
			ret.append(static_cast<QLabel *>(edit)->text());

	return ret;
}

#include "boardinfo.moc"
