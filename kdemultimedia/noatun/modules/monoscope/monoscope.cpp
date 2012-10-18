#include "monoscope.h"
#include <noatun/player.h>
#include <noatun/app.h>
#include <math.h>
#include <qpainter.h>
#include <kactionclasses.h>
#include <noatun/stdaction.h>
#include <klocale.h>

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new Monoscope();
	}
}

Monoscope::Monoscope() : QWidget(0,0,WRepaintNoErase), MonoScope(30), Plugin()
{
	NOATUNPLUGINC(Monoscope);

	mAction=0L;
	mLowColor=qRgb(0,0,0);
	mHighColor=qRgb(238,238,238);
	resize(320, 240);
	MonoScope::start();
	setCaption(i18n("Monoscope"));
	show();
	resizeEvent(0);
	repaint(0,0, QWidget::width(), height(), false);
	resizeEvent(0);
	setBackgroundColor(mLowColor);
}

Monoscope::~Monoscope()
{
	if(mAction)
		napp->pluginActionMenu()->remove(mAction);
}

void Monoscope::init()
{
	mAction = new KToggleAction(i18n("Toggle Monoscope"), 0, 0,
		this, SLOT(toggle()), this, "togglemonoscope");
	mAction->setChecked(!isHidden());
	napp->pluginActionMenu()->insert(mAction);
}

void Monoscope::toggle(void)
{
	if(isHidden())
		show();
	else
		hide();
}

void Monoscope::closeEvent(QCloseEvent *)
{
	hide();
}

void Monoscope::resizeEvent(QResizeEvent *)
{
	setSamples(width());
}

void Monoscope::scopeEvent(float *d, int size)
{
	// save cpu
	if(isHidden()) return;

	const bool line=false;

	int viewWidth =width();
	int viewHeight=height();

	float *end=d+size;
	int x=0;
	int heightHalf=viewHeight/4;
	int y=viewHeight/2;
	// reduce flicker
	QPixmap buffer(viewWidth, viewHeight, -1, QPixmap::BestOptim);
	buffer.fill(mLowColor);
	QPainter p(&buffer);
	p.setPen(mHighColor);
	repaint(rect());

	if (line)
		p.moveTo(0, y);

	while (d<=end)
	{
		float &n=*d;

		n *= heightHalf;
		int amp=(int)n;

		if (line) // line
			p.lineTo(x, y+amp);
		else // fill
			p.drawLine(x, y, x, y+amp);
		d++;
		x++;

	}
	if (line)
		p.drawLine(0, y, size, y);
	bitBlt(this, 0, 0, &buffer, 0, 0, viewWidth, viewHeight, Qt::CopyROP);
}

#include "monoscope.moc"

