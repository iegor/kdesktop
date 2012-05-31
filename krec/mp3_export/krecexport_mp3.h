/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef KREC_EXPORT_MP3_H
#define KREC_EXPORT_MP3_H

#include "krecexport_template.h"

#include <lame/lame.h>

class KTempFile;
class QFile;

class KRecExport_MP3 : public KRecExportItem {
   Q_OBJECT
public:
	KRecExport_MP3( QObject*, const char* =0, const QStringList& =0 );
	~KRecExport_MP3();

	KRecExport_MP3* newItem();

	QStringList extensions();
	QString exportFormat() { return QString( "Wave" ); }
public slots:
	bool initialize( const QString & );
	bool process();
	bool finalize();

private:
	QFile* _file;
	// Lame MP3 encoder
	void setLameParameters();
	lame_global_flags *gfp;
	unsigned char mp3buf[ LAME_MAXMP3BUFFER ];
	bool error_occurred;
	bool write_id3;
	bool init_done;
};

#endif

// vim:sw=4:ts=4
