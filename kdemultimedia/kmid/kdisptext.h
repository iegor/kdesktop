/**************************************************************************

    kdisptext.h  - The widget that displays the karaoke/lyrics text
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _kdisptext_h_
#define _kdisptext_h_

#include <qwidget.h>
#include <qscrollview.h>
#include <libkmid/player.h>

struct kdispt_ev 
{
    SpecialEvent *spev;
    int xpos;
    class QRect r;
    kdispt_ev *next;
};

struct kdispt_line
{
    kdispt_ev *ev;
    int num;
    int ypos;
    kdispt_line *next;
};

class QFont;
class QFontMetrics;
class QScrollBar;

class KDisplayText : public QScrollView 
{
    Q_OBJECT
private:
    QTextCodec *lyrics_codec;    

    QFontMetrics *qfmetr;
    QFont *qtextfont;
    
    int typeoftextevents;
    
    kdispt_line *linked_list_[2];
    int nlines_[2];
    kdispt_line *first_line_[2];
    kdispt_line *cursor_line_[2];
    kdispt_ev *cursor_[2];
    
    kdispt_line *linked_list; 
    kdispt_line *cursor_line;
    kdispt_ev *cursor;
    
    kdispt_line *first_line; // Pointer to first text at first visible line
    
    int nlines;     // Total number of lines
    int nvisiblelines; // Number of visible lines
    
    void RemoveLine(kdispt_line *tmpl);
    void RemoveLinkedList(void);
    
    int IsLineFeed(char c,int type=0);
    virtual void drawContents(QPainter *qpaint,int clipx,int clipy,int clipw,int cliph);

    kdispt_line * searchYOffset(int y, kdispt_line *start);
    
    int maxX[2];
    int maxY[2];
    int autoscrollv;

protected:
    virtual void resizeEvent(QResizeEvent *qre);
    virtual void timerEvent(QTimerEvent *e);
    
public:
    KDisplayText(QWidget *parent,const char *name);
    virtual ~KDisplayText();
    
    void ClearEv(bool totally=true);
    void AddEv(SpecialEvent *ev);
    void calculatePositions(void);
    
    void CursorToHome(void);
    
    void ChangeTypeOfTextEvents(int type); 
    int ChooseTypeOfTextEvents(void);
    void PaintIn(int type);
    void gotomsec(ulong i);
    
    QFont *getFont(void);
    void fontChanged(void);
    
    void saveLyrics(FILE *fh);
    
    void setLyricsEncoding(const QString &enc);

//    QSize sizeHint();
 
public slots:
    void ScrollDown();
    void ScrollUp();
    void ScrollPageDown();
    void ScrollPageUp();
    
};

#endif
