/**
 * Copyright (C) 1997-2002 the KGhostView authors. See file AUTHORS.
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

#include "marklist.moc"

#include <cassert>

#include <qimage.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

#include "kgv_miniwidget.h"

MarkListItem::MarkListItem(QWidget *parent, const QString &text, const QString &tip, const QColor &color, KGVMiniWidget* miniW, int pageNum)
    : QWidget( parent ),
    _miniWidget( miniW ),
    _pageNum( pageNum ),
    _requested( false )
{
    //kdDebug( 4500 ) << "MarkListItem::MarkListItem( _ , "  << text <<" , " << tip << " , " <<  color << ", _ ," << pageNum << " )" << endl;
    QBoxLayout *l = new QVBoxLayout( this, 5, 0 );
    _thumbnailW = new QWidget( this );
    _checkBox = new QCheckBox( text, this );
    l->addWidget( _thumbnailW, 1 );
    l->addWidget( _checkBox, 0, Qt::AlignHCenter );
    QWhatsThis::add( _checkBox, i18n( "Using this checkbox you can select pages for printing." ) );
    setFixedHeight( 100 );
    _backgroundColor = color;
    setPaletteBackgroundColor( _backgroundColor );
    QToolTip::add(this, tip);
    // TODO: Put a little page number or other place-holder when there is no thumbnail to display.
}

bool MarkListItem::isChecked() const
{
    return _checkBox->isChecked();
}

void MarkListItem::toggle()
{
    _checkBox->toggle();
}

void MarkListItem::setChecked( bool checked )
{
    _checkBox->setChecked(checked);
}

void MarkListItem::setPixmap( QPixmap thumbnail )
{
    // The line below is needed to work around certain "features" of styles such as liquid
    // see bug:61711 for more info (LPC, 20 Aug '03)
    _thumbnailW->setBackgroundOrigin( QWidget::WidgetOrigin );
    _thumbnailW->setPaletteBackgroundPixmap( thumbnail.convertToImage().smoothScale( _thumbnailW->size() ) );
    _requested = false;
}

void MarkListItem::setSelected( bool selected )
{
    if (selected)
	setPaletteBackgroundColor( QApplication::palette().active().highlight() );
    else
	setPaletteBackgroundColor( _backgroundColor );
}

void MarkListItem::resizeEvent( QResizeEvent * )
{
    if ( _thumbnailW->paletteBackgroundPixmap() )
	_thumbnailW->setPaletteBackgroundPixmap( _thumbnailW->paletteBackgroundPixmap()->convertToImage().smoothScale( _thumbnailW->size() ) );
}

void MarkListItem::paintEvent( QPaintEvent* )
{
    /* TODO:
     * We should cancel things which flipped into view and then flipped out.
     *
     * Now, if one scrolls through a 1000 page document to the end and then lingers on the
     * last pages, these will take forever to appear in thumbnail form.
     */
    if ( _requested ) return;
    if ( !_thumbnailW->paletteBackgroundPixmap() ||  _thumbnailW->paletteBackgroundPixmap()->isNull() ) {
	_miniWidget->getThumbnailService()->delayedGetThumbnail( _pageNum, this, SLOT( setPixmap( QPixmap ) ) );
	_requested = true;
    }
}


/* MarkList */

MarkList::MarkList( QWidget* parent, const char* name, KGVMiniWidget* mini)
    : QTable( parent, name ),
    _selected ( -1 ),
_miniWidget( mini )
{
    setNumCols( 1 );
    setLeftMargin( 0 ); // we don't want the vertical header
    horizontalHeader()->setLabel( 0, i18n("Page") );

    connect( this, SIGNAL( currentChanged( int, int ) ),
	    this, SIGNAL( selected( int ) ) );
}

QValueList<int> MarkList::markList() const
{
    QValueList<int> list;
    MarkListItem *_item;
    for(int i = 0; i < numRows(); i++)
    {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	if ( _item->isChecked() ) list << (i + 1);
    }
    return list;
}

void MarkList::insertItem( const QString& text, int index, const QString& tip)
{
    MarkListItem *_item;
    _item = new MarkListItem( this, text, tip, viewport()->paletteBackgroundColor(), _miniWidget, index );
    setNumRows( index + 1 );
    setCellWidget( index, 0, _item );
    setRowHeight( index, _item->height() );
}

void MarkList::select( int index )
{
    MarkListItem *_item;
    setCurrentCell( index, 0 );
    _item = dynamic_cast<MarkListItem *>( cellWidget( _selected, 0 ) );
    if (_item) _item -> setSelected( false );
    _selected = index;
    _item = dynamic_cast<MarkListItem *>( cellWidget( _selected, 0 ) );
    if (_item) _item -> setSelected( true );
    clearFocus();
}

void MarkList::clear()
{
    for ( int i = 0; i != numRows() ; ++i ) {
	clearCellWidget( i, 0 );
    }
    setNumRows( 0 );
}

void MarkList::markCurrent()
{
    MarkListItem *_item = dynamic_cast<MarkListItem *>( cellWidget( currentRow(), 0 ) );
    assert( _item );
    _item->toggle();
}

void MarkList::markAll()
{
    MarkListItem *_item;
    for(int i = 0; i < numRows(); i++)
    {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	_item->setChecked( true );
    }
}

void MarkList::markEven()
{
    MarkListItem *_item;
    for(int i = 1; i < numRows(); i = i + 2)
    {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	_item->setChecked( true );
    }
}

void MarkList::markOdd()
{
    MarkListItem *_item;
    for(int i = 0; i < numRows(); i = i + 2)
    {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	_item->setChecked( true );
    }
}

void MarkList::toggleMarks()
{
    MarkListItem *_item;
    for(int i = 0; i < numRows(); i++)
    {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	_item->toggle();
    }
}

void MarkList::removeMarks()
{
    MarkListItem *_item;
    for( int i = 0; i < numRows(); i++ ) {
	_item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	assert( _item );
	_item->setChecked( false );
    }
}

void MarkList::viewportResizeEvent ( QResizeEvent * )
{
    MarkListItem *_item;
    if( visibleWidth() != columnWidth( 0 ) )
    {
	setColumnWidth( 0, visibleWidth() );
	for( int i = 0; i < numRows(); ++i )
	{
	    _item = dynamic_cast<MarkListItem *>( cellWidget( i, 0 ) );
	    assert( _item );
	    _item->setFixedSize( visibleWidth(), _item->height() );
	}
    }
}

// vim:sw=4:sts=4:ts=8:noet
