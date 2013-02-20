// $Id: kclock.cpp 376058 2005-01-07 11:14:10Z mlaurent $
//
// kclock - Clock screen saver for KDE
// Copyright (c) 2003   Melchior FRANZ
//
// License:		GPL v2
// Author:		Melchior FRANZ  <mfranz@kde.org>
// Dependencies:	libart_lgpl_2   http://www.levien.com/libart/
//
#include <stdlib.h>
#include <string.h>

#include <qcheckbox.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qslider.h>

#include <kapplication.h>
#include <kcolorbutton.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <libart_lgpl/art_affine.h>
#include <libart_lgpl/art_misc.h>
#include <libart_lgpl/art_rgb.h>
#include <libart_lgpl/art_rgb_svp.h>
#include <libart_lgpl/art_svp_vpath.h>
#include <libart_lgpl/art_svp_vpath_stroke.h>
#include <libart_lgpl/art_vpath.h>

#include "kclock.h"
#include "kclock.moc"


const int COLOR_BUTTON_WIDTH = 80;
const int TIMER_INTERVALL = 100;
const int MAX_CLOCK_SIZE = 10;
const int DEFAULT_CLOCK_SIZE = 3;
const bool DEFAULT_KEEP_CENTERED = false;




extern "C" {
	KDE_EXPORT const char *kss_applicationName = "kclock.kss";
	KDE_EXPORT const char *kss_description = I18N_NOOP("Clock");
	KDE_EXPORT const char *kss_version = "1.0";

	KDE_EXPORT KScreenSaver *kss_create(WId id) {
		return new KClockSaver(id);
	}

	KDE_EXPORT QDialog *kss_setup() {
		return new KClockSetup();
	}
}




//-----------------------------------------------------------------------------

KClockSetup::KClockSetup(QWidget *parent, const char *name)
      : KDialogBase(parent, name, true, i18n( "Setup Clock Screen Saver" ),
			Ok|Cancel|Help, Ok, true),
	m_saver(0)
{
	readSettings();

	setButtonText( Help, i18n( "A&bout" ) );
	QWidget *main = makeMainWidget();

	QVBoxLayout *top = new QVBoxLayout(main, 0, spacingHint());

	QHBoxLayout *hbox = new QHBoxLayout;
	top->addLayout( hbox );

	QGroupBox *colgroup = new QGroupBox(i18n("Colors"), main);
	colgroup->setColumnLayout( 0, Horizontal );
	QGridLayout *grid = new QGridLayout( colgroup->layout(),
		5, 2, spacingHint() );

	QLabel *label = new QLabel(i18n("&Hour-hand:"), colgroup);
	KColorButton *colorButton = new KColorButton(m_hourColor, colgroup);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(const QColor &)),
			SLOT(slotHourColor(const QColor &)));
	grid->addWidget( label, 1, 1 );
	grid->addWidget( colorButton, 1, 2 );

	label = new QLabel(i18n("&Minute-hand:"), colgroup);
	colorButton = new KColorButton(m_minColor, colgroup);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(const QColor &)),
			SLOT(slotMinColor(const QColor &)));
	grid->addWidget( label, 2, 1 );
	grid->addWidget( colorButton, 2, 2 );

	label = new QLabel(i18n("&Second-hand:"), colgroup);
	colorButton = new KColorButton(m_secColor, colgroup);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(const QColor &)),
			SLOT(slotSecColor(const QColor &)));
	grid->addWidget( label, 3, 1 );
	grid->addWidget( colorButton, 3, 2 );

	label = new QLabel(i18n("Scal&e:"), colgroup);
	colorButton = new KColorButton(m_scaleColor, colgroup);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(const QColor &)),
			SLOT(slotScaleColor(const QColor &)));
	grid->addWidget( label, 4, 1 );
	grid->addWidget( colorButton, 4, 2 );

	label = new QLabel(i18n("&Background:"), colgroup);
	colorButton = new KColorButton(m_bgndColor, colgroup);
	colorButton->setFixedWidth(COLOR_BUTTON_WIDTH);
	label->setBuddy(colorButton);
	connect(colorButton, SIGNAL(changed(const QColor &)),
			SLOT(slotBgndColor(const QColor &)));
	grid->addWidget( label, 5, 1 );
	grid->addWidget( colorButton, 5, 2 );

	hbox->addWidget(colgroup);

	QWidget *m_preview = new QWidget(main);
	m_preview->setFixedSize(220, 165);
	m_preview->show();
	m_saver = new KClockSaver(m_preview->winId());
	hbox->addWidget(m_preview);

	label = new QLabel( i18n( "Si&ze:" ), main );
	top->addWidget( label );
	QSlider *qs = new QSlider(0, MAX_CLOCK_SIZE, 1, m_size, Horizontal, main);
	label->setBuddy( qs );
	qs->setTickInterval(1);
	qs->setTickmarks(QSlider::Below);
	connect(qs, SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
	top->addWidget( qs );

	bool rtl = kapp->reverseLayout();
	QHBox *qsscale = new QHBox(main);
	label = new QLabel(i18n("Small"), qsscale);
	label->setAlignment(rtl ? AlignRight : AlignLeft);
	label = new QLabel(i18n("Medium"), qsscale);
	label->setAlignment(AlignHCenter);
	label = new QLabel(i18n("Big"), qsscale);
	label->setAlignment(rtl ? AlignLeft : AlignRight);
	top->addWidget(qsscale);

	QCheckBox *keepCentered = new QCheckBox(i18n("&Keep clock centered"), main);
	keepCentered->setChecked(m_keepCentered);
	connect(keepCentered, SIGNAL(stateChanged(int)), SLOT(slotKeepCenteredChanged(int)));
	top->addWidget(keepCentered);
	top->addStretch();
}

