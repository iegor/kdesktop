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
#include "propertypanel.h"
#include "propertypanel.moc"
#include "module.h"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlineedit.h>

#include <arts/debug.h>

PropertyPanel::PropertyPanel( QWidget* parent, const char* name, WFlags fl )
	: PropertyPanelBase( parent, name, fl ),
	  component(0L),
	  port(0L)
{
	setTitleFont();
	setTitleColors();
	connect( kapp, SIGNAL( kdisplayFontChanged() ),
			 this, SLOT( setTitleFont() ));
	connect( kapp, SIGNAL( kdisplayPaletteChanged() ),
			 this, SLOT( setTitleColors() ));
	connect( portValueGroup, SIGNAL( clicked(int) ),
			 this, SLOT( pvModeChanged(int) ));
	connect( constantValueEdit, SIGNAL( returnPressed() ),
			 this, SLOT( writePortProperties() ));
	connect( constantValueComboBox, SIGNAL( activated(int) ),
			 this, SLOT( writePortProperties() ));
	connect( portCombo, SIGNAL( activated(int) ),
			 this, SLOT( comboPortSelected(int) ));
	connect( connectButton, SIGNAL( clicked() ),
			 this, SLOT( connectButtonClicked() ));

	constantValueComboBox->hide();
	setEnabled( false );
	tipLabel->hide();
}

void PropertyPanel::setTitleFont()
{
	QFont titleFont = font();
	titleFont.setPointSizeFloat( titleFont.pointSizeFloat()*1.5f );
	moduleNameLabel->setFont( titleFont );
	titleFont = font();
	titleFont.setPointSizeFloat( titleFont.pointSizeFloat()*1.15f );
	selectedLabel->setFont( titleFont );
}

QColorGroup PropertyPanel::highlightColorGroup( QColorGroup cg )
{
	cg.setColor( QColorGroup::Foreground, cg.highlightedText() );
	cg.setColor( QColorGroup::Background, cg.highlight() );
	return cg;
}

void PropertyPanel::setTitleColors()
{
	QPalette palette = titleFrame->palette();
	palette.setActive( highlightColorGroup( palette.active() ) );
	palette.setInactive( highlightColorGroup( palette.inactive() ) );
	palette.setDisabled( highlightColorGroup( palette.disabled() ) );
	titleFrame->setPalette( palette );
}

void PropertyPanel::resizeEvent ( QResizeEvent * )
{
	if(width()*3 < height()*4)
		mainBoxLayout->setDirection( QBoxLayout::TopToBottom );
	else
		mainBoxLayout->setDirection( QBoxLayout::LeftToRight );
}

/*************************************************************/

void PropertyPanel::setSelectedComponent( StructureComponent *component )
{
	//kdDebug() << QString("PropertyPanel::setSelectedComponent ") << component << endl;
	if( this->component == component ) return;

	this->component = component;
	portCombo->clear();
	modulePorts.clear();
	if(!component)
		setEnabled(false);
	else
	{
		moduleNameLabel->setText( component->name() );
		if(component->pixmap())
		{
			moduleIconLabel->setPixmap( *component->pixmap() );
			moduleIconLabel->setMinimumSize( moduleIconLabel->sizeHint() );
			moduleIconLabel->show();
		}
		else
			moduleIconLabel->hide();

		component->dumpPorts(modulePorts);

		// fill combobox
		for(std::list<ModulePort *>::iterator it = modulePorts.begin();
			 it != modulePorts.end(); it++)
		{
			QString portTitle = (*it)->description
				+ QString(" (%1)").arg( (*it)->PortDesc.type().direction == Arts::output ?
										i18n("OUTPUT") : i18n("INPUT") );
			portCombo->insertItem( portTitle );
		}
		//portCombo->setMinimumSize( portCombo->sizeHint() );
		setEnabled(true);
	}
}

void PropertyPanel::setSelectedPort( ModulePort *port )
{
	if( this->port == port ) return;

//	if( this->port ) writePortProperties( false ); // rereading not necessary here

	this->port = port;
	if(port)
	{
		setSelectedComponent( port->owner );
		rereadPortProperties();
		portValueGroup->setEnabled( true );
		QString tipText = i18n("Tip: Just typing numbers or alphabetic characters starts entering constant values.");
		tipLabel->setText( tipText );
		tipLabel->show();

		// select port in combobox - bah, should be easier ;-)
		portCombo->setCurrentItem(
			portCombo->listBox()->index(
				portCombo->listBox()->findItem( port->description + " (" ) ) );
	}
	else
	{
		portValueGroup->setEnabled( false );
		tipLabel->hide();
	}
}

void PropertyPanel::pvModeChanged( int newMode )
{
	if(!port) return; // sanity check

	switch( newMode )
	{
	case 0:
	case 1:
		writePortProperties();
		break;
	case 2:
		; // connection should already be established
	}
}

void PropertyPanel::comboPortSelected( int number )
{
	if(!component) return;

	std::list<ModulePort *>::iterator it;
	for( it = modulePorts.begin();
		  (it != modulePorts.end()) && number > 0; it++, number--)
		;
	setSelectedPort( *it );
	emit portSelected( *it );
}

void PropertyPanel::connectButtonClicked()
{
	if(!port) return; // sanity check

	pvConnectionButton->setChecked( true );
	emit startConnection( port );
}

// this is a slot connected to changed() of the editing stuff
void PropertyPanel::writePortProperties()
{
	writePortProperties( true );
}

