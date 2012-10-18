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

#ifndef SCROLLING_LABEL_H
#define SCROLLING_LABEL_H

#include <qwidget.h>
/**
 * A clever label that scrolls its contents as soon as there is not enough
 * space to show everything at once.
 **/
class ScrollingLabel : public QWidget
{
	Q_OBJECT

	public:
		ScrollingLabel(const QString &initialText,QWidget *parent,
			const char * name = 0);
		virtual ~ScrollingLabel();

		/**
		 * Sets the label's text
		 * set @p time (ms) if you want to display a temporary message
		 **/
		virtual void setText(const QString &text, int time = -1);

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;

		/**
		 * @return the label's text.
		 **/
		virtual QString text() const;

		/**
		 * Used to enable/disable scrolling manually
		 */
		virtual void setScroll(bool on);

	protected:

		virtual void paintEvent(QPaintEvent *);
		virtual void resizeEvent(QResizeEvent *);

	protected slots:

		virtual void scroll();
		virtual void restoreText();

	private:

		void _update();

		class Private;
		Private * d;
};

#endif
// vim:ts=2:sw=2:tw=78:noet
