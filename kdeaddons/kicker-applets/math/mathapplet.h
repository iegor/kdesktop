/*****************************************************************

Based on code 'Run' applet code, copyright (c) 2000 Matthias Elter <elter@kde.org>

Modifications made by Andrew Coles, 2004 <andrew_coles@yahoo.co.uk>

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

#ifndef __mathapplet_h__
#define __mathapplet_h__

#include <qstring.h>
#include <kpanelapplet.h>

class QLabel;
class QHBox;
class QPushButton;
class KHistoryCombo;
class KPopupMenu;

class MathApplet : public KPanelApplet
{
    Q_OBJECT

public:
    MathApplet(const QString& configFile, Type t = Stretch, int actions = 0,
	      QWidget *parent = 0, const char *name = 0);
    virtual ~MathApplet();

    int widthForHeight(int height) const;
    int heightForWidth(int width) const;

protected:
    void resizeEvent(QResizeEvent*);
    void positionChange(KPanelApplet::Position);

protected slots:
    void evaluate(const QString&);
    void popup_combo();
    void setButtonText();
    void useDegrees();
    void useRadians();

private:

    void initContextMenu();
    void mousePressEvent(QMouseEvent *e);
    
    KHistoryCombo  *_input;
    QLabel         *_label;
    QPushButton    *_btn;
    QHBox          *_hbox;
    KPopupMenu *mContextMenu;
    bool m_hasFocus;
};

#endif
