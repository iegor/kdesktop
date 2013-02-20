/**
 * Copyright (C) 2003, Luís Pedro Coelho
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "fullscreenfilter.h"

#include "kgvshell.h"
#include "kgv_view.h"
#include "kgv_miniwidget.h"
#include "kgvpageview.h"

FullScreenFilter::FullScreenFilter( KGVShell& parent )
	:QObject( &parent, "full-screen-filter" ),
	 parent( parent )
{
}

bool FullScreenFilter::eventFilter( QObject* /*object*/, QEvent* ev) {
	if ( QKeyEvent* keyevent = dynamic_cast<QKeyEvent*>( ev ) ) {
		if ( keyevent->key() == Key_Escape ) {
			parent.setFullScreen( false );
			keyevent->accept();
			return true;
		}
	}
	if ( QMouseEvent* mouseevent = dynamic_cast<QMouseEvent*>( ev ) ) {
		if ( mouseevent->stateAfter() & mouseevent->button() & LeftButton ) {
			// if ( The whole image is visible at once )
			if ( parent.m_gvpart->pageView()->contentsHeight() <= parent.m_gvpart->widget()->height() &&
			     parent.m_gvpart->pageView()->contentsWidth() <= parent.m_gvpart->widget()->width() ) {
				parent.m_gvpart->miniWidget()->nextPage();
				mouseevent->accept();
				return true;
			}
		}
	}
	return false;
}

#include "fullscreenfilter.moc"

