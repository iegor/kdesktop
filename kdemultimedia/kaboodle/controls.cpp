/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "controls.h"

Kaboodle::L33tSlider::L33tSlider(QWidget * parent, const char * name) :
	QSlider(parent,name), pressed(false)
{}
Kaboodle::L33tSlider::L33tSlider(Orientation o, QWidget * parent, const char * name) :
	QSlider(o,parent,name), pressed(false)
{}
Kaboodle::L33tSlider::L33tSlider(int minValue, int maxValue, int pageStep, int value,
	                   Orientation o, QWidget * parent, const char * name) :
	QSlider(minValue, maxValue, pageStep, value, o, parent,name), pressed(false)
{}

bool Kaboodle::L33tSlider::currentlyPressed() const
{
	return pressed;
}

void Kaboodle::L33tSlider::setValue(int i)
{
	if (!pressed)
		QSlider::setValue(i);
}

void Kaboodle::L33tSlider::mousePressEvent(QMouseEvent*e)
{
	if (e->button()!=RightButton)
	{
		pressed=true;
		QSlider::mousePressEvent(e);
	}
}

void Kaboodle::L33tSlider::mouseReleaseEvent(QMouseEvent*e)
{
	pressed=false;
	QSlider::mouseReleaseEvent(e);
	emit userChanged(value());
}

void Kaboodle::L33tSlider::wheelEvent(QWheelEvent *e)
{
	QSlider::wheelEvent(e);
	int newValue = value();

	if(newValue < minValue())
		newValue = minValue();
	else if(newValue > maxValue())
		newValue = maxValue();

	setValue(newValue);
	emit userChanged(newValue);
}

Kaboodle::SliderAction::SliderAction(const QString& text, int accel, const QObject *receiver,
                                 const char *member, QObject* parent, const char* name )
	: KAction( text, accel, parent, name )
{
	m_receiver = receiver;
	m_member = member;
}

int Kaboodle::SliderAction::plug( QWidget *w, int index )
{
	if (!w->inherits("KToolBar")) return -1;

	KToolBar *toolBar = (KToolBar *)w;
	int id = KAction::getToolButtonID();
	
	//Create it.
	m_slider=new L33tSlider(0, 1000, 100, 0, Horizontal, toolBar);
	m_slider->setMinimumWidth(10);
	toolBar->insertWidget(id, 10, m_slider, index );


	addContainer( toolBar, id );
	connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
	toolBar->setItemAutoSized( id, true );

	if (w->inherits( "KToolBar" ))
		connect(toolBar, SIGNAL(moved(KToolBar::BarPosition)), this, SLOT(toolbarMoved(KToolBar::BarPosition)));
	
	emit plugged();

	return containerCount() - 1;
}

void Kaboodle::SliderAction::toolbarMoved(KToolBar::BarPosition)
{
// I wish this worked :)
return;	
/*
	if (pos == KToolBar::Left || pos == KToolBar::Right)
	{
		m_slider->setOrientation(Vertical);
		m_slider->setFixedWidth(m_slider->height());
	}
	else
	{
		m_slider->setOrientation(Horizontal);
		m_slider->resize(m_slider->height(), m_slider->height());
	}
*/
}

void Kaboodle::SliderAction::unplug( QWidget *w )
{
	KToolBar *toolBar = (KToolBar *)w;
	int idx = findContainer( w );

	toolBar->removeItem( itemId( idx ) );
	removeContainer( idx );
}

#include "controls.moc"
