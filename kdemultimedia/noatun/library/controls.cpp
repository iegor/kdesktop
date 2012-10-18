#include <noatun/controls.h>

L33tSlider::L33tSlider(QWidget * parent, const char * name) :
	QSlider(parent,name), pressed(false)
{}
L33tSlider::L33tSlider(Orientation o, QWidget * parent, const char * name) :
	QSlider(o,parent,name), pressed(false)
{}
L33tSlider::L33tSlider(int minValue, int maxValue, int pageStep, int value,
	                   Orientation o, QWidget * parent, const char * name) :
	QSlider(minValue, maxValue, pageStep, value, o, parent,name), pressed(false)
{}

bool L33tSlider::currentlyPressed() const
{
	return pressed;
}

void L33tSlider::setValue(int i)
{
	if (!pressed)
		QSlider::setValue(i);
}

void L33tSlider::mousePressEvent(QMouseEvent*e)
{
	if (e->button()!=RightButton)
	{
		pressed=true;
		QSlider::mousePressEvent(e);
	}
}

void L33tSlider::mouseReleaseEvent(QMouseEvent*e)
{
	pressed=false;
	QSlider::mouseReleaseEvent(e);
	emit userChanged(value());
}

void L33tSlider::wheelEvent(QWheelEvent *e)
{
	QSlider::wheelEvent(e);
	int newValue=value() /* +e->delta()/120 */;
	if (newValue<minValue())
		newValue=minValue();
	else if (newValue>maxValue())
		newValue=maxValue();
	setValue(newValue);
	emit userChanged(newValue);
}


SliderAction::SliderAction(const QString& text, int accel, const QObject *receiver,
                                 const char *member, QObject* parent, const char* name )
	: KAction( text, accel, parent, name )
{
	m_receiver = receiver;
	m_member = member;
}

int SliderAction::plug( QWidget *w, int index )
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

void SliderAction::toolbarMoved(KToolBar::BarPosition)
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

void SliderAction::unplug( QWidget *w )
{
	KToolBar *toolBar = (KToolBar *)w;
	int idx = findContainer( w );

	toolBar->removeItem( menuId( idx ) );
	removeContainer( idx );
}

#include "controls.moc"
