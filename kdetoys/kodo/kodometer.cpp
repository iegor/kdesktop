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

#include "kodometer.h"

const double speedInterval = 500.0;
const double distanceInterval = 10.0;
const int speedSamples = 10;

static	struct conversionEntry ConversionTable[MAX_UNIT] = {
	{inch, I18N_NOOP("inch"), I18N_NOOP("inches"), 12.0,  2.54,
	       I18N_NOOP("cm"),   I18N_NOOP("cm"),     100.0, 3},
	{foot, I18N_NOOP("foot"),  I18N_NOOP("feet"),   5280.0, 0.3048,
	       I18N_NOOP("meter"), I18N_NOOP("meters"), 1000.0, 4},
	{mile, I18N_NOOP("mile"), I18N_NOOP("miles"), -1.0, 1.609344,
	       I18N_NOOP("km"),   I18N_NOOP("km"),    -1.0, 5}};

/*
 * Set the program up, do lots of ugly initialization.
 * Note that we use installEventFilter on the two KImageNumber's
 * to make clicks on them bring up the context-menu.
 */
Kodometer::Kodometer(QWidget* parent, const char* name)
	: QFrame(parent, name),
	dontRefresh(false),
        speed(0.0),
	lastDistance(0.0),
	XCoord(0), YCoord(0),
	lastXCoord(0), lastYCoord(0),
        pointerScreen(-1),
	lastPointerScreen(-1),
	Enabled(true),
	cyclesSinceLastSave(0),
	pollInterval(10),
	saveFrequency(10)
{
	display = KApplication::kApplication()->getDisplay();
	FindAllScreens();

	root = RootWindow(display, DefaultScreen(display));

	readSettings();
	if(AutoReset) {
		dontRefresh = true;
		resetTrip();
		dontRefresh = false;
	}

	lastDistance = Distance;

	lastDUnit = distanceUnit;
	lastTUnit = tripDistanceUnit;

	totalLabel = new KImageNumber(locate("appdata", "numbers.png"), this);
	tripLabel  = new KImageNumber(locate("appdata", "numbers.png"), this);

	totalLabel->installEventFilter(this);
	tripLabel->installEventFilter(this);

	// setup help menu
	help = new KHelpMenu(this, KGlobal::instance()->aboutData(), false);
	KPopupMenu* helpMnu = help->menu();

	// Make the popup menu
	menu = new KPopupMenu();

	menu->insertTitle(kapp->miniIcon(), KGlobal::instance()->aboutData()->programName());

	enabledID = menu->insertItem(i18n("&Enable"), this, SLOT(toggleEnabled()));
	metricID = menu->insertItem(i18n("&Metric Display"), this,
		SLOT(toggleUnits()));
	autoResetID = menu->insertItem(i18n("Auto &Reset Trip"), this,
		SLOT(toggleAutoReset()));
	menu->insertItem(i18n("Reset &Trip"), this, SLOT(resetTrip()));
	menu->insertItem(i18n("Reset &Odometer"), this, SLOT(resetTotal()));
	menu->insertSeparator();

	menu->insertItem(SmallIconSet("help"), i18n("&Help"), helpMnu);

	menu->insertItem(SmallIconSet("exit"), i18n("&Quit"), this, SLOT(quit()));
	menu->setCheckable(true);

	menu->setItemChecked(enabledID, Enabled);
	menu->setItemChecked(metricID, UseMetric);
	menu->setItemChecked(autoResetID, AutoReset);

	//start the timers that will rifresh the counter
	distanceID = startTimer((int)distanceInterval);
	speedID = startTimer((int)speedInterval);

	tripLabel->move(0, totalLabel->height());
	setFixedSize(tripLabel->width(),
		totalLabel->height() + tripLabel->height());

	UseMetric =! UseMetric;
	toggleUnits();

	refresh();
}

/*
 * Now I'm not really sure what this does.  I assume its here to find
 * all the displays on your system, and measure them.  During the mouse
 * tracking phase, we use the information stored here to determine how
 * far the mouse moved on a given screen.
 *
 * The point is, since you might have one 17" screen and on 21" screen,
 * lets measure them differently.  Surely this level of accurasy is
 * only provide to prove that the original author was a man's man.
 */