KClockSetup::~KClockSetup()
{
	delete m_saver;
}

void KClockSetup::readSettings()
{
	KConfig *config = KGlobal::config();

	config->setGroup("Settings");
	m_keepCentered = config->readBoolEntry("KeepCentered", DEFAULT_KEEP_CENTERED);
	m_size = config->readUnsignedNumEntry("Size", DEFAULT_CLOCK_SIZE);
	if (m_size > MAX_CLOCK_SIZE)
		m_size = MAX_CLOCK_SIZE;

	config->setGroup("Colors");
	QColor c = Qt::black;
	m_bgndColor = config->readColorEntry("Background", &c);

	c = Qt::white;
	m_scaleColor = config->readColorEntry("Scale", &c);
	m_hourColor = config->readColorEntry("HourHand", &c);
	m_minColor = config->readColorEntry("MinuteHand", &c);

	c = Qt::red;
	m_secColor = config->readColorEntry("SecondHand", &c);

	if (m_saver) {
		m_saver->setBgndColor(m_bgndColor);
		m_saver->setScaleColor(m_scaleColor);
		m_saver->setHourColor(m_hourColor);
		m_saver->setMinColor(m_minColor);
		m_saver->setSecColor(m_secColor);
	}
}


void KClockSetup::slotOk()
{
	KConfig *config = KGlobal::config();
	config->setGroup("Settings");
	config->writeEntry("Size", m_size);
	config->writeEntry("KeepCentered", m_keepCentered);

	config->setGroup("Colors");
	config->writeEntry("Background", m_bgndColor);
	config->writeEntry("Scale", m_scaleColor);
	config->writeEntry("HourHand", m_hourColor);
	config->writeEntry("MinuteHand", m_minColor);
	config->writeEntry("SecondHand", m_secColor);
	config->sync();
	accept();
}


void KClockSetup::slotHelp()
{
	KMessageBox::about(this, "<qt>" + i18n(
			"Clock Screen Saver<br>"
			"Version 1.0<br>"
			"<nobr>Melchior FRANZ (c) 2003</nobr>") +
			"<br><a href=\"mailto:mfranz@kde.org\">mfranz@kde.org</a>"
			"</qt>", QString::null, KMessageBox::AllowLink);
}


void KClockSetup::slotBgndColor(const QColor &color)
{
	m_bgndColor = color;
	if (m_saver)
		m_saver->setBgndColor(m_bgndColor);
}


void KClockSetup::slotScaleColor(const QColor &color)
{
	m_scaleColor = color;
	if (m_saver)
		m_saver->setScaleColor(m_scaleColor);
}


void KClockSetup::slotHourColor(const QColor &color)
{
	m_hourColor = color;
	if (m_saver)
		m_saver->setHourColor(m_hourColor);
}


void KClockSetup::slotMinColor(const QColor &color)
{
	m_minColor = color;
	if (m_saver)
		m_saver->setMinColor(m_minColor);
}


void KClockSetup::slotSecColor(const QColor &color)
{
	m_secColor = color;
	if (m_saver)
		m_saver->setSecColor(m_secColor);
}


void KClockSetup::slotSliderMoved(int v)
{
	if (m_saver)
		m_saver->restart(m_size = v);
}


