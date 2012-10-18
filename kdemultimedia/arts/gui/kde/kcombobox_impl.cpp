    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de
                  2001, 2002 Matthias Kretz
                             kretz@kde.org

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

#include "kcombobox_impl.h"
#include "kcombobox_impl.moc"
#include "anyref.h"
#include "stdio.h"

using namespace Arts;
using namespace std;

ComboBoxIntMapper::ComboBoxIntMapper(KComboBox_impl *impl, KComboBox *co)
	: QObject( co, "map Qt signal to aRts" )
	,impl(impl)
{
	connect(co, SIGNAL(activated(const QString &)), this, SLOT(activated(const QString &)));
}

void ComboBoxIntMapper::activated(const QString & newValue)
{
	impl->value(string(newValue.utf8().data()));
}

KComboBox_impl::KComboBox_impl( KComboBox * widget )
	: KWidget_impl( widget ? widget : new KComboBox )
{
	_kcombobox = static_cast<KComboBox*>( _qwidget );
	( void )new ComboBoxIntMapper( this, _kcombobox );
}

string KComboBox_impl::caption()
{
	return m_caption.utf8().data();
}

void KComboBox_impl::caption(const string& newCaption)
{
	m_caption = QString::fromUtf8(newCaption.c_str());
	// FIXME: do something with the caption here
}

vector<string> * KComboBox_impl::choices()
{
	return new vector<string>(m_choices);
}

void KComboBox_impl::choices(const vector<string> & newChoices)
{
	if(newChoices != m_choices)
	{
		m_choices = newChoices;
		_kcombobox->clear();
		for(vector<string>::const_iterator it = m_choices.begin(); it != m_choices.end(); ++it)
		{
			_kcombobox->insertItem(QString::fromUtf8(it->c_str()));
		}
		if(visible())
			choices_changed(newChoices);
	}
}

string KComboBox_impl::value()
{
	return m_value.utf8().data();
}

void KComboBox_impl::value(const string & newValue)
{
	if(newValue != m_value.utf8().data())
	{
		m_value = QString::fromUtf8(newValue.c_str());
		for(unsigned int i = 0; i < m_choices.size(); ++i)
		{
			if(m_choices[i] == newValue)
				_kcombobox->setCurrentItem(i);
		}
		if(visible())
			value_changed(newValue);
	}
}

REGISTER_IMPLEMENTATION(KComboBox_impl);

// vim: sw=4 ts=4
