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

Seeker::Seeker(QWidget * parent, const char * name)
	: L33tSlider(0, 1000, 10, 0, Horizontal, parent, name)
{
	connect(this, SIGNAL(userChanged(int)),		SLOT(slotValueChanged(int)));

	connect(napp->player(),				SIGNAL(timeout()),	SLOT(slotTimeout()));
}

Seeker::~Seeker()
{
}

void Seeker::slotValueChanged(int i)
{
	napp->player()->skipTo((int)(i*1000));
}

void Seeker::slotTimeout()
{
	if(!napp->player()->current()) return;
	if(currentlyPressed()) return;

	setRange(0, (int)napp->player()->getLength() / 1000 );
	setValue((int)napp->player()->getTime() / 1000 );
}

void Seeker::wheelEvent(QWheelEvent *)
{
	// ignore
}

// vim:ts=2:sw=2:tw=78:noet
#include "seeker.moc"
