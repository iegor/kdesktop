/*
 * Mouspedometa
 *      Based on the original Xodometer VMS/Motif sources.
 *
 * Written by Armen Nakashian
 *            Compaq Computer Corporation
 *            Houston TX
 *            22 May 1998
 *
 * If you make improvements or enhancements to Mouspedometa, please send
 * them back to the author at any of the following addresses:
 *
 *              armen@nakashian.com
 *
 * Thanks to Mark Granoff for writing the original Xodometer, and
 * the whole KDE team for making such a nice environment to write
 * programs in.
 *
 *
 * This software is provided as is with no warranty of any kind,
 * expressed or implied. Neither Digital Equipment Corporation nor
 * Armen Nakashian will be held accountable for your use of this
 * software.
 */

// -*-C++-*-;

#ifndef KODOMETER_H
#define KODOMETER_H

#include <math.h>

#include <qlabel.h>
#include <qptrlist.h>
#include <qkeycode.h>
#include <qtooltip.h>

#include <kaboutdata.h>
#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <ktoolbar.h>
#include <kwin.h>

#include <X11/Xlib.h>

#include "kimagenumber.h"

#define MAXARGS 25
#define K_Left 1
#define K_Right 2
#define K_Top 3
#define K_Bottom 4
#define MAX_SCREEN 16
#define MAJOR_VERSION 3
#define MINOR_VERSION 0

const int inch = 0;
const int foot = 1;
const int mile = 2;
const int MAX_UNIT = 3;
typedef int Units;


typedef struct {
    Window root;
    Screen *scr;
    int height, width;
    double PixelsPerMM;
} _screenInfo;


typedef struct conversionEntry {
    Units fromUnit;
    const char *fromUnitTag;
    const char *fromUnitTagPlural;
    double maxFromBeforeNext;
    double conversionFactor;
    const char *toUnitTag;
    const char *toUnitTagPlural;
    double maxToBeforeNext;
    int printPrecision;
} conversionEntry;

class Kodometer : public QFrame
{
	Q_OBJECT

public:
	Kodometer(QWidget* = 0, const char* = 0);
	~Kodometer() {}
	void refresh(void);
	void readSettings(void);
	void saveSettings(void);

	void timerEvent(QTimerEvent*);
	void mousePressEvent(QMouseEvent*);
	bool eventFilter( QObject *, QEvent *e );

public slots:
	void toggleEnabled();
	void toggleUnits();
	void toggleAutoReset();
	void resetTrip();
	void resetTotal();
	void quit();

protected:
	int dontRefresh;

	double multiplier(Units);
	int CalcDistance(void);
	QString FormatDistance(double &dist, Units);
	void FindAllScreens();

	KImageNumber* tripLabel;
	KImageNumber* totalLabel;
	KImageNumber* speedLabel;

	KPopupMenu* menu;
	KHelpMenu* help;
	int metricID;
	int autoResetID;
	int enabledID;

	double speed;
	QPtrList<double> speeds;
	double lastDistance;
	int distanceID;
	int speedID;

	Display *display;
	Window root, RootIDRet, ChildIDRet;
	int WinX, WinY;
	unsigned int StateMask;
	int XCoord, YCoord;
	int lastXCoord, lastYCoord;

	Units lastDUnit, lastTUnit;

	int pointerScreen, lastPointerScreen;
	int Enabled;
	int UseMetric;
	int AutoReset;
	double Distance, TripDistance;
	Units distanceUnit, tripDistanceUnit;
	char startDate_s[128];
	int cyclesSinceLastSave;
	int pollInterval;
	int saveFrequency;

	_screenInfo screenInfo[MAX_SCREEN];
	int screenCount;
};

#endif
