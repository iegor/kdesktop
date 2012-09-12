/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KATAPULTCATALOG_H
#define KATAPULTCATALOG_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

#include "match.h"

class QWidget;
class KConfigBase;

/**
@author Joe Ferris
*/
class KatapultCatalog : public QObject {
	Q_OBJECT

public:
	KatapultCatalog();
	virtual ~KatapultCatalog();
	
	void setQuery(QString);
	
	int status() const;
	Match bestMatch() const;
	
	virtual void initialize();
	virtual void readSettings(KConfigBase *);
	virtual void writeSettings(KConfigBase *);
	virtual QWidget * configure();
	
protected:
	QString query() const;
	
	void setStatus(int);
	void setBestMatch(Match);
	
	virtual void queryChanged();

private:
	QString _query;
	int _status;
	Match _bestMatch;

};

#endif
