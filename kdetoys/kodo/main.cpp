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

/*
**  Xodometer
**  Written by Mark H. Granoff/mhg
**             Digital Equipment Corporation
**             Littleton, MA, USA
**             17 March 1993
**
**  If you make improvements or enhancements to Xodometer, please send them
**  back to the author at any of the following addresses:
**
**		granoff@keptin.lkg.dec.com
**		granoff@UltraNet.com
**		72301.1177@CompuServe.com
**
**  Thanks to my friend and colleague Bob Harris for his suggestions and help.
**
**  This software is provided as is with no warranty of any kind, expressed or
**  implied. Neither Digital Equipment Corporation nor Mark Granoff will be
**  held accountable for your use of this software.
**
**  This software is released into the public domain and may be redistributed
**  freely provided that all source module headers remain intact in their
**  entirety, and that all components of this kit are redistributed together.
**
**  Modification History
**  --------------------
**  See ChangeLog
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kodometer.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
	/* Top level details */

	KAboutData *aboutData = new KAboutData("kodo",
		I18N_NOOP("KOdometer"), "3.2",
		I18N_NOOP("KOdometer measures your desktop mileage"),
		KAboutData::License_GPL,
		"(c) 1998, Armen Nakashian",
		I18N_NOOP("A mouse odometer"), 0,
		"dev@gioelebarabucci.com");
	aboutData->addAuthor("Armen Nakashian", 0,
		"armen@tourismo.com",
		"http://yawara.anime.net/kodo/");
	aboutData->addCredit("Gioele Barabucci",
		I18N_NOOP("KDE 2 porting and some code cleanups"),
		"dev@gioelebarabucci.com", "http://www.gioelebarabucci.com");
	KCmdLineArgs::init( argc, argv, aboutData );

	if (!KUniqueApplication::start())
		exit(0);

	KUniqueApplication a;

	Kodometer *o = new Kodometer();

	// Get rid of the title bar.
	KWin::setType(o->winId(), NET::Override );
	KWin::setState(o->winId(), NET::Sticky);
	// FIXME is this a hack?
//	o->setGeometry(0, 0, 0, 0);
	a.setMainWidget(o);
	// is the following this needed?
	// a.setTopWidget(o);

	o->show();

	return a.exec();
}
