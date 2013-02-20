
/**********************************************************************/
/*   TimeMon (c)  1994  Helmut Maierhofer                             */
/*   KDE-ified M. Maierhofer 1998                                     */
/*   maintained by Dirk A. Mueller <mueller@kde.org>                  */
/**********************************************************************/

/*
 * timemon.h
 *
 * Definitions for the timemon widget.
 */

#include <config.h>

#include <qpainter.h>
#include <qtimer.h>
#include <qtooltip.h>

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "timemon.h"
#include "confdlg.h"
#include "sample.h"

#include "timemon.moc"
#include <stdio.h>

extern "C"
{
    KDE_EXPORT KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
        KGlobal::locale()->insertCatalogue("ktimemon");
        KTimeMon *mon = new KTimeMon(configFile, KPanelApplet::Normal,
                                     KPanelApplet::Preferences, parent, "ktimemon");
        return mon;
    }
}

// Update colour settings with the new ones from the config dialog.
void KTimeMon::updateConfig(KConfDialog *d)
{
    kernelColour = d->getKernelColour();
    userColour = d->getUserColour();
    iowaitColour = d->getIOWaitColour();
    niceColour = d->getNiceColour();
    cachedColour = d->getCachedColour();
    usedColour = d->getUsedColour();
    buffersColour = d->getBuffersColour();
    mkernelColour = d->getMKernelColour();
    swapColour = d->getSwapColour();
    bgColour = d->getBgColour();
}

// -----------------------------------------------------------------------------
// some KPanelApplet API functions

int KTimeMon::widthForHeight(int height) const
{
    int s = (int) (vertical ? 2/3.*height : height);
    return s>=18? s : 18;
}


int KTimeMon::heightForWidth(int width) const
{
    int s = (int) (vertical ? width : 2/3.*width);
    return s>=18? s : 18;
}

void KTimeMon::preferences()
{
    configure();
}


// -----------------------------------------------------------------------------
// Repaint the object; get the current sample and paint the bar graphs
// correspondingly. Use a pixmap to minimise flicker.

void KTimeMon::paintEvent(QPaintEvent *)
{
    int w, h, x, y, b, r;

    w = vertical ? width() : height();
    h = vertical ? height() : width();

    r = w;  // remaining height

    x = 0;

    KSample::Sample s;

    if (sample != 0)
        s = sample->getSample(h);
    else
        s.fill(h);

    QPixmap pixmap(width(), height());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);

    b = r / 3;            // bar width
    r -= b;

    if (bgColour != colorGroup().background())
    {
        paintRect(x, 0, b, h, bgColour, &painter);
    }

    y = h - s.kernel; paintRect(x, y, b, s.kernel, kernelColour, &painter);
    y -= s.iowait; paintRect(x, y, b, s.iowait, iowaitColour, &painter);
    y -= s.user; paintRect(x, y, b, s.user, userColour, &painter);
    y -= s.nice; paintRect(x, y, b, s.nice, niceColour, &painter);

    x += b;
    b = r / 2;
    r -= b;

    if (bgColour != colorGroup().background())
    {
        paintRect(x, 0, b, h, bgColour, &painter);
    }

    y = h - s.mkernel; paintRect(x, y, b, s.mkernel, mkernelColour, &painter);
    y -= s.used; paintRect(x, y, b, s.used, usedColour, &painter);
    y -= s.buffers; paintRect(x, y, b, s.buffers, buffersColour, &painter);
    y -= s.cached; paintRect(x, y, b, s.cached, cachedColour, &painter);

    x += b;
    b = r;

    if (bgColour != colorGroup().background())
    {
        paintRect(x, 0, b, h, bgColour, &painter);
    }

    y = h - s.sused; paintRect(x, y, b, s.sused, swapColour, &painter);
    painter.end();

    bitBlt(this, 0, 0, &pixmap);
}

// -----------------------------------------------------------------------------
// Draw part of a bar, depending on the bar orientation.

void KTimeMon::paintRect(int x, int y, int w, int h, QColor c, QPainter *p)
{
    if (vertical)
        p->fillRect(x, y, w, h, c);
    else
        p->fillRect(width() - y - h, x, h, w, c);
}

