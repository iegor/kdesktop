/**************************************************************************

    channelcfgdlg.h  - The channel view config dialog
    Copyright (C) 1998  Antonio Larrosa Jimenez

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _channelcfgdlg_h_
#define _channelcfgdlg_h_

#include <kdialogbase.h>

class QVButtonGroup;
class QRadioButton;
class ChannelViewConfigDialog : public KDialogBase
{
    Q_OBJECT

public:
    
    ChannelViewConfigDialog(QWidget *parent,const char *name);
    
private:
    QVButtonGroup *qbg;
    QRadioButton *rb0;
    QRadioButton *rb1;
        
public slots:
    void modeselected(int idx);

public:
    static int selectedmode;
};

#endif
