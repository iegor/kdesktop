/**************************************************************************

    kdisptext.cpp  - The widget that displays the karaoke/lyrics text 
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
#include "kdisptext.h"
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qrect.h>
#include <qtextcodec.h>

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kinstance.h>
#include <klocale.h>
#include "version.h"


#define NUMPREVLINES 2
//#define DRAW_BOUNDING_RECTS


KDisplayText::KDisplayText(QWidget *parent,const char *name) : QScrollView(parent,name)
{
  first_line_[0]=first_line_[1]=NULL;
  linked_list_[0]=linked_list_[1]=NULL;
  cursor_line_[0]=cursor_line_[1]=NULL;
  cursor_[0]=cursor_[1]=NULL;
  nlines_[0]=nlines_[1]=0;
  linked_list=NULL;
  cursor_line=NULL;
  first_line=NULL;
  cursor=NULL;
  nlines=0;
  lyrics_codec=KGlobal::locale()->codecForEncoding();

  viewport()->setBackgroundColor(QColor (110,110,110));
//  setBackgroundMode(NoBackground);
  KConfig *kcfg=KGlobal::instance()->config();
  kcfg->setGroup("KMid");
  typeoftextevents=kcfg->readNumEntry("TypeOfTextEvents",1);
  QFont *qtextfontdefault=new QFont(KGlobalSettings::fixedFont().family(),22);
  qtextfont=new QFont(kcfg->readFontEntry("KaraokeFont",qtextfontdefault));
  delete qtextfontdefault;
  qfmetr=new QFontMetrics(*qtextfont);
  nvisiblelines=height()/qfmetr->lineSpacing();
  autoscrollv=0;
}

KDisplayText::~KDisplayText()
{
  RemoveLinkedList();
}

/*void KDisplayText::PreDestroyer(void)
{
  delete qfmetr;
  delete qtextfont;
}
*/

void KDisplayText::RemoveLine(kdispt_line *tmpl)
{
  kdispt_ev *tmpe;
  while (tmpl->ev!=NULL)
  {
    tmpe=tmpl->ev;
    tmpl->ev=tmpe->next;
    //    delete tmpe->spev; Remember that the Special Events that this pointer
    //		 points to is the same that the Player object has instantiated
    delete tmpe;
  }
}

void KDisplayText::RemoveLinkedList(void)
{
  cursor=NULL;
  cursor_line=NULL;
  first_line=NULL;
  linked_list=NULL;
  nlines=0;
  nlines_[0]=nlines_[1]=0;
  first_line_[0]=first_line_[1]=NULL;
  cursor_line_[0]=cursor_line_[1]=NULL;
  cursor_[0]=cursor_[1]=NULL;

  kdispt_line *tmpl;
  for (int i=0;i<2;i++)
  {
    while (linked_list_[i]!=NULL)
    {
      RemoveLine(linked_list_[i]);
      tmpl=linked_list_[i];
      linked_list_[i]=linked_list_[i]->next;
      delete tmpl;
    }
  }
}

void KDisplayText::ClearEv(bool totally)
{
  RemoveLinkedList();
  if (totally)
  {
    killTimers();
    autoscrollv=0;
    resizeContents(0,0);
    viewport()->repaint(TRUE);
  }
}

int KDisplayText::IsLineFeed(char c,int type)
{
  switch (type)
  {
    case (1) : if (/*(c==0)||*/(c=='\\')||(c=='/')||(c=='@')) return 1;break;
    case (5) : if (/*(c==0)||*/(c==10)||(c==13)) return 1;break;
    default :  if ((c==0)||(c==10)||(c==13)||(c=='\\')||(c=='/')||(c=='@')) return 1;break;
  }
  return 0;
}

