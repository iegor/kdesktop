//-----------------------------------------------------------------------------
//
// kbanner - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//

#ifndef __BANNER_H__
#define __BANNER_H__

#include <qtimer.h>

#include <kscreensaver.h>
#include <kdialogbase.h>

#define SATURATION 150
#define VALUE 255

class QLineEdit;
class KColorButton;
class KRandomSequence;

class KBannerSaver : public KScreenSaver
{
    Q_OBJECT
public:
    KBannerSaver( WId id );
    virtual ~KBannerSaver();

    void setSpeed( int spd );
    void setFont( const QString &family, int size, const QColor &color,
		    bool b, bool i );
    void setMessage( const QString &msg );
    void setTimeDisplay();
    void setCyclingColor(bool on);
    void setColor( QColor &color);

private:
    void readSettings();
    void initialize();

protected slots:
    void slotTimeout();

protected:
    QFont   font;
    QTimer	timer;
    QString	fontFamily;
    int		fontSize;
    bool	bold;
    bool	italic;
    QColor	fontColor;
    bool	cyclingColor;
    int		currentHue;
    bool	needUpdate;
    bool	needBlank;
    QString	message;
    bool	showTime;
    int		xpos, ypos, step, fsize;
    KRandomSequence *krnd;
    int		speed;
    int		colorContext;
    QPixmap	pixmap;
    QSize	pixmapSize;
};


class KBannerSetup : public KDialogBase
{
    Q_OBJECT
public:
    KBannerSetup( QWidget *parent = NULL, const char *name = NULL );

protected:
    void readSettings();
    void fillFontSizes();

private slots:
    void slotFamily( const QString & );
    void slotSize( int );
    void slotSizeEdit(const QString &);
    void slotColor(const QColor &);
    void slotCyclingColor(bool on);
    void slotBold( bool );
    void slotItalic( bool );
    void slotSpeed( int );
    void slotMessage( const QString & );
    void slotOk();
    void slotHelp();
    void slotTimeToggled(bool on);

private:
    QWidget *preview;
    KColorButton *colorPush;
    KBannerSaver *saver;
    QLineEdit *ed;
    QComboBox* comboSizes;

    QString message;
    bool    showTime;
    QString fontFamily;
    int	    fontSize;
    QColor  fontColor;
    bool    cyclingColor;
    bool    bold;
    bool    italic;
    int	    speed;
    QValueList<int> sizes;
};

#endif

