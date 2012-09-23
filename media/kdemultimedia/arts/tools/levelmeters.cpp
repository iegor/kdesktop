#include <qlayout.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qptrlist.h>
#include <kled.h>
#include "levelmeters.h"

const int PeakBar::peakMillis=1500;

PeakBar::PeakBar(QWidget *parent)
	: ACLevelMeter(parent)
	, maxValue( 0.0f )
	, minValue( 0.0f )
{
	clipped = false;
	displayMinPeak= false;
	horizontalMode= false;
	currentValue= 0.0f;
	
	lastValues.setAutoDelete( TRUE );
	
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
	setBackgroundMode(NoBackground);
	setMinimumSize(frameWidth()+7, 70);
}

void PeakBar::frameChanged() {
	setMinimumSize(frameWidth()+7, 70);
	QFrame::frameChanged();
}

QSize PeakBar::sizeHint() const {
	return QSize(13, 250);
}

void PeakBar::checkMinMax() {
	int mustRecheck= 0; // bool
	Observation *o;

	while ((o= lastValues.first()) && o->time.elapsed() > peakMillis) {
		lastValues.removeFirst();
		mustRecheck= 1;
	}

	if (mustRecheck) {
		maxValue= 0.f;
		minValue= 1.f;
		clipped = false;
		for (QPtrListIterator<Observation> it(lastValues); it.current(); ++it) {
			float value= it.current()->value;
			if (value>maxValue)
				maxValue= value;
			if (value<minValue)
				minValue= value;
			if (value > 1.f)
				clipped = true;
		}
	}
}

void PeakBar::drawContents(QPainter *p)
{
	QRect size= contentsRect();

	checkMinMax();
	
	p->setBrush(clipped ? darkRed : darkBlue);
	p->setPen(NoPen);
	p->drawRect(size);
		
	QRect bar= size;
	p->setBrush(clipped ? red : blue);
	if (horizontalMode) {
		bar.setWidth((int)(bar.width()*currentValue));
	} else {
		int newHeight= (int)(bar.height()*currentValue);
		bar.moveBy(0, bar.height()-newHeight);
		bar.setHeight(newHeight);
	}
	p->drawRect(bar);
		
	int y;
	// TODO: if (horizontalMode)
	if (displayMinPeak) {
		y= frameWidth()+size.height()-((int)(size.height()*minValue));
		p->setPen(white);
		p->drawLine(frameWidth(), y, frameWidth()+size.width()-1, y);
	}
	y= frameWidth()+size.height()-((int)(size.height()*maxValue));
	p->setPen(white);
	p->drawLine(frameWidth(), y, frameWidth()+size.width()-1, y);
}
		
void PeakBar::setValue(float f) {
	if (f > 1.f)
		clipped = true;

	currentValue= f;
	if (f>=maxValue)
		maxValue= f;
	if (displayMinPeak && (f<=minValue))
		minValue= f;
		
	lastValues.append(new Observation(f));
	
	repaint();
}

// -------------------------------------------------------------

PeakLevelMeters::PeakLevelMeters(QWidget *parent):
	StereoLevelMeter(parent), left(this), right(this), scaleView(this)
{
	QBoxLayout *layout= new QHBoxLayout(this);
	layout->addWidget(&left);
	//	layout->setStretchFactor(&left, 0);
	layout->addWidget(&right);
	//	layout->setStretchFactor(&right, 0);
	layout->addWidget(&scaleView);
	//	layout->setStretchFactor(&scaleView, 0);
	left.setLineWidth(2);
	right.setLineWidth(2);
	scaleView.setScaleMargins(right.frameWidth());
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

	setDbRange(36);
}
	
void PeakLevelMeters::setDbRange(int db) {
	dbRange= db;
	scaleView.setDbRange(db);
}

void PeakLevelMeters::setValues(float leftVal, float rightVal) {
	float f= 1.0f+levelToDB(leftVal)/dbRange;
	if (f<0.f) f= 0.f;
	left.setValue(f);

	f= 1.0f+levelToDB(rightVal)/dbRange;
	if (f<0.f) f= 0.f;
	right.setValue(f);
}

ScaleView::ScaleView(QWidget *parent): QFrame(parent) {
	font.setPixelSize(10);
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
}

void ScaleView::setDbRange(int db) {
	dbRange= db;
	repaint();
}

QSize ScaleView::sizeHint() const {
	return QSize(QFontMetrics(font).width("-88")+8, 250);
}

void ScaleView::drawContents(QPainter *p) {
	QRect size= contentsRect();
	size.rTop()+= upperMargin;
	size.rBottom()-= lowerMargin;

	QFrame::drawContents(p); // clear background
	
	int step=3;
	while (dbRange/step*20>size.height())
		step*=2;

	/*	float offset= ((float)size.height()*step)/dbRange, pos=0.f;
	p->setPen(black);
	p->drawLine(0, (, size.width()*3/5, y);*/
	
	p->setPen(black);
	p->setFont(font);
	QString numStr;
	for (int i=0; i<=dbRange; i++) {
		int y= size.top()+(size.height()-1)*i/dbRange;
		if (i%step==0) {
			p->drawLine(0, y, 4, y);
			numStr.setNum(-i);
			p->drawText(8, y+5, numStr);
		} else
			p->drawLine(0, y, 2, y);
	}
}

// -------------------------------------------------------------

LedMeter::LedMeter(QWidget *parent, bool blueState) : ACLevelMeter(parent) {
	setBackgroundColor(black);
	QBoxLayout * l = new QVBoxLayout( this );
	l->setAutoAdd(TRUE);
	for(int i=0;i<12;i++) {
		QColor c;
		if(blueState)
			c = blue;
		else {
			c = red;
			if(i>=2) c = yellow;
			if(i>=5) c = green;
		}

		// put each led in its own frame, since it seems to be broken
		QFrame *lframe = new QFrame(this);
		QBoxLayout *lfl = new QVBoxLayout( lframe );
		lfl->setAutoAdd(TRUE);
		leds[i] =
			new KLed(c,KLed::Off, KLed::Sunken, KLed::Circular,lframe);
	}
}

void LedMeter::setValue(float f)
{
	//printf("value %f\n",f);
	for(int i=11;i>=0;i--)
		{
			if(f > 0.06) leds[i]->setState(KLed::On);
			else		 leds[i]->setState(KLed::Off);
			f /= 1.25;
		}
}

// -------------------------------------------------------------

StereoLedMeters::StereoLedMeters(QWidget *parent)
	: StereoLevelMeter(parent), left(this), right(this)
{
	QBoxLayout *layout= new QHBoxLayout(this);
	layout->addWidget(&left);
	layout->addWidget(&right);
}
	
void StereoLedMeters::setValues(float leftVal, float rightVal) {
	left.setValue(leftVal);
	right.setValue(rightVal);
}

#include "levelmeters.moc"
