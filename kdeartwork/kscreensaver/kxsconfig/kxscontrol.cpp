//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <qlabel.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qxml.h>
#include <klocale.h>
#include <kfiledialog.h>
#include "kxscontrol.h"

//===========================================================================
KXSRangeControl::KXSRangeControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSRangeItem(name, config), mSlider(0), mSpinBox(0)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mSlider = new QSlider(mMinimum, mMaximum, 10, mValue, Qt::Horizontal, this);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

KXSRangeControl::KXSRangeControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSRangeItem(name, attr), mSlider(0), mSpinBox(0)
{
    if (attr.value("type") == "spinbutton" ) {
	QHBoxLayout *hb = new QHBoxLayout(this);
	if (!mLabel.isEmpty()) {
            QLabel *l = new QLabel(i18n(mLabel.utf8()), this);
	    hb->add(l);
	}
	mSpinBox = new QSpinBox(mMinimum, mMaximum, 1, this);
	mSpinBox->setValue(mValue);
	connect(mSpinBox, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
	hb->add(mSpinBox);
    } else {
	QString lowLabel = attr.value("_low-label");
	QString highLabel = attr.value("_high-label");
	QVBoxLayout *vb = new QVBoxLayout(this);
	if (!mLabel.isEmpty()) {
            QLabel *l = new QLabel(i18n(mLabel.utf8()), this);
	    vb->add(l);
	}
	QHBoxLayout *hb = new QHBoxLayout(vb);
	if (!lowLabel.isEmpty()) {
            QLabel *l = new QLabel(i18n(lowLabel.utf8()), this);
	    hb->addWidget(l);
	}
	mSlider = new QSlider(mMinimum, mMaximum, 10, mValue, Qt::Horizontal, this);
	connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
	hb->add(mSlider);
	if (!highLabel.isEmpty()){
            QLabel *l = new QLabel(i18n(highLabel.utf8()), this);
	    hb->addWidget(l);
	}
    }
}

void KXSRangeControl::slotValueChanged(int value)
{
  mValue = value;
  emit changed();
}

void KXSRangeControl::read(KConfig &config)
{
    KXSRangeItem::read(config);
    if ( mSpinBox )
	mSpinBox->setValue(mValue);
    else
	mSlider->setValue(mValue);
}

//===========================================================================
KXSDoubleRangeControl::KXSDoubleRangeControl(QWidget *parent,
                                  const QString &name, KConfig &config)
  : QWidget(parent), KXSDoubleRangeItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);

  int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));

  mSlider = new QSlider(0, 100, 10, value, Qt::Horizontal, this);
  connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
  l->add(mSlider);
}

