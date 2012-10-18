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

#ifndef KREC_EXPORT_TEMPLATE_H
#define KREC_EXPORT_TEMPLATE_H

#include <qobject.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qcstring.h>
#include <arts/common.h>

/**
 * This class is the template for all exportitems in KRec.
 *
 * To register your ExportItem you have to do @code registerAtGlobal( this ); @code
 * at the end of your constructor.
 *
 * @author Arnold Krille <arnold@arnoldarts.de>
*/
class KRecExportItem : public QObject {
   Q_OBJECT
private:
	/// Controls wether an export is running, or not...
	bool _running;
	int _samplingRate, _bits, _channels;
protected:
	KRecExportItem( QObject*, const char* =0, const QStringList& =0 );

	/**
	 * This registers this class at the KRecGlobal.
	 */
	static void registerAtGlobal( KRecExportItem* );

	int samplingRate() const;
	int bits() const;
	int channels() const;

	/// Helpers for reading and writing to an char-array
	Q_INT16 read16( char* array, int index );
	void write16(  char* array, Q_INT16 value, int index );
	void write32(  char* array, Q_INT32 value, int index );
public:
	~KRecExportItem();

	/**
	 * Returns a new Item for your export.
	 */
	virtual KRecExportItem* newItem()=0;

	/// Returns a list with the extensions.
	virtual QStringList extensions()=0;
	/// Returns the export format.
	virtual QString exportFormat()=0;

	bool running() const { return _running; }
public slots:
	/**
	 * Initializes the sound-settings.
	 * Has to be called before the initialize( const QString & ) function.
	 */
	void initialize( int samplingRate, int bits, int channels );
	/// Initializes the export.
	virtual bool initialize( const QString &filename )=0;
	/**
	 * Start the real export (including the emission of @see getData() while running=true).
	 * Best way to do this is to use @see QTimer::singleShot() since then the events are
	 * put at the end of the EventLoop not blocking the gui and everything else.
	 */
	bool start();
	virtual bool process() =0;

	/// Stops the export by setting running=false.
	void stop();
	/// Finish the export.
	virtual bool finalize()=0;
	//virtual void data( QByteArray& );
signals:
	/// Is emitted when this Item wants data to export.
	void getData( QByteArray& );

	void running( bool );
};


#endif

// vim:sw=4:ts=4
