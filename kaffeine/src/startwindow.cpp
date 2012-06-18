/*
 * startwindow.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2006-2007 Christophe Thommeret <hftom@free.fr>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcolor.h>
#include <qtooltip.h>

#include "version.h"
#include "startwindow.h"
#include "startwindow.moc"



MToolButton::MToolButton( QWidget *parent, const QString& targ ) : QToolButton( parent )
{
	target = targ;
	connect( this, SIGNAL(clicked()), SLOT(mClicked()) );
}

MToolButton::~MToolButton()
{
}

void MToolButton::mClicked()
{
	emit executed( target );
}



SLabel::SLabel( QWidget *parent ) : QLabel( parent )
{
}



void SLabel::paintEvent( QPaintEvent *pe )
{
	QLabel::paintEvent( pe );
	QColorGroup cg = parentWidget()->colorGroup();
	QColor base = cg.base();
	QColor selection = cg.highlight();
	int r = (base.red() + selection.red()) / 2;
	int b = (base.blue() + selection.blue()) / 2;
	int g = (base.green() + selection.green()) / 2;
	setPaletteBackgroundColor( QColor(r, g, b) );
}



StartWindow::StartWindow(QWidget* parent, const char* name) : QWidget(parent, name)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setAutoAdd(true);
	mainLayout->setSpacing(0);

	label1 = new SLabel(this);
	//label1->setPaletteBackgroundColor(QColor(238, 246, 255));
	label1->setText("<center><font size=\"7\"><b>"+i18n("[Kaffeine Player]")+"</b></font></center>");
	label2 = new SLabel(this);
	//label2->setPaletteBackgroundColor(QColor(238, 246, 255));
	label2->setText(QString("<center><font  size=\"5\">") + KAFFEINE_VERSION + "</font></center>");

	panel = new QWidget( this );
	panel->setPaletteBackgroundColor(QColor("White"));
	panel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );

	buttons.setAutoDelete( true );
}



StartWindow::~StartWindow()
{
	buttons.clear();
}



void StartWindow::registerTarget( const QString& uiName, const QString& pixName, const QString& targetName )
{
	MToolButton *btn = new MToolButton( panel, targetName );
	//QFont f = btn->font();
	//f.setPointSize( f.pointSize()+2 );
	//f.setBold(true);
	//btn->setFont( f );
	btn->setTextLabel( QString("&%1 %2").arg(buttons.count() + 1).arg(uiName));
	QToolTip::add(btn, uiName);
	btn->setTextPosition( QToolButton::Under );
	btn->setUsesTextLabel( true );
	btn->setIconSet( KGlobal::iconLoader()->loadIconSet(pixName, KIcon::Panel) );
	QSize size = btn->sizeHint();
	size.setHeight( size.height()+5 );
	btn->resize( size );
	connect( btn, SIGNAL(executed(const QString&)), this, SIGNAL(execTarget(const QString&)) );
	buttons.append( btn );
	btn->show();
	arrangeButtons( QSize( width(),height() ) );
}



void StartWindow::execTarget(int index)
{
	if ( index<1 || index>buttons.count() )
		return;
	buttons.at( index-1 )->animateClick();
}



void StartWindow::unregisterTarget( const QString& targetName )
{
	int i;
	MToolButton *btn;

	for ( i=0; i<(int)buttons.count(); i++ ) {
		btn = buttons.at(i);
		if ( btn->target==targetName ) {
			buttons.remove( btn );
			arrangeButtons( QSize( width(),height() ) );
			return;
		}
	}
}



void StartWindow::arrangeButtons( QSize e )
{
	MToolButton *btn;
	int x, y;
	int i, j, k, nb, nw, nh, mw, mh;
	int bw=0, bh=0, iter=20;
	int nnh;
	int ew = e.width();
	int eh = e.height()-label1->height()-label2->height();
	QPtrListIterator<MToolButton> it(buttons);

	nb = buttons.count();
	if ( !nb )
		return;
	it.toFirst();
	while ( it.current() ) {
		if ( it.current()->width()>bw )
			bw = it.current()->width();
		if ( it.current()->height()>bh )
			bh = it.current()->height();
		++it;
	}

	nw = nb;
	nh = 1;
	while ( (ew-(nw*(bw+iter)))<(eh-(nh*(bh+iter))) ) {
		if ( nw==1 )
			break;
		--nw;
		++nh;
	}

	nnh=0;
	while ( (nnh*nw)<nb )
		++nnh;

	mh = (eh-((nnh*bh)+((nnh-1)*iter)))/2;
	mw = (ew-((nw*bw)+((nw-1)*iter)))/2;

	k = 0;
	y = mh;
	for ( i=0; i<nh; i++ ) {
		x = mw;
		for ( j=0; j<nw; j++ ) {
			btn = buttons.at(k);
			if ( !btn )
				return;
			btn->resize(bw,bh);
			btn->move( x, y );
			++k;
			x = x+iter+bw;
		}
		y = y+iter+bh;
	}
}



void StartWindow::resizeEvent( QResizeEvent *e )
{
	arrangeButtons( e->size() );
}