void KClockSetup::slotKeepCenteredChanged(int c)
{
	if (m_saver)
		m_saver->setKeepCentered(m_keepCentered = c);
}





//-----------------------------------------------------------------------------

KClockPainter::KClockPainter(int width, int height)
      : m_width(width),
	m_height(height)
{
	m_buf = new Q_UINT8[m_width * m_height * 3];
	// build Cartesian coordinate system ranging from -1000 to +1000;
	// points with positive x and y are in the top right quarter
	m_matrix[0] = m_width / 2000.0;
	m_matrix[1] = 0;
	m_matrix[2] = 0;
	m_matrix[3] = m_height / -2000.0;
	m_matrix[4] = m_width / 2.0;
	m_matrix[5] = m_height / 2.0;
}


KClockPainter::~KClockPainter()
{
	delete[] m_buf;
}


void KClockPainter::copy(KClockPainter *p)
{
	memcpy(m_buf, p->image(), m_width * m_height * 3);
}


void KClockPainter::drawToImage(QImage *q, int xoffs = 0, int yoffs = 0)
{
	unsigned char *src = (unsigned char *)image();
	for (int y = 0; y < m_height; y++) {
		QRgb *dest = reinterpret_cast<QRgb *>(q->scanLine(y + yoffs)) + xoffs;
		for (int x = 0; x < m_width; x++, src += 3)
			*dest++ = qRgba(src[0], src[1], src[2], 255);
	}
}


void KClockPainter::setColor(const QColor &c)
{
	m_color = (c.red() << 24) | (c.green() << 16) | (c.blue() << 8) | 255;
}


void KClockPainter::setShadowColor(const QColor &c)
{
	m_shadow = (c.red() << 24) | (c.green() << 16) | (c.blue() << 8) | 255;
}


void KClockPainter::fill(const QColor &c)
{
	art_rgb_fill_run(m_buf, c.red(), c.green(), c.blue(), m_width * m_height);
}


void KClockPainter::drawRadial(double alpha, double r0, double r1, double width)
{
	ArtVpath *vx, *v = art_new(ArtVpath, 3);
	v[0].code = ART_MOVETO_OPEN;
	v[0].x = r0;
	v[0].y = 0;
	v[1].code = ART_LINETO;
	v[1].x = r1;
	v[1].y = 0;
	v[2].code = ART_END;

	double m[6] = {0, 0, 0, 0, 0, 0};
	art_affine_rotate(m, 90 - alpha);
	vx = art_vpath_affine_transform(v, m);
	art_free(v);
	v = art_vpath_affine_transform(vx, m_matrix);
	art_free(vx);

	double w = width * m_matrix[0];
	if (w < 1.0)
		w = 1.0;

	ArtSVP *path = art_svp_vpath_stroke(v, ART_PATH_STROKE_JOIN_MITER,
			ART_PATH_STROKE_CAP_BUTT, w, 4, .5);
	art_rgb_svp_alpha(path, 0, 0, m_width, m_height, m_color, m_buf, 3 * m_width, 0);
	art_svp_free(path);
	art_free(v);
}


void KClockPainter::drawDisc(double r)
{
	ArtVpath *v, *vx = art_vpath_new_circle(0, 0, r);
	v = art_vpath_affine_transform(vx, m_matrix);
	art_free(vx);

	ArtSVP *path = art_svp_from_vpath(v);
	art_rgb_svp_alpha(path, 0, 0, m_width, m_height, m_color, m_buf, 3 * m_width, 0);
	art_svp_free(path);

	path = art_svp_vpath_stroke(v, ART_PATH_STROKE_JOIN_MITER,
			ART_PATH_STROKE_CAP_BUTT, 2, 4, .5);
	art_free(v);

	art_rgb_svp_alpha(path, 0, 0, m_width, m_height, m_color, m_buf, 3 * m_width, 0);
	art_svp_free(path);
}


void KClockPainter::drawHand(const QColor &c, double angle, double length,
	double width, bool disc = true)
{
	const double shadow_width = 1.0;
	setColor(m_shadow);
	if (disc)
		drawDisc(width * 1.3 + shadow_width);
	drawRadial(angle, .75 * width, length + shadow_width, width + shadow_width);

	setColor(c);
	if (disc)
		drawDisc(width * 1.3);
	drawRadial(angle, .75 * width, length, width);
}






//-----------------------------------------------------------------------------

KClockSaver::KClockSaver(WId id)
      : KScreenSaver(id),
	m_showSecond(true)
{
	readSettings();
	setBackgroundColor(m_bgndColor);
	connect(&m_timer, SIGNAL(timeout()), SLOT(slotTimeout()));
	start(m_size);
	m_timer.start(TIMER_INTERVALL);
}


