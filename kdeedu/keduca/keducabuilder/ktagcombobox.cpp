/*
 * ktagcombobox.cpp - A combobox with support for submenues, icons and tags
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#define INCLUDE_MENUITEM_DEF 1
#include <qpainter.h>
#include <qpopupmenu.h>
#include <kdebug.h>


#include "ktagcombobox.h"
#include "ktagcombobox.moc"

KTagComboBox::~KTagComboBox ()
{
    delete _popup;
    delete _tags;
}

KTagComboBox::KTagComboBox (QWidget * parent, const char *name)
    : QComboBox(parent, name)
{
    _popup = new QPopupMenu(this);
    _tags = new QStringList;
    connect( _popup, SIGNAL(activated(int)),
             SLOT(internalActivate(int)) );
    connect( _popup, SIGNAL(highlighted(int)),
             SLOT(internalHighlight(int)) );
}

void KTagComboBox::popupMenu()
{
    _popup->popup( mapToGlobal( QPoint(0,0) ), _current );
}

void KTagComboBox::keyPressEvent( QKeyEvent *e )
{
    int c;

    if ( ( e->key() == Key_F4 && e->state() == 0 ) ||
         ( e->key() == Key_Down && (e->state() & AltButton) ) ||
         ( e->key() == Key_Space ) ) {
        if ( count() ) {
            _popup->setActiveItem( _current );
            popupMenu();
        }
        return;
    } else {
        e->ignore();
        return;
    }

    c = currentItem();
    emit highlighted( c );
    emit activated( c );
}

void KTagComboBox::mousePressEvent( QMouseEvent * )
{
    popupMenu();
}

void KTagComboBox::internalActivate( int index )
{
    if (_current == index) return;
    _current = index;
    emit activated( index );
    repaint();
}

void KTagComboBox::internalHighlight( int index )
{
    emit highlighted( index );
}

void KTagComboBox::clear()
{
    _popup->clear();
    _tags->clear();
}

int KTagComboBox::count() const
{
    return _tags->count();
}

static inline QPopupMenu *checkInsertIndex(QPopupMenu *popup, const QStringList *tags, const QString &submenu, int *index)
{
    int pos = tags->findIndex(submenu);

    QPopupMenu *pi = 0;
    if (pos != -1)
    {
        QMenuItem *p = popup->findItem(popup->idAt(pos));
        pi = p?p->popup():0;
    }
    if (!pi) pi = popup;

    if (*index > (int)pi->count())
        *index = -1;

    return pi;
}

void KTagComboBox::insertItem(const QIconSet& icon, const QString &text, const QString &tag, const QString &submenu, int index )
{
    QPopupMenu *pi = checkInsertIndex(_popup, _tags, submenu, &index);
    pi->insertItem(icon, text, count(), index);
    _tags->append(tag);
}

void KTagComboBox::insertItem(const QString &text, const QString &tag, const QString &submenu, int index )
{
    QPopupMenu *pi = checkInsertIndex(_popup, _tags, submenu, &index);
    pi->insertItem(text, count(), index);
    _tags->append(tag);
}

void KTagComboBox::insertSeparator(const QString &submenu, int index)
{
    QPopupMenu *pi = checkInsertIndex(_popup, _tags, submenu, &index);
    pi->insertSeparator(index);
    _tags->append(QString::null);
}

void KTagComboBox::insertSubmenu(const QString &text, const QString &tag, const QString &submenu, int index)
{
    QPopupMenu *pi = checkInsertIndex(_popup, _tags, submenu, &index);
    QPopupMenu *p = new QPopupMenu(pi);
    pi->insertItem(text, p, count(), index);
    _tags->append(tag);
    connect( p, SIGNAL(activated(int)),
             SLOT(internalActivate(int)) );
    connect( p, SIGNAL(highlighted(int)),
             SLOT(internalHighlight(int)) );
}

void KTagComboBox::changeItem( const QString &text, int index )
{
    _popup->changeItem( text, index);
    if (index == _current)
        repaint();
}

void KTagComboBox::paintEvent( QPaintEvent * ev)
{
    QComboBox::paintEvent(ev);

    QPainter p (this);

    // Text
    QRect clip(2, 2, width() - 4, height() - 4);
    if ( hasFocus())
        p.setPen( colorGroup().highlightedText() );
    p.drawText(clip, AlignCenter | SingleLine, _popup->text( _current ));

    // Icon
    QIconSet *icon = _popup->iconSet( _current );
    if (icon) {
        QPixmap pm = icon->pixmap();
        p.drawPixmap( 4, (height()-pm.height())/2, pm );
    }
}

bool KTagComboBox::containsTag( const QString &str ) const
{
    return _tags->contains(str) > 0;
}

QString KTagComboBox::currentTag() const
{
    return *_tags->at(currentItem());
}

QString KTagComboBox::tag(int i) const
{
    if (i < 0 || i >= count())
    {
        kdDebug() << "KTagComboBox::tag(), unknown tag " << i << endl;
        return QString::null;
    }
    return *_tags->at(i);
}

int KTagComboBox::currentItem() const
{
    return _current;
}

void KTagComboBox::setCurrentItem(int i)
{
    if (i < 0 || i >= count()) return;
    _current = i;
    repaint();
}

void KTagComboBox::setCurrentItem(const QString &code)
{
    int i = _tags->findIndex(code);
    if (code.isNull())
        i = 0;
    if (i != -1)
        setCurrentItem(i);
}

void KTagComboBox::setFont( const QFont &font )
{
    QComboBox::setFont( font );
    _popup->setFont( font );
}
