/*
 * Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
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

#include <qtimer.h>

#include <noatun/app.h>
#include <noatun/player.h>
#include "seeker.h"
#include "seeker.moc"

Seeker::Seeker(QWidget * parent)
	: QSlider(0, 60, 10, 0, Horizontal, parent),
		adjusting_(false)
{
	delayedUpdateTimer_ = new QTimer(this);

	connect(this, SIGNAL(sliderPressed()),		SLOT(slotSliderPressed()));
	connect(this, SIGNAL(sliderReleased()),		SLOT(slotSliderReleased()));
	connect(this, SIGNAL(valueChanged(int)),	SLOT(slotValueChanged(int)));

	connect(napp->player(),				SIGNAL(timeout()),	SLOT(slotTimeout()));
	connect(delayedUpdateTimer_,	SIGNAL(timeout()), 	SLOT(slotDelayedUpdate()));
}

Seeker::~Seeker()
{
}

	void
Seeker::slotSliderPressed()
{
	adjusting_ = true;
}

	void
Seeker::slotValueChanged(int)
{
	if (adjusting_)
		delayedUpdateTimer_->start(100, true);
}

	void
Seeker::slotDelayedUpdate()
{
	napp->player()->skipTo(value());
}

	void
Seeker::slotSliderReleased()
{
	delayedUpdateTimer_->stop();
	adjusting_ = false;
	napp->player()->skipTo(value());
}

	void
Seeker::wheelEvent(QWheelEvent *)
{
	// Ignore.
}

	void
Seeker::slotTimeout()
{
	blockSignals(true);
	{
		delayedUpdateTimer_->stop();

		int currentLength	=	int(napp->player()->getLength());
		int currentTime		=	int(napp->player()->getTime());

		if (currentLength != maxValue())
			setMaxValue(currentLength);

		if (!adjusting_)
		{
			if (currentTime != value())
				setValue(currentTime);
		}
	}
	blockSignals(false);
	update();
}

// vim:ts=2:sw=2:tw=78:noet
