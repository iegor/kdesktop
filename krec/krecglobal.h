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

#ifndef KREC_GLOBAL_H
#define KREC_GLOBAL_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>

class KConfig;
class QWidget;
class KStatusBar;

class KRecExportItem;

class KRecGlobal : public QObject {
   Q_OBJECT
private:
	KRecGlobal( QObject* =0, const char* =0 );
	~KRecGlobal();

public:

	/**
	 * @return Returns a pointer to the global KRecGlobal.
	 */
	static KRecGlobal* the();

	/**
	 * Sets the MainWidget.
	 */
	void setMainWidget( QWidget* );
	/**
	 * Returns a pointer to the mainwidget.
	 * Usefull to display Messageboxes, etc correctly without beeing a
	 * QWidget or knowing about a parent QWidget.
	 */
	QWidget* mainWidget();

	/// @return kapp->config()
	static KConfig* kconfig();

	/// Sets the Statusbar.
	void setStatusBar( KStatusBar* );
	/**
	 * Puts a message into the statusbar.
	 * Usefull for showing messages without knowing about the Statusbar.
	 */
	void message( const QString & );

	/// Registers a KRecExportItem
	static bool registerExport( KRecExportItem* );
	/**
	 * Returns a new KRecExportItem for the specified exportFormat.
	 * If now Item can be found the return value is 0.
	 */
	KRecExportItem* getExportItem( const QString &exportFormat );
	///
	KRecExportItem* getExportItemForEnding( const QString & );
	/// Returns a list of exportFormats.
	QStringList exportFormats() const;
	/// Returns a list of fileendings.
	QString exportFormatEndings() const;

	/**
	 * Gets/Sets the actual mode for formating time values.
	 *
	 * For description see krecfileview.h
	*/
	int timeFormatMode();
	void setTimeFormatMode( int );
	/// The framebase (how many frames per second) (25/30 for movies, 75 for CDs)
	int frameBase();
	void setFrameBase( int );
private:
	QWidget *_qwidget;
	KStatusBar *_statusbar;
	QDict <KRecExportItem> *_exports;
	QStringList _exportformats;
	int _timeformatcache, _framebasecache;
};

#endif

// vim:sw=4:ts=4
