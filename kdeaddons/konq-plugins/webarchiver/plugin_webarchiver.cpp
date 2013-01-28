/* This file is part of Webarchiver
 *  Copyright (C) 2001 by Andreas Schlapbach <schlpbch@iam.unibe.ch>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

/* $Id: plugin_webarchiver.cpp 593766 2006-10-09 00:13:35Z tyrerj $ */

/*
 * There are two recursions within this code:
 * - Recursively create DOM-Tree for referenced links which get recursively
 *   converted to HTML
 *
 * => This code has the potential to download whole sites to a TarGz-Archive
 */

//#define DEBUG_WAR

#include <qdir.h>
#include <qfile.h>

#include <kaction.h>
#include <kinstance.h>

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <khtml_part.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kglobalsettings.h>

#include "plugin_webarchiver.h"
#include "archivedialog.h"

typedef KGenericFactory<PluginWebArchiver> PluginWebArchiverFactory;
K_EXPORT_COMPONENT_FACTORY( libwebarchiverplugin,
	                    PluginWebArchiverFactory( "webarchiver" ) )

PluginWebArchiver::PluginWebArchiver( QObject* parent, const char* name,
                                      const QStringList & )
  : Plugin( parent, name )
{
  (void) new KAction( i18n("Archive &Web Page..."),
                      "webarchiver", 0,
                      this, SLOT(slotSaveToArchive()),
                      actionCollection(), "archivepage" );
}

PluginWebArchiver::~PluginWebArchiver()
{
}

void PluginWebArchiver::slotSaveToArchive()
{
  // ## Unicode ok?
  if( !parent() || !parent()->inherits("KHTMLPart"))
    return;
  KHTMLPart *part = static_cast<KHTMLPart *>( parent() );

  QString archiveName = QString::fromUtf8(part->htmlDocument().title().string().utf8());

  if (archiveName.isEmpty())
    archiveName = i18n("Untitled");

  // Replace space with underscore, proposed Frank Pieczynski <pieczy@knuut.de>

  archiveName = archiveName.simplifyWhiteSpace();
  archiveName.replace( "\\s:", " ");
  archiveName.replace( "?", "");
  archiveName.replace( ":", "");
  archiveName.replace( "/", "");
  archiveName = archiveName.replace( QRegExp("\\s+"), " ");

  archiveName = KGlobalSettings::documentPath() + "/" + archiveName + ".war" ;

  KURL url = KFileDialog::getSaveURL(archiveName, i18n("*.war *.tgz|Web Archives"), part->widget(),
					  i18n("Save Page as Web-Archive") );

  if (url.isEmpty()) { return; }

  if (!(url.isValid())) {
    const QString title = i18n( "Invalid URL" );
    const QString text = i18n( "The URL\n%1\nis not valid." ).arg(url.prettyURL());
    KMessageBox::sorry(part->widget(), text, title );
    return;
  }

  const QFile file(url.path());
  if (file.exists()) {
    const QString title = i18n( "File Exists" );
    const QString text = i18n( "Do you really want to overwrite:\n%1?" ).arg(url.prettyURL());
    if (KMessageBox::Continue != KMessageBox::warningContinueCancel( part->widget(), text, title, i18n("Overwrite") ) ) {
      return;
    }
  }

  ArchiveDialog *dialog=new ArchiveDialog(0L, url.path(), part);
  dialog->show();
  dialog->archive();
}

#include "plugin_webarchiver.moc"
