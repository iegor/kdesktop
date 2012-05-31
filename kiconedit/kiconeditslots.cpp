/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <qpaintdevicemetrics.h>
#include <qpainter.h>

#include <kkeydialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kprinter.h>
#include <kurldrag.h>

#include "kiconedit.h"
#include "kiconcolors.h"
#include "palettetoolbar.h"

#ifndef PICS_INCLUDED
#define PICS_INCLUDED
#include "pics/logo.xpm"
#endif

void KIconEdit::updateProperties()
{
    KIconEditProperties *props = KIconEditProperties::self();
    gridview->setShowRulers(props->showRulers());
    if(props->bgMode() == QWidget::FixedPixmap)
    {
        QPixmap pix(props->bgPixmap());
        if(pix.isNull())
        {
            QPixmap pmlogo((const char**)logo);
            pix = pmlogo;
        }
        gridview->viewPortWidget()->viewport()->setBackgroundPixmap(pix);
        m_paletteToolBar->setPreviewBackground(pix);
    }
    else
    {
        gridview->viewPortWidget()->viewport()
            ->setBackgroundColor(props->bgColor());
        m_paletteToolBar->setPreviewBackground(props->bgColor());
    }
    grid->setTransparencyDisplayType(props->transparencyDisplayType());
    grid->setTransparencySolidColor(props->transparencySolidColor());
    grid->setCheckerboardColor1(props->checkerboardColor1());
    grid->setCheckerboardColor2(props->checkerboardColor2());
    grid->setCheckerboardSize(props->checkerboardSize());
    grid->update();
}

void KIconEdit::slotNewWin(const QString & url)
{
    //kdDebug(4640) << "KIconEdit::openNewWin() - " << url << endl;

    KIconEdit *w = new KIconEdit(KURL(url), "kiconedit");
    Q_CHECK_PTR(w);
}


void KIconEdit::slotNew()
{
    bool cancel = false;
    if (grid->isModified())
    {
        int r = KMessageBox::warningYesNoCancel(this,
        i18n("The current file has been modified.\nDo you want to save it?"), QString::null, KStdGuiItem::save(), KStdGuiItem::discard());

        switch(r)
        {
            case KMessageBox::Yes:
                if(!icon->save(&grid->image()))
                {
                    cancel = true;
                }
                break;

            case KMessageBox::No:
                break;

            case KMessageBox::Cancel:
                cancel = true;
                break;

            default:
                break;
        }
    }
    if(!cancel)
    {
        KNewIcon newicon(this);
        if(newicon.exec())
        {
            int r = newicon.openStyle();
            if(r == KNewIcon::Blank)
            {
                grid->editClear();
                const QSize s = newicon.templateSize();
                //kdDebug(4640) << "Size: " << s.width() << " x " << s.height() << endl;
                grid->setSize(s);
                grid->setModified(false);
            }
            else if(r == KNewIcon::Template)
            {
                QString str = newicon.templatePath();
                icon->open(&grid->image(), KURL( str ));
            }
            icon->setUrl("");
            emit newname(i18n("Untitled"));
        }
    }
}


void KIconEdit::slotOpen()
{
    bool cancel = false;

    if( grid->isModified() )
    {
        int r = KMessageBox::warningYesNoCancel(this,
      i18n("The current file has been modified.\nDo you want to save it?"),QString::null, KStdGuiItem::save(), KStdGuiItem::discard());

        switch( r )
        {
            case KMessageBox::Yes:
                if(!icon->save( &grid->image() ))
                {
                    cancel = true;
                }
                break;

            case KMessageBox::No:
                break;

            case KMessageBox::Cancel:
                cancel = true;
                break;

            default:
                break;
        }
    }

    if( !cancel )
    {
        if (icon->promptForFile( &grid->image() ))
        {
            grid->setModified(false);
        }
    }
}

/*
    close only the current window
*/
void KIconEdit::slotClose()
{
    //kdDebug(4640) << "KIconEdit: Closing " <<  endl;
    close();
}

void KIconEdit::slotSave()
{
    //kdDebug(4640) << "KIconEdit: slotSave() " << endl;
    icon->save(&grid->image());
}


