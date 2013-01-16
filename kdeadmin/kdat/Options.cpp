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

#include <kapplication.h>
#include <kconfig.h>

#include "Options.h"

#include "Options.moc"

Options* Options::_instance = 0;

Options::Options()
{
    _config = KApplication::kApplication()->config();

    _defaultTapeSize   = _config->readNumEntry( "defaultTapeSize", 2*1024*1024 );
    _tapeBlockSize     = _config->readNumEntry( "tapeBlockSize", 20*512 );  // Use tar default.
    _tapeDevice        = _config->readEntry( "tapeDevice", "/dev/tape" ).copy();
    _tarCommand        = _config->readPathEntry( "tarCommand", "tar" ).copy();
    _loadOnMount       = _config->readNumEntry( "loadOnMount", FALSE );
    _lockOnMount       = _config->readNumEntry( "lockOnMount", FALSE );
    _ejectOnUnmount    = _config->readNumEntry( "ejectOnUnmount", FALSE );
    _variableBlockSize = _config->readNumEntry( "variableBlockSize", FALSE );
}

Options* Options::instance()
{
    if ( _instance == 0 ) {
        _instance = new Options();
    }
    return _instance;
}

void Options::sync()
{
    _config->sync();
}

int Options::getDefaultTapeSize()
{
    return _defaultTapeSize;
}

int Options::getTapeBlockSize()
{
    return _tapeBlockSize;
}

QString Options::getTapeDevice()
{
    return _tapeDevice;
}

QString Options::getTarCommand()
{
    return _tarCommand;
}

bool Options::getLoadOnMount()
{
    return _loadOnMount;
}

bool Options::getLockOnMount()
{
    return _lockOnMount;
}

bool Options::getEjectOnUnmount()
{
    return _ejectOnUnmount;
}

bool Options::getVariableBlockSize()
{
    return _variableBlockSize;
}

void Options::setDefaultTapeSize( int kbytes )
{
    if ( kbytes != getDefaultTapeSize() ) {
        _defaultTapeSize = kbytes;
        _config->writeEntry( "defaultTapeSize", _defaultTapeSize );
        emit sigDefaultTapeSize();
    }
}

void Options::setTapeBlockSize( int bytes )
{
    if ( bytes != getTapeBlockSize() ) {
        _tapeBlockSize = bytes;
        _config->writeEntry( "tapeBlockSize", _tapeBlockSize );
        emit sigTapeBlockSize();
    }
}

void Options::setTapeDevice( const QString & str )
{
    if ( getTapeDevice() != str ) {
        _tapeDevice = str;
        _config->writeEntry( "tapeDevice", _tapeDevice );
        emit sigTapeDevice();
    }
}

void Options::setTarCommand( const QString & str )
{
    if ( getTarCommand() != str ) {
        _tarCommand = str;
        _config->writePathEntry( "tarCommand", _tarCommand );
        emit sigTarCommand();
    }
}

void Options::setLoadOnMount( bool b )
{
    if ( getLoadOnMount() != b ) {
        _loadOnMount = b;
        _config->writeEntry( "loadOnMount", _loadOnMount );
        emit sigLoadOnMount();
    }
}

void Options::setLockOnMount( bool b )
{
    if ( getLockOnMount() != b ) {
        _lockOnMount = b;
        _config->writeEntry( "lockOnMount", _lockOnMount );
        emit sigLockOnMount();
    }
}

void Options::setEjectOnUnmount( bool b )
{
    if ( getEjectOnUnmount() != b ) {
        _ejectOnUnmount = b;
        _config->writeEntry( "ejectOnUnmount", _ejectOnUnmount );
        emit sigEjectOnUnmount();
    }
}

void Options::setVariableBlockSize( bool b )
{
    if ( getVariableBlockSize() != b ) {
        _variableBlockSize = b;
        _config->writeEntry( "variableBlockSize", _variableBlockSize );
        emit sigVariableBlockSize();
    }
}
