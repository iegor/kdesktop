#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kdialogbase.h>

#include <kapplication.h>
#include <klocale.h>

#include "channelview.h"
#include "channelcfgdlg.h"
#include "version.h"
#include <qlayout.h>
#include <qvbuttongroup.h>

ChannelViewConfigDialog::ChannelViewConfigDialog(QWidget *parent,const char *name) : KDialogBase(parent,name,TRUE,i18n("Configure Channel View"),Ok|Cancel, Ok)
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
    qbg = new QVButtonGroup(i18n("Choose Look Mode"),page,"qbg");
    
    rb0=new QRadioButton(i18n("3D look"),qbg,"3d");
    rb1=new QRadioButton(i18n("3D - filled"),qbg,"4d");

    qbg->setExclusive(TRUE);

    topLayout->addWidget( qbg );
    ((ChannelView::lookMode()==0)?rb0:rb1)->setChecked(TRUE);
    
    connect (qbg, SIGNAL(pressed(int)),this,SLOT(modeselected(int)));
}

void ChannelViewConfigDialog::modeselected(int idx)
{
selectedmode=idx;
}

int ChannelViewConfigDialog::selectedmode;
#include "channelcfgdlg.moc"