KXSDoubleRangeControl::KXSDoubleRangeControl(QWidget *parent,
                                  const QString &name, const QXmlAttributes &attr)
  : QWidget(parent), KXSDoubleRangeItem(name, attr)
{
    QString lowLabel = attr.value("_low-label");
    QString highLabel = attr.value("_high-label");
    QVBoxLayout *vb = new QVBoxLayout(this);
    if (!mLabel.isEmpty()) {
        QLabel *l = new QLabel(i18n(mLabel.utf8()), this);
	vb->add(l);
    }
    QHBoxLayout *hb = new QHBoxLayout(vb);
    if (!lowLabel.isEmpty()) {
        QLabel *l = new QLabel(i18n(lowLabel.utf8()), this);
	hb->addWidget(l);
    }
    int value = int((mValue - mMinimum) * 100 / (mMaximum - mMinimum));
    mSlider = new QSlider(0, 100, 10, value, Qt::Horizontal, this);
    connect(mSlider, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
    hb->add(mSlider);
    if (!highLabel.isEmpty()){
        QLabel *l = new QLabel(i18n(highLabel.utf8()), this);
	hb->addWidget(l);
    }
}

void KXSDoubleRangeControl::slotValueChanged(int value)
{
  mValue = mMinimum + value * (mMaximum - mMinimum) / 100.0;
  emit changed();
}

void KXSDoubleRangeControl::read(KConfig &config)
{
    KXSDoubleRangeItem::read(config);
    mSlider->setValue((int)((mValue - mMinimum) * 100.0 /
                            (mMaximum - mMinimum) + 0.5));
}

//===========================================================================
KXSCheckBoxControl::KXSCheckBoxControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QCheckBox(parent), KXSBoolItem(name, config)
{
  setText(mLabel);
  setChecked(mValue);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

KXSCheckBoxControl::KXSCheckBoxControl(QWidget *parent, const QString &name,
                                      const QXmlAttributes &attr)
  : QCheckBox(parent), KXSBoolItem(name, attr)
{
  setText(i18n(mLabel.utf8()));
  setChecked(mValue);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

void KXSCheckBoxControl::slotToggled(bool state)
{
  mValue = state;
  emit changed();
}

void KXSCheckBoxControl::read(KConfig &config)
{
    KXSBoolItem::read(config);
    setChecked(mValue);
}

//===========================================================================
KXSDropListControl::KXSDropListControl(QWidget *parent, const QString &name,
                                      KConfig &config)
  : QWidget(parent), KXSSelectItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mCombo = new QComboBox(this);
  for(uint i=0; i < mOptions.count(); i++)
      mCombo->insertItem( i18n(mOptions[i].utf8()) );
  mCombo->setCurrentItem(mValue);
  connect(mCombo, SIGNAL(activated(int)), SLOT(slotActivated(int)));
  l->add(mCombo);
}

KXSDropListControl::KXSDropListControl(QWidget *parent, const QString &name,
                                      const QXmlAttributes &attr)
  : QWidget(parent), KXSSelectItem(name, attr)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  mCombo = new QComboBox(this);
  connect(mCombo, SIGNAL(activated(int)), SLOT(slotActivated(int)));
  l->add(mCombo);
}

void KXSDropListControl::addOption(const QXmlAttributes &attr)
{
    KXSSelectItem::addOption( attr );
    mCombo->insertItem( i18n(mOptions[mOptions.count()-1].utf8()) );
    if ( (unsigned)mValue == mOptions.count()-1 )
	mCombo->setCurrentItem(mOptions.count()-1);
}

void KXSDropListControl::slotActivated(int indx)
{
  mValue = indx;
  emit changed();
}

void KXSDropListControl::read(KConfig &config)
{
    KXSSelectItem::read(config);
    mCombo->setCurrentItem(mValue);
}

//===========================================================================
KXSLineEditControl::KXSLineEditControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSStringItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));
  l->add(mEdit);
}

KXSLineEditControl::KXSLineEditControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSStringItem(name, attr)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));
  l->add(mEdit);
}

void KXSLineEditControl::textChanged( const QString &text )
{
  mValue = text;
  emit changed();
}

void KXSLineEditControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

//===========================================================================
KXSFileControl::KXSFileControl(QWidget *parent, const QString &name,
                                  KConfig &config)
  : QWidget(parent), KXSStringItem(name, config)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(mLabel, this);
  l->add(label);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));
  l->add(mEdit);
}

KXSFileControl::KXSFileControl(QWidget *parent, const QString &name,
                                  const QXmlAttributes &attr )
  : QWidget(parent), KXSStringItem(name, attr)
{
  QVBoxLayout *l = new QVBoxLayout(this);
  QLabel *label = new QLabel(i18n(mLabel.utf8()), this);
  l->add(label);
  QHBoxLayout *hb = new QHBoxLayout(l);
  mEdit = new QLineEdit(this);
  connect(mEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged(const QString &)));
  hb->add(mEdit);
  QPushButton *pb = new QPushButton( "...", this );
  connect( pb, SIGNAL(clicked()), this, SLOT(selectFile()) );
  hb->addWidget(pb);
}

void KXSFileControl::textChanged( const QString &text )
{
  mValue = text;
  emit changed();
}

void KXSFileControl::selectFile()
{
    QString f = KFileDialog::getOpenFileName();
    if ( !f.isEmpty() ) {
	mValue = f;
	mEdit->setText(mValue);
	emit changed();
    }
}

void KXSFileControl::read(KConfig &config)
{
    KXSStringItem::read(config);
    mEdit->setText(mValue);
}

#include "kxscontrol.moc"