void KIconEdit::slotSaveAs()
{
    //kdDebug(4640) << "KIconEdit: slotSaveAs() " << endl;
    icon->saveAs(&grid->image());
}


void KIconEdit::slotPrint()
{
    KPrinter printer;

    if ( printer.setup(this, i18n("Print %1").arg(icon->url().section('/', -1))) )
    {
        int margin = 10, yPos = 0;
        printer.setCreator("KDE Icon Editor");

        QPainter p;
        p.begin( &printer );
        QFontMetrics fm = p.fontMetrics();
        // need width/height
        QPaintDeviceMetrics metrics( &printer );

        p.drawText( margin, margin + yPos, metrics.width(), fm.lineSpacing(),
                        ExpandTabs | DontClip, icon->url() );
        yPos = yPos + fm.lineSpacing();
        p.drawPixmap( margin, margin + yPos, grid->pixmap() );
        p.end();
  }
}

void KIconEdit::slotZoomIn()
{
    grid->zoom(DirIn);
}

void KIconEdit::slotZoomOut()
{
    grid->zoom(DirOut);
}

void KIconEdit::slotZoom1()
{
    grid->zoomTo(1);
}

void KIconEdit::slotZoom2()
{
    grid->zoomTo(2);
}

void KIconEdit::slotZoom5()
{
    grid->zoomTo(5);
}

void KIconEdit::slotZoom10()
{
    grid->zoomTo(10);
}

void KIconEdit::slotCopy()
{
    grid->editCopy();
}

void KIconEdit::slotCut()
{
    grid->editCopy(true);
}

void KIconEdit::slotPaste()
{
    static_cast<KRadioAction*>(actionCollection()
        ->action("tool_find_pixel"))->setChecked(true);
    grid->setTool(KIconEditGrid::Find);
    grid->editPaste();
}

void KIconEdit::slotClear()
{
    grid->editClear();
}

void KIconEdit::slotSelectAll()
{
    grid->setTool(KIconEditGrid::SelectRect);
    grid->editSelectAll();
}

void KIconEdit::slotOpenRecent(const KURL& iconFile)
{
    bool cancel = false;

    if( grid->isModified() )
    {
        int r = KMessageBox::warningYesNoCancel(this,
      i18n("The current file has been modified.\nDo you want to save it?"),QString::null, KStdGuiItem::save(), KStdGuiItem::discard());

        switch( r )
        {
            case KMessageBox::Yes:
                if (!icon->save( &grid->image() ))
                {
                    cancel = true;
                }
                break;

            case KMessageBox::No:
                break;

            case KMessageBox::Cancel:
                cancel = true;
                break;

            default:
                break;
        }
    }

    if( !cancel )
    {
        if(icon->open(&grid->image(), iconFile))
        {
            grid->setModified(false);
        }
    }
}

void KIconEdit::slotConfigureSettings()
{
    KIconConfig* c = new KIconConfig(this);
    c->exec();
    delete c;
}

void KIconEdit::slotConfigureKeys()
{
    KKeyDialog::configure(actionCollection());

    KIconEdit *ki = 0L;
    for (ki = windowList.first(); ki; ki = windowList.next())
    {
        if (ki != this)
        {
            ki->updateAccels();
        }
    }
}

void KIconEdit::slotShowGrid()
{
  bool b = KIconEditProperties::self()->showGrid();
  grid->setGrid( !b );
  KIconEditProperties::self()->setShowGrid( !b );
}

void KIconEdit::slotToolPointer()
{
    grid->setTool(KIconEditGrid::Find);
}

void KIconEdit::slotToolFreehand()
{
    grid->setTool(KIconEditGrid::Freehand);
}

void KIconEdit::slotToolRectangle()
{
    grid->setTool(KIconEditGrid::Rect);
}

void KIconEdit::slotToolFilledRectangle()
{
    grid->setTool(KIconEditGrid::FilledRect);
}

void KIconEdit::slotToolCircle()
{
    grid->setTool(KIconEditGrid::Circle);
}

