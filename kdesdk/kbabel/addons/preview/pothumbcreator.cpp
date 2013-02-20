/*  
  This file is part of KBabel
  Copyright (C) 2001 Matthias Kiefer <kiefer@kde.org>

  Text painting code is based on the text file preview textthumbnail by
  Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org> 
                2000 Malte Starostik <malte@kde.org> 

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
  
*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>

#include <kapplication.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kpixmapsplitter.h>

#include "pothumbcreator.h"
#include "poinfo.h"

using namespace KBabel;

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new PoThumbCreator;
    }
}

PoThumbCreator::PoThumbCreator()
    : m_splitter(0)
{
}

PoThumbCreator::~PoThumbCreator()
{
    if(m_splitter)
        delete m_splitter;
}

bool PoThumbCreator::create(const QString &path, int width, int height, QImage &img)
{
    if ( !m_splitter )
    {
        m_splitter = new KPixmapSplitter;
        QString pixmap = locate( "data", "konqueror/pics/thumbnailfont_7x4.png" );
        if ( !pixmap.isEmpty() )
        {
            m_splitter->setPixmap( QPixmap( pixmap ));
            m_splitter->setItemSize( QSize( 4, 7 ));
        }
    }

    bool ok = false;
    PoInfo poInfo;
    QStringList wordList;

    // We do not call msgfmt, as a thumbnail must be created fast.
    if( PoInfo::info( path, poInfo, wordList, false, true, false ) == OK )
    {
       ok = true;
       
       QPixmap pix;
       if (height * 3 > width * 4)
           pix.resize(width, width * 4 / 3);
       else
           pix.resize(height * 3 / 4, height);

       pix.fill( QColor( 245, 245, 245 ) ); // light-grey background

      
       
       // one pixel for the rectangle, the rest. whitespace
       int xBorder = 1 + pix.width()/16;    // minimum x-border
       int yBorder = 1 + pix.height()/16; // minimum y-border

       int circle = 16*360;
       int fuzzyAngle = poInfo.fuzzy*circle/poInfo.total;
       int untransAngle = poInfo.untranslated*circle/poInfo.total;
       
       int w = pix.width()-2*xBorder;
       int h = pix.height()*2/3-2*yBorder;

       int d = QMIN(w,h);
       xBorder = (pix.width()-d)/2;
       yBorder = (pix.height()*2/3-d)/2;
       
       QPainter p(&pix);
       
       if(fuzzyAngle>0)
       {
           p.setBrush(Qt::blue);
           
           if(poInfo.total == poInfo.fuzzy)
           {
               p.drawEllipse(xBorder,yBorder,d,d);
           }
           else
           {
               p.drawPie(xBorder,yBorder, d, d, 0, -fuzzyAngle);
           }
       }
       
       if(untransAngle>0)
       {
           p.setBrush(Qt::red);

           if(poInfo.untranslated == poInfo.total)
           {
               p.drawEllipse(xBorder,yBorder,d,d);
           }
           else
           {
               p.drawPie(xBorder,yBorder, d, d, -fuzzyAngle, -untransAngle);
           }
       }
       
       if(circle - fuzzyAngle - untransAngle>0)
       {
           p.setBrush(Qt::darkGreen);

           if(poInfo.fuzzy==0 && poInfo.untranslated==0)
           {
               p.drawEllipse(xBorder,yBorder,d,d);
           }
           else
           {
               p.drawPie(xBorder,yBorder, d, d, -fuzzyAngle-untransAngle
               , -(circle-fuzzyAngle-untransAngle) );
           }
       }
       
       p.end();
       
       

       QRect rect;

       QSize chSize = m_splitter->itemSize(); // the size of one char
       int xOffset = chSize.width();
       int yOffset = chSize.height();

       // one pixel for the rectangle, the rest. whitespace
       xBorder = 1 + pix.width()/16;    // minimum x-border
       yBorder = 1 + pix.height()/16; // minimum y-border

       // calculate a better border so that the text is centered
       int canvasWidth = pix.width() - 2*xBorder;
       int canvasHeight = pix.height()/3 -  2*yBorder;
       int numCharsPerLine = (int) (canvasWidth / chSize.width());
       int numLines = (int) (canvasHeight / chSize.height());

        int rest = pix.width() - (numCharsPerLine * chSize.width());
        xBorder = QMAX( xBorder, rest/2); // center horizontally
        rest = pix.height()/3 - (numLines * chSize.height());
        yBorder = QMAX( yBorder, rest/2); // center vertically
        // end centering


        QString text;
        
        if(numCharsPerLine < 30)
        {
            if(!poInfo.revision.isEmpty())
                text += poInfo.revision+'\n';
            if(!poInfo.lastTranslator.isEmpty())
                text += poInfo.lastTranslator+'\n';
            if(!poInfo.languageTeam.isEmpty())
                text += poInfo.languageTeam+'\n';
            if(!poInfo.contentType.isEmpty())
                text += poInfo.contentType+'\n';
            if(!poInfo.creation.isEmpty())
                text += poInfo.creation+'\n';
            if(!poInfo.project.isEmpty())
                text += poInfo.project+'\n';
            if(!poInfo.mimeVersion.isEmpty())
                text += "MIME-Version: "+poInfo.mimeVersion+'\n';
            if(!poInfo.encoding.isEmpty())
                text += poInfo.encoding+'\n';
 
            int lines = text.contains('\n')+1;
            if(lines < numLines)
            {
                text = poInfo.headerComment+'\n'+text;
                if(!poInfo.others.isEmpty())
                    text += poInfo.others+'\n';
            }

            if(text.at(text.length()-1) == '\n')
                text.truncate(text.length()-1);
        }
        else
        {   
            if(!poInfo.headerComment.isEmpty())
                text += poInfo.headerComment+'\n';
            if(!poInfo.project.isEmpty())
                text += "Project-Id-Version: "+poInfo.project+'\n';
            if(!poInfo.creation.isEmpty())
                text += "POT-Creation-Date: "+poInfo.creation+'\n';
            if(!poInfo.revision.isEmpty())
                text += "PO-Revision-Date: "+ poInfo.revision+'\n';
            if(!poInfo.lastTranslator.isEmpty())
                text += "Last-Translator: "+poInfo.lastTranslator+'\n';
            if(!poInfo.languageTeam.isEmpty())
                text += "Language-Team: "+poInfo.languageTeam+'\n';
            if(!poInfo.contentType.isEmpty())
                text += "Content-Type: "+poInfo.contentType+'\n';
            if(!poInfo.mimeVersion.isEmpty())
                text += "MIME-Version: "+poInfo.mimeVersion+'\n';
            if(!poInfo.encoding.isEmpty())
                text += "Content-Transfer-Encoding: "+poInfo.encoding+'\n';
            if(!poInfo.others.isEmpty())
                text += poInfo.others;


            if(text.at(text.length()-1) == '\n')
                text.truncate(text.length()-1);
        }
               /*
            double p = ((double)(poInfo.total-poInfo.fuzzy-poInfo.untranslated))*100/poInfo.total;
            text = locale->formatNumber(p,0)+'%';
        */
        // find the maximum string length to center the text
        QStringList lineList=QStringList::split('\n',text);
        uint max=0;
        for( QStringList::Iterator it = lineList.begin(); it != lineList.end()
                ; ++it )
        {
            if((*it).length() > max)
                max = (*it).length();
        }        
        rest = pix.width() - (max * chSize.width());
        xBorder = QMAX( xBorder, rest/2); // center horizontally
        rest = pix.height()/3 - (lineList.count() * chSize.height());
        yBorder = QMAX( yBorder, rest/2); // center vertically

        // where to paint the characters
        int x = xBorder, y = pix.height()*2/3; 
        int posNewLine  = pix.width() - (chSize.width() + xBorder);
        int posLastLine = pix.height() - (chSize.height() + yBorder);
        bool newLine = false;
        Q_ASSERT( posNewLine > 0 );
        const QPixmap *fontPixmap = &(m_splitter->pixmap());


        for ( uint i = 0; i < text.length(); i++ )
        {
            if ( x > posNewLine || newLine ) // start a new line?
            {
                x = xBorder;
                y += yOffset;

                if ( y > posLastLine ) // more text than space
                    break;

                // after starting a new line, we also jump to the next
                // physical newline in the file if we don't come from one
                if ( !newLine )
                {
                    int pos = text.find( '\n', i );
                    if ( pos > (int) i )
                        i = pos +1;
                }

                newLine = false;
           }

           // check for newlines in the text (unix,dos)
           QChar ch = text.at( i );
           if ( ch == '\n' )
           {
               newLine = true;
               continue;
           }
           else if ( ch == '\r' && text.at(i+1) == '\n' )
           {
               newLine = true;
               i++; // skip the next character (\n) as well
               continue;
           }
                
           rect = m_splitter->coordinates( ch );
           if ( !rect.isEmpty() )
           {
               bitBlt( &pix, QPoint(x,y), fontPixmap, rect, Qt::CopyROP );
           }

           x += xOffset; // next character
       }

        // very very seldom a babelfish lives in po files and even
        // in this seldom cases they are usually hidden ;-)
        if(pix.width() > 40 && KApplication::random()%2000 == 0)
        {
           QPixmap kbabelPix; 
           if(pix.width() < 80)
           {
               kbabelPix = KGlobal::iconLoader()->loadIcon("kbabel"
                       ,KIcon::Small,16,KIcon::DefaultState,0,true);
           }
           else if(pix.width() < 150)
           {
               kbabelPix = KGlobal::iconLoader()->loadIcon("kbabel"
                       ,KIcon::Desktop,32,KIcon::DefaultState,0,true);
           }
           else
           {
               kbabelPix = KGlobal::iconLoader()->loadIcon("kbabel"
                       ,KIcon::Desktop,48,KIcon::DefaultState,0,true);
           }
           
           int x = pix.width()-kbabelPix.width()-4;
           x = QMAX(x,0);
           if(!kbabelPix.isNull())
           {
               bitBlt(&pix, QPoint(x,4), &kbabelPix, kbabelPix.rect()
                       , Qt::CopyROP);
           }
        }

       img = pix.convertToImage();
    }

    return ok;
}

ThumbCreator::Flags PoThumbCreator::flags() const
{
    return (Flags)(DrawFrame);
}