// Show a tool-tip with some status information.
void KTimeMon::maybeTip(const QPoint& p)
{
    if (sample == 0) return;    // no associated sample...
    if(!rect().contains(p)) return;

    KSample::Sample s = sample->getSample(100); // scale to 100(%)
    int idle = 100 - s.kernel - s.user - s.nice;
    if ( idle < 0 )
        idle = 0;
    QString str = i18n("cpu: %1% idle\nmem: %2 MB %3% free\nswap: %4 MB %5% free")
                .arg(idle)
                .arg(KGlobal::locale()->formatNumber(s.used/100.*s.mtotal, 0))
                .arg(100-s.used)
                .arg(KGlobal::locale()->formatNumber(s.stotal, 0))
                .arg(100-s.sused);

    tip(rect(), str);
}

// -- KTimeMon definition ------------------------------------------------

// Initialise the member variables, read the configuration data base,
// set up the widget, and start the timer.
KTimeMon::KTimeMon(const QString& configFile, Type type, int actions,
               QWidget *parent, const char *name)
    : KPanelApplet(configFile, type, actions, parent, name, WRepaintNoErase), QToolTip(this),
    configDialog(0), bgProcess(0),
    kernelColour("red1"), userColour("blue"),
    niceColour("yellow"), iowaitColour("darkgreen"),
    usedColour("blue1"), buffersColour("yellow"),
    cachedColour("darkgreen"), mkernelColour("red1"),
    swapColour("cyan3"), bgColour(colorGroup().background())
{
    mouseAction[0] = NOTHING;
    mouseAction[1] = NOTHING;
    mouseAction[2] = MENU;

    KConfig* conf = config();
    conf->setGroup("Parameters");
    interval = conf->readUnsignedNumEntry("Interval", 500);
    autoScale = conf->readBoolEntry("AutoScale", true);

    pageScale = conf->readUnsignedNumEntry("PageScale", 10);
    swapScale = conf->readUnsignedNumEntry("SwapScale", 5);
    ctxScale = conf->readUnsignedNumEntry("ContextScale", 300);
    for (int i = 0; i < MAX_MOUSE_ACTIONS; i++) {
        QString n;
        n.setNum(i);

        mouseAction[i] = (MouseAction)
            conf->readUnsignedNumEntry(QString("MouseAction")+n, mouseAction[i]);
        mouseActionCommand[i] = conf->readPathEntry(QString("MouseActionCommand")+n);
    }

    conf->setGroup("Interface");
    kernelColour = conf->readColorEntry("KernelColour", &kernelColour);
    userColour = conf->readColorEntry("UserColour", &userColour);
    niceColour = conf->readColorEntry("NiceColour", &niceColour);
    iowaitColour = conf->readColorEntry("IOWaitColour", &iowaitColour);
    cachedColour = conf->readColorEntry("CachedColour", &cachedColour);
    usedColour = conf->readColorEntry("UsedColour", &usedColour);
    buffersColour = conf->readColorEntry("BuffersColour", &buffersColour);
    swapColour = conf->readColorEntry("SwapColour", &swapColour);
    mkernelColour = conf->readColorEntry("MKernelColour", &mkernelColour);
    bgColour = conf->readColorEntry("BgColour", &bgColour);

    vertical = conf->readBoolEntry("Vertical", true);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(interval);

    sample = new KSample(this, autoScale, pageScale, swapScale, ctxScale);

    QString aboutmsg = i18n("KTimeMon for KDE\n"
                          "Maintained by Dirk A. Mueller <dmuell@gmx.net>\n"
                          "Written by M. Maierhofer (m.maierhofer@tees.ac.uk)\n"
                          "Based on timemon by H. Maierhofer");

    hmenu = new KHelpMenu(this, aboutmsg);
    menu = new KPopupMenu(this);

    menu->insertTitle( SmallIcon( "ktimemon" ), i18n( "System Monitor" ) ) ;
    menu->insertItem(i18n("Horizontal Bars"), 4);
    menu->insertItem(SmallIcon( "configure" ), i18n( "Preferences..." ), 2);
    menu->insertSeparator();
    menu->insertItem(SmallIcon( "help" ), i18n("Help"), hmenu->menu(), 1);

    menu->connectItem(2, this, SLOT(configure()));
    menu->connectItem(4, this, SLOT(orientation()));

    menu->setCheckable(true);

    vertical = !vertical;               // and similar for orientation
    orientation();
}

// -----------------------------------------------------------------------------

// delete the member variables
KTimeMon::~KTimeMon()
{
    delete sample;
    delete bgProcess;
    KGlobal::locale()->removeCatalogue("ktimemon");
}


