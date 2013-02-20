/* This file is part of the KDE project
 * Copyright (C) 2002 Nadeem Hasan <nhasan@kde.org>
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
 */

#include "kfile_txt.h"

#include <kgenericfactory.h>
#include <kdebug.h>

#include <qfile.h>
#include <qstringlist.h>
#include <qregexp.h>

typedef KGenericFactory<KTxtPlugin> TxtFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_txt, TxtFactory("kfile_txt"))

KTxtPlugin::KTxtPlugin(QObject *parent, const char *name,
        const QStringList &args) : KFilePlugin(parent, name, args)
{
    kdDebug(7034) << "Text file meta info plugin\n";
    makeMimeTypeInfo( "text/plain" );
}

void KTxtPlugin::makeMimeTypeInfo(const QString& mimeType)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo(mimeType);

    KFileMimeTypeInfo::GroupInfo* group =
            addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;
    item = addItemInfo(group, "Lines", i18n("Lines"), QVariant::Int);
    setAttributes(item, KFileMimeTypeInfo::Averaged);
    item = addItemInfo(group, "Words", i18n("Words"), QVariant::Int);
    setAttributes(item, KFileMimeTypeInfo::Averaged);
    item = addItemInfo(group, "Characters", i18n("Characters"), QVariant::ULongLong);
    setAttributes(item, KFileMimeTypeInfo::Averaged);
    item = addItemInfo(group, "Format", i18n("Format"), QVariant::String);
}

bool KTxtPlugin::readInfo(KFileMetaInfo& info, uint)
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    QFile f(info.path());
    if (!f.open(IO_ReadOnly))
        return false;

    bool firstline = true;
    int totLines = 0;
    int totWords = 0;
    unsigned long long totChars = f.size();
    QString fileFormat;
    QString line;
    bool skipTotals = (totChars > 100*1024); // 100K is the max we read

    unsigned int bytesRead = 0;
    while (!f.atEnd())
    {
        f.readLine(line, 4096);

        int len = line.length();

        // The checks below are necessary to handle embedded NULLs
        // QFile::readLine() does not handle them well
        bytesRead += len;
        if (bytesRead > totChars)
            break;
        if (len == 0)
            break;

        if (firstline)
        {
            firstline = false;
            if (line[len-1]=='\n')
            {
                if (len>=2 && line[len-2]=='\r')
                    fileFormat = i18n("DOS");
                else
                    fileFormat = i18n("UNIX");
            }
            else if (line[len-1]=='\r')
                fileFormat = i18n("Macintosh");
            if (skipTotals)
                break;
        }

        totWords += (QStringList::split(QRegExp("\\s+"), line)).count();
        totLines++;
    }

    if (fileFormat.isEmpty())
        fileFormat = i18n("Unknown");

    kdDebug(7034) << "Lines: " << totLines << endl;
    kdDebug(7034) << "Words: " << totWords << endl;
    kdDebug(7034) << "Characters: " << totChars << endl;
    kdDebug(7034) << "fileFormat: " << fileFormat << endl;

    KFileMetaInfoGroup group = appendGroup(info, "General");
    if (!skipTotals)
    {
        appendItem(group, "Lines", totLines);
        appendItem(group, "Words", totWords);
    }
    appendItem(group, "Characters", totChars);
    appendItem(group, "Format", fileFormat);

    return true;
}

#include "kfile_txt.moc"
