/* Disks.cc
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
#include "Disks.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

StringList ptable::disklist()
{
	/* The basics behind scanning for a disk are simple: If it can be     *
	 * opened, it exists.                                                 *
	 * Using access() is not sufficient - that's successful if the device *
	 * node exists but there's no actual device.                          *
	 * Checking /proc/devices would work, but it wouldn't autoload modules*
	 * so chances are we'd be missing something.                          */
	StringList dsk;
	int fd;
	bool finished=false;
	#define CHECK_DEV(d)	if((fd=open(d, O_RDONLY))<0) { \
					finished=true; \
					continue; \
				} \
				close(fd); \
				dsk+=d
	String dev="/dev/hd";	// IDE devices: /dev/hd[a-t]
	for(char a='a'; a<='t'; a++) {
		// We can't use finished for IDE drives - it's perfectly OK
		// to have a /dev/hdc but no /dev/hdb.
		CHECK_DEV(dev+a);
	}
	finished=false;
	dev="/dev/sd";		// SCSI disks 0-25
	for(char a='a'; a<='z' && !finished; a++) {
		CHECK_DEV(dev+a);
	}
	for(char prefix='a'; prefix <='d' && !finished; prefix++) {
		dev="/dev/sd" + prefix; // SCSI disks 26-127
		for(char a='a'; (a<='x' || (a<='z' && prefix<'d')) && !finished; a++) {
			CHECK_DEV(dev+a);
		}
	}
	finished=false;
	dev="/dev/i2o/hd";	// I2O disks 0-25: /dev/i2o/hd[a-z]
	for(char a='a'; a<='z' && !finished; a++) {
		CHECK_DEV(dev+a);
	}
	for(char prefix='a'; prefix <='d' && !finished; prefix++) {
		dev="/dev/i2o/hd" + prefix; // I2O disks 26-127
		for(char a='a'; (a<='x' || (a<='z' && prefix<'d')) && !finished; a++) {
			CHECK_DEV(dev+a);
		}
	}
	finished=false;
	dev="/dev/pd";		// Parallel port disks: /dev/pd[a-d]
	for(char a='a'; a<='d' && !finished; a++) {
		CHECK_DEV(dev+a);
	}
	finished=false;
	for(unsigned int i=0; i<=31 && !finished; i++) { // Software RAID
		String device;
		device.sprintf("/dev/md%u", i);
		CHECK_DEV(device);
	}
	finished=false;
	for(unsigned int i=0; i<=7; i++) { //Compaq Smart Array
		for(unsigned int j=0; j<=15 && !finished; j++) {
			String device;
			device.sprintf("/dev/ida/c%ud%u", i, j);
			CHECK_DEV(device);
		}
	}
	finished=false;
	for(unsigned int i=0; i<=7; i++) { // Mylex DAC960
		for(unsigned int j=0; j<=31 && !finished; j++) {
			String device;
			device.sprintf("/dev/rd/c%ud%u", i, j);
			CHECK_DEV(device);
		}
	}
	finished=false;
	dev="/dev/ed";		// MCA ESDI harddisks: /dev/ed[ab]
	for(char a='a'; a<='b' && !finished; a++) {
		CHECK_DEV(dev+a);
	}
	finished=false;
	dev="/dev/xd";		// XT (8-bit) harddisks: /dev/xd[ab]
	for(char a='a'; a<='b' && !finished; a++) {
		CHECK_DEV(dev+a);
	}
	return dsk;
}
StringList ptable::partlist()
{
	StringList s;
	StringList d=disklist();
	for(StringList::const_iterator it=d.begin(); it!=d.end(); it++) {
		for(int i=1; i<32; i++) {
			String drive;
			drive.sprintf("%s%u", (*it).cstr(), i);
			int fd=open(drive, O_RDONLY);
			if(fd>=0) {
				char test;
				if(read(fd, &test, 1)>0)
					s += drive;
				close(fd);
			} else
				break;
		}
	}
	return s;
}
ptable::ptable(StringList const &disks)
{
	partition.clear();
	id.clear();
	mountpt.clear();
	for(StringList::const_iterator it=disks.begin(); it!=disks.end(); it++)
		scandisk(*it);
}
ptable::ptable(String const &disk)
{
	partition.clear();
	id.clear();
	mountpt.clear();
	scandisk(disk);
}
void ptable::scandisk(String const &disk)
{
	String cmd;
	cmd.sprintf("fdisk -l %s 2>&1", (char*)disk);
	FILE *fdisk=popen(cmd, "r");
	char *buf=(char *) malloc(1024);
	String dev;
	while(fgets(buf, 1024, fdisk)) {
		if(strncmp("/dev/", buf, 5)==0) { // Partition entry
			// We don't care about active vs. non-active partitions.
			// Remove the sign.
			while(strchr(buf, '*')) *strchr(buf, '*')=' ';
			// blanks are blanks...
			while(strchr(buf, '\t')) *strchr(buf, '\t')=' ';
			// Get the device
			*strchr(buf, ' ')=0;
			dev=buf;
			partition += buf;
			// And figure out where/if it's mounted
			mountpt[dev]=mountpoint(dev);
			// Lastly, get the partition type.
			strcpy(buf, buf+strlen(buf)+1);
			while(isspace(*buf)) strcpy(buf, buf+1);
			strcpy(buf, strchr(buf, ' ')); // skip Start
			while(isspace(*buf)) strcpy(buf, buf+1);
			strcpy(buf, strchr(buf, ' ')); // skip End
			while(isspace(*buf)) strcpy(buf, buf+1);
			strcpy(buf, strchr(buf, ' ')); // skip Blocks
			while(isspace(*buf)) strcpy(buf, buf+1);
			id[dev]=strtol(buf, NULL, 16);
		}
	}
	pclose(fdisk);
	free(buf);
}
String ptable::mountpoint(String const &device, bool fstab_fallback)
{
	char *buf=new char[1024];
	FILE *f=fopen("/etc/mtab", "r");
	String mp="";
	while(fgets(buf, 1024, f)) {
		if(strchr(buf,' '))
			*strchr(buf, ' ')=0;
		if(strchr(buf,'\t'))
			*strchr(buf, '\t')=0;
		if(device.cmp(buf)) {
			strcpy(buf, buf+strlen(buf)+1);
			while(isspace(*buf))
				strcpy(buf, buf+1);
			if(strchr(buf,' '))
				*strchr(buf, ' ')=0;
			if(strchr(buf,'\t'))
				*strchr(buf, '\t')=0;
			mp=buf;
			mp=mp.simplifyWhiteSpace();
			break;
		}
	}
	fclose(f);
	if(mp.empty() && fstab_fallback) {
		f=fopen("/etc/fstab", "r");
		while(fgets(buf, 1024, f)) {
			if(strchr(buf,' '))
				*strchr(buf, ' ')=0;
			if(strchr(buf,'\t'))
				*strchr(buf, '\t')=0;
			if(device.cmp(buf)) {
				strcpy(buf, buf+strlen(buf)+1);
				while(isspace(*buf))
					strcpy(buf, buf+1);
				if(strchr(buf,' '))
					*strchr(buf, ' ')=0;
				if(strchr(buf,'\t'))
					*strchr(buf, '\t')=0;
				mp=buf;
				mp=mp.simplifyWhiteSpace();
				break;
			}
		}
		fclose(f);
	}
	delete buf;
	return mp;
}
String ptable::device(String const &mountpt, bool fstab_fallback)
{
	char *buf=new char[1024];
	FILE *f=fopen("/etc/mtab", "r");
	String dev="";
	while(fgets(buf, 1024, f)) {
		if(strchr(buf,' '))
			*strchr(buf, ' ')=0;
		if(strchr(buf,'\t'))
			*strchr(buf, '\t')=0;
		String device=buf;
		strcpy(buf, buf+strlen(buf)+1);
		while(isspace(*buf))
			strcpy(buf, buf+1);
		if(strchr(buf,' '))
			*strchr(buf, ' ')=0;
		if(strchr(buf,'\t'))
			*strchr(buf, '\t')=0;
		String mp=buf;
		mp=mp.simplifyWhiteSpace();
		if(mp==mountpt) {
			dev=device;
			break;
		}
	}
	fclose(f);

	if(dev.empty() && fstab_fallback) {
		// The FS is not mounted - maybe it could be, though.
		f=fopen("/etc/fstab", "r");
		while(fgets(buf, 1024, f)) {
			if(strchr(buf,' '))
				*strchr(buf, ' ')=0;
			if(strchr(buf,'\t'))
				*strchr(buf, '\t')=0;
			String device=buf;
			strcpy(buf, buf+strlen(buf)+1);
			while(isspace(*buf))
				strcpy(buf, buf+1);
			if(strchr(buf,' '))
				*strchr(buf, ' ')=0;
			if(strchr(buf,'\t'))
				*strchr(buf, '\t')=0;
			String mp=buf;
			mp=mp.simplifyWhiteSpace();
			if(mp==mountpt) {
				dev=device;
				break;
			}
		}
		fclose(f);
	}
	delete buf;
	return dev;
}
