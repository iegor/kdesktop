/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *  $Id: kfile_html.cpp 607813 2006-11-25 21:00:52Z mkoller $
 */

#include "kfile_html.h"
#include "kfile_html.moc"
#include <kgenericfactory.h>
#include <kmimetype.h>
#include <kurl.h>
#include <kprocess.h>
#include <kdebug.h>
#include <qcstring.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextcodec.h>

typedef KGenericFactory<KHtmlPlugin> HtmlFactory;

K_EXPORT_COMPONENT_FACTORY( kfile_html, HtmlFactory( "kfile_html" ) )

KHtmlPlugin::KHtmlPlugin( QObject *parent, const char *name,
                          const QStringList &args )
    : KFilePlugin( parent, name, args )
{
    kdDebug(7034) << "html plugin\n";

    KFileMimeTypeInfo* info = addMimeTypeInfo("text/html");

    KFileMimeTypeInfo::GroupInfo* group;
    KFileMimeTypeInfo::ItemInfo* item;

    group = addGroupInfo(info, "General", i18n("General"));
    addItemInfo(group, "Doctype", i18n("Document Type"), QVariant::String);
    addItemInfo(group, "Javascript", i18n("JavaScript"), QVariant::Bool);
    item = addItemInfo(group, "Title", i18n("Title"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Name);

    group = addGroupInfo(info, "Metatags", i18n("Meta Tags"));
    addVariableInfo(group, QVariant::String, 0);
}


bool KHtmlPlugin::readInfo( KFileMetaInfo& info, uint )
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    QFile f(info.path());
    if (!f.open(IO_ReadOnly))
        return false;

    // we're only interested in the header, so just read until before </head>
    // or until <body> if the author forgot it
    // In this case, it's better to limit the size of the buffer to something
    // sensible. Think a 0-filled 3GB file with an .html extension.
    int maxBufSize = QMIN(f.size(), 32768);
    QByteArray data(maxBufSize + 1);
    f.readBlock(data.data(), maxBufSize);
    data[maxBufSize]='\0';

    QString s(data);

    int start=0, last=0;
    QRegExp exp;
    exp.setCaseSensitive(false);
    exp.setMinimal(true);

    KFileMetaInfoGroup group = appendGroup(info, "General");

    exp.setPattern("\\s*<\\s*!doctype\\s*([^>]*)\\s*>");
    if (exp.search(s, last) != -1)
    {
        kdDebug(7034) << "DocType: " << exp.capturedTexts().join("-") << endl;
        appendItem(group, "Doctype", exp.cap(1));
        last += exp.matchedLength();
    }

    QString title;
    exp.setPattern("<\\s*title\\s*>\\s*(.*)\\s*<\\s*/\\s*title\\s*>");
    if (exp.search(s, last) != -1)
    {
        title = exp.cap(1);
        last += exp.matchedLength();
    }

    KFileMetaInfoGroup metatags = appendGroup(info, "Metatags");

    QString meta, name, content;
    exp.setPattern("<\\s*meta\\s*([^>]*)\\s*>");
    QRegExp rxName("(?:name|http-equiv)\\s*=\\s*\"([^\"]+)\"", false);
    QRegExp rxContent("content\\s*=\\s*\"([^\"]+)\"", false);
    QRegExp rxCharset("charset\\s*=\\s*(.*)", false);
    QTextCodec *codec = 0;

    // find the meta tags
    last = 0;
    while (1)
    {
        if ((start=exp.search(s, last)) == -1)
            break;
        meta = exp.cap(1);
        last = start+exp.matchedLength();

        kdDebug(7034) << "Found Meta: " << meta << endl;

        if (rxName.search(meta) == -1)
            continue;
        name = rxName.cap(1);

        if (rxContent.search(meta) == -1)
            continue;
        content = rxContent.cap(1);

        appendItem(metatags, name, content.left(50));

        // check if it has a charset defined
        if ( rxCharset.search(content) != -1 )
        {
            kdDebug(7034) << "CodecForName : " << rxCharset.cap(1) << endl;
            codec = QTextCodec::codecForName(rxCharset.cap(1).ascii());
        }
    }

    if ( ! title.isEmpty() )
    {
        if ( codec )
        {
          title = codec->toUnicode(title.ascii());
          kdDebug(7034) << "Codec : " << codec->name() << endl;
        }

        appendItem(group, "Title", title);
    }

    // find out if it contains javascript
    exp.setPattern("<script>");

    appendItem(group, "Javascript", QVariant( s.find(exp)!=-1, 42));

    return true;
}

