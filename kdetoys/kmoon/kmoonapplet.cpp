/*
 *   kmoon - a moon phase indicator
 *   $Id: kmoonapplet.cpp 465369 2005-09-29 14:33:08Z mueller $
 *   Copyright (C) 1998,2000  Stephan Kulow
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <qbitmap.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qlayout.h>

#include <dcopclient.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kwin.h>
#include <kstartupinfo.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcmdlineargs.h>
#include <kiconloader.h>
#include <kiconeffect.h>
#include <kconfig.h>
#include <kdialogbase.h>

#include "version.h"
#include "kmoondlg.h"
#include "kmoonwidget.h"
#include "kmoonapplet.h"

const char *description = I18N_NOOP("Moon Phase Indicator for KDE");

extern "C"
{
  KDE_EXPORT KPanelApplet *init(QWidget *parent, const QString& configFile)
  {
    KGlobal::locale()->insertCatalogue("kmoon");
    return new MoonPAWidget(configFile, KPanelApplet::Normal,
			    KPanelApplet::About|KPanelApplet::Preferences,
			    parent, "kmoonapplet");
  }
}

MoonPAWidget::MoonPAWidget(const QString& configFile, Type type, int actions,
			   QWidget *parent, const char *name)
  : KPanelApplet(configFile, type, actions, parent, name)
{
    KConfig *config = KGlobal::config();
    config->setGroup("General");

    QVBoxLayout *vbox = new QVBoxLayout(this, 0,0);
    moon = new MoonWidget(this);
    moon->setAngle(config->readNumEntry("Rotation", 0));
    moon->setNorthHemi(config->readBoolEntry("Northern", true));
    moon->setMask(config->readBoolEntry("Mask", false));
    vbox->addWidget(moon);
    startTimer(1000 * 60 * 20);

    popup = new QPopupMenu();
    popup->insertItem(SmallIcon("kmoon"),
		      i18n("&About"), this,
		      SLOT(showAbout()));
    popup->insertItem(SmallIcon("configure"), i18n("&Configure..."), this,
		      SLOT(settings()));

    // missuse timerEvent for initialising
    timerEvent(0);
}

MoonPAWidget::~MoonPAWidget()
{
  delete moon;
  delete popup;
}

void MoonPAWidget::showAbout()
{
    KDialogBase *dialog = new KDialogBase(
                                i18n("About Moon Phase Indicator"),
                                KDialogBase::Yes,
                                KDialogBase::Yes, KDialogBase::Yes,
                                0, "about", true, true,
                                KStdGuiItem::ok() );

    QPixmap ret = DesktopIcon("kmoon");
    QString text = i18n(description) + QString::fromLatin1("\n\n") +
		   i18n("Written by Stephan Kulow <coolo@kde.org>\n"
                            "\n"
                            "Made an applet by M G Berberich "
                            "<berberic@fmi.uni-passau.de>\n"
                            "\n"
                            "Lunar code by Chris Osburn "
                            "<chris@speakeasy.org>\n"
                            "\n"
                            "Moon graphics by Tim Beauchamp "
                            "<timb@googol.com>");

    dialog->setIcon(ret);

    KMessageBox::createKMessageBox(dialog, ret, text, QStringList(), QString::null, 0, KMessageBox::Notify);
}

void MoonPAWidget::settings()
{
	KMoonDlg dlg(moon->angle(), moon->northHemi(), moon->mask(),
		     this, "moondlg");
	if (dlg.exec() == KMoonDlg::Accepted) {
		moon->setAngle(dlg.getAngle());
                moon->setNorthHemi(dlg.getNorthHemi());
		moon->setMask(dlg.getMask());
		KConfig *config = KGlobal::config();
		config->setGroup("General");
		config->writeEntry("Rotation", moon->angle());
                config->writeEntry("Northern", moon->northHemi());
                config->writeEntry("Mask", moon->mask());
                config->sync();
	}
	repaint();
}

void MoonPAWidget::timerEvent( QTimerEvent * )
{
    time_t clock;
    time(&clock);
    struct tm *t = localtime(&clock);
    moon->calcStatus(mktime(t));
    moon->repaint();
}

void MoonPAWidget::mousePressEvent( QMouseEvent *e)
{
    if (!popup)
      return;

    if (e->button() == RightButton) {
      popup->popup(mapToGlobal(e->pos()));
      popup->exec();
    }
    if (e->button() == LeftButton) {
      showAbout();
    }
}

#include "kmoonapplet.moc"
