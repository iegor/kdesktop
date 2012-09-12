/***************************************************************************
 *   Copyright (C) 2005 by Bastian Holst                                   *
 *   bastianholst@gmx.de                                                   *
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
#ifndef AMAROKCATALOG_H
#define AMAROKCATALOG_H

#include <katapultcatalog.h>
#include <kgenericfactory.h>
#include <kconfigbase.h>
#include "song.h"

class QWidget;
class QString;

/** 
@author Bastian Holst
*/
class AmarokCatalog : public KatapultCatalog
{
	Q_OBJECT
public:
	AmarokCatalog(QObject*, const char*, const QStringList&);
	virtual ~AmarokCatalog();
	
	unsigned int minQueryLen() const;
	void readSettings(KConfigBase *);
	void writeSettings(KConfigBase *);
	QWidget * configure();
	//virtual void initialize();
	
public slots:
	void minQueryLenChanged(int);
	
protected:
	void queryChanged();
	void reset();
	void checkCollectionType();	
	
private:
	unsigned int _minQueryLen;
	bool _dynamicCollection;
	bool _gotCollectionStatus;	
	Song _result;
};

#endif //AMAROkCATALOG_H