// Apply the settings from the configuration dialog and save them.
void KTimeMon::apply()
{
    stop();
    interval = configDialog->getInterval();
    cont();

    updateConfig(configDialog);

    sample->setScaling(configDialog->getAutoScale(),
                       configDialog->getPageScale(),
                       configDialog->getSwapScale(),
                       configDialog->getCtxScale());

    for (int i = 0; i < MAX_MOUSE_ACTIONS; i++) {
        mouseAction[i] = (MouseAction) configDialog->getMouseAction(i);
        mouseActionCommand[i] = configDialog->getMouseActionCommand(i);
    }

    update();
    writeConfiguration();
}

void KTimeMon::stop()
{
    timer->stop();
}

void KTimeMon::cont()
{
    timer->start(interval);
}

// Dump the current configuration entries to the data base.
void KTimeMon::writeConfiguration()
{
    KConfig* conf = config();
    conf->setGroup("Interface");
    conf->writeEntry("KernelColour", kernelColour);
    conf->writeEntry("UserColour", userColour);
    conf->writeEntry("NiceColour", niceColour);
    conf->writeEntry("IOWaitColour", iowaitColour);
    conf->writeEntry("CachedColour", cachedColour);
    conf->writeEntry("UsedColour", usedColour);
    conf->writeEntry("BuffersColour", buffersColour);
    conf->writeEntry("MKernelColour", mkernelColour);
    conf->writeEntry("SwapColour", swapColour);
    conf->writeEntry("BgColour", bgColour);
    conf->writeEntry("Mode", true);
    conf->writeEntry("Vertical", vertical);

    conf->setGroup("Parameters");
    conf->writeEntry("Interval", interval);
    conf->writeEntry("AutoScale", autoScale);
    conf->writeEntry("PageScale", pageScale);
    conf->writeEntry("SwapScale", swapScale);
    conf->writeEntry("ContextScale", ctxScale);
    conf->writeEntry("WidgetSize", size());
    for (int i = 0; i < MAX_MOUSE_ACTIONS; i++) {
        QString n;
        n.setNum(i);

        conf->writeEntry(QString("MouseAction")+n, (unsigned)mouseAction[i]);
        conf->writePathEntry(QString("MouseActionCommand")+n, mouseActionCommand[i]);
    }
    conf->sync();
}

// Make the KSample object update its internal sample and repaint the
// object.
void KTimeMon::timeout()
{
    sample->updateSample();
    update();
}

// This is called when the session management strikes, and also when the
// main program exits with a code of 0 (i.e. there was no error).
void KTimeMon::save()
{
    writeConfiguration();
}

// -----------------------------------------------------------------------------
// Update the configuration dialog with the current values and show it.

void KTimeMon::configure()
{
    if (configDialog == 0) configDialog = new KConfDialog(this);
    configDialog->update();
    configDialog->show();
}

// -----------------------------------------------------------------------------
// Change the orientation of the status bars

void KTimeMon::orientation()
{
    vertical = !vertical;

    KConfig* conf = config();
    conf->setGroup("Interface");
    conf->writeEntry("Vertical", vertical);

    menu->setItemChecked(4, !vertical);

    update();
    emit updateLayout();
}

// Pop up the menu when the appropriate button has been pressed.
void KTimeMon::mousePressEvent(QMouseEvent *event)
{
    if (event == 0) return;

    int index = -1;
    if (event->button() == LeftButton) index = 0;
    else if (event->button() == MidButton) index = 1;
    else if (event->button() == RightButton) index = 2;

    if (index == -1) return;

    switch (mouseAction[index]) {
    case NOTHING:
        break;
    case SWITCH:
        break;
    case MENU:
        menu->popup(mapToGlobal(event->pos()));
    break;
    case COMMAND:
        runCommand(index);
        break;
    }
}

// Start the given command
void KTimeMon::runCommand(int index)
{
    // just in case it still hangs around
    delete bgProcess;

    bgProcess = new KShellProcess;
    *bgProcess << mouseActionCommand[index];
    connect(bgProcess, SIGNAL(receivedStderr(KProcess *, char *, int)),
            this, SLOT(commandStderr(KProcess *, char *, int)));
    bgProcess->start(KProcess::DontCare, KProcess::Stderr);
}

// -----------------------------------------------------------------------------
// Check if there is any diagnostic output (command not found or such)

void KTimeMon::commandStderr(KProcess * /*proc*/, char *buffer, int /*length*/)
{
    QString msgbuf;

    msgbuf  = i18n("Got diagnostic output from child command:\n\n");
    msgbuf += QString::fromLocal8Bit(buffer);

    KMessageBox::information(this, msgbuf);
}


// -----------------------------------------------------------------------------