void PropertyPanel::writePortProperties( bool reread )
{
	//kdDebug() << QString("PropertyPanel::writePortProperties") << endl;
	if(!port) return; // sanity check

	bool dirty = false;

	if(!pvConnectionButton->isChecked())
	{
		if(port->PortDesc.isConnected())
		{
			port->PortDesc.disconnectAll();
			dirty = true;
		}
	}

	if(pvNotSetButton->isChecked() &&
		(port->PortDesc.isConnected() || port->PortDesc.hasValue()))
	{
		port->PortDesc.hasValue(false);
		dirty = true;
	}

	if(pvConstantButton->isChecked())
	{
		std::string type = port->PortDesc.type().dataType;
		QString newvalue = constantValueEdit->text();

		Arts::Any a;
		a.type = type;
		Arts::Buffer b;
		if(type == "float")
			b.writeFloat(newvalue.toFloat());
		else if(type == "long")
			b.writeLong(newvalue.toLong());
		else if(type == "string")
			b.writeString(newvalue.local8Bit().data());
		else if(type == "boolean")
		{
			b.writeBool(newvalue.upper() == "TRUE" || newvalue.upper() == "T"
						|| newvalue == "1");
		}
		else if(isEnum(type))
		{
			b.writeLong(selectedEnumValue(type));
		}

		if(b.size() > 0)
		{
			b.read(a.value, b.size());
			port->PortDesc.value(a);
			dirty = true;
		}
	}

	if( dirty )
		emit portPropertiesChanged( port );

	if( reread )
		rereadPortProperties();
}

void PropertyPanel::rereadPortProperties()
{
	//kdDebug() << QString("PropertyPanel::rereadPortProperties") << endl;
	if(!port) return; // sanity check

	std::string dataType = port->PortDesc.type().dataType;
	if(isEnum(dataType))
	{
		constantValueEdit->hide();
		constantValueComboBox->show();

		fillEnumChoices(dataType);
	}
	else
	{
		constantValueEdit->show();
		constantValueComboBox->hide();
	}

	if( port->PortDesc.hasValue() )
	{
		pvConstantButton->setChecked( true );

		QString constValue;

		Arts::Any value = port->PortDesc.value();
		Arts::Buffer b;
		b.write(value.value);

		if(isEnum(value.type))
		{
			long v = b.readLong();
			constantValueComboBox->setCurrentItem(findEnumIndex(value.type,v));
		}
		else
		{
			if(value.type == "float")
				constValue.sprintf("%2.4f", b.readFloat());
			else if(value.type == "long")
				constValue.sprintf("%ld", b.readLong());
			else if(value.type == "string")
			{
				std::string s;
				b.readString(s);
				constValue = s.c_str();
			}
			else if(value.type == "boolean")
			{
				if(b.readBool())
					constValue = "true";
				else
					constValue = "false";
			}
			else constValue = ("*unknown type* " + value.type).c_str();

			constantValueEdit->setText( constValue );
		}
	}
	else if( port->PortDesc.isConnected() )
		pvConnectionButton->setChecked( true );
	else
	{
		pvNotSetButton->setChecked( true );
		constantValueEdit->clear();
	}

	pvConnectionButton->setEnabled( port->PortDesc.isConnected() );
}

bool PropertyPanel::eventFilter( QObject *o, QEvent *e )
{
	if( port && !constantValueEdit->hasFocus() && (e->type() == QEvent::KeyPress) ) {
//		kdDebug() << QString("  ..is KeyPress") << endl;
		QString entered = static_cast<QKeyEvent *>(e)->text();
		bool goodString = entered.length() > 0;

//		kdDebug() << QString("pressed '%1'").arg(entered) << endl;

		for( unsigned int i = 0; i < entered.length(); i++)
			goodString = goodString && entered[i].isLetterOrNumber();

		if( goodString )
		{
			pvConstantButton->setChecked( true );
			constantValueEdit->setText( entered );
			constantValueEdit->setFocus();
//			kdDebug() << "keyPress used in propPanel" << endl;
            return TRUE; // eat event
		}
	}
//	else
//		kdDebug() << "event type = " << e->type() << " != " << QEvent::KeyPress << endl;
	return FALSE; // PropertyPanelBase::eventFilter( o, e );
}

bool PropertyPanel::isEnum(const std::string& type)
{
	Arts::InterfaceRepoV2 interfaceRepo = Arts::DynamicCast(Arts::Dispatcher::the()->interfaceRepo());
	return (interfaceRepo.identifyType(type) == Arts::tiEnum);
}

void PropertyPanel::fillEnumChoices(const std::string& type)
{
	Arts::EnumDef edef =
		Arts::Dispatcher::the()->interfaceRepo().queryEnum(type);

	constantValueComboBox->clear();

	std::vector<Arts::EnumComponent>::const_iterator eci;
	for(eci = edef.contents.begin(); eci != edef.contents.end(); ++eci)
		constantValueComboBox->insertItem(QString::fromUtf8(eci->name.c_str()));
}

long PropertyPanel::selectedEnumValue(const std::string& type)
{
	unsigned int i = (unsigned int)constantValueComboBox->currentItem();
	Arts::EnumDef edef =
		Arts::Dispatcher::the()->interfaceRepo().queryEnum(type);

	if(i >= edef.contents.size()) i = 0;
	if(edef.contents.size() == 0) return 0;
	return edef.contents[i].value;
}

int PropertyPanel::findEnumIndex(const std::string& type, long value)
{
	Arts::EnumDef edef =
		Arts::Dispatcher::the()->interfaceRepo().queryEnum(type);

	for(unsigned int i = 0; i < edef.contents.size(); i++)
		if(edef.contents[i].value == value)
			return i;
	return 0;
}
