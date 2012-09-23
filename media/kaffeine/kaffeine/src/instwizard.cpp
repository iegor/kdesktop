/*
 * instwizard.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <ktextbrowser.h>
#include <kservice.h>

#include <qcheckbox.h>
#include <qvbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qstringlist.h>
#include <kdeversion.h>
#include <kapplication.h>

//#include <xine.h>

#include "version.h"
#include "instwizard.h"
#include "instwizard.moc"
#ifdef HAVE_DVB
#include "dvbconfig.h"
#endif

void InstWizard::internalWizard()
{
	setCaption(i18n("Kaffeine %1 Installation Wizard").arg(KAFFEINE_VERSION));
	resize(400,400);

	/************** installation check ************************/

	KTextBrowser* page1 = new KTextBrowser(this);
	addPage(page1, i18n("Installation Check"));
	setNextEnabled(page1, true);
	setBackEnabled(page1, false);
	setHelpEnabled(page1, false);
	setFinishEnabled(page1, false);

	QString infoString;
	QTextStream info(&infoString, IO_WriteOnly);

	//KAFFEINE-PART

	info << "<b>" << i18n("Kaffeine-Xine") << "...</b><br>";
	KService::Ptr service = KService::serviceByDesktopName("xine_part");
	if ((service) && (service->serviceTypes().contains("KaffeinePart")))
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	else
		info << "<font color=\"DarkRed\">" << i18n("Part not found. Please check your installation!") << "</font>";

	//KAFFEINE-PART
	info << "<br><hr><b>" << "KDE" << "...</b><br>";
	info << i18n("Found version") << ": " << KDE::versionString() << "<br>";
	if (KDE::version() >= KDE_MAKE_VERSION(3,2,0))
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	else
		info << "<font color=\"DarkRed\">" << i18n("Kaffeine requires KDE >= %1.").arg("3.2") << "</font>";

	//XINE-LIB
	//Should engine depending stuff be here?
	/*
	info << "<br><hr><b>" << "xine-lib" << "...</b><br>";
	QString xineVersion = xine_get_version_string();
	info << i18n("Found version") << ": " << xineVersion << "<br>";
	if (xine_check_version(1,0,0))
	{
		int major, minor, sub;
		xine_get_version(&major, &minor, &sub);
		if (xineVersion.contains("CVS", false))
			info << "<font color=\"DarkBlue\">" << i18n("Developer version.") << "</font>";

		if ((major == 1) && (minor == 0) && (sub == 0) && (xineVersion.contains("rc", false)))
		{
			info << "<font color=\"DarkRed\">" << i18n("Kaffeine requires xine-lib >= %1. Download the latest version here:").arg("1.0") << " <a href=\"http://www.xinehq.de\">http://www.xinehq.de</a>.</font>";
		}
		else
			info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	}
	else
		info << "<font color=\"DarkRed\">" << i18n("Kaffeine requires xine-lib >= %1. Download the latest version here:").arg("1.0")
		<< " <a href=\"http://www.xinehq.de\">http://www.xinehq.de</a>.</font>";
	*/
	//WIN32-CODECS
	info << "<hr><b>" << i18n("WIN32 Codecs") << "...</b><br>";
	QDir d("/usr/lib/win32");
	QStringList entries = d.entryList("wmv*");
	if (entries.count())
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	else
		info << "<font color=\"DarkRed\">" << i18n("No WIN32 codecs found in /usr/lib/win32. You're not able to play Windows Media 9 files, newer Real Media files and some less common formats. Download the codecs here:")
		<< " <a href=\"http://www1.mplayerhq.hu/homepage/design7/codecs.html\">http://www.mplayerhq.hu</a>.</font>";

	//LIBDVDCSS
	info << "<br><hr><b>" << "libdvdcss" << "...</b><br>";
	bool dvdcss = false;
	d = QDir("/usr/lib");
	entries = d.entryList("libdvdcss.so*");
	if (entries.count())
		dvdcss = true;
	else
	{
		d = QDir("/usr/local/lib");
		entries = d.entryList("libdvdcss.so*");
		if (entries.count())
			dvdcss = true;
		else
		{
			d = QDir("/usr/lib64");
			entries = d.entryList("libdvdcss.so*");
			if (entries.count())
				dvdcss = true;
		}
	}
	if (!dvdcss)
		info << "<font color=\"DarkRed\">" << i18n("libdvdcss not found. You're not able to play encrypted (most commercial) DVD's. You can get the library here (but using it may violate copyright regulations of your country!):")
		<< " <a href=\"http://developers.videolan.org/libdvdcss/\">http://developers.videolan.org/libdvdcss</a>.</font>";
	else
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";

	//DVD-DRIVE
	info << "<br><hr><b>" << i18n("DVD Drive") << "...</b><br>";
	KProcess process;
	process << "/sbin/hdparm" << "-d" << "/dev/dvd";
	connect(&process, SIGNAL(receivedStdout(KProcess*,char*,int)), this, SLOT(slotStdout(KProcess*, char*, int)));
	process.start(KProcess::Block, KProcess::Stdout);

	if (stdout.contains('1'))
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	else if (stdout.contains('0'))
		info << "<font color=\"DarkRed\">" << i18n("DMA mode off! For smooth DVD playback run as root:") +  " \"hdparm -d1 /dev/dvd\".</font>";
	else
		info << "<font color=\"DarkBlue\">" << i18n("Can't check DMA mode. Permission denied or no such device:")
		<< " \"/dev/dvd\".</font>";

	//DVB-DEVICES
