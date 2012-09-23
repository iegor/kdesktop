/*
   Copyright (c) 1999-2000 Stefan Schimanski <1Stein@gmx.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <klistbox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kconfig.h>

#include "pref.h"
#include "userinterface.h"


KaimanPrefDlg::KaimanPrefDlg(QObject *parent )
    : CModule(i18n("Kaiman Skins"), i18n("Skin Selection for the Kaiman Plugin"), "style", parent)
{
    // create widgets
    QVBoxLayout *topLayout = new QVBoxLayout( this, 6, 11 );
    QLabel *label = new QLabel( i18n("Kaiman Skins"), this, "label" );
    topLayout->addWidget( label );

    _skinList = new KListBox( this, "skinList" );
    topLayout->addWidget( _skinList, 1 );
	reopen();
}


KaimanPrefDlg::~KaimanPrefDlg()
{
}


void KaimanPrefDlg::save()
{
    KConfig *config=KGlobal::config();
    config->setGroup("Kaiman");
    config->writeEntry("SkinResource", skin() );
    config->sync();

    Kaiman *l=Kaiman::kaiman;
    if ( l ) {
        l->changeStyle( skin() );
    }
}

void KaimanPrefDlg::reopen()
{
    _skinList->clear();
    // fill with available skins
    KGlobal::dirs()->addResourceType("skins", KStandardDirs::kde_default("data") + "noatun/skins/kaiman/");
    QStringList list = KGlobal::dirs()->resourceDirs("skins");
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++)
        readSkinDir(*it);

    // load current config
    KConfig *config=KGlobal::config();
    config->setGroup("Kaiman");
    QString skin = config->readEntry( "SkinResource", Kaiman::DEFAULT_SKIN );
    QListBoxItem *item = _skinList->findItem( skin );
    if ( item )
        _skinList->setCurrentItem( item );
    else
        _skinList->setCurrentItem( 0 );
}


void KaimanPrefDlg::setSkin( QString skin )
{
    _skinList->setCurrentItem( _skinList->findItem( skin ) );
}


QString KaimanPrefDlg::skin()
{
    return _skinList->currentText();
}


void KaimanPrefDlg::readSkinDir( const QString &dir )
{
    kdDebug() << "readSkinDir " << dir << endl;

    QDir directory( dir );
    if (!directory.exists())
    return;

    const QFileInfoList *list = directory.entryInfoList();
    QFileInfoListIterator it(*list);

    while ( it.current() ) {
        kdDebug() << it.current()->absFilePath() << endl;
        QFileInfo skindata( it.current()->absFilePath()+"/skindata" );

        if ( skindata.exists() ) {
            _skinList->insertItem( it.current()->baseName() );
        }

        ++it;
    }
}

#include "pref.moc"
