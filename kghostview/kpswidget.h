/** 
 * Copyright (C) 2000-2003 the KGhostView authors. See file AUTHORS.
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

#ifndef __KPSWIDGET_H__
#define __KPSWIDGET_H__

#include <queue>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qpixmap.h>

#include "dscparse_adapter.h"
#include "configuration.h"

#include <X11/X.h>

class KProcess;

class KGVConfigDialog;
class MessagesDialog;

/**
 * @class KPSWidget
 *
 * @brief KPSWidget is a widget on which Ghostscript can render.
 *
 * @ref ghostscript_interface
 */

class KPSWidget : public QWidget
{
    Q_OBJECT

public:
    KPSWidget( QWidget* parent = 0, const char* name = 0 );
    ~KPSWidget();

    /**
     * Start up the Ghostscript interpreter. Returns true if Ghostscript
     * could be started; otherwise false.
     */
    bool startInterpreter();

    /**
     * Stop the interpreter process.
     */
    void stopInterpreter();

    /**
     * Returns true if the interpreter is ready for new input.
     */
    bool isInterpreterReady() const;

    bool isInterpreterBusy() const;

    /**
     * Returns true if the interpreter is running; otherwise returns false.
     */
    bool isInterpreterRunning() const;

    /**
     * Tell ghostscript to start the next page.
     * Returns false if ghostscript is not running, or not ready to start
     * another page. If another page is started, sets the _interpreterReady 
     * flag and cursor.
     */
    bool nextPage();

    /**
     * Queue a portion of a PostScript file for output to ghostscript and 
     * start processing the queue.
     *
     * fp: FILE* of the file in question. Should be seek()able.
     * begin: position in file to start.
     * len: number of bytes to write.
     *
     * If an interpreter is not running, nothing is queued and false is 
     * returned.
     */
    bool sendPS( FILE*, unsigned int begin, unsigned int end );
   
    /**
     * Sets the filename of the ghostscript input. 
     * @p usePipe indicates whether we use a pipe for
     * communication or let ghoscript read the file itself.
     */
    void setFileName( const QString&, bool usePipe );

    /**
     * Set the bounding box of the drawable. See my comment in the source
     * file.
     */
    void setBoundingBox( const KDSCBBOX& );

    /**
     * Set the orientation of the page.
     */
    void setOrientation( CDSC_ORIENTATION_ENUM );

    /**
     * Sets the resolution according to the physical resolution of the screen 
     * and the magnification value.
     */
    void setMagnification( double magnification );

    /**
     * @return the boundingbox of the drawable.
     */
    const KDSCBBOX& boundingBox() const;

    /**
     * @return the current orientation.
     */
    CDSC_ORIENTATION_ENUM orientation() const;

    /**
     * Double buffering means that all the drawing is done outside the
     * screen and the finished picture is then flashed to the screen.
     * This reduces flicker ( to almost none ) at the price of speed.
     *
     * By default, KPSWidget is *not* double buffered.
     */
    bool isDoubleBuffered() const { return _doubleBuffer; }
    void setDoubleBuffering( bool n );


    void clear();

public slots:
    /**
     * Call this when the settings have changed.
     */
    void readSettings();
signals:
    /**
     * This signal gets emited whenever a page is finished, but contains a reference to the pixmap
     * used to hold the image.
     *
     * Don't change the pixmap or bad things will happen. This is the backing pixmap of the display.
     */
    void newPageImage( QPixmap image );

    /**
     * This signal is emitted whenever the ghostscript process has
     * written data to stdout or stderr.
     */
    void output( char* data, int len );

    /**
     * This means that gs exited uncleanly
     *
     * @param msg a <strong>translated</strong> error message to display the user which may be null if we cannot tell anything important
     */
    void ghostscriptError( const QString& mgs );

protected:
    struct Record 
    {
	Record( FILE* fp_, long begin_, unsigned len_ )
	    :fp( fp_ ), begin( begin_ ), len( len_ ) { }

	FILE* fp;
	long begin;
	unsigned int len;
    };

    // void resizeEvent( QResizeEvent* );
    bool x11Event( XEvent* );

    /**
     * Setup the widget according to the current settings for the 
     * boundingBox, the resolution and the orientation. This involves
     * the following things:
     * - Resize the widget
     * - Resize and clear the background pixmap.
     * - Setup the GHOSTVIEW and GHOSTVIEW_COLORS properties.
     *
     * Make sure ghostscript isn't running when calling this method.
     */
    void setupWidget();
    
    void setGhostscriptPath( const QString& );
    void setGhostscriptArguments( const QStringList& );
    void setPalette( Configuration::EnumPalette::type );

protected slots:
    void gs_input( KProcess* );
    void gs_output( KProcess*, char* buffer, int len );
    void interpreterFailed();
    void slotProcessExited( KProcess* );
	
private:
    Window _gsWindow; // Destination of ghostscript messages.

    enum AtomName { GHOSTVIEW = 0, GHOSTVIEW_COLORS, NEXT, PAGE, DONE };
    Atom _atoms[5];

    QPixmap _backgroundPixmap;

    /**
     * The following properties determine how Ghostscript is started.
     * If any of these is changed, Ghostscript needs to be restarted.
     */
    QString     _ghostscriptPath;
    QStringList _ghostscriptArguments;
    QString     _fileName;
    bool        _usePipe;
    bool        _doubleBuffer;

    /**
     * Flag set when one of the properties _ghostscriptPath, 
     * _ghostscriptArguments or _fileName has been changed.
     */
    bool _ghostscriptDirty;
 
