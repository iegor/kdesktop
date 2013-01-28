/************************************************************

Copyright (c) 1996-2002 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "datepicker.h"


#include <kdatepicker.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kwin.h>

DatePicker::DatePicker(QWidget *parent, const QDate& date)
    : QVBox( parent, 0, WType_TopLevel | WDestructiveClose |
             WStyle_Customize | WStyle_StaysOnTop | WStyle_NoBorder )
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    KWin::setOnAllDesktops( handle(), true );
    picker = new KDatePicker(this, date);
    picker->setCloseButton(true);

    /* name and icon for kicker's taskbar */
    setCaption(i18n("Calendar"));
    setIcon(SmallIcon("date"));
}

void DatePicker::keyReleaseEvent(QKeyEvent *e)
{
	DATEPICKER_INHERITED::keyReleaseEvent(e);
	if (e->key() == Qt::Key_Escape)
		close();
}
