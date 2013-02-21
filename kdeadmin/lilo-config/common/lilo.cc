/* lilo.cc
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by A. Seigo and W. Bastian.
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
#include "lilo.h"
#include "Disks.h"
#include "Files.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <string.h>

using namespace std;

bool liloimage::isLinux() const {
	const_iterator it=begin();
	if((*it).contains("image"))
		return true;
	else
		return false;
}

liloimage *liloimages::find(String const &s) const
{
	String t = String::escapeForRegExp(s);

	String regex="^[ \t]*label[ \t]*=[ \t]*\"?" + t + "\"?[ \t]*";
	for(const_iterator it=begin(); it!=end(); it++) {
		if(!(*it).grep(regex).empty())
			return (liloimage*)&(*it);
	}
	return 0;
}
void liloimages::remove(String const &s)
{
	liloimage *i=find(s);
	for(iterator it=begin(); it!=end(); it++)
		if(*it==*i) {
			erase(it);
			break;
		}
}

liloconf::liloconf(String const &filename)
{
	checked=false;
	defaults.clear();
	images.clear();
	if(filename.empty()) {
		probe();
	} else {
		StringList s;
		if(s.readfile(filename))
			set(s);
		else
			probe();
	}
}
void liloconf::set(StringList const &s)
{
	defaults.clear();
	images.clear();
	checked=false;
	bool inHeader=true;
	bool start;
	liloimage *image=0;
	for(StringList::const_iterator it=s.begin(); it!=s.end(); it++) {
		String s=*it;
		start=false;
		s=s.simplifyWhiteSpace();
		if(s.empty())
			continue;
		if(s.left(5)==(String)"other" && (s.mid(6, 1)==' ' || s.mid(6, 1)=='=')) {
			inHeader=false;
			start=true;
		}
		if(s.left(5)==(String)"image" && (s.mid(6, 1)==' ' || s.mid(6, 1)=='=')) {
			inHeader=false;
			start=true;
		}
		if(inHeader) {
			defaults+=*it;
		} else if(start) {
			if(image)
				images.insert(images.end(), *image);
			image=new liloimage;
			*image += *it;
		} else {
			*image += *it;
		}
	}
	if(image)
		images.insert(images.end(), *image);
}
void liloconf::set(String const &s)
{
	set((StringList)s);
}
void liloconf::writeFile(String const &filename)
{
	ofstream f;
	f.open(filename, ios::out);
	f << *this << endl;
	f.close();
	chmod(filename, 0600);
}
bool liloconf::install(bool probeonly)
{
	char *lilotmp=strdup("/tmp/liloXXXXXX");
	String command;
	int fd=mkstemp(lilotmp);
	// Unfortunately, gcc 3.1 and higher don't have ofstream::attach
	// anymore. Pity, used to be immensely useful.
	// f.attach(fd);
	close(fd);
	ofstream f(lilotmp, ios::out);
	f << defaults << endl;
	for(liloimages::iterator it=images.begin(); it!=images.end(); it++) {
		f << *it << endl;
	}
	f.close();

	if(probeonly)
		command.sprintf("/sbin/lilo -v -t -C %s 2>&1", (char const * const)lilotmp);
	else
		command.sprintf("/sbin/lilo -v -C %s 2>&1", (char const * const)lilotmp);
	output="";
	FILE *lilo=popen(command, "r");
	char *buf=(char *) malloc(1024);
	while(fgets(buf, 1024, lilo))
		output += buf;
	free(buf);
	ok=(pclose(lilo)==0);
	unlink(lilotmp);
	free(lilotmp);
	checked=true;
	return ok;
}
bool const liloconf::isOk()
{
	if(!checked)
		check();
	return ok;
}
String const liloconf::liloOut()
{
	if(!checked)
		check();
	return output;
}
bool liloconf::probe()
{
	ptable p;
	StringList drives=p.disklist();
	String const root=p.device("/", true);
	checked=false;
	defaults.clear();
	images.clear();
	/* Set some reasonable defaults... */
	// Try to figure out the boot device first...
	if(drives.contains("/dev/hda")) {	
		defaults += "boot=/dev/hda"; // 1st IDE/ATAPI harddisk
		defaults += "lba32";	// otherwise it is assumed
	}
	else if(drives.contains("/dev/sda")) {
		defaults += "boot=/dev/sda"; // 1st SCSI harddisk
		defaults += "linear"; // some SCSI disks need this
	} else if(drives.contains("/dev/i2o/hda"))
		defaults += "boot=/dev/i2o/hda"; // 1st I2O harddisk
	else if(drives.contains("/dev/eda"))
		defaults += "boot=/dev/eda"; // 1st MCA ESDI harddisk
	else if(drives.contains("/dev/pda"))
		defaults += "boot=/dev/pda"; // 1st Parallel port IDE disk
	else
		defaults += "boot=Insert_your_boot_device_here"; // shouldn't happen
	defaults += "prompt";
	defaults += "timeout=50";
        if(!access("/boot/message", F_OK))
		defaults += "message=/boot/message";
	defaults += "root=" + root;

	/* Scan for available operating systems...
	 * The list of what to do for each partition type is based on my
	 * best guess. I don't have anything but Linux (Red Hat Linux 7.0),
	 * FreeBSD (5.0-CURRENT), OpenBSD (2.6), FreeDOS (some CVS snapshot)
	 * and DR-DOS (7.03), so anything else might be wrong.
	 * If you have any additions or corrections, please send them to
	 * bero@redhat.com.
	 */
	// Scan for Linux kernels in the currently running system
	// The following may or may not be specific to Red Hat Linux and
	// similar distributions... If you're using a distribution that does
	// things differently, tell me how it should be done there.
	StringList files=Files::glob("/boot/*", Files::File);
	for(StringList::iterator it=files.begin(); it!=files.end(); it++) {
		struct stat s;
		if(lstat(*it, &s)) // If we can't stat it, it can't be a kernel
			continue;
		if(s.st_size<131072) // if you managed to compile a kernel at less than 128k, you're cheating. ;)
			continue;
		if((*it).contains("System.map") || (*it).contains("initrd")) // definitely not a kernel
			continue;
		if((*it).contains("vmlinux")) {
			// If the kernel exists both in compressed and in
			// uncompressed form, ignore the uncompressed one.
			String compr=(*it).replace("vmlinux", "vmlinuz");
			if(!access(compr, F_OK))
				continue;
		}
		String version=(*it).regex("(test-?|pre-?)?([0-9]\\.[0-9]\\.[0-9]+)(-?[0-9A-Za-z]+)*");
		String label=version;
		if(version.empty()) // not a recognized kernel
			version="linux";
		
		if (label.empty())
		{
			label = (*it);
			if (label.find('/') != string::npos)
				label = label.right(label.length()-1-label.rfind('/'));
		}	
		// Check if we have an initial ramdisk (initrd) for this kernel...
		String initrd1; // first guess
		String initrd2; // second guess
		if((*it).contains("vmlinuz")) {
			initrd1=(*it).replace("vmlinux", "initrd")+".img";
			initrd2=(*it).replace("vmlinuz", "initrd.img");
		}	
		else if((*it).contains("vmlinux")) {
			initrd1=(*it).replace("vmlinux", "initrd")+".img";
			initrd2=(*it).replace("vmlinuz", "initrd.img");
		}
		else if((*it).contains("kernel")) {
			initrd1=(*it).replace("kernel", "initrd")+".img";
			initrd2=(*it).replace("vmlinuz", "initrd.img");
		}
		else if((*it).contains("linux")) {
			initrd1=(*it).replace("linux", "initrd")+".img";
			initrd2=(*it).replace("vmlinuz", "initrd.img");
		}
		else {
			initrd1="/boot/initrd-"+version+".img";
			initrd2="/boot/initrd.img-"+version;
		}
		
		String initrd = "";
		if(!access(initrd1, F_OK))
		    initrd = initrd1;
		else if(!access(initrd2, F_OK))
		    initrd = initrd2;    

		if(label.size()>15) // LILO can't handle this
			if(label.contains("enterprise"))
				label=label.replace("enterprise", "E");
		if(label.size()>15)
			label=label.left(15);

		// label, kernel, root, initrd, optional, append, vga, readonly,
		// literal, ramdisk
		addLinux(label, *it, root, initrd);
	}
	addLinux("Linux_Compiled", "/usr/src/linux/arch/i386/boot/bzImage", root, "", true); // User-compiled kernel that wasn't moved...

	// Scan for other OSes... and Linux kernels on other partitions.
	for(StringList::iterator it=p.partition.begin(); it!=p.partition.end(); it++) {
		switch(p.id[*it]) {
		case 0x01: // FAT12... Might be some really really old DOS.
		case 0x04: // FAT16 < 32 M... Probably another old DOS.
		case 0x06: // FAT16
		case 0x0b: // FAT32
		case 0x0c: // FAT32 (LBA)
		case 0x0e: // FAT16 (LBA)
		case 0x14: // Hidden FAT16 < 32 M...
		case 0x16: // Hidden FAT16
		case 0x1b: // Hidden FAT32
		case 0x1c: // Hidden FAT32 (LBA)
		case 0x1e: // Hidden FAT16 (LBA)
		case 0x24: // NEC DOS
		case 0x55: // EZ-Drive... I think this was some DOS tool
			   // to see "large" disks ages ago, so it may be
			   // a DOS partition... Not sure about this one.
		case 0xc1: // DRDOS/sec
		case 0xc4: // DRDOS/sec
		case 0xc6: // DRDOS/sec
		{
			// Try to determine which type of DOS we're using
			String mp=p.mountpt[*it];
			String lbl="DOS";
			if(mp.empty()) {
				char *tmp=tmpnam(NULL);
				tmp=tmpnam(NULL);
				mkdir(tmp, 0700);
				if(!mount(*it, tmp, "msdos", MS_MGC_VAL|MS_RDONLY|MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL))
					mp=tmp;
				else if(!mount(*it, mp, "vfat", MS_MGC_VAL|MS_RDONLY|MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL))
					mp=tmp;
			}
			if(!mp.empty()) {
				struct stat s;
				if(stat(mp+"/ibmbio.com", &s) && stat(mp+"/io.sys", &s) && stat(mp+"/ntldr", &s)) // Doesn't look like a bootable DOS partition, ignore it
					break;
				if(!stat(mp+"/drdos.386", &s))
					lbl="DR-DOS";
				else if(!stat(mp+"/ntldr", &s))
					lbl="NT";
				else if(!stat(mp+"/msdos.sys", &s)) {
					if(s.st_size < 4096)
						/* msdos.sys is actual code in DOS, it's a config file in SuckOS */
						if(!stat(mp+"/windows/system/sfc.exe", &s)) /* This is (supposed to be?) a component of Suck98 but not Suck95 */
							lbl="Windows98";
						else
							lbl="Windows95";
				}
				if(p.mountpt[*it].empty()) {
					umount(mp);
					sync();
					rmdir(mp);
				}
			}
			addOther(lbl, *it);
			break;
		}
		case 0x02: // Xenix root... Does Xenix actually boot this way?
			addOther("Xenix", *it);
			break;
		case 0x07: // HPFS or NTFS... Is there any way to check which?
			   // (without trying to mount them... many kernels
			   // support neither)
		case 0x17: // Hidden HPFS or NTFS
			addOther("NT", *it);
			break;
		case 0x09: // AIX
			addOther("AIX", *it);
			break;
		case 0x83: // My favorite :)
		case 0xfd: // Linux RAID
			// TODO: scan other FSes for kernels...
			break;
		case 0x84:
		{
			// CLASH: SystemSoft MobilePRO BIOS and
			// various Dell BIOSes use the same
			// ID (0x84) for its hibernation partitions
			// as OS/2 does for hidden C: drives.
			// Fortunately, hibernation partitions are easy to
			// recognize...
			int fd=open(*it, O_RDONLY);
			char *header=(char *) malloc(20);
			read(fd, header, 20);
			close(fd);
			if(strncmp(header, "SystemSoft", 10)==0) // It's a hibernation partition
				break;
			else if(strncmp(header+3, "Dell Suspend", 12)==0) { // It's a Dell hibernation partition
				// Dell BIOSes are ultimately buggy: They don't do resume-from-disk
				// properly.
				// Hibernation partitions are bootable and need to be loaded by the
				// boot manager instead.
				addOther("SuspendToDisk", *it);
				break;
			}
			addOther("OS2", *it, false, "/boot/os2_d.b");
		}
		case 0x0a: // OS/2 Boot Manager
			addOther("OS2", *it, false, "/boot/os2_d.b");
			break;
		case 0x10: // OPUS... (what is that?)
			addOther("OPUS", *it);
			break;
		case 0x3c: // Partition Magic
			addOther("PartitionMagic", *it);
			break;
		case 0x40: // Venix 80286
			addOther("Venix", *it);
			break;
		case 0x4d: // QNX
			addOther("QNX", *it);
			break;
		case 0x52: // CP/M (does anyone dual-boot between CP/M and
			   // Linux? Would be interesting to see. ;) )
		case 0xdb: // CP/M/CTOS
			addOther("CPM", *it);
			break;
		case 0x63: // GNU/Hurd
			addOther("GNU_Hurd", *it);
			break;
		case 0x64: // Novell Netware
		case 0x65: // Novell Netware
			addOther("Netware", *it);
			break;
		case 0x75: // PC/IX (what is that?)
			addOther("PCIX", *it);
			break;
		case 0x80: // Old Minix
		case 0x81: // Minix and some VERY old Linux kernels
			addOther("Minix", *it);
			break;
		case 0x9f: // BSD/OS
		case 0xb7: // BSDI
			addOther("BSD_OS", *it);
			break;
		case 0xa5: // Some BSDs... Is there any way to determine which
			   // one?
			addOther("BSD", *it);
			break;
		case 0xa6: // OpenBSD
			addOther("OpenBSD", *it);
			break;
		case 0xa7: // NeXTSTEP
			addOther("NeXT", *it);
			break;
		case 0xc7: // Syrinx (what is that?)
			addOther("Syrinx", *it);
			break;
		case 0xeb: // BeOS
			addOther("BeOS", *it);
			break;
		case 0xfe: // LANstep (what is that?)
			addOther("LANstep", *it);
			break;
		case 0xff: // BBT (what is that?)
			addOther("BBT", *it);
			break;
		}
	}
	//	addOther("floppy", "/dev/fd0", true);
	//Would be nice, but LILO can't handle an optional
	//other=nonexistantdevice entry ATM.
	return true;
}
void liloconf::addLinux(String const &label, String const &kernel, String const &root, String const &initrd, bool optional, String const &append, String const &vga, bool readonly, String const &literal, String const &ramdisk)
{
	liloimage *lx_image=new liloimage;
	*lx_image += "image="+kernel;
	*lx_image += "\tlabel=\""+label+"\"";
	if(!root.empty())
		*lx_image += "\troot="+root;
	if(readonly)
		*lx_image += "\tread-only";
	else
		*lx_image += "\tread-write";
	if(!initrd.empty())
		*lx_image += "\tinitrd=\""+initrd+"\"";
	if(!append.empty())
		*lx_image += "\tappend=\""+append+"\"";
	if(!vga.empty())
		*lx_image += "\tvga=\""+vga+"\"";
	if(!literal.empty())
		*lx_image += "\tliteral=\""+literal+"\"";
	if(!ramdisk.empty())
		*lx_image += "\tramdisk=\""+ramdisk+"\"";
	if(optional)
		*lx_image += "\toptional";
	images.insert(images.end(), *lx_image);
}
void liloconf::addOther(String const &name, String const &partition, bool const &optional, String const &chain)
{
	liloimage *other=new liloimage;
	*other += "other="+partition;
	*other += "\tlabel=\""+name+"\"";
	if(optional)
		*other += "\toptional";
	if(!chain.empty())
		*other += "\tloader="+chain+"\"";
	images.insert(images.end(), *other);
}
void liloconf::remove(String const &label)
{
	String t = String::escapeForRegExp(label);

	String regex="[ \t]*label[ \t]*=[ \t]*\"?"+t+"\"?[ \t]*";
	for(liloimages::iterator it=images.begin(); it!=images.end(); it++) {
		if(!(*it).grep(regex).empty()) {
			images.erase(it);
			break;
		}
	}
}
void liloconf::removeKernel(String const &kernel)
{
	String t = String::escapeForRegExp(kernel);

	String regex="[ \t]*(image|other)[ \t]*=[ \t]*\"?"+t+"\"?[ \t]*";
	for(liloimages::iterator it=images.begin(); it!=images.end(); it++) {
		if(!(*it).grep(regex).empty()) {
			images.erase(it);
			break;
		}
	}
}
StringList const &liloconf::entries() const
{
	StringList *s=new StringList;
	for(liloimages::const_iterator it=images.begin(); it!=images.end(); it++) {
		String lbl=(*it).grep("[ \t]*label[ \t]*=.*");
		lbl=lbl.mid(lbl.locate("label")+6);
		while(isspace(lbl.chr(0)) || lbl.chr(0)=='=' || lbl.chr(0)=='\"')
			lbl=lbl.mid(2);
		while(isspace(lbl.right().chr(0)) || lbl.right()==(String)"\"")
			lbl=lbl.left(lbl.size()-1);
		s->add(lbl);
	}
	return *s;
}
String const liloconf::dflt() const
{
	String dflt="";
	for(StringList::const_iterator it=defaults.begin(); it!=defaults.end() && dflt.empty(); it++)
		if(!((*it).regex("^[ \t]*default[ \t]*=")).empty())
			dflt=(*it).simplifyWhiteSpace();
	if(dflt.empty()) {
		liloimages::const_iterator it=images.begin();
		if (it != images.end())
			dflt=(*it).grep("^[ \t]*label[ \t]*=").simplifyWhiteSpace();
	}
	if(!dflt.empty()) {
		dflt=dflt.mid(dflt.locate("=")+2).simplifyWhiteSpace();
		if(dflt.left()==(String)"\"")
			dflt=dflt.mid(2).simplifyWhiteSpace();
		if(dflt.right()==(String)"\"")
			dflt=dflt.left(dflt.size()-1).simplifyWhiteSpace();
	}
	return dflt;
}
void liloconf::setDefault(String const &dflt)
{
	bool ready=false;
	for(StringList::const_iterator it=defaults.begin(); !ready && it!=defaults.end(); it++)
		if(!((*it).regex("^[ \t]*default[ \t]*=")).empty()) {
			defaults.remove(*it);
			ready=true;
		}
	defaults+="default=" + dflt;
}
liloconf::operator String() const
{
	String s=defaults;
	s+="\n";
	for(liloimages::const_iterator it=images.begin(); it!=images.end(); it++) {
		s+=*it;
		s+="\n";
	}
	return s;
}

ostream &operator <<(ostream &os, liloconf const &l)
{
	os << l.defaults << endl;
	for(liloimages::const_iterator it=l.images.begin(); it!=l.images.end(); it++)
		os << *it << endl;
	return os;
}
ostream &operator <<(ostream &os, liloconf const *l)
{
	os << l->defaults << endl;
	for(liloimages::const_iterator it=l->images.begin(); it!=l->images.end(); it++)
		os << *it << endl;
	return os;
}
