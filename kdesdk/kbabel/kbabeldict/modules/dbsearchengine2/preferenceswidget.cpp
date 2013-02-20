#include <qradiobutton.h>
#include <qslider.h>
#include <qspinbox.h>  
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kurlrequester.h>
#include <qtoolbutton.h>
#include <klineedit.h>
#include <kstandarddirs.h>
#include <knuminput.h>

#include "dbse2.h"
#include "preferenceswidget.h"

KDB2PreferencesWidget::KDB2PreferencesWidget(QWidget *parent, const char* name)
		: PrefWidget(parent,name) 
{
	QVBoxLayout *layout = new QVBoxLayout(this);
//	QLabel *label = new QLabel(i18n("Settings for KDE Database Search Engine"),this);
//	layout->addWidget(label);

	dbpw = new DBSearchEnginePrefWidget(this);
	dbpw->dbDirectory->setMode(KFile::Directory | KFile::LocalOnly);
	dbpw->show();	
	layout->addWidget(dbpw);
	setMinimumSize(300,300);

	standard();

//    connect(dbpw->browseTB_3,SIGNAL(clicked()),SLOT(browse1()));

    emit restoreNow();   //Fill with actual params.			

}

KDB2PreferencesWidget::~KDB2PreferencesWidget()
{
}

void KDB2PreferencesWidget::apply()
{
emit applyNow();
}

void KDB2PreferencesWidget::cancel()
{
emit restoreNow();
}

void KDB2PreferencesWidget::standard()
{
QString defaultDir;
 KStandardDirs * dirs = KGlobal::dirs();
 if(dirs)
 {
     defaultDir = dirs->saveLocation("data");
     if(defaultDir.right(1)!="/")
         defaultDir+="/";
     defaultDir += "kbabeldict/dbsearchengine2";
 }
dbpw->dbDirectory->setURL(defaultDir);

dbpw->autoUpdate->setChecked(true);

dbpw->useSentence->setChecked(true);
dbpw->useGlossary->setChecked(true);
dbpw->useExact->setChecked(true);
dbpw->useDivide->setChecked(true);
dbpw->useAlpha->setChecked(true);
dbpw->useWordByWord->setChecked(true);
dbpw->useDynamic->setChecked(true);
dbpw->scoreDivide->setValue(90);
dbpw->scoreExact->setValue(100);
dbpw->scoreSentence->setValue(90);
dbpw->scoreWordByWord->setValue(70);
dbpw->scoreGlossary->setValue(98);
dbpw->scoreAlpha->setValue(98);
dbpw->scoreDynamic->setValue(80);

dbpw->numberOfResult->setValue(5);
dbpw->minScore->setValue(60);

dbpw->firstCapital->setChecked(true);
dbpw->allCapital->setChecked(false);
dbpw->accelerator->setChecked(true);
dbpw->sameLetter->setChecked(true);
dbpw->checkLang->setChecked(true);
dbpw->useFilters->setChecked(false);
dbpw->dateToday->setChecked(false);
/*
 */
 //dbpw->dirInput->setURL(defaultDir); 
}

#include "preferenceswidget.moc"