#ifdef HAVE_DVB
	info << "<br><hr><b>" << i18n("DVB-Device") << "...</b><br>";
	if (DVBconfig::haveDvbDevice())
	{
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";
	}
	else
	{
		info << "<font color=\"DarkBlue\">" << i18n("No DVB-Devices found. The DVB related functions will be hidden.") << "</font>";
	}
#endif

	//DISTRIBUTION
	info << "<br><hr><b>" << i18n("Distribution") << "...</b><br>";
	if (QFile::exists("/etc/SuSE-release"))
	{
		QFile file("/etc/SuSE-release");
		file.open(IO_ReadOnly);
		QTextStream stream(&file);
		info << "<font color=\"DarkBlue\">" << i18n("Found") << ": " << stream.readLine() << ". </font>"
		<< i18n("The xine-lib shipped by SuSE \"may lack certain features because of legal requirements (potential patent violation)\". You should use the packages from here:")
		<< " <a href=\"http://packman.links2linux.de/?action=124\">http://packman.links2linux.de</a>.";
		file.close();
	}
	else
		info << "<font color=\"DarkGreen\">" << i18n("Ok.") << "</font>";


	info << "<hr><br><b>" << i18n("RESULT") << ":</b> ";

	if (infoString.contains("DarkRed"))
	{
		info << i18n("Found some problems, but nevertheless Kaffeine may work.");
	}
	else
	{
		info << i18n("All ok!");
	}

	info << "<br><br";

	page1->setText(infoString);

	/************** installation options **********************/
	QVBox* page2 = new QVBox(this);
	page2->setSpacing(20);

	QCheckBox *m_useMMSHelper = new QCheckBox(i18n("Use Kaffeine as helper application for mms:// (Microsoft Media) streams"), page2);
	if (!locateLocal("services", "mms.protocol"))
		m_useMMSHelper->setChecked(true);

	QCheckBox *m_useRTSPHelper = new QCheckBox(i18n("Use Kaffeine as helper application for rtsp:// (Real Media and others) streams"), page2);
	if (!locateLocal("services", "rtsp.protocol"))
		m_useRTSPHelper->setChecked(true);

	QCheckBox *m_createDesktopIcon = new QCheckBox(i18n("Create a Kaffeine icon on desktop"), page2);
	if (!QFile::exists( QDir::homeDirPath() + "/Desktop/kaffeine.desktop"))
		m_createDesktopIcon->setChecked(true);

	addPage(page2, i18n("Installation Options"));
	setNextEnabled(page2, false);
	setBackEnabled(page2, true);
	setHelpEnabled(page2, false);
	setFinishEnabled(page2, true);

	if (exec() == QDialog::Accepted) {
		kdDebug() << "WizardDialog: finished pressed" << endl;

		KProcess process;

		QStringList dirs = KGlobal::dirs()->findDirs("data", "kaffeine");
		QString homePath = KGlobal::dirs()->localkdedir();
		kdDebug() << "WizardDialog: KDE homedir: " << homePath << endl;
		QString dataPath;
		if (dirs[0].left(homePath.length()) != homePath) /* global data path */
			dataPath = dirs[0];
		else
			dataPath = dirs[1];

		if (m_useMMSHelper->isChecked())
		{
			process << "cp" << dataPath + "mms.protocol" << locateLocal("services", "mms.protocol");
			kdDebug() << "WizardDialog: cp " << dataPath + "mms.protocol" << " " << locateLocal("services", "mms.protocol") << endl;
			process.start(KProcess::Block);
			process.clearArguments();
		}

		if (m_useRTSPHelper->isChecked())
		{
			process << "cp" << dataPath + "rtsp.protocol" << locateLocal("services", "rtsp.protocol");
			kdDebug() << "WizardDialog: cp " << dataPath + "rtsp.protocol" << " " << locateLocal("services", "rtsp.protocol") << endl;
			process.start(KProcess::Block);
			process.clearArguments();
		}

		//dirs = KGlobal::dirs()->findDirs("apps", "Multimedia");
		dirs = KGlobal::dirs()->findDirs("xdgdata-apps", "kde");
		if (!dirs.isEmpty())
			dataPath = dirs[0];
		else
			kdError() << "WizardDialog: Applications path not found!" << endl;

		if (m_createDesktopIcon->isChecked())
		{
			process << "cp" << dataPath + "kaffeine.desktop" << QDir::homeDirPath() + "/Desktop/kaffeine.desktop";
			kdDebug() << "WizardDialog: cp " << dataPath + "kaffeine.desktop" << " " << QDir::homeDirPath() + "/Desktop/kaffeine.desktop" << endl;
			process.start(KProcess::Block);
			process.clearArguments();
		}
	}
}

void InstWizard::slotStdout(KProcess *, char *buffer, int buflen)
{
	QString output = QString::fromLatin1(buffer, buflen);
	kdDebug() << "WizardDialog: got from hdparm: " << output << "\n";
	stdout.append(output);
}