void KDisplayText::AddEv(SpecialEvent *ev)
{
  if ((ev->type==1) || (ev->type==5))
  {
    int idx=(ev->type==1)? 0 : 1;
    if (linked_list_[idx]==NULL)
    {
      linked_list_[idx]=new kdispt_line;
      linked_list_[idx]->next=NULL;
      linked_list_[idx]->num=1;
      linked_list_[idx]->ev=new kdispt_ev;
      cursor_line_[idx]=linked_list_[idx];
      cursor_[idx]=cursor_line_[idx]->ev;
      cursor_[idx]->spev=ev;
      cursor_[idx]->next=NULL;    
      first_line_[idx]=linked_list_[idx];
      first_line=first_line_[idx];
      nlines_[idx]=1;
    }
    else
    {
      if (IsLineFeed(ev->text[0],ev->type))
      {
	nlines_[idx]++;
	cursor_line_[idx]->next=new kdispt_line;
	cursor_line_[idx]=cursor_line_[idx]->next;
	cursor_line_[idx]->num=nlines_[idx];
	cursor_line_[idx]->ev=new kdispt_ev;
	cursor_line_[idx]->next=NULL;
	cursor_[idx]=cursor_line_[idx]->ev;
      }
      else
      {
	cursor_[idx]->next=new kdispt_ev;
	cursor_[idx]=cursor_[idx]->next;
      }
      cursor_[idx]->spev=ev;
      cursor_[idx]->next=NULL;
    }
  }
}

void KDisplayText::calculatePositions(void)
{
    int typeoftextevents=1;
    int fin=0;
    kdispt_line *tmpl;
    kdispt_ev *tmp;
    int tmpx=0;
    int tmpy=0;
    int tmpw=0;
    int maxx=0;
    nlines=nlines_[(typeoftextevents==1)? 0:1];
    int lineSpacing=qfmetr->lineSpacing();
    int descent=qfmetr->descent();
    while (!fin)
    {
        tmpl=linked_list_[(typeoftextevents==1)?0:1];
        tmpy=lineSpacing;
        maxx=0;
        while (tmpl!=NULL)
        {
            tmp=tmpl->ev;
            tmpx=5;
            while (tmp!=NULL)
            {
                if (tmp->spev->type==typeoftextevents)
                {
                    if (IsLineFeed(tmp->spev->text[0],typeoftextevents))
                    {
                        tmpy+=lineSpacing;
                        tmpx=5;

                        tmp->xpos=tmpx;
                        if (tmp->spev->text[0]!=0) tmpw=qfmetr->width(lyrics_codec->toUnicode(&tmp->spev->text[1]));
				else tmpw=0;
                        tmp->r=qfmetr->boundingRect(lyrics_codec->toUnicode(&tmp->spev->text[1]));
                    }
                    else
                    {
                        tmp->xpos=tmpx;
                        tmpw=qfmetr->width(lyrics_codec->toUnicode(tmp->spev->text));
                        tmp->r=qfmetr->boundingRect(lyrics_codec->toUnicode(tmp->spev->text));
                    }
	// We add 5 pixels above, below and to the right because of a
	// problem with latest released Xft
                    tmp->r.moveBy(tmpx,tmpy-tmp->r.height()-tmp->r.y()-5);
		    tmp->r.setHeight(tmp->r.height()+descent+10);
		    tmp->r.setWidth(tmp->r.width()+5);
                    tmpx+=tmpw;
		    if (tmpx>maxx) maxx=tmpx;
                }
                tmp=tmp->next;
            }
            tmpl->ypos=tmpl->num*lineSpacing;
            tmpy=tmpl->ypos;
            tmpl=tmpl->next;
        }
        maxX[(typeoftextevents==1)?0:1]=maxx+10;
        maxY[(typeoftextevents==1)?0:1]=
		nlines_[(typeoftextevents==1)?0:1]*lineSpacing+descent+10;

        if (typeoftextevents==1) typeoftextevents=5;
        else fin=1;
    }

}

kdispt_line *KDisplayText::searchYOffset(int y, kdispt_line *start)
{
kdispt_line *t=start;
while (t!=NULL)
   {
//   if (t->ypos+qfmetr->descent()>y) return start;
   if (t->ypos+qfmetr->descent()+20>y) return start;
   start=t;
   t=t->next;
   };
return start;
}

