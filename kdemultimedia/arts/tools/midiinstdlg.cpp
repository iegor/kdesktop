/*

    Copyright (C) 1998 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "midiinstdlg.h"
#include <klocale.h>
#include <kstandarddirs.h>

#include <kapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qfile.h>
#include <qdir.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <kdebug.h>
#include <qbutton.h>
#include <qpushbutton.h>
#include <kstdguiitem.h>

static QStringList getArtsPath()
{
    QStringList artsPath;
	QString dir = locate("data", "artsbuilder/examples/");
	artsPath += dir;
    QString home = QDir::homeDirPath() + "/arts/structures/";
    artsPath += home;
	return artsPath;
}

static QStringList listFiles(QString directory, QString extension)
{
	QStringList result;
    QStringList artsPath = getArtsPath();

    QStringList::Iterator it;
    for ( it = artsPath.begin(); it != artsPath.end(); it++ ) {
        QString pathname = *it + "/" + directory;
        QDir dir(pathname, extension);
        if (dir.exists()) {
            //kdDebug() << "found dir " << dir.absPath() << endl;
            result += dir.entryList();
        }
    }

	return result;
}

MidiInstDlg::MidiInstDlg(QWidget *parent)
	:QDialog(parent,"instrument",TRUE)
{
	QVBoxLayout *mainlayout = new QVBoxLayout(this);

// caption label: title

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(QString(" ")+i18n("Instrument")+QString(" "));
	captionlabel->setAlignment(AlignCenter);
	//min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler2 = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler2);
	mainlayout->addSpacing(5);

// combobox

	box = new QComboBox(this);

    QStringList instruments = listFiles(".","*.arts");
    QStringList::Iterator it;
    for ( it = instruments.begin(); it != instruments.end(); it++ ) {
        QString modname = *it;
        QString prefix = QString::fromLatin1("instrument_");
        if (modname.length() > 5)
            modname.truncate(modname.length()-5);  // kill .arts extension
        if ( (modname.startsWith(prefix)) && (!modname.contains("_GUI")) )
            box->insertItem(modname.mid(prefix.length()));
            //kdDebug() << "inserted instrument: " << modname.mid(prefix.length()) << endl;
    }


    QStringList maps = listFiles(".","*.arts-map");

    for ( it = maps.begin(); it != maps.end(); it++ ) {
        QString modname = *it;
        QString prefix = QString::fromLatin1("instrument_");
        if (modname.length() > 9)
            modname.truncate(modname.length()-9);  // kill .arts-map extension
        if (modname.startsWith(prefix))
            box->insertItem(modname.mid(prefix.length()));
            //kdDebug() << "inserted map: " << modname.mid(prefix.length()) << endl;
    }

	mainlayout->addWidget(box);

// hruler

	mainlayout->addSpacing(5);
	KSeparator *ruler = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(ruler);
	mainlayout->addSpacing(5);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	bbox->addButton(KStdGuiItem::help(), this, SLOT( help() ));
	bbox->addStretch(1);

	QButton *okbutton = bbox->addButton(KStdGuiItem::ok());
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	bbox->layout();

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

	mainlayout->freeze();
}

QCString MidiInstDlg::filename()
{
    QStringList artsPath = getArtsPath();
	QString instrument = box->currentText();

    QStringList::Iterator it;

    for ( it = artsPath.begin(); it != artsPath.end(); it++ ) {
        QString pathname = *it + QString::fromLatin1("/instrument_") + instrument + QString::fromLatin1(".arts");
        QFileInfo fi(pathname);
        if (fi.exists() && fi.isReadable())
            return QFile::encodeName(pathname);

        pathname = *it + QString::fromLatin1("/instrument_") + instrument + QString::fromLatin1(".arts-map");
        fi.setFile(pathname);
        if (fi.exists() && fi.isReadable())
            return QFile::encodeName(pathname);
    }

	return "";
}

void MidiInstDlg::help()
{
	KApplication::kApplication()->invokeHelp("", "artsbuilder");
}

#include "midiinstdlg.moc"
