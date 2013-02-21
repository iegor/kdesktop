// $Id: OptionsDlg.cpp 465369 2005-09-29 14:33:08Z mueller $
//
// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdlib.h>

#include "Options.h"
#include "OptionsDlg.h"
#include "OptionsDlgWidget.h"

#include <qcheckbox.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>

#include "OptionsDlg.moc"
OptionsDlg::OptionsDlg( QWidget* parent, const char* name )
        : KDialogBase( Swallow, i18n ("Options"), Ok | Apply | Cancel, 
		Ok, parent, name, true, true ), apply (0)
{
	_baseWidget = new OptionsDlgWidget ( 0 );
	setMainWidget (_baseWidget);
	
	connect( _baseWidget, SIGNAL( valueChanged () ), this, SLOT( slotChanged() ) );

    connect( this, SIGNAL( okClicked () ), this, SLOT( slotOK() ) );
    connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
    connect( this, SIGNAL( cancelClicked() ), this, SLOT( slotCancel() ) );

    int size = Options::instance()->getDefaultTapeSize();
    if ( ( size >= 1024*1024 ) && ( size % ( 1024*1024 ) == 0 ) ) {
        // GB
        size /= 1024*1024;
        _baseWidget->_defaultTapeSizeUnits->setCurrentItem( 1 );
    } else {
        // MB
        size /= 1024;
        _baseWidget->_defaultTapeSizeUnits->setCurrentItem( 0 );
    }
    _baseWidget->_defaultTapeSize->setValue( size );

    _baseWidget->_tapeBlockSize->setValue( Options::instance()->getTapeBlockSize() );

    _baseWidget->_tapeDevice->setText( Options::instance()->getTapeDevice() );
    _baseWidget->_tarCommand->setText( Options::instance()->getTarCommand() );
    _baseWidget->_loadOnMount->setChecked( Options::instance()->getLoadOnMount() );
    _baseWidget->_lockOnMount->setChecked( Options::instance()->getLockOnMount() );
    _baseWidget->_ejectOnUnmount->setChecked( Options::instance()->getEjectOnUnmount() );
    _baseWidget->_variableBlockSize->setChecked( Options::instance()->getVariableBlockSize() );

    enableButtonApply ( false );
    configChanged = false;
}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::slotChanged()
{
    enableButtonApply ( true );
    configChanged = true;
}

void OptionsDlg::slotOK()
{
    if( configChanged )
        slotApply();
    accept();
}

void OptionsDlg::slotApply()
{
    int size = _baseWidget->_defaultTapeSize->value();
    if ( _baseWidget->_defaultTapeSizeUnits->currentItem() == 0 ) {
        // MB
        size *= 1024;
    } else {
        // GB
        size *= 1024*1024;
    }
    Options::instance()->setDefaultTapeSize( size );

    Options::instance()->setTapeBlockSize( _baseWidget->_tapeBlockSize->value() );

    Options::instance()->setTapeDevice( _baseWidget->_tapeDevice->text() );

    Options::instance()->setTarCommand( _baseWidget->_tarCommand->text() );

    Options::instance()->setLoadOnMount( _baseWidget->_loadOnMount->isChecked() );

    Options::instance()->setLockOnMount( _baseWidget->_lockOnMount->isChecked() );

    Options::instance()->setEjectOnUnmount( _baseWidget->_ejectOnUnmount->isChecked() );

    Options::instance()->setVariableBlockSize( _baseWidget->_variableBlockSize->isChecked() );

    Options::instance()->sync();
    enableButtonApply( false );
    configChanged = false;
}

void OptionsDlg::slotCancel()
{
    reject();
}