void KDisplayText::drawContents(QPainter *p, int /*clipx*/, int clipy, int /*clipw*/, int cliph)
{
    p->setFont(*qtextfont);
    if (linked_list==NULL) return;
    int i=0;
    p->setPen(yellow);
    int colorplayed=1;
    if (cursor==NULL) colorplayed=0; // Thus, the program doesn't change the color
    kdispt_line *tmpl=linked_list;
    kdispt_ev *tmp;

#ifdef KDISPTEXTDEBUG
    printf("events displayed %d\n",typeoftextevents);
#endif

    tmpl=searchYOffset(clipy,linked_list);
    int nlinestodraw=1;
    kdispt_line *t=tmpl;
    while ((t!=NULL)&&(t->ypos+qfmetr->descent()<clipy+cliph))
       {
       nlinestodraw++;
       t=t->next;
       }

    i=0;
    while ((i<nlinestodraw)&&(tmpl!=NULL))
    {
        tmp=tmpl->ev;
        while ((tmp!=NULL)&&(tmp->spev->type!=typeoftextevents)) tmp=tmp->next;
        while (tmp!=NULL)
        {
            if ( colorplayed &&
                 //	(tmp->spev->absmilliseconds>=cursor->spev->absmilliseconds))
                 (tmp->spev->id>=cursor->spev->id))
            {
                p->setPen(black);
                colorplayed=0;
            }

            if (IsLineFeed(tmp->spev->text[0],tmp->spev->type))
                p->drawText(tmp->xpos,tmpl->ypos,lyrics_codec->toUnicode(&tmp->spev->text[1]));
            else
                p->drawText(tmp->xpos,tmpl->ypos,lyrics_codec->toUnicode(tmp->spev->text));

#ifdef DRAW_BOUNDING_RECTS
	    p->setPen(red);
	    p->drawRect(tmp->r);
 	    p->setPen((colorplayed)?yellow:black);
#endif
            tmp=tmp->next;
            while ((tmp!=NULL)&&(tmp->spev->type!=typeoftextevents)) tmp=tmp->next;
        }
        i++;
        tmpl=tmpl->next;
    }

}


void KDisplayText::resizeEvent(QResizeEvent *e)
{
    QScrollView::resizeEvent(e);
    nvisiblelines=visibleHeight()/qfmetr->lineSpacing();
    if ( (nlines>nvisiblelines) || (nvisiblelines==0) )
        resizeContents(maxX[(typeoftextevents==1)?0:1],maxY[(typeoftextevents==1)?0:1]);
    else
        resizeContents(0,0);

}

void KDisplayText::CursorToHome(void)
{
/*    KConfig *kcfg=KGlobal::instance()->config();
    kcfg->setGroup("KMid");
    typeoftextevents=kcfg->readNumEntry("TypeOfTextEvents",1);
*/
    linked_list=linked_list_[(typeoftextevents==1)? 0:1];
    nlines=nlines_[(typeoftextevents==1)? 0:1];
    cursor_line_[0]=linked_list_[0];
    first_line_[0]=cursor_line_[0];
    if (cursor_line_[0]!=NULL) cursor_[0]=cursor_line_[0]->ev;
    cursor_line_[1]=linked_list_[1];
    first_line_[1]=cursor_line_[1];
    if (cursor_line_[1]!=NULL) cursor_[1]=cursor_line_[1]->ev;
    
    if (linked_list==NULL)
    {
        cursor_line=NULL;
        cursor=NULL;
        first_line=NULL;
    }
    else
    {
        cursor_line=linked_list;
        cursor=cursor_line->ev;
        first_line=linked_list;
    }

    nvisiblelines=visibleHeight()/qfmetr->lineSpacing();
    if (nlines>nvisiblelines) 
      resizeContents(maxX[(typeoftextevents==1)?0:1],maxY[(typeoftextevents==1)?0:1]);
    else
      resizeContents(0,0); 


    setContentsPos(0,0);
    viewport()->repaint(true);
} 