KClockSaver::~KClockSaver()
{
	m_timer.stop();
	stop();
}


void KClockSaver::start(int size)
{
	m_diameter = int(QMIN(width(), height()) * (size + 4) / 14.0);
	m_x = (width() - m_diameter) / 2;
	m_y = (height() - m_diameter) / 2;

	m_image = new QImage(m_diameter, m_diameter, 32);
	m_scale = new KClockPainter(m_diameter, m_diameter);
	m_clock = new KClockPainter(m_diameter, m_diameter);

	m_clock->setShadowColor(qRgb((m_bgndColor.red() + m_scaleColor.red()) / 2,
			(m_bgndColor.green() + m_scaleColor.green()) / 2,
			(m_bgndColor.blue() + m_scaleColor.blue()) / 2));
	drawScale();
	forceRedraw();
}


void KClockSaver::stop()
{
	delete m_clock;
	delete m_scale;
	delete m_image;
}


void KClockSaver::restart(int size)
{
	m_timer.stop();
	stop();
	start(size);
	drawScale();
	m_timer.start(TIMER_INTERVALL);
}


void KClockSaver::setKeepCentered(bool b)
{
	m_keepCentered = b;
	if (b) {
		m_x = (width() - m_diameter) / 2;
		m_y = (height() - m_diameter) / 2;
	}
	forceRedraw();
}


void KClockSaver::readSettings()
{
	KConfig *config = KGlobal::config();

	config->setGroup("Settings");
	m_keepCentered = config->readBoolEntry("KeepCentered", DEFAULT_KEEP_CENTERED);
	m_size = config->readUnsignedNumEntry("Size", DEFAULT_CLOCK_SIZE);
	if (m_size > MAX_CLOCK_SIZE)
		m_size = MAX_CLOCK_SIZE;

	config->setGroup("Colors");
	QColor c = Qt::black;
	m_bgndColor = config->readColorEntry("Background", &c);

	c = Qt::white;
	m_scaleColor = config->readColorEntry("Scale", &c);
	m_hourColor = config->readColorEntry("HourHand", &c);
	m_minColor = config->readColorEntry("MinuteHand", &c);

	c = Qt::red;
	m_secColor = config->readColorEntry("SecondHand", &c);
}


void KClockSaver::drawScale()
{
	m_scale->fill(m_bgndColor);
	m_scale->setColor(m_scaleColor);

	for (int i = 0; i < 360; i += 6)
		if (i % 30)
			m_scale->drawRadial(i, 920, 980, 15);
		else
			m_scale->drawRadial(i, 825, 980, 40);

	forceRedraw();
}


void KClockSaver::drawClock()
{
	double hour_angle = m_hour * 30.0 + m_minute * .5;
	double minute_angle = m_minute * 6.0 + m_second * .1;
	double second_angle = m_second * 6.0;

	m_clock->copy(m_scale);
	m_clock->drawHand(m_hourColor, hour_angle, 600, 55, false);
	m_clock->drawHand(m_minColor, minute_angle, 900, 40);

	if (m_showSecond)
		m_clock->drawHand(m_secColor, second_angle, 900, 30);
}


void KClockSaver::slotTimeout()
{
	QTime t = QTime::currentTime();
	int s = t.second();
	if (s == m_second)
		return;

	m_second = m_showSecond ? s : 0;
	m_hour = t.hour();
	m_minute = t.minute();

	drawClock();
	QPainter p(this);

	if (width() < 256) {
		// intended for the control module preview: always fill the whole area
		QImage *img = new QImage(width(), height(), 32);
		img->fill(qRgb(m_bgndColor.red(), m_bgndColor.green(), m_bgndColor.blue()));
		m_clock->drawToImage(img, m_x, m_y);
		p.drawImage(0, 0, *img);
		delete img;
	} else {
		m_clock->drawToImage(m_image);
		p.drawImage(m_x, m_y, *m_image);
	}

	if (!m_keepCentered) {
		static int xstep = 1;
		static int ystep = -1;
		int i;

		m_x += xstep;
		if (m_x <= 0)
			m_x = 0, xstep = 1;
		else if (m_x >= (i = width() - m_diameter))
			m_x = i, xstep = -1;

		m_y += ystep;
		if (m_y <= 0)
			m_y = 0, ystep = 1;
		else if (m_y >= (i = height() - m_diameter))
			m_y = i, ystep = -1;
	}
}


