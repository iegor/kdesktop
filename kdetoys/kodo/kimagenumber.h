/*
 * Mouspedometa
 *      Based on the original Xodometer VMS/Motif sources.
 *
 * Written by Armen Nakashian
 *            Compaq Computer Corporation
 *            Houston TX
 *            22 May 1998
 *
 * If you make improvements or enhancements to Mouspedometa, please send
 * them back to the author at any of the following addresses:
 *
 *              armen@nakashian.com
 *
 * Thanks to Mark Granoff for writing the original Xodometer, and
 * the whole KDE team for making such a nice environment to write
 * programs in.
 *
 *
 * This software is provided as is with no warranty of any kind,
 * expressed or implied. Neither Digital Equipment Corporation nor
 * Armen Nakashian will be held accountable for your use of this
 * software.
 */

#ifndef KIMGNUM_H
#define KIMGNUM_H

#include <qframe.h>
#include <qpixmap.h>

class KImageNumber : public QFrame
{
	Q_OBJECT
	Q_PROPERTY( double m_value READ value WRITE setValue )
public:
	KImageNumber(const QString& font, QWidget* parent=0, const char* name=0);

	virtual ~KImageNumber();

	void paintEvent(QPaintEvent*);
	virtual QSize sizeHint() const;
	double value() const;

public slots:
	void setValue(double v);

protected:
	double m_value;
	QPixmap* fontPix;
};

#endif
