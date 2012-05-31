/*

	Copyright (C) 2001 Hans Meine <hans_meine@gmx.net>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include "propertypanelbase.h"
#include "qpalette.h"

#include <vector>
#include <list>
#include <string>

class StructureComponent;
class ModulePort;

class PropertyPanel: public PropertyPanelBase
{
	Q_OBJECT
	
public:
	PropertyPanel( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );

public slots:
	void setSelectedComponent( StructureComponent *component );
	void setSelectedPort( ModulePort *port );

protected slots:
	void setTitleFont();
	void setTitleColors();
	
	void pvModeChanged( int newMode );
	void comboPortSelected( int number );
	void connectButtonClicked();
	
	void writePortProperties(); // default reread-> true
	void writePortProperties( bool reread );
	void rereadPortProperties();

	bool eventFilter( QObject *, QEvent * );

signals:
	void startConnection( ModulePort * );
	void portSelected( ModulePort * );
	void portPropertiesChanged( ModulePort * ); // might be emitted a little bit too often (see writePortP..)
	
protected:
	enum ConnType { ctNone, ctValue, ctConnection };
	void resizeEvent ( QResizeEvent * );
	
	StructureComponent *component;
	ModulePort *port;
	std::list<ModulePort *> modulePorts;

	QColorGroup highlightColorGroup( QColorGroup cg );
	bool isEnum(const std::string& type);
	void fillEnumChoices(const std::string& type);
	long selectedEnumValue(const std::string& type);
	int findEnumIndex(const std::string& type, long value);
};

#endif // PROPERTYPANEL_H
