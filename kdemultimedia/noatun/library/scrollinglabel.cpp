/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <noatun/scrollinglabel.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qtooltip.h>

class ScrollingLabel::Private
{
	public:

		Private()
			:	scrollSize	(0),
				pos					(0),
				add					(false),
				scroll			(true)
		{
		}

		// Order dependency.
		QString		text;
		QString		originalText;
		QPixmap		buf;
		QTimer		scrollTimer;
		QTimer		resetTimer;
		int				scrollSize;
		int				pos;
		bool			add;
		bool			scroll;
		// End order dependency.
};

ScrollingLabel::ScrollingLabel
(
 const QString	& initialText,
 QWidget				* parent,
 const char			* name
)
	: QWidget(parent, name)
{
	d = new Private;

	connect(&d->scrollTimer,	SIGNAL(timeout()), this, SLOT(scroll()));
	connect(&d->resetTimer,		SIGNAL(timeout()), this, SLOT(restoreText()));

	setText(initialText);
}

ScrollingLabel::~ScrollingLabel()
{
	delete d;
}

	void
ScrollingLabel::setText(const QString & t, int time)
{
	d->resetTimer.stop();

	if (-1 != time)
	{
		restoreText();
		d->originalText	= d->text;
		d->text = t;
		d->resetTimer.start(time, true);
		_update();
	}
	else
	{
		d->text = d->originalText = t;
		_update();
	}

        QToolTip::remove(this);
	QToolTip::add(this, d->text);
}

	void
ScrollingLabel::restoreText()
{
	d->text = d->originalText;
	_update();
}

	void
ScrollingLabel::_update()
{
	d->scrollTimer.stop();

	d->pos = 0;
	d->add = false;

	int w = fontMetrics().width(d->text);
	int h = fontMetrics().height();

	setFixedHeight(h);

	d->scrollSize = QMAX(0, w - width());

	d->buf.resize(w, h);
	d->buf.fill(colorGroup().background());

	QPainter p(&d->buf);
	p.setFont(font());
	p.drawText(0, fontMetrics().ascent(), d->text);

	if (d->scroll && (d->scrollSize > 0))
		d->scrollTimer.start(100, true);

	repaint(false);
}

	void
ScrollingLabel::paintEvent(QPaintEvent *)
{
	bitBlt
		(this, 0, 0, &d->buf, d->pos, 0, d->pos + width(), height(), Qt::CopyROP);
}

	void
ScrollingLabel::resizeEvent(QResizeEvent *)
{
	_update();
}

	void
ScrollingLabel::scroll()
{
	d->scrollTimer.stop();

	repaint(false);

	int scrollTime = 100;

	if (d->pos == d->scrollSize || d->pos == 0)
	{
		d->add = !d->add;
		scrollTime = 800;
	}

	d->pos += (d->add ? 1 : -1);

	if (d->scroll)
		d->scrollTimer.start(scrollTime, true);
}

	QSize
ScrollingLabel::sizeHint() const
{
	return fontMetrics().boundingRect(d->text).size();
}

	QSize
ScrollingLabel::minimumSizeHint() const
{
	return QSize(0, fontMetrics().height());
}

	QString
ScrollingLabel::text() const
{
	return d->text;
}

	void
ScrollingLabel::setScroll(bool b)
{
	d->scroll = b;
	_update();
}

#include "scrollinglabel.moc"

// vim:ts=2:sw=2:tw=78:noet

