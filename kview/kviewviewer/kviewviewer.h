/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
                  2001-2002 Matthias Kretz <kretz@kde.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#ifndef __kviewviewer_h__
#define __kviewviewer_h__

#include "kimageviewer/viewer.h"
#include "kviewkonqextension.h"

#include <qvaluevector.h>
#include "kviewvieweriface.h"
#include <kdemacros.h>

namespace KImageViewer { class Canvas; }
namespace KIO { class Job; }

class KTempFile;
class KAction;
class KActionMenu;
class KToggleAction;
class KSelectAction;
class KAboutData;
class QBuffer;
class QSize;
class KDirWatch;
template<class T>
class QCache;

class KDE_EXPORT KViewViewer : public KImageViewer::Viewer, public KViewViewerIface
{
	Q_OBJECT
	friend class KViewKonqExtension;

	public:
		KViewViewer( QWidget * parentWidget, const char * widgetName,
				QObject * parent, const char * name, const QStringList & );
		virtual ~KViewViewer();

		KImageViewer::Canvas * canvas() const { return m_pCanvas; }
		static KAboutData * createAboutData();
		virtual void setReadWrite( bool readwrite = true );
		KParts::BrowserExtension * browserExtension() const { return m_pExtension; }

		bool saveAs( const KURL & );
		void setModified( bool );

	public slots:
		virtual bool openURL( const KURL & );
		virtual bool closeURL();
		virtual void newImage( const QImage & );
		virtual void reload();

	protected:
		bool eventFilter( QObject *, QEvent * ); // for DnD
		void abortLoad();
		virtual bool openFile();
		virtual bool saveFile();

		virtual void setupActions();
		void guiActivateEvent( KParts::GUIActivateEvent * );

		void writeSettings();

	protected slots:
		void readSettings();
		void zoomChanged( double );
		void slotJobFinished( KIO::Job * );
		void slotData( KIO::Job *, const QByteArray & );

		void slotSave();
		void slotSaveAs();
		void slotZoomIn();
		void slotZoomOut();
		void setZoom( const QString & );
		void updateZoomMenu( double zoom );
		void slotFlipH();
		void slotFlipV();
		void slotRotateCCW();
		void slotRotateCW();
		void slotFitToWin();
		void slotDel();

		void slotPopupMenu( const QPoint & );
		void slotResultSaveAs( KIO::Job * );

		void slotFileDirty( const QString & );
		void slotReloadUnmodified();

		void slotToggleScrollbars();

		void loadPlugins();

		void switchBlendEffect();
		void hasImage( bool );

	private:
		QWidget * m_pParentWidget;
		KIO::Job * m_pJob;
		KViewKonqExtension * m_pExtension;
		KImageViewer::Canvas * m_pCanvas;
		KTempFile * m_pTempFile;
		QBuffer * m_pBuffer;
		KDirWatch * m_pFileWatch;

		// Actions:
		KAction * m_paZoomIn;
		KAction * m_paZoomOut;
		KSelectAction * m_paZoom;
		KActionMenu * m_paFlipMenu;
		KAction * m_paFlipH;
		KAction * m_paFlipV;
		KAction * m_paRotateCCW;
		KAction * m_paRotateCW;
		KAction * m_paSave;
		KAction * m_paSaveAs;
		KAction * m_paFitToWin;
		KToggleAction * m_paShowScrollbars;

		QString m_popupDoc;
		QString m_mimeType;
		QString m_newMimeType;
		QString m_sCaption;
		
		QValueVector<unsigned int> m_vEffects;
};

// vim:sw=4:ts=4

#endif
