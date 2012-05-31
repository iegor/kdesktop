/**
 * Copyright (C) 1997-2002 the KGhostView authors. See file AUTHORS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MARKLIST_H
#define MARKLIST_H

#include <qcheckbox.h>
#include <qtable.h>

class KGVMiniWidget;

class MarkListItem : public QWidget
{
    Q_OBJECT
public:
    MarkListItem( QWidget *parent, const QString &text, const QString &tip, const QColor &color, KGVMiniWidget*, int );

    bool isChecked() const;

public slots:
    void toggle();
    void setChecked( bool checked );
    void setPixmap( QPixmap thumbnail );

    void setSelected( bool selected );

private:
    void resizeEvent( QResizeEvent * );
    void paintEvent( QPaintEvent* );
private:
    QWidget *_thumbnailW;
    QCheckBox *_checkBox;
    QColor _backgroundColor;
    KGVMiniWidget* _miniWidget;
    const int _pageNum;
    bool _requested;
};

class MarkList: public QTable
{
    Q_OBJECT

public:
    MarkList( QWidget* parent = 0, const char* name = 0, KGVMiniWidget* = 0 );

    QValueList<int> markList() const;
    void insertItem( const QString& text, int index = -1,
                     const QString& tip = QString::null );

public slots:
    void select( int index );
    void markCurrent();
    void markAll();
    void markEven();
    void markOdd();
    void toggleMarks();
    void removeMarks();
    void clear();

protected:
    virtual void viewportResizeEvent ( QResizeEvent * );

signals:
    void selected( int );

private:
    int _selected;
    KGVMiniWidget* _miniWidget;
};

#endif

// vim:sw=4:sts=4:ts=8:noet
