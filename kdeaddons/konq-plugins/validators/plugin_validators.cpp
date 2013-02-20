/* This file is part of Validators
 *
 *  It's a merge of the HTML- and the CSSValidator
 *
 *  Copyright (C) 2001 by  Richard Moore <rich@kde.org>
 *                         Andreas Schlapbach <schlpbch@iam.unibe.ch>
 *
 *  for information how to write your own plugin see:
 *    http://developer.kde.org/documentation/tutorials/dot/writing-plugins.html
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

/* $Id: plugin_validators.cpp 465369 2005-09-29 14:33:08Z mueller $ */

#include <kinstance.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kaction.h>
#include <khtml_part.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kaboutdata.h>

#include "plugin_validators.h"
#include "validatorsdialog.h"


typedef KGenericFactory<PluginValidators> PluginValidatorsFactory;
static const KAboutData aboutdata("validatorsplugin", I18N_NOOP("Validate Web Page") , "1.0" );
K_EXPORT_COMPONENT_FACTORY( libvalidatorsplugin,
	                    PluginValidatorsFactory( &aboutdata ) )

PluginValidators::PluginValidators( QObject* parent, const char* name,
                                    const QStringList & )
  : Plugin( parent, name ), m_configDialog(0), m_part(0)
{
  setInstance(PluginValidatorsFactory::instance());

  m_menu = new KActionMenu ( i18n( "&Validate Web Page" ), "validators",
			     actionCollection(), "validateWebpage" );
  m_menu->setDelayed( false );

  m_menu->insert( new KAction( i18n( "Validate &HTML" ),
			       "htmlvalidator", 0,
			       this, SLOT(slotValidateHTML()),
			       actionCollection(), "validateHTML") );

  m_menu->insert( new KAction( i18n( "Validate &CSS" ),
			       "cssvalidator", 0,
			       this, SLOT(slotValidateCSS()),
			       actionCollection(), "validateCSS") );

  m_menu->insert( new KAction( i18n( "Validate &Links" ),
			       0,
			       this, SLOT(slotValidateLinks()),
			       actionCollection(), "validateLinks") );

  m_menu->setEnabled( false );

  if ( parent && parent->inherits( "KHTMLPart" ))
    {
      m_menu->insert( new KAction( i18n( "C&onfigure Validator..." ),
				   "configure", 0,
				   this, SLOT(slotConfigure()),
				   actionCollection(), "configure") );

      m_part = static_cast<KHTMLPart *>( parent );
      m_configDialog = new ValidatorsDialog( m_part->widget() );
      setURLs();

      connect( m_part, SIGNAL(started(KIO::Job*)), this,
             SLOT(slotStarted(KIO::Job*)) );
    }
}

PluginValidators::~PluginValidators()
{
  delete m_configDialog;
// Dont' delete the action. KActionCollection as parent does the job already
// and not deleting it at this point also ensures that in case we are not unplugged
// from the GUI yet and the ~KXMLGUIClient destructor will do so it won't hit a
// dead pointer. The kxmlgui factory keeps references to the actions, but it does not
// connect to their destroyed() signal, yet (need to find an elegant solution for that
// as it can easily increase the memory usage significantly) . That's why actions must
// persist as long as the plugin is plugged into the GUI.
//  delete m_menu;
}

void PluginValidators::setURLs()
{
  m_WWWValidatorUrl = KURL(m_configDialog->getWWWValidatorUrl());
  m_CSSValidatorUrl = KURL(m_configDialog->getCSSValidatorUrl());
  m_WWWValidatorUploadUrl = KURL(m_configDialog->getWWWValidatorUploadUrl());
  m_CSSValidatorUploadUrl = KURL(m_configDialog->getCSSValidatorUploadUrl());
  m_linkValidatorUrl = KURL(m_configDialog->getLinkValidatorUrl());
}

void PluginValidators::slotStarted( KIO::Job* )
{
  // The w3c validator can only access http URLs, and upload local files.
  // No https, probably no webdav either.
  m_menu->setEnabled( m_part->url().isLocalFile()
                      || m_part->url().protocol().lower() == "http" );
}

void PluginValidators::slotValidateHTML()
{
  validateURL(m_WWWValidatorUrl, m_WWWValidatorUploadUrl);
}

void PluginValidators::slotValidateCSS()
{
  validateURL(m_CSSValidatorUrl, m_CSSValidatorUploadUrl);
}

void PluginValidators::slotValidateLinks()
{
  validateURL(m_linkValidatorUrl, KURL());
}

void PluginValidators::slotConfigure()
{
  m_configDialog->show();
  setURLs();
}

void PluginValidators::validateURL(const KURL &url, const KURL &uploadUrl)
{
  // The parent is assumed to be a KHTMLPart
  if ( !parent()->inherits("KHTMLPart") )
    {
      QString title = i18n( "Cannot Validate Source" );
      QString text = i18n( "You cannot validate anything except web pages with "
			   "this plugin." );

      KMessageBox::sorry( 0, text, title );
      return;
    }

  KURL validatorUrl(url);

  // Get URL
  KURL partUrl = m_part->url();
  if ( !partUrl.isValid() ) // Just in case ;)
    {
      QString title = i18n( "Malformed URL" );
      QString text = i18n( "The URL you entered is not valid, please "
			   "correct it and try again." );
      KMessageBox::sorry( 0, text, title );
      return;
    }

  if (partUrl.isLocalFile())
    {
      if ( validatorUrl.isEmpty() ) {
        QString title = i18n( "Upload Not Possible" );
        QString text = i18n( "Validating links is not possible for local "
                             "files." );
        KMessageBox::sorry( 0, text, title );
        return;
      }
      validatorUrl = uploadUrl;
    }
  else
    {
      if (partUrl.hasPass())
        {
          KMessageBox::sorry(
          	m_part->widget(),
          	i18n("<qt>The selected URL cannot be verified because it contains "
          	     "a password. Sending this URL to <b>%1</b> would put the security "
          	     "of <b>%2</b> at risk.</qt>")
          	     .arg(validatorUrl.host()).arg(partUrl.host()));
          return;
        }
      // Set entered URL as a parameter
      QString q = partUrl.url();
      q = KURL::encode_string( q );
      QString p = "uri=";
      p += q;
      validatorUrl.setQuery( p );
    }
  kdDebug(90120) << "final URL: " << validatorUrl.url() << endl;

  emit m_part->browserExtension()->openURLRequest( validatorUrl );
}

#include <plugin_validators.moc>
