/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-203 by Stanislav Visnovsky
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
#ifndef VALIDATEDLG_H
#define VALIDATEDLG_H

#include <qmap.h>

#include <kdialogbase.h>
#include <kurl.h>

class CatManListItem;
class ValidateProgressWidget;
class KDataToolInfo;
class KDataTool;
class KPopupMenu;

struct IgnoreItem
{
    KURL fileURL;
    QStringList msgid;
    QStringList msgstr;
    uint index;
    QString validationTool;
};

QDataStream & operator<<( QDataStream & stream, const IgnoreItem &ident );
QDataStream & operator>>( QDataStream & stream, IgnoreItem & ident);

class ValidateProgressDialog : public KDialogBase
{
    Q_OBJECT

public:
    ValidateProgressDialog(const QString& ignoreURL, QWidget* parent, const char *name=0);
    virtual ~ValidateProgressDialog();
    
    void setIgnoreFuzzy(bool enable) { _ignoreFuzzy = enable; }
    void setMarkAsFuzzy(bool enable) { _setAsFuzzy = enable; }

public slots:
    void validate(const KDataToolInfo &tool, const QPtrList<CatManListItem> files);

signals:
    void errorDoubleClicked(const QString file, const int messageNumber);

private slots:
    /** implementation of the validation itself */
    void validate_internal();
    
    /** call this to stop current validation*/
    void stop();
    
    /** If user doubleclicked an item, this slot will be invoked.
     *  It emits errorDoubleClicked signal for corresponding error*/
    void errorItemDoubleClicked(QListBoxItem * item);
    
    /** show our context menu */
    void showContextMenu(QListBoxItem * item, const QPoint & pos);

private:
    void readIgnores();
    void writeIgnores();
    
    ValidateProgressWidget *_mainWidget;
    QString _ignoreURL;
    KDataTool* _tool;
    QString _toolID;
    QPtrList<CatManListItem> _files;
    bool _stopped;
    KPopupMenu* _errorMenu;
    
    bool _ignoreFuzzy;
    bool _setAsFuzzy;
    
    QValueList<IgnoreItem> _ignores;
    QMap<QString,IgnoreItem> _errors;
};

#endif // VALIDATEDLG_H
