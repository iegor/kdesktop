    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
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

#ifndef ARTS_GUI_KCOMBOBOX_IMPL_H
#define ARTS_GUI_KCOMBOBOX_IMPL_H
#include "kwidget_impl.h"
#include <kcombobox.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>


namespace Arts {

class KComboBox_impl;
class ComboBoxIntMapper :public QObject {
	Q_OBJECT
	KComboBox_impl *impl;
public:
	ComboBoxIntMapper(KComboBox_impl *impl, KComboBox *co);
public slots:
	void activated(const QString &);
};

class KComboBox_impl : virtual public Arts::ComboBox_skel,
                       public Arts::KWidget_impl
{
protected:
	KComboBox * _kcombobox;
	QString m_caption;
	QString m_value;
    std::vector<std::string> m_choices;

	void applyValue();

public:
	KComboBox_impl( KComboBox * w = 0 );
	void constructor( Widget p ) { parent( p ); }

	std::string caption();
	void caption(const std::string & newCaption);

	std::vector<std::string> * choices();
	void choices(const std::vector<std::string> & newChoices);

	std::string value();
	void value(const std::string & newValue);
};

}
#endif /* ARTS_GUI_KCOMBOBOX_IMPL_H */

// vim: sw=4 ts=4
