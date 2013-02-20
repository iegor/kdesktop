/*
 *   kmoon - a moon phase indicator
 *   Copyright (C) 1998  Stephan Kulow
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

#include <qslider.h>
#include <qlayout.h>
#include <klocale.h>
#include <qwhatsthis.h>
#include <qvbox.h>
#include <kapplication.h>
#include <qpushbutton.h>

#include "kmoondlg.h"
#include "kmoonwidget.h"

KMoonDlg::KMoonDlg(int a, bool n, bool m, QWidget *parent, const char *name)
    : KDialogBase(parent, name, true, i18n("Change View"),
                  Ok|Cancel|Help), angle(a), north(n), mask(m)
{
	QWidget *page = new QWidget( this );
	setMainWidget(page);
	QHBoxLayout *topLayout = new QHBoxLayout( page, 0, spacingHint() );

        QVBox *vbox = new QVBox(page);
        QHBox *hbox1 = new QHBox(vbox);
        hbox1->setSpacing(15);

	QLabel *label = new QLabel( i18n("View angle:"), hbox1, "caption" );
	QString text = i18n("You can use this to rotate the moon to the correct\n"
                            "angle for your location.\n"
			    "\n"
                            "This angle is (almost) impossible to\n"
                            "calculate from any system-given data,\n"
                            "therefore you can configure how you\n"
                            "want KMoon to display your moon here.\n"
                            "The default value is 0, but it is very\n"
                            "unlikely that you would see the moon\n"
                            "at this angle.");
	QWhatsThis::add(label, text);

	slider = new QSlider( -90, 90, 2, angle, Qt::Horizontal, hbox1, "slider" );
	slider->setTickmarks(QSlider::Above);
	slider->setTickInterval(45);
	slider->setEnabled(QPixmap::defaultDepth() > 8);
	label->setEnabled(QPixmap::defaultDepth() > 8);
	QWhatsThis::add(slider, text);
	connect(slider, SIGNAL(valueChanged(int)), SLOT(angleChanged(int)));

	QHBox *hbox2 = new QHBox(vbox);
	hbox2->setSpacing(spacingHint());

        hemitoggle = new QPushButton(hbox2);
	hemitoggle->setText(north ? i18n("Switch to Southern Hemisphere") :
			    i18n("Switch to Northern Hemisphere"));

        connect(hemitoggle, SIGNAL(clicked()), SLOT(toggleHemi()));

        masktoggle = new QPushButton(hbox2);
	masktoggle->setText(mask ? i18n("Switch Masking Off") :
			    i18n("Switch Masking On"));

        connect(masktoggle, SIGNAL(clicked()), SLOT(toggleMask()));
        topLayout->addWidget(vbox);

	moon = new MoonWidget(page, "preview");
	moon->setMinimumSize(50, 50);
	moon->setMaximumSize(200,200);
	QWhatsThis::add(moon, i18n("The moon as KMoon would display it\n"
							   "following your current setting and time."));
	topLayout->addWidget(moon);
        connect(this, SIGNAL(helpClicked()), SLOT(help()));
	// disableResize();
}

void KMoonDlg::angleChanged(int value) {
    angle = value;
    moon->setAngle(value);
}

void KMoonDlg::help() {
    kapp->invokeHelp(QString::fromLatin1("config"));
}

void KMoonDlg::toggleHemi() {
    moon->setNorthHemi(!moon->northHemi());
    north = moon->northHemi();
    hemitoggle->setText(north ? i18n("Switch to Southern Hemisphere") :
			i18n("Switch to Northern Hemisphere"));
}

void KMoonDlg::toggleMask() {
    moon->setMask(!moon->mask());
    mask = moon->mask();
    masktoggle->setText(mask ? i18n("Switch Masking Off") :
			i18n("Switch Masking On"));
}

#include "kmoondlg.moc"
