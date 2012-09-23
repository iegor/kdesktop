/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This document is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This document is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this document; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DOCUMENTCATALOG_H
#define DOCUMENTCATALOG_H

#include <kgenericfactory.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qdir.h>

#include "katapultcatalog.h"

class QWidget;
class Document;

/**
@author Joe Ferris
*/
class DocumentCatalog : public KatapultCatalog
{
	Q_OBJECT
public:
	DocumentCatalog(QObject *, const char *, const QStringList&);
	virtual ~DocumentCatalog();

	virtual void initialize();
	virtual void queryChanged();
	virtual void readSettings(KConfigBase *);
	virtual void writeSettings(KConfigBase *);
	virtual unsigned int minQueryLen() const;
	virtual QWidget * configure();
	
public slots:
	void minQueryLenChanged(int);
	void toggleshowPreview(bool);
	
private:
	void refreshFolders();
	void refreshFiles();

	QPtrList<Document> files;
	Document *currentDirDoc;
	QStringList folders;
	QString currentPath;
	QDir dir;
	bool filesListed;
	int _minQueryLen;
	int queryMatched;
	bool _showPreview;
};

#endif
