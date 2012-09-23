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

#ifndef __CONTROLS_H
#define __CONTROLS_H

#include <qguardedptr.h>

#include <kaction.h>
#include <ktoolbar.h>
#include <qslider.h>
#include <qstringlist.h>

class QComboBox;
class QLabel;

namespace Kaboodle
{
/**
 * A slider that can be moved around while being
 * changed internally
 **/
class L33tSlider : public QSlider
{
Q_OBJECT
public:
	L33tSlider(QWidget * parent, const char * name=0);
	L33tSlider(Orientation, QWidget * parent, const char * name=0);
	L33tSlider(int minValue, int maxValue, int pageStep, int value,
	           Orientation, QWidget * parent, const char * name=0);

	bool currentlyPressed() const;
signals:
	/**
	 * emmited only when the user changes the value by hand
	 **/
	void userChanged(int value);

public slots:
	virtual void setValue(int);
protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent *e);

private:
	bool pressed;
};

/**
 * A slider for your toolbar
 **/
class SliderAction : public KAction
{
Q_OBJECT
public:
	SliderAction(const QString& text, int accel, const QObject *receiver,
	             const char *member, QObject* parent, const char* name );
	virtual int plug( QWidget *w, int index = -1 );
	virtual void unplug( QWidget *w );
	QSlider* slider() const { return m_slider; }

signals:
	void plugged();

public slots:
	void toolbarMoved(KToolBar::BarPosition pos);
private:
	QGuardedPtr<QSlider> m_slider;
	QStringList m_items;
	const QObject *m_receiver;
	const char *m_member;
};

}

#endif
