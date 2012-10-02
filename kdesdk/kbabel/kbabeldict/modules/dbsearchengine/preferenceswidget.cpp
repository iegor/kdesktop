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

#include "dbseprefwidget.h"
#include "preferenceswidget.h"

PreferencesWidget::PreferencesWidget(QWidget *parent, const char* name)
		: PrefWidget(parent,name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
//	QLabel *label = new QLabel(i18n("Settings for KDE database search engine"),this);
//	layout->addWidget(label);

	dbpw = new DBSearchEnginePref(this);
	dbpw->dirInput->setMode(KFile::Directory | KFile::LocalOnly);
	
	layout->addWidget(dbpw);
	resize(QSize(200,200).expandedTo(minimumSizeHint()));

//    connect(dbpw->browseTB_3,SIGNAL(clicked()),SLOT(browse1()));

    emit restoreNow();   //Fill with actual params.			

}

PreferencesWidget::~PreferencesWidget()
{
}

void PreferencesWidget::apply()
{
emit applyNow();
}

void PreferencesWidget::cancel()
{
emit restoreNow();
}

void PreferencesWidget::standard()
{


dbpw->caseSensitiveCB->setChecked(false);
dbpw->normalizeCB->setChecked(true);
dbpw->removeContextCB->setChecked(true);

dbpw->oneWordSubCB->setChecked(true);
dbpw->twoWordSubCB->setChecked(false);


dbpw->RegExpRB->setChecked(false);
dbpw->normalTextRB->setChecked(true);
dbpw->equalCB->setChecked( true );
dbpw->containsCB->setChecked( true);
dbpw->containedCB->setChecked( true );
 
 
dbpw->oneWordSubSB->setValue(20);
dbpw->twoWordSubSB->setValue(8);

dbpw->maxSB->setValue(500);
dbpw->thresholdSL->setValue(50);
dbpw->thresholdOrigSL->setValue(50);

dbpw->allRB->setChecked( false);
dbpw->slistRB->setChecked( true);
dbpw->rlistRB->setChecked( false );
	
dbpw->nothingCB->setChecked(true); 
dbpw->freqSB->setValue(300);
 
dbpw->ignoreLE->setText("&.:");
 
dbpw->autoAddCB_2->setChecked(true);

QString defaultDir;
 KStandardDirs * dirs = KGlobal::dirs();
 if(dirs)
 {
     defaultDir = dirs->saveLocation("data");
     if(defaultDir.right(1)!="/")
         defaultDir+="/";
     defaultDir += "kbabeldict/dbsearchengine";
 }
 
 dbpw->dirInput->setURL(defaultDir); 
}

void PreferencesWidget::setName(QString n)
{
dbpw->filenameLB->setText(i18n("Scanning file: %1").arg(n));
}

void PreferencesWidget::setEntries(int i)
{
dbpw->entriesLB->setText(i18n("Entries added: %1").arg(i));

}

#include "preferenceswidget.moc"
