/* This file is part of the KDE project
   Copyright (C) 2001 Holger Freyther <freyther@yahoo.com>
   

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; version 2
	 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __kdirmenu_h
#define __kdirmenu_h

#include <qpopupmenu.h>
#include <qptrlist.h>
#include <qmap.h>

class KAction;
class KURL;

class KDirMenu : public QPopupMenu {
    Q_OBJECT
public:
    KDirMenu( QWidget *parent, const KURL &src, const QString &_path,
            const QString &name, bool showfiles = false  );
    ~KDirMenu( );
    void setPath( const QString &_path);
    void insert( KDirMenu *menu, const QString &path );
protected:
    int target_id;
    static QMap<QString, QPixmap> *_icons;
		
signals:
    void fileChosen( const QString &_path );
private:
    QString path;
    QString name;
    KURL src;
    KAction *action;
    QPtrList<KDirMenu> children;
    void initIconMap( );
public slots:
    void slotAboutToShow( );
    void slotAboutToHide( );
    void slotFileSelected(const QString &_path );
    /** No descriptions */
    void new_slot();
};

#endif // __kdirmenu_h

