/* This file is part of the KDE project

   Copyright (C) 2003 Georg Robbers <Georg.Robbers@urz.uni-hd.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _AKREGATORPLUGIN_H_
#define _AKREGATORPLUGIN_H_

#include <konq_popupmenu.h>
#include <kfileitem.h>
#include <kconfig.h>

#include "pluginbase.h"

class KAction;
class KHTMLPart;

namespace Akregator
{

class AkregatorMenu : public KonqPopupMenuPlugin, PluginBase
{
  Q_OBJECT
public:
    AkregatorMenu( KonqPopupMenu *, const char *name, const QStringList &list );
    virtual ~AkregatorMenu();

public slots:
    void slotAddFeed();

protected:
    bool isFeedUrl(const QString &s);
    bool isFeedUrl(const KFileItem *item);

private:
    QStringList m_feedMimeTypes;
    KConfig *m_conf;
    KHTMLPart *m_part;
    QString m_feedURL;
};

}

#endif

