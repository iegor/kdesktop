/*
 * Copyright (C) 2003 Benjamin C Meyer (ben+kbinaryclock@meyerhome.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "kbinaryclock.h"
#include "datepicker.h"

#include <kapplication.h>
#include <kconfigdialog.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kglobalsettings.h>

#include <qradiobutton.h>
#include <kcolorbutton.h>
#include <kpopupmenu.h>
#include <qslider.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qlabel.h>

#include <kprocess.h>
#include <kstandarddirs.h>
#include <qclipboard.h>
#include <kled.h>

extern "C"
{
	KDE_EXPORT KPanelApplet* init( QWidget *parent, const QString& configFile ) {
		KGlobal::locale()->insertCatalogue( "kbinaryclock");
		return new KBinaryClock( configFile, KPanelApplet::Normal,
							KPanelApplet::Preferences, parent, "kbinaryclock");
	}
}

KConfigDialogImp::KConfigDialogImp( QWidget *parent, const char *name, KConfigSkeleton *prefs, KDialogBase::DialogType dialogType, KDialogBase::ButtonCode defaultButton, bool modal) :
		KConfigDialog(parent, name, prefs, dialogType,(KDialogBase::ButtonCode) (KDialogBase::Default | KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel ), defaultButton, modal)
{
	// As a temporary mesure until the kicker applet's app name is set to the
	// applets name so KDialogBase gets the right info.
	setPlainCaption(i18n("Configure - KBinaryClock"));
	setIcon(SmallIcon("date"));

	settings = new SettingsImp(0, "General");
	addPage(settings, i18n("General"),	"package_settings");
	connect(this, SIGNAL(widgetModified()), settings, SLOT(updatePreview()));
}

SettingsImp::SettingsImp(QWidget* parent, const char* name, WFlags fl): Settings(parent, name, fl){
}

/**
 * Update the preview
 */
void SettingsImp::updatePreview(){
	int shape = Shape_Circular->isChecked() ? Prefs::EnumShape::Circular : Prefs::EnumShape::Rectangular;
	int look = KLed::Raised;
	look = Look_Flat->isChecked() ? Prefs::EnumLook::Flat : look;
	look = Look_Sunken->isChecked() ? Prefs::EnumLook::Sunken : look;
	QColor color = kcfg_Color->color();
	int darkFactor = kcfg_DarkFactor->value();
	QColor backgroundColor = kcfg_Background->color();
	frame1->setBackgroundColor(backgroundColor);

	kLed1->setBackgroundColor(backgroundColor);
	kLed2->setBackgroundColor(backgroundColor);
	kLed3->setBackgroundColor(backgroundColor);
	kLed4->setBackgroundColor(backgroundColor);
	kLed5->setBackgroundColor(backgroundColor);
	kLed6->setBackgroundColor(backgroundColor);

	kLed1->setShape((KLed::Shape)shape);
	kLed2->setShape((KLed::Shape)shape);
	kLed3->setShape((KLed::Shape)shape);
	kLed4->setShape((KLed::Shape)shape);
	kLed5->setShape((KLed::Shape)shape);
	kLed6->setShape((KLed::Shape)shape);

	kLed1->setColor(color);
	kLed2->setColor(color);
	kLed3->setColor(color);
	kLed4->setColor(color);
	kLed5->setColor(color);
	kLed6->setColor(color);

	kLed1->setLook((KLed::Look)look);
	kLed2->setLook((KLed::Look)look);
	kLed3->setLook((KLed::Look)look);
	kLed4->setLook((KLed::Look)look);
	kLed5->setLook((KLed::Look)look);
	kLed6->setLook((KLed::Look)look);

	kLed1->setDarkFactor(darkFactor);
	kLed2->setDarkFactor(darkFactor);
	kLed3->setDarkFactor(darkFactor);
	kLed4->setDarkFactor(darkFactor);
	kLed5->setDarkFactor(darkFactor);
	kLed6->setDarkFactor(darkFactor);
}

/**
 * Constructor, create LED's
 */
KBinaryClock::KBinaryClock(const QString& configFile, Type type, int actions, QWidget *parent, const char *name)
	: KPanelApplet(configFile, type, actions, parent, name), ledWidth(6),
	  _calendar(NULL), _disableCalendar(false), 
		prefs( new Prefs(sharedConfig())), m_tooltip(this)
{
	prefs->readConfig();
	setBackgroundOrigin(AncestorOrigin);
	for(int i=0; i < 4; i++){
		for(int j=0; j < ledWidth;j++){
			KLed *led = new KLed( this );
			led->setBackgroundOrigin(AncestorOrigin);
			ledMatrix[j][i] = led;
		}
	}

	// Why does kicker start out with a size of 800x409?
	// Kicker bug?
	resize(60,42);

	updateClock();
	loadSettings();
	QTimer *timer=new QTimer(this);
	connect (timer, SIGNAL (timeout()), this, SLOT (updateClock()));
	timer->start(500,false);
}

