/* general.cpp
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by M. Laurent and W. Bastian.
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
#include "general.moc"
#include "EditWidget.h"
#include <Disks.h>
#include <qlayout.h>
#include <ui.h>
#include <qwhatsthis.h>

#include <stdlib.h>

General::General(liloconf *l, QWidget *parent, const char *name):QWidget(parent, name)
{
	lilo=l;
	QVBoxLayout *layout=new QVBoxLayout(this);
	layout->setMargin(SPACE_MARGIN);
	layout->setSpacing(SPACE_INSIDE);
	QHBox *drv=new QHBox(this);
	QLabel *drive_lbl=new QLabel(_("Install &boot record to drive/partition:"), drv);
	drive=new QComboBox(false, drv);
	drive_lbl->setBuddy(drive);
	StringList p=ptable::disklist();
	p+=ptable::partlist();
	p.sort();
	for(StringList::const_iterator it=p.begin(); it!=p.end(); it++)
		drive->insertItem((*it).cstr());
	connect(drive, SIGNAL(activated(int)), SIGNAL(configChanged()));
	layout->addWidget(drv);
	QWhatsThis::add(drv, _("Select the drive or partition you want to install the LILO boot loader to here. Unless you intend to use other boot managers in addition to LILO, this should be the MBR (master boot record) of your boot drive.<br>In this case, you should probably select <i>/dev/hda</i> if your boot drive is an IDE drive or <i>/dev/sda</i> if your boot drive is SCSI."));

	QHBox *to=new QHBox(this);
	QLabel *to_lbl=new QLabel(_("Boot the default kernel/OS &after:"), to);
	timeout=new QSpinBox(0, 1000000, 1, to);
	timeout->setSuffix(_("/10 seconds"));
	connect(timeout, SIGNAL(valueChanged(int)), SIGNAL(configChanged()));
	to_lbl->setBuddy(timeout);
	layout->addWidget(to);
	QWhatsThis::add(to, _("LILO will wait the amount of time specified here before booting the kernel (or OS) marked as <i>default</i> in the <b>Images</b> tab."));

	QHBox *modes=new QHBox(this);
	linear=new QCheckBox(_("Use &linear mode"), modes);
	connect(linear, SIGNAL(clicked()), SIGNAL(configChanged()));
	QWhatsThis::add(linear, _("Check this box if you want to use the linear mode.<br>Linear mode tells the boot loader the location of kernels in linear addressing rather than sector/head/cylinder.<br>linear mode is required for some SCSI drives, and shouldn't hurt unless you're planning to create a boot disk to be used with a different computer.<br>See the lilo.conf man page for details."));
	compact=new QCheckBox(_("Use &compact mode"), modes);
	connect(compact, SIGNAL(clicked()), SIGNAL(configChanged()));
	QWhatsThis::add(compact, _("Check this box if you want to use the compact mode.<br>The compact mode tries to merge read requests for adjacent sectors into a single read request. This reduces load time and keeps the boot map smaller, but will not work on all systems."));
	layout->addWidget(modes);

	QHBox *opts=new QHBox(this);
	lock=new QCheckBox(_("&Record boot command lines for defaults"), opts);
	connect(lock, SIGNAL(clicked()), SIGNAL(configChanged()));
	QWhatsThis::add(lock, "<qt>"+_("Checking this box enables automatic recording of boot command lines as the defaults for the following boots. This way, lilo \"locks\" on a choice until it is manually overridden.\nThis sets the <b>lock</b> option in lilo.conf."));
	restricted=new QCheckBox(_("R&estrict parameters"), opts);
	connect(restricted, SIGNAL(clicked()), SIGNAL(configChanged()));
	connect(restricted, SIGNAL(clicked()), SLOT(check_pw()));
	QWhatsThis::add(restricted, _("If this box is checked, a password (entered below) is required only if any parameters are changed (i.e. the user can boot <i>linux</i>, but not <i>linux single</i> or <i>linux init=/bin/sh</i>).\nThis sets the <b>restricted</b> option in lilo.conf.<br>This sets a default for all Linux kernels you want to boot. If you need a per-kernel setting, go to the <i>Operating systems</i> tab and select <i>Details</i>."));
	layout->addWidget(opts);

	QHBox *pw=new QHBox(this);
	use_password=new QCheckBox(_("Require &password:"), pw);
	connect(use_password, SIGNAL(clicked()), SIGNAL(configChanged()));
	connect(use_password, SIGNAL(clicked()), SLOT(check_pw()));
	password=new QLineEdit(pw);
	password->setMaxLength(15);
	password->setEchoMode(QLineEdit::Password);
	connect(password, SIGNAL(textChanged(const QString &)), SIGNAL(configChanged()));
	QWhatsThis::add(pw, _("Enter the password required for bootup (if any) here. If <i>restricted</i> above is checked, the password is required for additional parameters only.<br><b>WARNING:</b> The password is stored in clear text in /etc/lilo.conf. You'll want to make sure nobody untrusted can read this file. Also, you probably don't want to use your normal/root password here.<br>This sets a default for all Linux kernels you want to boot. If you need a per-kernel setting, go to the <i>Operating systems</i> tab and select <i>Details</i>."));
	layout->addWidget(pw);

	QHBox *vgab=new QHBox(this);
	QLabel *vlbl=new QLabel(_("&Default graphics mode on text console:"), vgab);
	vga=new QComboBox(false, vgab);
	vlbl->setBuddy(vga);
	QWhatsThis::add(vgab, _("You can select the default graphics mode here.<br>If you intend to use a VGA graphics mode, you must compile the kernel with support for framebuffer devices. The <i>ask</i> setting brings up a prompt at boot time.<br>This sets a default for all Linux kernels you want to boot. If you need a per-kernel setting, go to the <i>Operating systems</i> tab and select <i>Details</i>."));
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
        connect( vga, SIGNAL(activated ( int )), SIGNAL(configChanged()));
	layout->addWidget(vgab);

	prompt=new QCheckBox(_("Enter LILO &prompt automatically"), this);
	QWhatsThis::add(prompt, _("If this box is checked, LILO goes to the LILO prompt whether or not a key is pressed. If it is turned off, LILO boots the default operating system unless shift is pressed (in that case, it goes to the LILO prompt).<br>This sets the <i>prompt</i> option in lilo.conf."));
        connect(prompt, SIGNAL(clicked()), SIGNAL(configChanged()));

	layout->addWidget(prompt);

	update();
}
void General::saveChanges()
{
	QString to;
	to.sprintf("%u", timeout->value());
        QString boot = drive->currentText();
	lilo->defaults.set("boot", boot.isEmpty() ? "" : boot.latin1());
	lilo->defaults.set("timeout", to.latin1());
	if(compact->isChecked() && lilo->defaults.grep("^[ \t]*compact[ \t]*$").empty())
		lilo->defaults += "compact";
	else if(!compact->isChecked() && !lilo->defaults.grep("^[ \t]*compact[ \t]*$").empty())
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*compact[ \t]*$"));
	if(linear->isChecked() && lilo->defaults.grep("^[ \t]*linear[ \t]*$").empty())
		lilo->defaults += "linear";
	else if(!linear->isChecked() && !lilo->defaults.grep("^[ \t]*linear[ \t]*$").empty())
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*linear[ \t]*$"));
	if(lock->isChecked() && lilo->defaults.grep("^[ \t]*lock[ \t]*$").empty())
		lilo->defaults += "lock";
	else if(!lock->isChecked() && !lilo->defaults.grep("^[ \t]*lock[ \t]*$").empty())
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*lock[ \t]*$"));
	if(restricted->isChecked() && lilo->defaults.grep("^[ \t]*restricted[ \t]*$").empty())
		lilo->defaults += "restricted";
	else if(!restricted->isChecked() && !lilo->defaults.grep("^[ \t]*restricted[ \t]*$").empty())
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*restricted[ \t]*$"));
	if(restricted->isChecked() || use_password->isChecked())
		lilo->defaults.set("password", password->text().latin1());
	else
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*password[ \t]*=.*"));
	if(vga->currentText()=="default") {
		if(!lilo->defaults.grep("[ \t]*vga[ \t]*=").empty())
			lilo->defaults.remove(lilo->defaults.grep("[ \t]*vga[ \t]*="));
	} else {
		QString mode=vga->currentText();
		if(mode!="ask") {
			mode=mode.mid(mode.find('(')+1);
			mode=mode.left(mode.length()-1);
		}
		lilo->defaults.set("vga", mode.latin1());
	}
	if(prompt->isChecked() && lilo->defaults.grep("^[ \t]*prompt[ \t]*$").empty())
		lilo->defaults += "prompt";
	else if(!prompt->isChecked() && !lilo->defaults.grep("^[ \t]*prompt[ \t]*$").empty())
		lilo->defaults.remove(lilo->defaults.grep("^[ \t]*prompt[ \t]*$"));
}
void General::update()
{
	QString boot=lilo->defaults.get("boot").cstr();
	for(int i=0; i<drive->count(); i++)
		if(drive->text(i)==boot)
			drive->setCurrentItem(i);
	timeout->setValue(atoi(lilo->defaults.get("timeout")));
	compact->setChecked(!lilo->defaults.grep("^[ \t]*compact[ \t]*$").empty());
	linear->setChecked(!lilo->defaults.grep("^[ \t]*linear[ \t]*$").empty());
	lock->setChecked(!lilo->defaults.grep("^[ \t]*lock[ \t]*$").empty());
	restricted->setChecked(!lilo->defaults.grep("^[ \t]*restricted[ \t]*$").empty());
	String pw=lilo->defaults.grep("^[ \t]*password[ \t]*=.*");
	use_password->setChecked(!pw.empty());
	if(!pw.empty())
		password->setText(lilo->defaults.get("password").cstr());
	check_pw();
	QString mode=lilo->defaults.get("vga", "").cstr();
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
	prompt->setChecked(!lilo->defaults.grep("^[ \t]*prompt[ \t]*$").empty());
}
void General::check_pw()
{
	password->setEnabled(restricted->isChecked() || use_password->isChecked());
}

void General::makeReadOnly()
{
    drive->setEnabled( false );
    timeout->setEnabled( false );
    linear->setEnabled( false );
    compact->setEnabled( false );
    lock->setEnabled( false );
    restricted->setEnabled( false );
    use_password->setEnabled( false );
    password->setEnabled( false );
    vga->setEnabled( false );
    prompt->setEnabled( false );

}
