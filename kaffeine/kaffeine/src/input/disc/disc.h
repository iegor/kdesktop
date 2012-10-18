/*
 * disc.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef DISC_H
#define DISC_H

#include <kaction.h>
#include <kstdaction.h>
#include <kconfig.h>
#include <klistview.h>

#include <qframe.h>
#include <qvbox.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qguardedptr.h>

#include "kaffeineinput.h"
#include "paranoia.h"

class MRL;

class MLabel : public QLabel
{
	Q_OBJECT

public:
	MLabel( QWidget *parent );
	~MLabel() {}

protected:
	void paintEvent( QPaintEvent * );
};



class MListView : public KListView
{
	Q_OBJECT

public:
	MListView( QWidget *parent );
	~MListView() {}

protected:
	virtual void resizeEvent(QResizeEvent*);
};



class Disc : public KaffeineInput
{
	Q_OBJECT

public:
	Disc(QWidget *parent, QObject *objParent, const char *name=0);
	~Disc();

	// Reimplemented from KaffeineInput
public:
	QWidget *wantPlayerWindow();
	QWidget *inputMainWidget();
	void mergeMeta(const MRL&);
	bool nextTrack( MRL& );
	bool previousTrack( MRL& );
	bool currentTrack( MRL& );
	bool trackNumber( int, MRL& );
	bool playbackFinished( MRL& );
	void toggleLayout( bool );
	void playerStopped();
	void getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames );
	void togglePanel();
	bool execTarget( const QString& );
	void saveConfig();
	//***************************************

public slots:
	void startCD( const QString &device="", bool rip=false );
	void startDVD( const QString &device="" );
	void startVCD( const QString &device="" );
	void startRIP();

public:
	QVBox *mainWidget;
	QVBox *playerBox;

private:
	void loadConfig( KConfig* config );
	void saveConfig( KConfig* config );
	void setCurrent( int n );
	void setupActions();

	QLabel *artistLab, *albumLab;
	QGuardedPtr<QWidget> widg;
	QToolButton *ripBtn, *cdBtn;
	QToolButton *enc;
	QSplitter *split;
	QGuardedPtr<QFrame> panel;
	MLabel *discLab;
	Paranoia *para;
	MListView *list;
	int trackCurrent;
	QString currentDevice;
	QPixmap currentPixmap;
	QWidget *encodeWidget;
	QTimer encodeTimer;
	QProgressBar *progressBar;

private slots:
	void trackSelected( QListViewItem* );
	void encode();
	void encodeProgress();
	void setEncoding( bool );

signals:
	void signalRequestForDVD();
	void signalRequestForVCD();
};

#endif /* DISC_H */