KBinaryClock::~KBinaryClock()
{
	delete prefs;
	KGlobal::locale()->removeCatalogue( "kbinaryclock");
}

/**
 * Return the computed height of the widget.
 */
int KBinaryClock::widthForHeight( int height ) const {
	return (height-2)/4*ledWidth;
}

/**
 * Return the computed width of the widget.
 */
int KBinaryClock::heightForWidth( int width ) const {
	return (width/ledWidth)*4;
}

void KBinaryClock::resizeEvent( QResizeEvent *e ) {
	int width = e->size().width();
	for (int i=0; i < ledWidth; i++)
		for (int j=0; j < 4; j++)
			ledMatrix[i][j]->setGeometry( QRect( (width/ledWidth)*i, (width/ledWidth)*j, width/ledWidth, width/ledWidth) );
}

/**
 * Load the settings for the clock.
 */
void KBinaryClock::loadSettings(){
	int shape = prefs->shape();
	int look = prefs->look();
	QColor color = prefs->color();

	int darkFactor = prefs->darkFactor();
	QColor backgroundColor = prefs->background();
	bool modifyBackground = false;
	if(backgroundColor != KApplication::palette().active().background()){
		setPaletteBackgroundColor(backgroundColor);
		modifyBackground = true;
	}

	bool showSeconds = prefs->show_Seconds();
	ledWidth = (showSeconds == true) ? 6 : 4;
	for(int i=0; i < 4; i++){
		for(int j=4; j < 6;j++){
			if(showSeconds)
				ledMatrix[j][i]->show();
			else
				ledMatrix[j][i]->hide();
		}
	}
	for(int i=0; i < 4; i++){
		for(int j=0; j < ledWidth;j++){
			ledMatrix[j][i]->setShape((KLed::Shape)shape);
			ledMatrix[j][i]->setColor(color);
			ledMatrix[j][i]->setLook((KLed::Look)look);
			ledMatrix[j][i]->setDarkFactor(darkFactor);
			// Dammed if you do, dammed if you don't
			if(modifyBackground || ledMatrix[j][i]->backgroundColor() != backgroundColor)
				ledMatrix[j][i]->setPaletteBackgroundColor(backgroundColor);
		}
	}
	updateLayout();
}

/**
 * Show Settings dialog.
 */
void KBinaryClock::preferences(){
	if(KConfigDialog::showDialog("settings"))
		return;

	KConfigDialogImp *dialog = new KConfigDialogImp(this, "settings", prefs, KDialogBase::Swallow);
	connect(dialog, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));
	dialog->show();
	dialog->settings->updatePreview();
}

/**
 * Get the time and update the LED's
 */
void KBinaryClock::updateClock(){
	QString time = "hhmmss";
	if(KGlobal::locale()->use12Clock())
		time += "ap";

	QString currentTime = (QTime::currentTime()).toString(time);
	int splice[6];
	splice[0] = currentTime.mid( 0, 1 ).toInt();
	splice[1] = currentTime.mid( 1, 1 ).toInt();
	splice[2] = currentTime.mid( 2, 1 ).toInt();
	splice[3] = currentTime.mid( 3, 1 ).toInt();
	splice[4] = currentTime.mid( 4, 1 ).toInt();
	splice[5] = currentTime.mid( 5, 1 ).toInt();

	for (int i=0; i<ledWidth; i++) {
		(splice[i] & 8) != 0 ? ledMatrix[i][0]->setState(KLed::On) : ledMatrix[i][0]->setState(KLed::Off);
		(splice[i] & 4) != 0 ? ledMatrix[i][1]->setState(KLed::On) : ledMatrix[i][1]->setState(KLed::Off);
		(splice[i] & 2) != 0 ? ledMatrix[i][2]->setState(KLed::On) : ledMatrix[i][2]->setState(KLed::Off);
		(splice[i] & 1) != 0 ? ledMatrix[i][3]->setState(KLed::On) : ledMatrix[i][3]->setState(KLed::Off);
	}

	// TODO add hide_Off_Leds checkbox to ui file post 3.3
	// sense we can't add strings.
	if(prefs->hide_Off_Leds())
	for (int i=0; i<ledWidth; i++) {
		for( int j=0; j < 4;j++){
			if(ledMatrix[i][j]->state() == KLed::Off)
				ledMatrix[i][j]->hide();
			else
				ledMatrix[i][j]->show();
		}
	}
}

/**
 * Catch the right click press
 */
 void KBinaryClock::mousePressEvent(QMouseEvent *event) {
	switch (event->button()) {
		case QMouseEvent::RightButton:
			QToolTip::remove(this);
			openContextMenu();
			break;
		case QMouseEvent::LeftButton:
			toggleCalendar();
			QToolTip::remove(this);
			break;
		case QMouseEvent::MidButton:
			QToolTip::remove(this);
			break;
		default:
			break;
	}
}