void KIconEdit::slotToolFilledCircle()
{
    grid->setTool(KIconEditGrid::FilledCircle);
}

void KIconEdit::slotToolEllipse()
{
    grid->setTool(KIconEditGrid::Ellipse);
}

void KIconEdit::slotToolFilledEllipse()
{
    grid->setTool(KIconEditGrid::FilledEllipse);
}

void KIconEdit::slotToolSpray()
{
    grid->setTool(KIconEditGrid::Spray);
}

void KIconEdit::slotToolFlood()
{
    grid->setTool(KIconEditGrid::FloodFill);
}

void KIconEdit::slotToolLine()
{
    grid->setTool(KIconEditGrid::Line);
}

void KIconEdit::slotToolEraser()
{
    grid->setTool(KIconEditGrid::Eraser);
}

void KIconEdit::slotToolSelectRect()
{
    grid->setTool(KIconEditGrid::SelectRect);
}

void KIconEdit::slotToolSelectCircle()
{
    grid->setTool(KIconEditGrid::SelectCircle);
}

void KIconEdit::slotSaved()
{
    grid->setModified(false);
}

void KIconEdit::slotUpdateZoom( int s )
{
    m_actZoomOut->setEnabled( s>1 );
}

void KIconEdit::slotUpdateStatusPos(int x, int y)
{
    QString str = i18n("Status Position", "%1, %2").arg(x).arg(y);
    statusbar->changeItem( str, 0);
}

void KIconEdit::slotUpdateStatusSize(int x, int y)
{
    QString str = i18n("Status Size", "%1 x %2").arg(x).arg(y);
    statusbar->changeItem( str, 1);
}

void KIconEdit::slotUpdateStatusScaling(int s)
{
    KIconEditProperties::self()->setGridScale( s );
    QString str;

    str.sprintf("1:%d", s);
    statusbar->changeItem( str, 2);
}

void KIconEdit::slotUpdateStatusColors(uint)
{
    QString str = i18n("Colors: %1").arg(grid->numColors());
    statusbar->changeItem( str, 3);
}

void KIconEdit::slotUpdateStatusColors(uint n, uint *)
{
    QString str = i18n("Colors: %1").arg(n);
    statusbar->changeItem( str, 3);
}


void KIconEdit::slotUpdateStatusMessage(const QString & msg)
{
    statusbar->changeItem( msg, 4);
}


void KIconEdit::slotUpdateStatusName(const QString & name)
{
    m_name = name;

    QString text = m_name;

    if(grid->isModified())
    {
        text += " ["+i18n("modified")+"]";
    }

    setCaption(text);
}


void KIconEdit::slotUpdateStatusModified(bool)
{
    slotUpdateStatusName(m_name);
}

void KIconEdit::slotUpdatePaste(bool state)
{
    m_actPaste->setEnabled(state);
    m_actPasteNew->setEnabled(state);
}


void KIconEdit::slotUpdateCopy(bool state)
{
    m_actCopy->setEnabled(state);
    m_actCut->setEnabled(state);
}


void KIconEdit::slotOpenBlank(const QSize s)
{
    grid->loadBlank( s.width(), s.height());
}


void KIconEdit::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(KURLDrag::canDecode(e));
}


/*
    accept drop of a file - opens file in current window
    old code to drop image, as image, should be removed
*/
void KIconEdit::dropEvent( QDropEvent *e )
{
    //kdDebug(4640) << "Got QDropEvent!" << endl;

    KURL::List fileList;
    bool loadedinthis = false;

    if(KURLDrag::decode(e, fileList))
    {
        for(KURL::List::ConstIterator it = fileList.begin(); 
            it != fileList.end(); ++it)
        {
            //kdDebug(4640) << "In dropEvent for "  <<  (*it).prettyURL() << endl;
            const KURL &url = *it;
            if(url.isValid())
            {
                if (!grid->isModified() && !loadedinthis)
                {
                    icon->open(&grid->image(), url);
                    loadedinthis = true;
                }
                else
                {
                    slotNewWin(url.url());
                }
            }
        }
    }
}


