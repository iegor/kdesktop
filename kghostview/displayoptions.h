/**
 * Copyright (C) 2003, Luís Pedro Coelho
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef DISPLAYOPTIONS_H
#define DISPLAYOPTIONS_H
#include <qstring.h>
#include <qvaluelist.h>
#include "dscparse_adapter.h"
#include <kdemacros.h>
class KCmdLineArgs;
class KConfig;

class KDE_EXPORT DisplayOptions 
{
	public:
	DisplayOptions();

	// default generated DisplayOptions(const DisplayOptions&); 
	// default generated ~DisplayOptions(); 
	// default generated DisplayOptions& operator = (const DisplayOptions&); 

	void reset() { *this = DisplayOptions(); }

	void restoreOverrideOrientation() { setOverrideOrientation( CDSC_ORIENT_UNKNOWN ); }
	void setOverrideOrientation(CDSC_ORIENTATION_ENUM e)  { _overrideOrientation = e; }
	CDSC_ORIENTATION_ENUM overrideOrientation() const { return _overrideOrientation; }

	void restoreOverridePageMedia() { _overridePageMedia = QString::null; }
	void setOverridePageMedia(const QString& newMedia) { _overridePageMedia = newMedia; }
	const QString& overridePageMedia() const { return _overridePageMedia; }

	/** 
	 * The current page.
	 * Note that the pages here are 0-based, although the user should see 1-based pages.
	 *
	 * In parsing cmdline-args, we transform --page=1 into setPage( 0 );
	 */
	int page() const { return _page; }
	void setPage( int newPage ) { _page = newPage; }

	double magnification() const;
	void setMagnification( double );
	/** 
	 * Goes to the next degree of magnification if possible. If we cannot zoom in any more, it returns false,
	 * leaving the magnification untouched.
	 */
	bool zoomIn();
	bool zoomOut();

	bool canZoomIn() const;
	bool canZoomOut() const;

	/** 
	 * Parses command line options.
	 */
	static DisplayOptions parse ( KCmdLineArgs * );
	/**
	 * Transforms the object in a string representation.
	 * Useful for storing in config files, for example.
	 *
	 * \return string representation or null on error.
	 *
	 * \sa fromString
	 */
	static QString toString( const DisplayOptions& );
	/**
	 * Reads the display options from a string formatted by toString.
	 *
	 * \return true if the convertion succeeded.
	 */
	static bool fromString( DisplayOptions&, const QString& );

	/**
	 * Returns a list of values that normally we can get through
	 * zoomIn() & zoomOut()
	 */
	static QValueList<double> normalMagnificationValues();

	private:

	unsigned closestIndex() const;

	CDSC_ORIENTATION_ENUM _overrideOrientation;
	QString               _overridePageMedia;
	int                   _page;
	double                _magnification;
};

inline
DisplayOptions::DisplayOptions()
	:_overrideOrientation( CDSC_ORIENT_UNKNOWN ),
	 _overridePageMedia ( QString::null ),
	 _page( 0 )
{
	setMagnification( 1.0 );
}

#endif // DISPLAYOPTIONS_H
