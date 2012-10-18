#include "prefs.h"
#include "voiceprint.h"

#include <klocale.h>
#include <kglobal.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kcolorbutton.h>
#include <kconfig.h>

Prefs::Prefs(QObject* parent)
	: CModule(i18n("Voiceprint"), i18n("Options for the Voiceprint Visualization"), "xapp", parent)
{
	QVBoxLayout	*king=new QVBoxLayout(this);
	QHBoxLayout *minor;

	QLabel *label;
	mForeground=new KColorButton(this);
	label=new QLabel(mForeground, i18n("&Foreground color:"), this);
	minor=new QHBoxLayout(king);
	minor->addWidget(label);
	minor->addWidget(mForeground);

	mBackground=new KColorButton(this);
	label=new QLabel(mBackground, i18n("&Background color:"), this);
	minor=new QHBoxLayout(king);
	minor->addWidget(label);
	minor->addWidget(mBackground);
	
	mLine=new KColorButton(this);
	label=new QLabel(mForeground, i18n("&Sweep color:"), this);
	minor=new QHBoxLayout(king);
	minor->addWidget(label);
	minor->addWidget(mLine);

	king->addStretch();
}

void Prefs::reopen()
{
	KConfig *config=KGlobal::config();
	config->setGroup("VoicePrint");
        QColor black(0, 0, 0);
        QColor blue(0, 0, 222);
	mBackground->setColor(config->readColorEntry("Background", &black));
	mForeground->setColor(config->readColorEntry("Foreground", &blue));
	mLine->setColor(config->readColorEntry("Line", &black));
}

void Prefs::save()
{
	KConfig *config=KGlobal::config();
	config->setGroup("VoicePrint");
	config->writeEntry("Background", mBackground->color());
	config->writeEntry("Foreground", mForeground->color());
	config->writeEntry("Line", mLine->color());

	config->sync();

	VoicePrint *l=VoicePrint::voicePrint;
	if (l)
		l->setColors(mBackground->color(), mForeground->color(), mLine->color());

}

#include "prefs.moc"

