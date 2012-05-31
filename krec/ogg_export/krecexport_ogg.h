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

#ifndef KREC_EXPORT_OGG_H
#define KREC_EXPORT_OGG_H

#include <config.h>

#ifdef HAVE_VORBIS

#include "krecexport_template.h"

#include <vorbis/vorbisenc.h>

class KTempFile;
class QFile;

class KRecExport_OGG : public KRecExportItem {
   Q_OBJECT
public:
	KRecExport_OGG( QObject*, const char* =0, const QStringList& =0 );
	~KRecExport_OGG();

	KRecExport_OGG* newItem();

	QStringList extensions();
	QString exportFormat() { return QString( "OGG-Vorbis" ); }
public slots:
	bool initialize( const QString & );
	bool process();
	bool finalize();

private:
	QFile* _file;

	bool init_done;

	void setOggParameters();
	ogg_stream_state os; // take physical pages, weld into a logical stream of packets
	ogg_page         og; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet       op; // one raw packet of data for decode
	vorbis_info      vi; // struct that stores all the static vorbis bitstream settings
	vorbis_comment   vc; // struct that stores all the user comments
	vorbis_dsp_state vd; // central working state for the packet->PCM decoder
	vorbis_block     vb; // local working space for packet->PCM decode
	bool write_vorbis_comments;
};

#endif

#endif

// vim:sw=4:ts=4
