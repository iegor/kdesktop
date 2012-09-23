/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qlabel.h>
#include <qlayout.h>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kurllabel.h>

#include "kcmkmrml.h"
#include <dcopclient.h>

#include "mainpage.h"
#include <version.h>

using namespace KMrmlConfig;

static const int COL_FILENAME = 1;

typedef KGenericFactory<KCMKMrml, QWidget> MrmlFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_kmrml, MrmlFactory("kmrml") )

KCMKMrml::KCMKMrml(QWidget *parent, const char *name, const QStringList & ):
    KCModule(MrmlFactory::instance(), parent, name)
{
    KAboutData* ab = new KAboutData(
        "kcmkmrml",
        I18N_NOOP("KCMKMrml"),
        KMRML_VERSION,
        I18N_NOOP("Advanced Search Control Module"),
            KAboutData::License_GPL,
        I18N_NOOP( "Copyright 2002, Carsten Pfeiffer" ),
        0,
        "http://devel-home.kde.org/~pfeiffer/kmrml/" );
    ab->addAuthor( "Carsten Pfeiffer", 0, "pfeiffer@kde.org" );
    setAboutData( ab );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( KDialog::spacingHint() );
    m_mainPage = new MainPage( this, "main page" );

    layout->addWidget( m_mainPage );

    connect( m_mainPage, SIGNAL( changed( bool ) ), SIGNAL( changed( bool )));

    checkGiftInstallation();
}

KCMKMrml::~KCMKMrml()
{
}

void KCMKMrml::checkGiftInstallation()
{
    QString giftExe = KGlobal::dirs()->findExe( "gift" );
    QString giftAddCollectionExe = KGlobal::dirs()->findExe( "gift-add-collection.pl" );

    if ( giftExe.isEmpty() || giftAddCollectionExe.isEmpty() )
    {
        QString errorMessage = 
            i18n("Cannot find executables \"gift\" and/or \"gift-add-collection.pl\" in the PATH.\n"
                 "Please install the \"GNU Image Finding Tool\".");
        KMessageBox::error( this, errorMessage );
        m_mainPage->hide();
        QLabel *errorLabel = new QLabel( errorMessage, this );
        errorLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
        KURLLabel *urlLabel = new KURLLabel( "http://www.gnu.org/software/gift", QString::null, this ); 
        urlLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
        connect( urlLabel, SIGNAL( leftClickedURL( const QString& )), kapp, SLOT( invokeBrowser( const QString& )) );
        QLayout *l = layout();
        l->addItem( new QSpacerItem( 0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
        l->add( errorLabel );
        l->add( urlLabel );
        l->addItem( new QSpacerItem( 0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
        errorLabel->show();
    }
    else
        load();
}

void KCMKMrml::defaults()
{
    if (KMessageBox::warningContinueCancel(this,
         i18n("Do you really want the configuration to be reset "
              "to the defaults?"), i18n("Reset Configuration"), KStdGuiItem::cont())
        != KMessageBox::Continue)
        return;

    m_mainPage->resetDefaults();

    emit changed( true );
}

void KCMKMrml::load()
{
    m_mainPage->load();

    emit changed( true );
}

void KCMKMrml::save()
{
    m_mainPage->save();

    emit changed( false );
}

QString KCMKMrml::quickHelp() const
{
    return i18n("<h1>Image Index</h1>"
                "KDE can make use of the GNU Image Finding Tool (GIFT) to "
                "perform queries based not just on filenames, but on "
                "file content."
                "<p>For example, you can search for an image by giving an example "
                "image that looks similar to the one you are looking for.</p>"
                "<p>For this to work, your image directories need to be "
                "indexed by, for example, the GIFT server.</p>"
                "<p>Here you can configure the servers (you can also query "
                "remote servers) and the directories to index.</p>"
        );
}

#include "kcmkmrml.moc"
