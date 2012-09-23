/** 
 * Copyright (C) 2000-2002 the KGhostView authors. See file AUTHORS.
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

#ifndef __KGVSHELL_H__
#define __KGVSHELL_H__

#include <qstring.h>

#include <kparts/mainwindow.h>

class QTimer;

class KRecentFilesAction;
class ScrollBox;
class KGVPart;
class KAction;
class KConfig;
class KTempFile;
class KPopupMenu;
class DisplayOptions;
class FullScreenFilter;

class KDE_EXPORT KGVShell : public KParts::MainWindow
{
    Q_OBJECT

public:
    KGVShell();
    virtual ~KGVShell();

public slots:
    void openURL( const KURL& url );
    void openStdin();
    void setDisplayOptions( const DisplayOptions& );
    void slotRMBClick();

protected slots:
    void slotFileOpen();
    void slotShowMenubar();
    void slotQuit();
    void slotMaximize();
    void slotResize();
    void slotUpdateFullScreen();
    void slotReset();
    void slotDocumentState();
    void slotConfigureToolbars();
    void slotNewToolbarConfig();

protected:
    // session management
    virtual void saveProperties( KConfig *config );
    virtual void readProperties( KConfig *config );
    
    void readSettings();
    void writeSettings();
    void enableStateDepActions( bool enable );
    void setFullScreen( bool );

private:
    
    friend class FullScreenFilter;
    
    KGVPart* m_gvpart;
    QString cwd;

    KAction* openact;
    KToggleAction* _showMenuBarAction;
    KToggleAction* m_fullScreenAction;
    FullScreenFilter* m_fsFilter;
    KPopupMenu* _popup;
    KRecentFilesAction* recent;
    KTempFile* _tmpFile; // Used for storing data received from stdin
};

#endif

// vim:sw=4:sts=4:ts=8:noet