void Kodometer::FindAllScreens(void)
{
	int Dh, DhMM, Dw, DwMM;
	double vPixelsPerMM, hPixelsPerMM;

	screenCount = ScreenCount(display);
//	kdDebug() << "Display has " <<  screenCount <<
//		" screen" << (screenCount == 1 ? "" : "s") << endl;

	for(int i = 0; i < screenCount; i++) {
//		kdDebug() << "Screen " << i << endl;
		screenInfo[i].root = RootWindow(display, i);
		screenInfo[i].scr = XScreenOfDisplay(display, i);

		screenInfo[i].height = Dh = HeightOfScreen(screenInfo[i].scr);
		DhMM = HeightMMOfScreen(screenInfo[i].scr);
		screenInfo[i].width = Dw = WidthOfScreen(screenInfo[i].scr);
		DwMM = WidthMMOfScreen(screenInfo[i].scr);
//		kdDebug() << "    Height is " << Dh << " pixels (" << DhMM <<
//			"mm)" << endl;
//		kdDebug() << "    Width is " << Dw << " pixels (" << DwMM <<
//			"mm)" << endl;

		vPixelsPerMM = (double)Dh / (double)DhMM;
		hPixelsPerMM = (double)Dw / (double)DwMM;
		screenInfo[i].PixelsPerMM = (vPixelsPerMM + hPixelsPerMM) / 2.0;
//		kdDebug() << "    Vertical pixels/mm  are " << vPixelsPerMM <<
//			"mm" << endl;
//		kdDebug() << "    Horizontal pixels/mm are " << hPixelsPerMM <<
//			"mm" << endl;
//		kdDebug() << "    Average pixels/mm are " <<
//			screenInfo[i].PixelsPerMM << "mm" << endl;
	}
}

/*
 * Here's where we override events to the KImgNum's to display
 * the context menu
 */
bool Kodometer::eventFilter( QObject *, QEvent *e )
{
	if ( e->type() == QEvent::MouseButtonPress ) {
		mousePressEvent((QMouseEvent*)e);
		return true;
	}
	return false;
}

/*
 * Show the context menu
 */
void Kodometer::mousePressEvent(QMouseEvent* e)
{
	//FIXME fix this!
	//dontRefresh = true;
	menu->popup(mapToGlobal(e->pos()));
}

/*
 * Called when the timer expires to query the pointer position,
 * compare it to the last known position, and then to calculate
 * the distance moved.
 */
void Kodometer::timerEvent(QTimerEvent* e)
{
	if (Enabled) {
		if(e->timerId() == distanceID) {
			lastPointerScreen = pointerScreen;
			lastXCoord = XCoord;
			lastYCoord = YCoord;
			XQueryPointer (display, root, &RootIDRet, &ChildIDRet, &XCoord,
				&YCoord, &WinX, &WinY, &StateMask);
			if (CalcDistance()) {
//				kdDebug() << "Mouse moved" << endl;
				if (!dontRefresh) {
					refresh();
					cyclesSinceLastSave++;
				}
			}
		}
	}
}

// Guess!
void Kodometer::toggleEnabled()
{
	Enabled = !Enabled;
	menu->setItemChecked(enabledID,Enabled);
	refresh();
}

// Try again!
void Kodometer::toggleAutoReset()
{
	AutoReset = !AutoReset;
	menu->setItemChecked(autoResetID,AutoReset);
	refresh();
}


// You're getting warm!
void Kodometer::toggleUnits()
{
	UseMetric =! UseMetric;

	menu->setItemChecked(metricID, UseMetric);

	QToolTip::remove(totalLabel);
	QToolTip::remove(tripLabel);
	if(!UseMetric) {
		QToolTip::add(totalLabel,
			i18n(ConversionTable[distanceUnit].fromUnitTagPlural));
		QToolTip::add(tripLabel,
			i18n(ConversionTable[tripDistanceUnit].fromUnitTagPlural));
	} else {
		QToolTip::add(totalLabel,
			i18n(ConversionTable[distanceUnit].toUnitTagPlural));
		QToolTip::add(tripLabel,
			i18n(ConversionTable[tripDistanceUnit].toUnitTagPlural));
	}
	refresh();
}


// Were you dropped on your head as a child?
void Kodometer::resetTrip()
{
	TripDistance = 0.0;
	tripDistanceUnit = inch;
	if (!dontRefresh)
		refresh();
}

// I was!
void Kodometer::resetTotal()
{
	resetTrip();

	Distance = 0.0;
	distanceUnit = inch;

	TripDistance = 0.0;
	tripDistanceUnit = inch;
	refresh();
}

/*
 * Set the values in all the KImgNums, do metric conversions,
 * and make the screen look like reality.
 */
