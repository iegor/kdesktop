//////////////////////////////////////////////////////////////////////////////
// qinxstyleconfig.h
// -------------------
// Config module for Qinx widget style
// -------------------
// Copyright (c) 2002-2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef QINXSTYLECONFIG_H
#define QINXSTYLECONFIG_H

#include "styledialog.h"

class KConfig;

class QinxStyleConfig : public StyleDialog
{
    Q_OBJECT
public:
    QinxStyleConfig(QWidget* parent);
    ~QinxStyleConfig();
    
signals:
    void changed(bool);

public slots:
    void save();
    void defaults();

protected slots:
    void updateChanged();

private:
    bool oldphotontabs;
    bool oldphotonmenus;
    bool oldusegradients;
    bool oldhighlights;
};


#endif // QINXSTYLECONFIG_H