void KDisplayText::PaintIn(int type)
{
  bool paint=false;
  if (type!=typeoftextevents)
  {
    int idx=(type==1)?0:1;
    if (cursor_[idx]==NULL) return;
    cursor_[idx]=cursor_[idx]->next;
    while ((cursor_[idx]==NULL)&&(cursor_line_[idx]!=NULL))
    {
      cursor_line_[idx]=cursor_line_[idx]->next;
      if (cursor_line_[idx]!=NULL)
      {
	cursor_[idx]=cursor_line_[idx]->ev;
	if ((cursor_line_[idx]->num>first_line_[idx]->num+NUMPREVLINES)
	    &&(cursor_line_[idx]->num<first_line_[idx]->num+nvisiblelines+1))
	  if ((first_line_[idx]!=NULL)&&(first_line_[idx]->num+nvisiblelines<=nlines_[idx])) first_line_[idx]=first_line_[idx]->next;
      }
    }
    return;
  }

  if ((cursor==NULL)||(cursor_line==NULL))
  {
    printf("KDispT : cursor == NULL !!!\n");
    return;
  }

  kdispt_ev *tmp=cursor;
  if (cursor->spev->type==typeoftextevents)
  {
//    int x,y;
//    contentsToViewport(cursor->xpos,cursor_line->ypos,x,y);

    cursor=cursor->next;
    paint=true;

  }
  else 
    cursor=cursor->next;


  while ((cursor==NULL)&&(cursor_line!=NULL))
  {
    cursor_line=cursor_line->next;
    if (cursor_line!=NULL) 
    {
      cursor=cursor_line->ev;
      if ((cursor_line->ypos>contentsY()+(visibleHeight()*5/8))&&
	  (cursor_line->ypos<contentsY()+visibleHeight()+autoscrollv))
        {
	bool b=(autoscrollv==0);	
	autoscrollv+=qfmetr->lineSpacing();
	if (b) startTimer(100);
	else {
	   killTimers();
	   startTimer(100/(autoscrollv/qfmetr->lineSpacing()+1));
	}
	}
//	scrollBy(0,qfmetr->lineSpacing());
    }
  }
  if (paint) repaintContents(tmp->r);
}

void KDisplayText::gotomsec(ulong i)
{
    int notidx=(typeoftextevents==1)?1:0;
    
    if (linked_list_[notidx]!=NULL) 
    {
        cursor_line_[notidx]=linked_list_[notidx];
        first_line_[notidx]=cursor_line_[notidx];
        cursor_[notidx]=cursor_line_[notidx]->ev;
        while ((cursor_line_[notidx]!=NULL)&&(cursor_[notidx]->spev->absmilliseconds<i))
        {
            cursor_[notidx]=cursor_[notidx]->next;
            while ((cursor_[notidx]==NULL)&&(cursor_line_[notidx]!=NULL))
            {
                cursor_line_[notidx]=cursor_line_[notidx]->next;
                if (cursor_line_[notidx]!=NULL) 
                {
                    cursor_[notidx]=cursor_line_[notidx]->ev;
                    if ((cursor_line_[notidx]->num>first_line_[notidx]->num+NUMPREVLINES)
                        &&(cursor_line_[notidx]->num<first_line_[notidx]->num+nvisiblelines+1))
                        if ((first_line_[notidx]!=NULL)&&(first_line_[notidx]->num+nvisiblelines<=nlines_[notidx])) first_line_[notidx]=first_line_[notidx]->next;
                }
            }
        }
    }
    
    if (linked_list!=NULL) 
    {
        cursor_line=linked_list;
        cursor=cursor_line->ev;
        first_line=linked_list;
        while ((cursor_line!=NULL)&&(cursor->spev->absmilliseconds<i))
        {
            cursor=cursor->next;
            while ((cursor==NULL)&&(cursor_line!=NULL))
            {
                cursor_line=cursor_line->next;
                if (cursor_line!=NULL) 
                {
                    cursor=cursor_line->ev;
                    if ((cursor_line->num>first_line->num+NUMPREVLINES)
                        &&(cursor_line->num<first_line->num+nvisiblelines+1))
                        if ((first_line!=NULL)&&(first_line->num+nvisiblelines<=nlines)) first_line=first_line->next;
                }
            }
            
            
        }
        
	killTimers();
	autoscrollv=0;
        setContentsPos(0,first_line->ypos);
        viewport()->repaint();
        
    }
}

QFont *KDisplayText::getFont(void)
{
    return qtextfont;
}

