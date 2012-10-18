#include "kfader.h"
#include "kfader.moc"

void KFader::init()
{
}

KFader::KFader( QWidget * parent, const char * name )
	: QSlider( Qt::Vertical, parent, name )
{
	init();
}

KFader::KFader( int minValue, int maxValue, int pageStep, int value, QWidget * parent, const char * name )
	: QSlider( minValue, maxValue, pageStep, value, Qt::Vertical, parent, name )
{
	init();
}

KFader::~KFader()
{
}

void KFader::setColor( QColor & )
{
}

void KFader::setRange( int minValue, int maxValue )
{
	QRangeControl::setRange( minValue, maxValue );
}
