/*
	Copyright (C) 2000 Stefan Westerfeld <stefan@space.twc.de>
	              2001 Charles Samuels <charles@kde.org>
                  2002 Matthias Kretz <kretz@kde.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public License
	along with this library; see the file COPYING.LIB.  If not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.

*/

#include "klineedit_impl.h"
#include "klineedit_impl.moc"

using namespace Arts;
using namespace std;

KLineEditStringMapper::KLineEditStringMapper(KLineEdit_impl *impl, KLineEdit *ke)
	:impl(impl)
{
	connect(ke, SIGNAL(textChanged(const QString&)),
			this, SLOT(textChanged(const QString&)));
}

void KLineEditStringMapper::textChanged(const QString& newText)
{
	impl->textChanged(newText.utf8().data());
}

KLineEdit_impl::KLineEdit_impl( KLineEdit * widget )
	: KWidget_impl( widget ? widget : new KLineEdit )
{
	_klineedit = static_cast<KLineEdit*>( _qwidget );
	( void )new KLineEditStringMapper( this, _klineedit );
}

string KLineEdit_impl::text()
{
	return _klineedit->text().utf8().data();
}

void KLineEdit_impl::text(const string& newText)
{
	_klineedit->setText(QString::fromUtf8(newText.c_str()));
}

void KLineEdit_impl::textChanged(const string& newText)
{
	text_changed(newText);
}

string KLineEdit_impl::caption()
{
	return ""; // FIXME
}

void KLineEdit_impl::caption(const string& /*newCaption*/)
{
	// FIXME
}

REGISTER_IMPLEMENTATION(KLineEdit_impl);

// vim: sw=4 ts=4
