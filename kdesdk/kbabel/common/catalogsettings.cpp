/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2004	  by Stanislav Visnovsky
			    <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
**************************************************************************** */
#include "catalogsettings.h"
#include "kbprojectsettings.h"
#include <kconfig.h>
#include <kdebug.h>
#include <kemailsettings.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprocess.h>
#include <ktempfile.h>

#include <stdlib.h>

#include <qfile.h>
#include <qtextcodec.h>
#include <qregexp.h>

#include <stdlib.h>

using namespace KBabel;

QString KBabel::charsetString(const QTextCodec *codec)
{
    if(codec)
    {
        QString encodingStr = codec->mimeName();
	if ( encodingStr.startsWith("CP " ) )
    	    encodingStr.remove( 2, 1 );
	else if ( encodingStr.startsWith("IBM " ) )
            encodingStr.replace( "IBM ", "CP" );
        return encodingStr;
    }
    else
      return QString::null;
}

QString KBabel::GNUPluralForms(const QString& lang)
{
    KTempFile infile, outfile;
    
    QTextStream* str = infile.textStream();
    
    *str << "# SOME DESCRIPTIVE TITLE." << endl;
    *str << "# Copyright (C) YEAR Free Software Foundation, Inc." << endl;
    *str << "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR." << endl;
    *str << "#" << endl;
    *str << "#, fuzzy" << endl;
    *str << "msgid \"\"" << endl;
    *str << "msgstr \"\"" << endl;
    *str << "\"Project-Id-Version: PACKAGE VERSION\\n\"" << endl;
    *str << "\"POT-Creation-Date: 2002-06-25 03:23+0200\\n\"" << endl;
    *str << "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"" << endl;
    *str << "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"" << endl;
    *str << "\"Language-Team: LANGUAGE <LL@li.org>\\n\"" << endl;
    *str << "\"MIME-Version: 1.0\\n\"" << endl;
    *str << "\"Content-Type: text/plain; charset=CHARSET\\n\"" << endl;
    *str << "\"Content-Transfer-Encoding: ENCODING\\n\"" << endl;
    
    infile.close();

    KProcess msginit;
    
    msginit << "msginit";
    msginit 
	<< "-l" 
	<< lang 
	<< "-i" 
	<< infile.name() 
	<< "-o" 
	<< outfile.name() 
	<< "--no-translator" 
	<< "--no-wrap" ;
    
    msginit.start( KProcess::Block );
    
    QString res("");
    
    if( msginit.normalExit() )
    {
	// parse out the plural form string
	QFile f(outfile.name());
	if( f.open (IO_ReadOnly) )
	{
	    QTextStream str(&f);
	    
	    QString line;
	    do {
		line = str.readLine();
		
		if( line.startsWith( "\"Plural-Forms:" ) )
		{
		    kdDebug() << "Plural form line: " << line << endl;
		    QRegExp re( "^\"Plural-Forms: *(.*)\\\\n\"" );
		    re.search( line );
		    res = re.cap(1);
		    break;
		}
	    } while (!str.atEnd() );	    
	}
	else 
	{
	    kdWarning() << "Cannot open the file with plural form definition" << endl;
	}
    }
    
    infile.unlink();
    outfile.unlink();
    
    return res;
}

QString KBabel::charsetString(const int e)
{
    QString encodingStr;

    switch(e)
    {
        case ProjectSettingsBase::Locale:
        {
            QTextCodec *codec=QTextCodec::codecForLocale();
            if(codec)
               encodingStr=charsetString(codec);
            else
               encodingStr="unknown";

            break;
        }
        case ProjectSettingsBase::UTF8:
        {
            encodingStr="UTF-8";
            break;
        }
        case ProjectSettingsBase::UTF16:
        {
            encodingStr="UTF-16";
            break;
        }
    }

    return encodingStr;
}

QString Defaults::Identity::authorName()
{
    KEMailSettings emSet;
    return emSet.getSetting(KEMailSettings::RealName);
}

QString Defaults::Identity::authorLocalizedName()
{
    return authorName();
}

QString Defaults::Identity::authorEmail()
{
    KEMailSettings emSet;
    return emSet.getSetting(KEMailSettings::EmailAddress);
}

QString Defaults::Identity::mailingList()
{
    QString lang=Defaults::Identity::languageCode();
    int temp=lang.find("_");
    lang=lang.left(temp);
    return lang+"@li.org";
}

QString Defaults::Identity::languageCode()
{   
    // first try to get the language from KDE settings
    KLocale *locale = KGlobal::locale();
    QString lang;
    if(locale)
    {
        lang=locale->languageList().first();
    }

    if(lang.isEmpty())
    {
        lang=getenv("LC_ALL");
        if(lang.isEmpty())
        {
            lang=getenv("LC_MESSAGES");
            if(lang.isEmpty())
            {
        	lang=getenv("LANG");
            }
         }
    }

    return lang;
}

QString Defaults::Identity::timezone()
{
    QString timezone=getenv("TIMEZONE");
    if(timezone.isEmpty())
       timezone="GMT";

    return timezone;
}

QStringList Defaults::Tag::tagExpressions()
{
    QStringList list;

    list.append("</[A-Za-z0-9\\n]+>");
    list.append("<[A-Za-z0-9\\n]+[^>]*/?>");
    list.append("http:\\/\\/[a-zA-Z0-9\\.\\-_/~]+");
    list.append("mailto:[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+");
    list.append("<?[a-z0-9\\.\\-_]+@[a-z0-9\\.\\-_]+>?");
    list.append("&[a-z,A-Z,\\-,0-9,#\\.]*;");

    return list;
}

QStringList Defaults::Tag::argExpressions()
{
    QStringList list;

    list.append("%[ndioxXucsfeEgGp]");
    list.append("%([0-9]+(\\$))?[-+'#0]?[0-9]*(.[0-9]+)?[hlL]?[dioxXucsfeEgGp]");
    list.append("%[0-9]+");

    return list;
}
