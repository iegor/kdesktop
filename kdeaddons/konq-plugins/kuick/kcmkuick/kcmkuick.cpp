/***************************************************************************
                          kcmkuick.cpp - control module for kuick
                             -------------------
    copyright            : (C) 2001 by Holger Freyther <freyther@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.        *
 *                                                                         *
 ***************************************************************************/

#include "kcmkuick.h"

#include <qlayout.h>
#include <qfile.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kservice.h>

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstring.h>

typedef KGenericFactory<KCMKuick, QWidget> KuickFactory;
K_EXPORT_COMPONENT_FACTORY ( kcm_kuick, KuickFactory( "kcmkuick" ) )

KCMKuick::KCMKuick(QWidget *parent, const char *name, const QStringList &)
:KCModule(parent, name)
{
    KAboutData *ab=new KAboutData( "kcmkuick", I18N_NOOP("KCM Kuick"),
       "0.2",I18N_NOOP("KControl module for Kuick's configuration"), KAboutData::License_GPL,
       "(c) 2001, Holger Freyther", 0, 0, "freyther@kde.org");
    ab->addAuthor("Holger Freyther",0, "freyther@kde.org");
    setAboutData( ab );

    QVBoxLayout *topLayout = new QVBoxLayout(this, 0, 0);
    dialog = new KCMKuickDialog(this);
    topLayout->add(dialog);
    topLayout->addStretch();

    connect( dialog->m_sbCopy, SIGNAL(valueChanged(int) ), SLOT(configChanged() ) );
    connect( dialog->m_sbMove, SIGNAL(valueChanged(int) ), SLOT(configChanged() ) );
    connect( dialog->pbCopyClear, SIGNAL(pressed() ), SLOT(slotClearCopyCache() ) );
    connect( dialog->pbMoveClear, SIGNAL(pressed() ), SLOT(slotClearMoveCache() ) );
    connect( dialog->m_chkShow, SIGNAL(clicked() ), SLOT(slotShowToggled() ) );
    
    load();
}

void KCMKuick::slotShowToggled()
{
    bool showChecked = dialog->m_chkShow->isChecked();
    dialog->m_grpCopy->setEnabled( showChecked );
    dialog->m_grpMove->setEnabled( showChecked );
    configChanged();
}

void KCMKuick::load()
{
    KConfig config( "konquerorrc");
    config.setGroup("kuick-copy");
    dialog->m_sbCopy->setValue(config.readNumEntry("ShowRecent",5) );

    config.setGroup("kuick-move" );
    dialog->m_sbMove->setValue(config.readNumEntry("ShowRecent",5) );
    
    KConfig cfg("kuick_plugin.desktop", true, false, "services");
    cfg.setDesktopGroup();
    bool hidden=cfg.readBoolEntry("Hidden", false);
    dialog->m_chkShow->setChecked(!hidden);
    slotShowToggled();
    
    emit changed(false);
}

KCMKuick::~KCMKuick()
{
}

void KCMKuick::load(const QString & /*s*/)
{
}

void KCMKuick::configChanged()
{
    emit changed(true);
}

void KCMKuick::save()
{
    KConfig config("konquerorrc" );
    config.setGroup("kuick-copy");

    config.writeEntry("ShowRecent", dialog->m_sbCopy->value() );

    config.setGroup("kuick-move" );
    config.writeEntry("ShowRecent", dialog->m_sbMove->value() );

    config.sync();		//is it necessary ?

    if ( dialog->m_chkShow->isChecked() ) {       
       QString servicespath = KGlobal::dirs()->saveLocation( "services");
       QFile::remove(servicespath+"/kuick_plugin.desktop");
    }
    else {
       KConfig cfg("kuick_plugin.desktop", false, false, "services");
       cfg.setDesktopGroup();
       cfg.writeEntry("Hidden", true);
    }
    KService::rebuildKSycoca(this);
    
    emit changed(false);
}

void KCMKuick::slotClearCopyCache( ) {
    KConfig config("konquerorrc");
    config.setGroup("kuick-copy" );
    config.writePathEntry("Paths", QStringList() );
    config.sync();		//is it necessary ?
}

void KCMKuick::slotClearMoveCache() {
    KConfig config("konquerorrc");
    config.setGroup("kuick-move" );
    config.writePathEntry("Paths", QStringList() );
    config.sync();		//is it necessary ?
}

void KCMKuick::defaults()
{
    dialog->m_sbCopy->setValue(4);
    dialog->m_sbMove->setValue(4);
    
    dialog->m_chkShow->setChecked(true);
    slotShowToggled();
    
    emit changed( true );
}

QString KCMKuick::quickHelp() const
{
    return i18n("<h1>Kuick</h1> With this module you can configure Kuick, the KDE quick"
		"copy and move plugin for Konqueror.");
}

#include "kcmkuick.moc"
