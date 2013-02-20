/* This file is part of the KDE Project
   Copyright (C) 2001 Kurt Granroth <granroth@kde.org>
     Original code: plugin code, connecting to Babelfish and support for selected text
   Copyright (C) 2003 Rand2342 <rand2342@yahoo.com>
     Submenus, KConfig file and support for other translation engines

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "plugin_babelfish.h"

#include <kaction.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>

#include <khtml_part.h>

typedef KGenericFactory<PluginBabelFish> BabelFishFactory;
static const KAboutData aboutdata("babelfish", I18N_NOOP("Translate Web Page") , "1.0" );
K_EXPORT_COMPONENT_FACTORY( libbabelfishplugin, BabelFishFactory( &aboutdata ) )

PluginBabelFish::PluginBabelFish( QObject* parent, const char* name,
	                          const QStringList & )
  : Plugin( parent, name )
{
  setInstance(BabelFishFactory::instance());

  m_menu = new KActionMenu( i18n("Translate Web &Page"), "babelfish",
                          actionCollection(), "translatewebpage" );
  m_menu->setDelayed( false );

  m_en = new KActionMenu( i18n("&English To"), "babelfish",
    actionCollection(), "translatewebpage_en" );
  m_fr = new KActionMenu( i18n("&French To"), "babelfish",
    actionCollection(), "translatewebpage_fr" );
  m_de = new KActionMenu( i18n("&German To"), "babelfish",
    actionCollection(), "translatewebpage_de" );
  m_es = new KActionMenu( i18n("&Spanish To"), "babelfish",
    actionCollection(), "translatewebpage_es" );
  m_pt = new KActionMenu( i18n("&Portuguese To"), "babelfish",
    actionCollection(), "translatewebpage_pt" );
  m_it = new KActionMenu( i18n("&Italian To"), "babelfish",
    actionCollection(), "translatewebpage_it" );
  m_nl = new KActionMenu( i18n("&Dutch To"), "babelfish",
    actionCollection(), "translatewebpage_nl" );

  m_en->insert( new KAction( i18n("&Chinese (Simplified)"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_zh") );
  m_en->insert( new KAction( i18n("Chinese (&Traditional)"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_zhTW") );
  m_en->insert( new KAction( i18n("&Dutch"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_nl") );
  m_en->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_fr") );
  m_en->insert( new KAction( i18n("&German"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_de") );
  m_en->insert( new KAction( i18n("&Italian"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_it") );
  m_en->insert( new KAction( i18n("&Japanese"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_ja") );
  m_en->insert( new KAction( i18n("&Korean"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_ko") );
  m_en->insert( new KAction( i18n("&Norwegian"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_no") );
  m_en->insert( new KAction( i18n("&Portuguese"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_pt") );
  m_en->insert( new KAction( i18n("&Russian"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_ru") );
  m_en->insert( new KAction( i18n("&Spanish"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_es") );
  m_en->insert( new KAction( i18n("T&hai"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "en_th") );

  m_fr->insert( new KAction( i18n("&Dutch"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_nl") );
  m_fr->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_en") );
  m_fr->insert( new KAction( i18n("&German"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_de") );
  m_fr->insert( new KAction( i18n("&Italian"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_it") );
  m_fr->insert( new KAction( i18n("&Portuguese"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_pt") );
  m_fr->insert( new KAction( i18n("&Spanish"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "fr_es") );

  m_de->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "de_en") );
  m_de->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "de_fr") );

  m_es->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "es_en") );
  m_es->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "es_fr") );

  m_pt->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "pt_en") );
  m_pt->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "pt_fr") );

  m_it->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "it_en") );
  m_it->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "it_fr") );

  m_nl->insert( new KAction( i18n("&English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "nl_en") );
  m_nl->insert( new KAction( i18n("&French"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "nl_fr") );

  m_menu->insert( new KAction( i18n("&Chinese (Simplified) to English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "zh_en") );
  m_menu->insert( new KAction( i18n("Chinese (&Traditional) to English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "zhTW_en") );
  m_menu->insert( m_nl );
  m_menu->insert( m_en );
  m_menu->insert( m_fr );
  m_menu->insert( m_de );
  m_menu->insert( m_it );
  m_menu->insert( new KAction( i18n("&Japanese to English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "ja_en") );
  m_menu->insert( new KAction( i18n("&Korean to English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "ko_en") );
  m_menu->insert( m_pt );
  m_menu->insert( new KAction( i18n("&Russian to English"), 0,
                             this, SLOT(translateURL()),
                             actionCollection(), "ru_en") );
  m_menu->insert( m_es );
  m_menu->setEnabled( true );

  // TODO: we could also support plain text viewers...
  if ( parent && parent->inherits( "KHTMLPart" ) )
  {
    KParts::ReadOnlyPart* part = static_cast<KParts::ReadOnlyPart *>(parent);
    connect( part, SIGNAL(started(KIO::Job*)), this,
             SLOT(slotStarted(KIO::Job*)) );
  }
}

PluginBabelFish::~PluginBabelFish()
{
  delete m_menu;
}

void PluginBabelFish::slotStarted( KIO::Job* )
{
  if ( parent()->inherits("KHTMLPart") &&
  // Babelfish wants http URLs only. No https.
       static_cast<KParts::ReadOnlyPart *>(parent())->url().protocol().lower() == "http" )
  {
      m_menu->setEnabled( true );
  }
  else
  {
      m_menu->setEnabled( false );
  }
}

void PluginBabelFish::translateURL()
{
  // we need the sender() for the language name
  if ( !sender() )
    return;

  // The parent is assumed to be a KHTMLPart
  if ( !parent()->inherits("KHTMLPart") )
  {
    QString title = i18n( "Cannot Translate Source" );
    QString text = i18n( "Only web pages can be translated using "
                         "this plugin." );

    KMessageBox::sorry( 0L, text, title );
    return;
  }

  // Select engine
  KConfig cfg( "translaterc", true );
  QString engine = cfg.readEntry( sender()->name(), "babelfish" );

  // Get URL
  KHTMLPart *part = dynamic_cast<KHTMLPart *>(parent());
  if ( !part )
    return;

  // we check if we have text selected.  if so, we translate that. If
  // not, we translate the url
  QString totrans;
  if ( part->hasSelection() )
  {
    if( engine == "reverso" || engine == "tsail" )
    {
      KMessageBox::sorry( 0L, 
              i18n( "Only full webpages can be translated for this language pair." ),i18n( "Translation Error" ) );
      return;
    }
    totrans = KURL::encode_string( part->selectedText() );
  } else {
    KURL url = part->url();
    // Check syntax
    if ( !url.isValid() )
    {
      QString title = i18n( "Malformed URL" );
      QString text = i18n( "The URL you entered is not valid, please "
                           "correct it and try again." );
      KMessageBox::sorry( 0L, text, title );
      return;
    }
    totrans = KURL::encode_string( url.url() );
  }

  // Create URL
  KURL result;
  QString query;
  if( engine == "freetranslation" ) {
    query = "sequence=core&Submit=FREE Translation&language=";
    if( sender()->name() == QString( "en_es" ) )
      query += "English/Spanish";
    else if( sender()->name() == QString( "en_de" ) )
      query += "English/German";
    else if( sender()->name() == QString( "en_it" ) )
      query += "English/Italian";
    else if( sender()->name() == QString( "en_nl" ) )
      query += "English/Dutch";
    else if( sender()->name() == QString( "en_pt" ) )
      query += "English/Portuguese";
    else if( sender()->name() == QString( "en_no" ) )
      query += "English/Norwegian";
    else if( sender()->name() == QString( "en_zh" ) )
      query += "English/SimplifiedChinese";
    else if( sender()->name() == QString( "en_zhTW" ) )
      query += "English/TraditionalChinese";
    else if( sender()->name() == QString( "es_en" ) )
      query += "Spanish/English";
    else if( sender()->name() == QString( "fr_en" ) )
      query += "French/English";
    else if( sender()->name() == QString( "de_en" ) )
      query += "German/English";
    else if( sender()->name() == QString( "it_en" ) )
      query += "Italian/English";
    else if( sender()->name() == QString( "nl_en" ) )
      query += "Dutch/English";
    else if( sender()->name() == QString( "pt_en" ) )
      query += "Portuguese/English";
    else // Should be en_fr
      query += "English/French";
    if ( part->hasSelection() )
    {
      result = KURL( "http://ets.freetranslation.com" );
      query += "&mode=html&template=results_en-us.htm&srctext=";
    } else {
      result = KURL( "http://www.freetranslation.com/web.asp" );
      query += "&url=";
    }
    query += totrans;
  } else if( engine == "parsit" ) {
    // Does only English -> Thai
    result = KURL( "http://c3po.links.nectec.or.th/cgi-bin/Parsitcgi.exe" );
    query = "mode=test&inputtype=";
    if ( part->hasSelection() )
      query += "text&TxtEng=";
    else
      query += "html&inputurl=";
    query += totrans;
  } else if( engine == "reverso" ) {
    result = KURL( "http://www.reverso.net/url/frame.asp" );
    query = "autotranslate=on&templates=0&x=0&y=0&directions=";
    if( sender()->name() == QString( "de_fr" ) )
      query += "524292";
    else if( sender()->name() == QString( "fr_en" ) )
      query += "65544";
    else if( sender()->name() == QString( "fr_de" ) )
      query += "262152";
    else if( sender()->name() == QString( "de_en" ) )
      query += "65540";
    else if( sender()->name() == QString( "en_de" ) )
      query += "262145";
    else if( sender()->name() == QString( "en_es" ) )
      query += "2097153";
    else if( sender()->name() == QString( "es_en" ) )
      query += "65568";
    else if( sender()->name() == QString( "fr_es" ) )
      query += "2097160";
    else // "en_fr"
      query += "524289";
    query += "&url=";
    query += totrans;
  } else if( engine == "tsail" ) {
    result = KURL( "http://www.t-mail.com/cgi-bin/tsail" );
    query = "sail=full&lp=";
    if( sender()->name() == QString( "zhTW_en" ) )
      query += "tw-en";
    else if( sender()->name() == QString( "en_zhTW" ) )
      query += "en-tw";
    else
    {
      query += sender()->name();
      query[15] = '-';
    }
    query += totrans;
  } else if( engine == "voila" ) {
    result = KURL( "http://trans.voila.fr/voila" );
    query = "systran_id=Voila-fr&systran_lp=";
    query += sender()->name();
    if ( part->hasSelection() )
      query += "&systran_charset=utf-8&systran_text=";
    else
      query += "&systran_url=";
    query += totrans;
  } else {
    // Using the altavista babelfish engine
    result = KURL( "http://babelfish.altavista.com/babelfish/tr" );
    query = "lp=";
    query += sender()->name();
    if ( part->hasSelection() )
      query += "&text=";
    else
      query += "&url=";
    query += totrans;
  }

  result.setQuery( query );

  // Connect to the fish
  emit part->browserExtension()->openURLRequest( result );
}

#include <plugin_babelfish.moc>
