/* Details.cpp
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include "Details.moc"

#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <ui.h>

Details::Details(liloimage *lilo, QWidget *parent, const char *name, WFlags /* f */)
 : KDialogBase(parent, name, true, QString::null, Ok|Cancel, Ok, true)
{
	l=lilo;

        QVBox *page = makeVBoxMainWidget();

	QHBox *vgab=new QHBox(page);
	QLabel *vlbl=new QLabel(_("&Graphics mode on text console:"), vgab);
	vga=new QComboBox(false, vgab);
	vlbl->setBuddy(vga);
	QWhatsThis::add(vgab, _("You can select the graphics mode for this kernel here.<br>If you intend to use a VGA graphics mode, you must compile the kernel with support for framebuffer devices. The <i>ask</i> setting brings up a prompt at boot time."));
	vga->insertItem(_("default"));
	vga->insertItem(_("ask"));
	vga->insertItem(_("text 80x25 (0)"));
	vga->insertItem(_("text 80x50 (1)"));
	vga->insertItem(_("text 80x43 (2)"));
	vga->insertItem(_("text 80x28 (3)"));
	vga->insertItem(_("text 80x30 (4)"));
	vga->insertItem(_("text 80x34 (5)"));
	vga->insertItem(_("text 80x60 (6)"));
	vga->insertItem(_("text 40x25 (7)"));
	vga->insertItem(_("VGA 640x480, 256 colors (769)"));
	vga->insertItem(_("VGA 640x480, 32767 colors (784)"));
	vga->insertItem(_("VGA 640x480, 65536 colors (785)"));
	vga->insertItem(_("VGA 640x480, 16.7M colors (786)"));
	vga->insertItem(_("VGA 800x600, 256 colors (771)"));
	vga->insertItem(_("VGA 800x600, 32767 colors (787)"));
	vga->insertItem(_("VGA 800x600, 65536 colors (788)"));
	vga->insertItem(_("VGA 800x600, 16.7M colors (789)"));
	vga->insertItem(_("VGA 1024x768, 256 colors (773)"));
	vga->insertItem(_("VGA 1024x768, 32767 colors (790)"));
	vga->insertItem(_("VGA 1024x768, 65536 colors (791)"));
	vga->insertItem(_("VGA 1024x768, 16.7M colors (792)"));
	vga->insertItem(_("VGA 1280x1024, 256 colors (775)"));
	vga->insertItem(_("VGA 1280x1024, 32767 colors (793)"));
	vga->insertItem(_("VGA 1280x1024, 65536 colors (794)"));
	vga->insertItem(_("VGA 1280x1024, 16.7M colors (795)"));
	
	readonly=new QCheckBox(_("Mount root filesystem &read-only"), page);
	QWhatsThis::add(readonly, _("Mount the root filesystem for this kernel read-only. Since the init scripts normally take care of remounting the root filesystem in read-write mode after running some checks, this should always be turned on.<br>Don't turn this off unless you know what you're doing."));

	unsafe=new QCheckBox(_("Do not check &partition table"), page);
	QWhatsThis::add(unsafe, _("This turns off some sanity checks while writing the configuration. This should not be used under \"normal\" circumstances, but it can be useful, for example, by providing the capability of booting from a floppy disk, without having a floppy in the drive every time you run lilo.<br>This sets the <i>unsafe</i> keyword in lilo.conf."));

	QHBox *opts=new QHBox(page);
	lock=new QCheckBox(_("&Record boot command lines for defaults"), opts);
	QWhatsThis::add(lock, "<qt>" + _("Checking this box enables automatic recording of boot command lines as the default for the following bootups. This way, lilo \"locks\" on a choice until it is manually overridden.<br>This sets the <b>lock</b> option in lilo.conf"));
	restricted=new QCheckBox(_("R&estrict parameters"), opts);
	connect(restricted, SIGNAL(clicked()), SLOT(check_pw()));
	QWhatsThis::add(restricted, _("If this box is checked, a password (entered below) is required only if any parameters are changed (i.e. the user can boot <i>linux</i>, but not <i>linux single</i> or <i>linux init=/bin/sh</i>).\nThis sets the <b>restricted</b> option in lilo.conf."));

	QHBox *pw=new QHBox(page);
	use_password=new QCheckBox(_("Require &password:"), pw);
	connect(use_password, SIGNAL(clicked()), SLOT(check_pw()));
	password=new QLineEdit(pw);
	password->setMaxLength(15);
	password->setEchoMode(QLineEdit::Password);
	QWhatsThis::add(pw, _("Enter the password required for bootup (if any) here. If <i>restricted</i> above is checked, the password is required for additional parameters only.<br><b>WARNING:</b> The password is stored in clear text in /etc/lilo.conf. You'll want to make sure nobody untrusted can read this file. Also, you probably don't want to use your normal/root password here."));

	if(l) {
		QString mode=l->get("vga", "").cstr();
		if(mode.isEmpty())
			vga->setCurrentItem(0);
		else if(mode=="ask")
			vga->setCurrentItem(1);
		else
			for(int i=0; i<vga->count(); i++) {
				if(vga->text(i).contains("(" + mode + ")")) {
					vga->setCurrentItem(i);
					break;
				}
			}
		readonly->setChecked(!l->grep("[ \t]*read-only[ \t]*").empty());
		unsafe->setChecked(!l->grep("[ \t]*unsafe[ \t]*").empty());
		lock->setChecked(!l->grep("[ \t]*lock[ \t]*").empty());
		restricted->setChecked(!l->grep("[ \t]*restricted[ \t]*").empty());
		password->setText(l->get("password").cstr());
	}
	
	check_pw();
}

void Details::check_pw()
{
	password->setEnabled(restricted->isChecked() || use_password->isChecked());
}

QString Details::vgaMode() const
{
	QString s=vga->currentText();
	if(s=="default")
		return "";
	else if(s!="ask") {
		s=s.mid(s.find('(')+1);
		s=s.left(s.length()-1);
	}
	return s;
}
