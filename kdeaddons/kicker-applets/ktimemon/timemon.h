/* -*- C++ -*- */

/**********************************************************************/
/*   TimeMon (c)  1994  Helmut Maierhofer			      */
/*   KDE-ified M. Maierhofer 1998                                     */
/*   maintained by Dirk A. Mueller <mueller@kde.org                   */
/**********************************************************************/

/*
 * timemon.h
 *
 * Definitions for the timemon widget.
 */

#ifndef TIMEMON_H
#define TIMEMON_H

#include <qtooltip.h>
#include <kiconloader.h>
#include <kpanelapplet.h>

// -- global constants ---------------------------------------------------

const int MAX_MOUSE_ACTIONS = 3; // event handlers for the three buttons only

// -- forward declaration ------------------------------------------------
class KSample;
class KConfDialog;
class QPaintEvent;
class QMouseEvent;
class QPainter;
class KProcess;
class KShellProcess;
class KHelpMenu;
class KPopupMenu;

// -- KTimeMon declaration -----------------------------------------------

/*
 * KTimeMon
 *
 * This is the main widget of the application. It handles the configuration
 * dialog and may have an associated KTimeMonWidget in the panel (in which
 * case it hides itself).
 */

class KTimeMon : public KPanelApplet, QToolTip {
    Q_OBJECT
public:
    enum MouseAction { NOTHING, SWITCH, MENU, COMMAND };

    KTimeMon(const QString& configFile, Type t = Normal, int actions = 0,
             QWidget *parent = 0, const char *name = 0);
    virtual ~KTimeMon();

    void writeConfiguration();	// write back the configuration data

    // reimplemented from KPanelApplet
    virtual int widthForHeight(int height) const;
    virtual int heightForWidth(int width) const;

    virtual void preferences();

    void stop();
    void cont();

public slots:
    void timeout();		// timer expired
    void save();			// session management callback
    void apply();			// apply configuration information

protected:
    virtual void maybeTip(const QPoint&);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void updateConfig(KConfDialog *d);
    virtual void paintEvent(QPaintEvent *event);

private slots:			// called from the menu
    void configure();		// show the configuration dialog
    void orientation();		// switch vertical/horizontal orientation
    void commandStderr(KProcess *proc, char *buffer, int length);

private:
    void runCommand(int index);
    void paintRect(int x, int y, int w, int h, QColor c, QPainter *p);

    unsigned interval;
    bool autoScale;
    unsigned pageScale, swapScale, ctxScale;
    KPopupMenu* menu;
    KHelpMenu* hmenu;
    QTimer* timer;
    KConfDialog *configDialog;
    MouseAction mouseAction[MAX_MOUSE_ACTIONS];
    QString mouseActionCommand[MAX_MOUSE_ACTIONS];
    KShellProcess *bgProcess;

    KSample *sample;
    QColor kernelColour, userColour, niceColour, iowaitColour;
    QColor usedColour, buffersColour, cachedColour, mkernelColour;
    QColor swapColour, bgColour;
    bool vertical, tooltip;

    friend class KConfDialog;
};

#endif // TIMEMON_H