void KDisplayText::fontChanged(void)
{
    KConfig *kcfg=KGlobal::instance()->config();
    kcfg->setGroup("KMid");
    QFont *qtextfontdefault=new QFont(*qtextfont);
    delete qtextfont;
    qtextfont=new QFont(kcfg->readFontEntry("KaraokeFont",qtextfontdefault));
    delete qtextfontdefault;
    qfmetr=new QFontMetrics(*qtextfont);
    calculatePositions();
    nvisiblelines=height()/qfmetr->lineSpacing();
    viewport()->repaint(TRUE);
}

void KDisplayText::ChangeTypeOfTextEvents(int type)
{
    int idxold=(typeoftextevents==1)?0:1;
    int idxnew=(type==1)?0:1;
    cursor_line_[idxold]=cursor_line;
    first_line_[idxold]=first_line;
    cursor_[idxold]=cursor;
    linked_list=linked_list_[idxnew];
    cursor_line=cursor_line_[idxnew];
    first_line=first_line_[idxnew];
    cursor=cursor_[idxnew];
    nlines=nlines_[idxnew];
    typeoftextevents=type;
    if (first_line!=NULL) 
    {
        nvisiblelines=height()/qfmetr->lineSpacing();
        if (nlines>nvisiblelines)
            resizeContents(maxX[(typeoftextevents==1)?0:1],maxY[(typeoftextevents==1)?0:1]);
        else
            resizeContents(0,0);

        setContentsPos(0,first_line->ypos);
    }
    viewport()->repaint(TRUE);
}

int KDisplayText::ChooseTypeOfTextEvents(void)
{
    return (nlines_[0]>nlines_[1])? 1 : 5;
}

void KDisplayText::ScrollDown()
{
    scrollBy(0,2/**qfmetr->lineSpacing()*/);
}

void KDisplayText::ScrollUp()
{
    scrollBy(0,-2/**qfmetr->lineSpacing()*/);
}

void KDisplayText::ScrollPageDown()
{
    scrollBy(0,nvisiblelines*qfmetr->lineSpacing());
}

void KDisplayText::ScrollPageUp()
{
    scrollBy(0,-nvisiblelines*qfmetr->lineSpacing());
}

void KDisplayText::saveLyrics(FILE *fh)
{
    kdispt_line *Lptr=linked_list_[(typeoftextevents==1)? 0:1];
    while (Lptr!=NULL)
    {
        kdispt_ev *Cptr=Lptr->ev;
        if (Cptr!=NULL)
        {
            if (strcmp(Cptr->spev->text,"")!=0) 
                if (IsLineFeed(Cptr->spev->text[0],Cptr->spev->type))
                    fputs(&Cptr->spev->text[1],fh);
                else
                    fputs(Cptr->spev->text,fh);
            Cptr=Cptr->next;
        }
        while (Cptr!=NULL)
        {
            fputs(Cptr->spev->text,fh);
            Cptr=Cptr->next;
        }
        fputs("\n",fh);
        Lptr=Lptr->next;
    }
}

void KDisplayText::timerEvent(QTimerEvent *e)
{
    int dy;
    if (autoscrollv>0)
    {
    dy=2+autoscrollv/50;
    scrollBy(0,dy);
    autoscrollv-=dy;
    if (autoscrollv<0) 
	{
	killTimer(e->timerId());
	autoscrollv=0;
	}
    }
    else
    if (autoscrollv<0)
    {
//    dy=(autoscrollv<-2*qfmetr->lineSpacing())?-7:-2;
    dy=-2+autoscrollv/50;
    scrollBy(0,dy);
    autoscrollv-=dy;
    if (autoscrollv>0) 
	{
	killTimer(e->timerId());
	autoscrollv=0;
	}
    }
    else
	killTimer(e->timerId());
    
}

void KDisplayText::setLyricsEncoding(const QString &enc)
{
  QTextCodec *newcodec;
  if (enc.isEmpty())
    newcodec=KGlobal::locale()->codecForEncoding();
  else
    newcodec=QTextCodec::codecForName(enc.latin1()); 

  if (newcodec!=lyrics_codec)
  {
    if (newcodec)
    {
      lyrics_codec=newcodec;
      fontChanged();
    }
  }      
}

#include "kdisptext.moc"
