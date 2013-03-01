//////////////////////////////////////////////////////////////////////////////
// polyesterconfig.h
// -------------------
// Config module for Smooth Blend window decoration
// -------------------
// Copyright (c) 2005 Marco Martin
// -------------------
// derived from Smooth Blend
// Copyright (c) 2005 Ryan Nickell
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

#ifndef polyesterCONFIG_H
#define polyesterCONFIG_H

#include <qobject.h>

class KConfig;
class ConfigDialog;

class polyesterConfig : public QObject {
    Q_OBJECT
public:
    polyesterConfig(KConfig* config, QWidget* parent);
    ~polyesterConfig();

signals:
    void changed();

public slots:
    void load(KConfig*);
    void save(KConfig*);
    void defaults();

protected slots:
    void selectionChanged(int);

private:
    KConfig *config_;
    ConfigDialog *dialog_;
};

#endif // polyesterCONFIG_H