void Kodometer::refresh(void)
{
	if(distanceUnit != lastDUnit) {
		lastDUnit = distanceUnit;
		QToolTip::remove(totalLabel);
		if(!UseMetric)
			QToolTip::add(totalLabel,
				i18n(ConversionTable[distanceUnit].fromUnitTagPlural));
		else
			QToolTip::add(totalLabel,
				i18n(ConversionTable[distanceUnit].toUnitTagPlural));
	}

	if(tripDistanceUnit != lastTUnit) {
		lastTUnit = tripDistanceUnit;
		QToolTip::remove(tripLabel);
		if(!UseMetric)
			QToolTip::add(tripLabel,
				i18n(ConversionTable[tripDistanceUnit].fromUnitTagPlural));
		else
			QToolTip::add(tripLabel,
				i18n(ConversionTable[tripDistanceUnit].toUnitTagPlural));
	}

	//now draw everything
	QString distance_s;
	QString trip_s;
        double distance_d = 0;
        double trip_d = 0;

	if (Enabled) {
            distance_d = Distance;
            distance_s = FormatDistance(distance_d, distanceUnit);
            trip_d = TripDistance;
            trip_s = FormatDistance(trip_d, tripDistanceUnit);
	} else {
            distance_s = "------";
            trip_s = "------";
	}

	totalLabel->setValue(distance_d);
	tripLabel->setValue(trip_d);
}

/*
 * Not sure what this does, its from the original program.
 */
double Kodometer::multiplier(Units unit)
{
	double m = 10;

	switch (unit) {
		case mile : m *= 10.0;
		case foot : m *= 10.0;
		case inch : m *= 10.0; break;
	}
	return m;
}

/*
 * This is the bitch function where the _real_ work is done.  I
 * could have re-invented the query_pointer code, but this one is a best.
 *
 * This is code from the original program, responsible for converting the
 * number of pixels traveled into  a real-world coordinates.
 */
int Kodometer::CalcDistance(void)
{
	double dist, sum;
	int X, Y;
	double distMM, distInches, finalNewDist;
	double oldDistance, oldTripDistance;
	double newDistance, newTripDistance;
	int i, j, finalScreen, increment;
	Units oldDistanceUnit, oldTripDistanceUnit, currentUnit;
	int distanceChanged, tripDistanceChanged;

	int screenOrientation = K_Left;

	i = j = finalScreen = increment = 0;

	if ((lastXCoord == 0) && (lastYCoord == 0))
		return false;

	if ((lastXCoord == XCoord) && (lastYCoord == YCoord))
		return false;

	//Figure out which screen the pointer is on
	if (screenCount > 1) {
		while (i < screenCount)
			if (RootIDRet == screenInfo[i].root)
				break;
			else
				i++;
	}

	pointerScreen = i;

//	kdDebug() << "CalcDistance: screen: " << pointerScreen <<
//		 " x: " << XCoord << " y: " << YCoord << endl;

	// Adjust XCoord or YCoord for the screen its on, relative to screen 0
	// and screenOrientation.

	if (lastPointerScreen != -1 && pointerScreen != lastPointerScreen) {
		switch (screenOrientation) {
			case K_Left:
			case K_Top:
				finalScreen = 0;
				j = QMAX(pointerScreen,lastPointerScreen) - 1;
				increment = -1;
				break;
			case K_Right:
			case K_Bottom:
				finalScreen = QMAX(pointerScreen,lastPointerScreen) - 1;
				j = 0;
				increment = 1;
				break;
		}
		do {
			switch (screenOrientation) {
				case K_Left:
				case K_Right:
					if (pointerScreen > lastPointerScreen)
						XCoord += screenInfo[j].width;
					else
						lastXCoord += screenInfo[j].width;
					break;
				case K_Top:
				case K_Bottom:
					if (pointerScreen > lastPointerScreen)
						YCoord += screenInfo[j].height;
					else
						lastYCoord += screenInfo[j].height;
					break;
			}
			if (j != finalScreen)
				j += increment;
		} while (j != finalScreen);
//		kdDebug() << "    Adjusted for screen ch: x: " << XCoord <<
//			" y: " << YCoord << endl;
	}

//	kdDebug() << "In: Distance: " << Distance <<
//		" Trip Distance: " << TripDistance << endl;

	// Calculate distance in pixels first
	// using Pitagora

	X = XCoord - lastXCoord;
	X = X*X;

	Y = YCoord - lastYCoord;
	Y = Y*Y;

	sum = (double)X + (double)Y;
	dist = sqrt(sum);

	// Convert to millimeters
	distMM = dist / screenInfo[pointerScreen].PixelsPerMM;

	// Convert to inches
	distInches = distMM * 0.04;

	// Add an appropriate value to Distance, which may be
	// in a unit other than inches
	currentUnit = inch;
	finalNewDist = distInches;

	while (currentUnit < distanceUnit) {
		finalNewDist =
			finalNewDist / ConversionTable[currentUnit].maxFromBeforeNext;
		currentUnit++;
//		kdDebug() << "    New dist: " << dist << "p, " << distMM << "mm, " <<
//			distInches << "in, " << finalNewDist << " " <<
//			ConversionTable[currentUnit+1].fromUnitTagPlural << endl;
	}

//	kdDebug() << " Next part" << endl;

	oldDistance = Distance * multiplier(distanceUnit);

	Distance += finalNewDist;
	oldDistanceUnit = distanceUnit;

	if (ConversionTable[distanceUnit].maxFromBeforeNext != -1.0 &&
		Distance >= ConversionTable[distanceUnit].maxFromBeforeNext)
	{
		Distance = Distance / ConversionTable[distanceUnit].maxFromBeforeNext;
		distanceUnit++;
	}

	newDistance = Distance * multiplier(distanceUnit);
	distanceChanged = (distanceUnit != oldDistanceUnit ||
		(unsigned int)oldDistance != (unsigned int)newDistance);

	// Add an appropriate value to TripDistance, which may be
	// in a unit other than inches
	currentUnit = inch;
	finalNewDist = distInches;

	while (currentUnit < tripDistanceUnit) {
		finalNewDist = finalNewDist /
			ConversionTable[currentUnit].maxFromBeforeNext;
		currentUnit++;
	}

	oldTripDistance = TripDistance * multiplier(tripDistanceUnit);
	TripDistance += finalNewDist;
	oldTripDistanceUnit = tripDistanceUnit;

	if (ConversionTable[tripDistanceUnit].maxFromBeforeNext != -1.0 &&
		TripDistance >= ConversionTable[tripDistanceUnit].maxFromBeforeNext)
	{
		TripDistance = TripDistance /
			ConversionTable[tripDistanceUnit].maxFromBeforeNext;
		tripDistanceUnit++;
	}

	newTripDistance = TripDistance * multiplier(tripDistanceUnit);
	tripDistanceChanged = ((tripDistanceUnit != oldTripDistanceUnit) ||
		((unsigned int)oldTripDistance != (unsigned int)newTripDistance));


//	kdDebug() << "Out: Distance: " << Distance <<
//		"Trip Distance: " << TripDistance << endl;

	if ((distanceChanged) || (tripDistanceChanged))
		return true;
	else
		return false;
}

