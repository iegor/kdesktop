/*
 * kgradprocess.h
 *
 * Copyright (C) 2003-2005 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef KGRADPROGRESS_H
#define KGRADPROGRESS_H

#include <qlabel.h>
#include <qpixmap.h>



class KGradProgress : public QLabel
{

	Q_OBJECT
	
public: 

	KGradProgress( QWidget *parent );
	~KGradProgress();
	virtual void paintEvent(QPaintEvent *event);
	virtual QSize sizeHint();
	virtual QSizePolicy sizePolicy();

public slots:

	void setProgress( int progress );

protected:

	virtual void resizeEvent( QResizeEvent *e );

private:

	void draw();

	QPixmap *barPix;
	QColor colorDown, colorUp;
	int current;
};

#endif /* KGRADPROGRESS_H */