/**
 * Deal with right click's
 */
void KBinaryClock::openContextMenu() {
		bool bImmutable = config()->isImmutable();

		KPopupMenu *menu = new KPopupMenu();
		menu->insertTitle( SmallIcon( "clock" ), i18n( "KBinaryClock" ) );

		KLocale *loc = KGlobal::locale();
		QDateTime dt = QDateTime::currentDateTime();

		KPopupMenu *copyMenu = new KPopupMenu( menu );
		copyMenu->insertItem(loc->formatDateTime(dt), 201);
		copyMenu->insertItem(loc->formatDate(dt.date()), 202);
		copyMenu->insertItem(loc->formatDate(dt.date(), true), 203);
		copyMenu->insertItem(loc->formatTime(dt.time()), 204);
		copyMenu->insertItem(loc->formatTime(dt.time(), true), 205);
		copyMenu->insertItem(dt.date().toString(), 206);
		copyMenu->insertItem(dt.time().toString(), 207);
		copyMenu->insertItem(dt.toString(), 208);
		connect( copyMenu, SIGNAL( activated(int) ), this, SLOT( slotCopyMenuActivated(int) ) );

		if (!bImmutable)
		{
				if (kapp->authorize("user/root"))
				{
						menu->insertItem(SmallIcon("date"), i18n("&Adjust Date && Time..."), 103, 4);
				}
				menu->insertItem(SmallIcon("kcontrol"), i18n("Date && Time &Format..."), 104, 5);
		}

		menu->insertItem(SmallIcon("editcopy"), i18n("C&opy to Clipboard"), copyMenu, 105, 6);
		if (!bImmutable)
		{
				menu->insertSeparator(7);
				menu->insertItem(SmallIcon("configure"), i18n("&Configure KBinaryClock..."), 102, 8);
		}
		int result = menu->exec( QCursor::pos() );

		KProcess proc;
		switch (result) {
	case 102:
		preferences();
		break;
	case 103:
		proc << locate("exe", "kdesu");
		proc << "--nonewdcop";
		proc << QString("%1 clock --lang %2")
				.arg(locate("exe", "kcmshell"))
				.arg(KGlobal::locale()->language());
		proc.start(KProcess::DontCare);
		break;
	case 104:
		proc << locate("exe", "kcmshell");
		proc << "language";
					proc.start(KProcess::DontCare);
		break;
	case 110:
		preferences();
		break;
		} /* switch() */
		delete menu;
}

void KBinaryClock::slotCopyMenuActivated( int id ) {
	QPopupMenu *m = (QPopupMenu *) sender();
	QString s = m->text(id);
	QApplication::clipboard()->setText(s);
}

void KBinaryClock::toggleCalendar()
{
	if (_calendar && !_disableCalendar) {
		// calls slotCalendarDeleted which does the cleanup for us
		_calendar->close();
		return;
	}
	if (_calendar || _disableCalendar){
		return;
	}
	_calendar = new DatePicker(this, QDateTime::currentDateTime().date());
	connect( _calendar, SIGNAL( destroyed() ), SLOT( slotCalendarDeleted() ));

	// some extra spacing is included if aligned on a desktop edge
	QPoint c = mapToGlobal(QPoint(0,0));

	int w = _calendar->sizeHint().width() + 28;
															// Added 28 px. to size poperly as said in API
	int h = _calendar->sizeHint().height();

	switch (position()) {
	case KPanelApplet::pLeft:	c.setX(c.x()+width()+2);	break;
	case KPanelApplet::pRight:	c.setX(c.x()-w-2);		break;
	case KPanelApplet::pTop:	c.setY(c.y()+height()+2);	break;
	case KPanelApplet::pBottom:	c.setY(c.y()-h-2);		break;
		}

		// make calendar fully visible
		QRect deskR = KGlobalSettings::desktopGeometry(QPoint(0,0));

		if (c.y()+h > deskR.bottom())	c.setY(deskR.bottom()-h-1);
		if (c.x()+w > deskR.right())				c.setX(deskR.right()-w-1);

		_calendar->move(c);
		_calendar->show();
}

void KBinaryClock::slotCalendarDeleted()
{
		_calendar = 0L;
		// don't reopen the calendar immediately ...
		_disableCalendar = true;
		QTimer::singleShot(100, this, SLOT(slotEnableCalendar()));
}

void KBinaryClock::slotEnableCalendar()
{
		_disableCalendar = false;
}

ClockAppletToolTip::ClockAppletToolTip( KBinaryClock *clock ) : QToolTip( clock ), m_clock( clock ) {}

void ClockAppletToolTip::maybeTip( const QPoint & /*point*/ )
{
	tip(m_clock->geometry(), KGlobal::locale()->formatDate(QDateTime::currentDateTime().date(), false));
}

#include "kbinaryclock.moc"
