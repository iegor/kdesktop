#ifndef THUMBNAILSERVICE_H
#define THUMBNAILSERVICE_H
/** 
 * Copyright (C) 1997-2003 the KGhostView authors. See file AUTHORS.
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

#include <set>
#include <qobject.h>
#include <qpixmap.h>
#include <qguardedptr.h>
#include <stdio.h>

class KPSWidget;
class KGVMiniWidget;
class QTimer;

class ThumbnailService : public QObject {
	Q_OBJECT
	public:
		ThumbnailService( KGVMiniWidget* parent, const char* name = 0 );
		~ThumbnailService();

	public slots:
		void delayedGetThumbnail( int page, QObject* receiver, const char* slot, bool urgent = false );
		/**
		 * Cancels the request matching the signature below.
		 *
		 * @param page the page of the request. Use "-1" for wildcard
		 * @param receiver the receiver. Use null for wildcard
		 * @param slot Use null for wildcard
		 */
		void cancelRequests( int page, QObject* receiver, const char* slot);
	
		void reset();
		/**
		 * This "suspends" the service.
		 * Unlike @ref reset(), if you call setEnabled( false ),
		 * old requests will be kept and will be serviced when
		 * you call setEnabled( true ) later.
		 */
		void setEnabled( bool );

	signals:
		/** 
		 * Don't connect to this directly
		 */
		void relayPixmap( QPixmap );

	private slots:
		void slotDone( QPixmap );
		void processOne();

	private:

	struct Request {
		Request( int p, QObject* r, const char* s ) :
		    page( p ), receiver( r ), slot( s ), urgent( false ) { }
		Request( int p, QObject* r, const char* s, bool u ) :
		    page( p ), receiver( r ), slot( s ), urgent( u ) { }

		int page;
		QObject* receiver;
		const char* slot;
		bool urgent;
		bool operator < ( Request ) const;
	};
	static bool pageCmp( Request, Request );
	std::set<Request> pending;
	QGuardedPtr<KPSWidget> _thumbnailDrawer;
	KGVMiniWidget* _mini;
	QTimer* timer_;
	bool _busy;
	bool _enabled;
};

// vim:sw=4:sts=4:ts=8:sta:tw=78:noet
#endif // THUMBNAILSERVICE_H