/*
 * This code can probably go away.  Its doing conversions from inches to
 * other units.  Its ugly C-style stuff, that  should't be done in a
 * pretty OO world.
 */
#define THERE_IS_A_NEXT (ConversionTable[unit].maxToBeforeNext != -1.0)
QString Kodometer::FormatDistance(double &dist, Units unit)
{
    QString string;
    const char *tag;
    int precision;

    if (UseMetric) {
        dist = dist * ConversionTable[unit].conversionFactor;
        if ((THERE_IS_A_NEXT) &&
            (dist > ConversionTable[unit].maxToBeforeNext))
        {
            dist = dist / ConversionTable[unit].maxToBeforeNext;
            unit++;
        }
        if (dist == 1.0)
            tag = ConversionTable[unit].toUnitTag;
        else
            tag = ConversionTable[unit].toUnitTagPlural;
	} else {
            if (dist == 1.0)
                tag = ConversionTable[unit].fromUnitTag;
            else
                tag = ConversionTable[unit].fromUnitTagPlural;
	}
	precision = ConversionTable[unit].printPrecision;

	string.sprintf ("%.*f %s", precision, dist, tag);
	return string;
}

/*
 * Use KConfig to read all settings from disk.  Note that whatever
 * happens here overrides the defaults, but there's not much
 * sanity-checking.
 */
void Kodometer::readSettings(void)
{
	KConfig* config = KGlobal::config();
	config->setGroup("Settings");

	UseMetric = config->readNumEntry("UseMetric", false);
	AutoReset = config->readNumEntry("AutoReset", true);

	TripDistance = config->readDoubleNumEntry("Trip", 0.0);
	Distance = config->readDoubleNumEntry("Distance", 0.0);

	distanceUnit = config->readNumEntry("DistanceUnit", inch);
	tripDistanceUnit = config->readNumEntry("TripUnit", inch);
}


/*
 * Save reality for use in the next session.
 */
void Kodometer::saveSettings(void)
{
	KConfig* config = KGlobal::config();
	config->setGroup("Settings");

	config->writeEntry("UseMetric", UseMetric);
	config->writeEntry("AutoReset", AutoReset);

	config->writeEntry("Trip", TripDistance);
	config->writeEntry("Distance", Distance);

	config->writeEntry("TripUnit", tripDistanceUnit);
	config->writeEntry("DistanceUnit", distanceUnit);

	config->sync();
}

// What in the world can this do?
void Kodometer::quit()
{
	saveSettings();
	kapp->quit();
}

#include "kodometer.moc"
