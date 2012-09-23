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

#ifndef KREC_EXPORT_WAVE_H
#define KREC_EXPORT_WAVE_H

#include "krecexport_template.h"

class KTempFile;
class QFile;

class KRecExport_Wave : public KRecExportItem {
   Q_OBJECT
public:
	KRecExport_Wave( QObject*, const char* =0, const QStringList& =0 );
	~KRecExport_Wave();

	KRecExport_Wave* newItem();

	QStringList extensions();
	QString exportFormat() { return QString( "Wave" ); }
public slots:
	bool initialize( const QString & );
	bool process();
	bool finalize();

private:
	QFile* _file;
};

#endif

// vim:sw=4:ts=4
