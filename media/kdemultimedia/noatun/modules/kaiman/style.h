// -*- C++ -*-
/*
   Copyright (c) 2000 Stefan Schimanski (1Stein@gmx.de)
                 1999-2000 Christian Esken (esken@kde.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef KaimanStyle_H
#define KaimanStyle_H

#include <qstring.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qevent.h>
#include <qptrvector.h>

class KaimanStyleElement : public QWidget
{
    Q_OBJECT
public:
    KaimanStyleElement(QWidget *parent, const char *name=0);
    ~KaimanStyleElement();

    virtual void loadPixmaps(QString &val_s_filename);

    QString     element;
    QString     filename;
    QPoint      upperLeft;
    QSize      dimension;

    bool options[3];
    int digits;

    bool optionPrelight;
    bool optionStatuslight;

    int pixmapLines;
    int pixmapColumns;

    QPtrVector<QPixmap> pixmaps;

public slots:
    void setPixmap( int num );

protected:
    void paintEvent(QPaintEvent *qpe);
    void dropEvent( QDropEvent *event );
    void dragEnterEvent( QDragEnterEvent *event );

    int pixmapNum;

private:
    int _currentPixmap;
};


class KaimanStyleMasked : public KaimanStyleElement
{
 Q_OBJECT
public:
    KaimanStyleMasked(QWidget *parent, const char *name=0)
        : KaimanStyleElement( parent, name ) {};

    virtual void loadPixmaps(QString &val_s_filename)
        {
            KaimanStyleElement::loadPixmaps( val_s_filename );
            if(pixmaps[0]->mask())
                setMask(*pixmaps[0]->mask());
        };
};


class KaimanStyleButton : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleButton(QWidget *parent, const char *name=0);
    ~KaimanStyleButton();

    // Button states.
    enum { NormalUp=0, NormalDown, LitUp, LitDown, PrelightUp, PrelightLitUp, StateListEND };

    QPtrVector<int> I_pmIndex;

    void setLit(bool);
    void setPrelight(bool);
    void setDown(bool);
    bool lit();
    bool prelit();
    bool down();
    void updateButtonState();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *qme);
    void mouseReleaseEvent(QMouseEvent *qme);
    void enterEvent(QEvent * );
    void leaveEvent ( QEvent * );

private:
    int i_i_currentState;
    bool i_b_lit;
    bool i_b_prelit;
    bool i_b_down;
};


class KaimanStyleSlider : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleSlider(int min, int max, QWidget *parent, const char *name=0);
    ~KaimanStyleSlider();

    int value() { return _value; };

    static const bool optionVertical;
    static const bool optionReversed;

public slots:
    void setValue( int value );
    void setValue( int value, int min, int max );

signals:
    void newValue( int value );
    void newValueDrag( int value );
    void newValueDrop( int value );

protected:
    void mouseMoveEvent(QMouseEvent *qme);
    void mousePressEvent(QMouseEvent *qme);
    void mouseReleaseEvent(QMouseEvent *qme);
    void paintEvent(QPaintEvent *qpe);
    void enterEvent(QEvent * );
    void leaveEvent ( QEvent * );

    int pos2value( int x, int y );

    bool _down;
    bool _lit;
    int _value;
    int _min, _max;
};


class KaimanStyleBackground : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleBackground(QWidget *parent, const char *name=0);
    ~KaimanStyleBackground();

protected:
    void mousePressEvent(QMouseEvent *qme);
    void mouseReleaseEvent(QMouseEvent *qme);
    void mouseMoveEvent(QMouseEvent *qme);

private:
    bool i_b_move;
    QPoint i_point_dragStart;
    QPoint i_point_lastPos;
};


class KaimanStyleValue : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleValue(int min, int max, QWidget *parent, const char *name=0);
    ~KaimanStyleValue();

    int value() { return _value; };

public slots:
    void setValue( int value );
    void setValue( int value, int min, int max );

private:
    int _min, _max, _value;
};


class KaimanStyleState : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleState(QWidget *parent, const char *name=0);
    ~KaimanStyleState();

    int value() { return _value; };

public slots:
    void setValue( int value );

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *qme);

private:
    int _value;
};


class KaimanStyleNumber : public KaimanStyleElement
{
 Q_OBJECT
public:
    KaimanStyleNumber(QWidget *parent, const char *name=0);
    ~KaimanStyleNumber();

    virtual void loadPixmaps(QString &val_s_filename);

    static const bool optionCentered = 1;

    int value() { return _value; };

public slots:
    void setValue( int value );

protected:
    void paintEvent(QPaintEvent *qpe);

private:
    int _value;
};


class KaimanStyleText : public KaimanStyleElement
{
 Q_OBJECT
public:
    KaimanStyleText(QWidget *parent, const char *name=0);
    ~KaimanStyleText();

    virtual void loadPixmaps(QString &val_s_filename);

    static const bool optionExtended;

    QString value() { return _value; };

    void startAnimation( int delay );
    void stopAnimation();

public slots:
    void setValue( QString value );

protected:
    void paintEvent(QPaintEvent *qpe);

protected slots:
    void timeout();

private:
    QString _value;
    int _pos;
    int _direction;
    int _delay;
    QTimer *_timer;
};


class KaimanStyleAnimation : public KaimanStyleMasked
{
 Q_OBJECT
public:
    KaimanStyleAnimation(int delay, QWidget *parent, const char *name=0);
    ~KaimanStyleAnimation();

public slots:
    void start();
    void pause();
    void stop();

protected:
    void timeout();

private:
    int _delay,_frame;
    QTimer *_timer;
};

class KaimanStyle : public QWidget
{
 Q_OBJECT
public:
    KaimanStyle(QWidget *parent, const char *name=0);
    ~KaimanStyle();

    enum { background, mask, play_Button, stop_Button, pause_Button, prev_Button, next_Button,  repeat_Button, shuffle_Button, playlist_Button, mixer_Button, exit_Button, Iconify_Button, Config_Button, Alt_Skin_Button, Minute_Number, Second_Number, in_Rate_Number, in_Hz_Number, song_Number, status_Item, cPU_Number, digit_Large, digit_Small_Default, title, volume_Item, volume_Slider, position_Item, position_Slider };

    enum { ParsingError=1, FileNotFound };

    /// Finds a style element, according  to it's name. Returns 0 when element is not available.
    KaimanStyleElement* find(const char* val_s_elemName);

    /// Tries to load the given style and returns success (true) or failure (false)
    bool loadStyle(const QString &styleName, const QString &descFile="skindata" );
    QString skinName() { return i_skinName; };

    /// Returns the mask
    QBitmap* Mask();

    virtual bool eventFilter( QObject *o, QEvent *e );

private:
    // Parses the "skindata" file and returns success (true) or failure (false)
    int parseStyleFile(QString &l_s_tmpName);
    QString getToken(QString &val_s_string, char val_c_separator);
    void interpretTokens(QStringList& ref_s_tokens);
    bool loadPixmaps();

    /// The name of the style, e.g. "k9"
    QString     i_s_styleName;
    /// The base directory, where the style is found. For example
    /// "/opt/kde/share/apps/kaiman/Skins/k9/" or  "/opt/kde/share/apps/kaiman/Skins/k9.tgz"
    QString     i_s_styleBase;


    // The mask of the complete style. Used for doing shaped windows
    QBitmap     i_bitmap_Mask;

    /// All style elements are stored here.
    QPtrVector<KaimanStyleElement> I_styleElem;

    // The parent window. In other words: The container that holds all the KaimanStyleElement's
    QWidget*            i_qw_parent;

    QPtrList<QWidget> i_sliders;
    bool i_eventSemaphore;

    QString i_smallFont;
    QString i_largeFont;
    QString i_skinName;
};


#endif
