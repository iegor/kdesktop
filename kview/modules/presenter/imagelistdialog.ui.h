/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void ImageListDialog::init()
{
    kdDebug( 4630 ) << k_funcinfo << endl;
    m_pInterval->setRange( 0, 60000, 1000 );
    noSort();
}


void ImageListDialog::noSort()
{
    kdDebug( 4630 ) << k_funcinfo << endl;
    m_pListView->setSorting( 1000 );
}