    /**
     * The following properties determine how Ghostscript renders its
     * pages. If any of these is changed, the widget needs to be setup, 
     * and Ghostscript needs to be restarted.
     */
    CDSC_ORIENTATION_ENUM _orientation;
    KDSCBBOX              _boundingBox;
    float                 _magnification;
    Configuration::EnumPalette::type _palette;

    /**
     * Flag set when one of the properties _orientation, _boundingBox,
     * _dpi[X|Y] or _palette has been changed.
     */
    bool _widgetDirty;

    KProcess* _process;
    char* _buffer;
 
    std::queue<Record> _inputQueue;

    bool _stdinReady;
    bool _interpreterBusy;
    bool _interpreterReady;
};

inline const KDSCBBOX& KPSWidget::boundingBox() const
{
    return _boundingBox;
}

inline CDSC_ORIENTATION_ENUM KPSWidget::orientation() const
{
    return _orientation;
}

/**
 * @page ghostview_interface Ghostview interface to Ghostscript
 *
 * When the GHOSTVIEW environment variable is set, Ghostscript draws on
 * an existing drawable rather than creating its own window. Ghostscript
 * can be directed to draw on either a window or a pixmap.
 *
 * 
 * @section window Drawing on a Window
 *
 * The GHOSTVIEW environment variable contains the window id of the target
 * window.  The window id is an integer.  Ghostscript will use the attributes
 * of the window to obtain the width, height, colormap, screen, and visual of
 * the window. The remainder of the information is gotten from the GHOSTVIEW
 * property on that window.
 *
 * 
 * @section pixmap Drawing on a Pixmap
 *
 * The GHOSTVIEW environment variable contains a window id and a pixmap id.
 * They are integers separated by white space.  Ghostscript will use the
 * attributes of the window to obtain the colormap, screen, and visual to use.
 * The width and height will be obtained from the pixmap. The remainder of the
 * information, is gotten from the GHOSTVIEW property on the window.  In this
 * case, the property is deleted when read.
 *
 * 
 * @section gv_variable The GHOSTVIEW environment variable
 *
 * @par parameters: 
 *     <tt> window-id [pixmap-id] </tt>
 *
 * @par scanf format: 
 *     @code "%d %d" @endcode
 *
 * @par Explanation of the parameters
 *     @li @e window-id: 
 *         tells Ghostscript where to:
 *             - read the GHOSTVIEW property
 *             - send events
 *         If pixmap-id is not present, Ghostscript will draw on this window.
 *
 *     @li @e pixmap-id: 
 *         If present, tells Ghostscript that a pixmap will be used as the 
 *         final destination for drawing. The window will not be touched for 
 *         drawing purposes.
 *
 *
 * @section gv_property The GHOSTVIEW property
 *
 * @par type: 
 *     STRING
 *
 * @par parameters: 
 *     <tt> bpixmap orient llx lly urx ury xdpi ydpi [left bottom top right] 
 *     </tt>
 *
 * @par scanf format: 
 *     @code "%d %d %d %d %d %d %f %f %d %d %d %d" @endcode
 *
 * @par Explanation of the parameters
 *     @li @e bpixmap:
 *         pixmap id of the backing pixmap for the window. If no pixmap is to
 *         be used, this parameter should be zero. This parameter must be zero
 *         when drawing on a pixmap.
 *
 *     @li <em>orient:</em> 
 *         orientation of the page. The number represents clockwise rotation 
 *         of the paper in degrees.  Permitted values are 0, 90, 180, 270.
 *
 *     @li <em>llx, lly, urx, ury:</em>
 *         Bounding box of the drawable. The bounding box is specified in 
 *         PostScript points in default user coordinates. (Note the word 
 *         @e drawable. This means that this bounding box is generally not 
 *         the same as the BoundingBox specified in the DSC. In case 
 *         DocumentMedia is specified, it is equal to the Media's bounding 
 *         box.)
 *
 *     @li <em>xdpi, ydpi:</em> 
 *         Resolution of window. (This can be derived from the other 
 *         parameters, but not without roundoff error. These values are 
 *         included to avoid this error.)
 *
 *     @li <em>left, bottom, top, right (optional):</em>
 *         Margins around the window. The margins extend the imageable area 
 *         beyond the boundaries of the window. This is primarily used for 
 *         popup zoom windows. I have encountered several instances of 
 *         PostScript programs that position themselves with respect to the 
 *         imageable area. The margins are specified in PostScript points.  
 *         If omitted, the margins are assumed to be 0.
 *
 * 
 * @section events Events from Ghostscript
 *
 * If the final destination is a pixmap, the client will get a property 
 * notify event when Ghostscript reads the GHOSTVIEW property causing it to 
 * be deleted.
 *
 * Ghostscript sends events to the window where it read the GHOSTVIEW 
 * property. These events are of type ClientMessage.  The message_type is set 
 * to either PAGE or DONE.  The first long data value gives the window to be 
 * used to send replies to Ghostscript.  The second long data value gives the 
 * primary drawable. If rendering to a pixmap, it is the primary drawable. 
 * If rendering to a window, the backing pixmap is the primary drawable. If 
 * no backing pixmap is employed, then the window is the primary drawable. 
 * This field is necessary to distinguish multiple Ghostscripts rendering to 
 * separate pixmaps where the GHOSTVIEW property was placed on the same 
 * window.
 *
 * The PAGE message indicates that a "page" has completed.  Ghostscript will
 * wait until it receives a ClientMessage whose message_type is NEXT before
 * continuing.
 *
 * The DONE message indicates that Ghostscript has finished processing.
 */

#endif // __KPSWIDGET_H__


// vim:sw=4:sts=4:ts=8:noet
