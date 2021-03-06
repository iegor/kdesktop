 /***************************************************************************

              dialog page for characters from the phonetic alphabet

    -----------------------------------------------------------------------

    begin          : Sun Dec 9 2001

    copyright      : (C) 2001-2002 Ewald Arnold <kvoctrain@ewald-arnold.de>
                     (C) 2001-2002 The KDE-EDU team
                     (C) 2005 Peter Hedlund <peter.hedlund@kdemail.net>

    -----------------------------------------------------------------------

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef PHONETICENTRYPAGE_H
#define PHONETICENTRYPAGE_H

#include <qpushbutton.h>

#include <kdialogbase.h>

class PhoneticEntryPage : public KDialogBase
{
  Q_OBJECT

public:
  PhoneticEntryPage(const QFont &ipafont, QWidget *parent = 0, const char *name = 0, bool modal = false);

signals:
  void charSelected(wchar_t);

protected:
  void keyPressEvent( QKeyEvent *e );

  friend class PhoneticButton;
};


class PhoneticButton : public QPushButton
{
  Q_OBJECT

public:
  PhoneticButton (const QString & text, QWidget * parent, PhoneticEntryPage *_page, const char * name = 0)
  : QPushButton (text, parent, name), page(_page) {}

protected slots:
  void slotClicked();

protected:
  PhoneticEntryPage *page;
};


#endif
