#ifndef __CONTROLS_H
#define __CONTROLS_H

#include <qguardedptr.h>

#include <kaction.h>
#include <ktoolbar.h>
#include <qslider.h>
#include <qstringlist.h>
#include <kdemacros.h>

class QComboBox;
class QLabel;

/**
 * A slider that can be moved around while being
 * changed internally
 *
 * @short Special QSlider based class suitable for time sliders
 * @author Charles Samuels
 * @version 2.3
 **/
class KDE_EXPORT L33tSlider : public QSlider
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
	virtual void wheelEvent(QWheelEvent *);

private:
	bool pressed;
};

/**
 * @short A slider for your toolbar
 * @author Charles Samuels
 * @version 2.3
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

#endif
