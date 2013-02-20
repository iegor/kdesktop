/* **************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer <kiefer@kde.org>
  charDiff algorithm by Wolfram Diestel <wolfram@steloj.de>
  wordDiff algorithm by Nick Shaforostoff <shafff@ukr.net>

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

************************************************************************** */

#include "diff.h"

#include <kdebug.h>


LCSprinter::LCSprinter(const QStringList &s_1, const QStringList &s_2, QValueVector<LCSMarker> *b_, const uint nT_, uint index):s1(s_1),s2(s_2),b(b_),nT(nT_)
{
    it1=s1.begin();
    it2=s2.begin();
    printLCS(index);
}

void LCSprinter::printLCS(uint index)
{
    //fprintf(stderr,"%2d. %2d. %2d. %2d\n",(uint)(*b)[index],nT,index%nT, index);
    if (index % nT == 0 || index < nT)
    {
    //original LCS algo doesnt have to deal with ins before first common
        uint bound = index%nT;
        for (index=0; index<bound; ++index)
        {
            resultString.append("<KBABELADD>");
            resultString.append(*it2);
            ++it2;
            resultString.append("</KBABELADD>");
        }

        return;
    }

    if (ARROW_UP_LEFT == (*b)[index])
    {
        printLCS(index-nT-1);
        resultString.append(*it1);
        ++it1;
        ++it2;
        return;
    }
    else if (ARROW_UP == (*b)[index])
    {
        printLCS(index-nT);
        resultString.append("<KBABELDEL>");
        resultString.append(*it1);
        ++it1;
        resultString.append("</KBABELDEL>");
        return;
    }
    else
    {
        printLCS(index-1);
        resultString.append("<KBABELADD>");
        resultString.append(*it2);
        ++it2;
        resultString.append("</KBABELADD>");
        return;
    }

}



QString wordDiff(const QString& str1, const QString& str2)
{
    //separate punctuation marks etc from words as _only_ they may have changed
    QStringList s1, s2;

    uint i=0;
    uint j=0;
    uint l1=str1.length();
    uint l2=str2.length();
    QString temp;
    temp.reserve(16);
    /*
    while ( i<l1 )
    {
        if (str1[i]==QChar(0x003C) && (str1[i+1]==QChar(0x002F) || str1[i+1].isLetter()) ) // 0x003C="<", 0x002F="/"
        {// skip tag
            while(i<l1 && str1[i]!=QChar(0x003E)) temp += str1[i++];
            if (i++ <l1) temp += QChar(0x003E);
            s1.append(temp);
            temp.truncate(0);
        }

        while ( i<l1 && str1[i].isLetter() )
        {
            temp += str1[i++];
        }
        if (!temp.isEmpty())
        {
            s1.append(temp);
            temp.truncate(0);
        };

        if (str1[i]==QChar(0x003C) && (str1[i+1]==QChar(0x002F) || str1[i+1].isLetter()) ) // 0x003C="<", 0x002F="/"
        {// skip tag
            while(i<l1 && str1[i]!=QChar(0x003E)) temp += str1[i++];
            if (i++ <l1) temp += QChar(0x003E);
            s1.append(temp);
            temp.truncate(0);
        }

        while ( i<l1 && !(str1[i].isLetter() || str1[i]==QChar(0x003C)) )
        {
            temp += str1[i++];
        }
        if (!temp.isEmpty())
        {
            s1.append(temp);
            temp.truncate(0);
        };
    }

    i=0;
    while ( i<l2 )
    {
        if (str2[i]==QChar(0x003C) && (str2[i+1]==QChar(0x002F) || str2[i+1].isLetter()) ) // 0x003C="<", 0x002F="/"
        {// skip tag
            while(i<l2 && str2[i]!=QChar(0x003E)) temp += str2[i++];
            if (i++ <l2) temp += QChar(0x003E);
            s2.append(temp);
            temp.truncate(0);
        }

        while ( i<l2 && str2[i].isLetter() )
        {
            temp += str2[i++];
        }
        if (!temp.isEmpty())
        {
            s2.append(temp);
            temp.truncate(0);
        };
        //FIXME bounds
        if (str2[i]==QChar(0x003C) && (str2[i+1]==QChar(0x002F) || str2[i+1].isLetter()) ) // 0x003C="<", 0x002F="/"
        {// skip tag
            while(i<l2 && str2[i]!=QChar(0x003E)) temp += str2[i++];
            if (i++ <l2) temp += QChar(0x003E);
            s2.append(temp);
            temp.truncate(0);
        }

        while ( i<l2 && !(str2[i].isLetter() || str2[i]==QChar(0x003C)) )
        {
            temp += str2[i++];
        }
        if (!temp.isEmpty())
        {
            s2.append(temp);
            temp.truncate(0);
        };
    }
*/

    while ( i<l1 )
    {
        while ( i<l1 && str1[i].isLetter() )
        {
            temp += str1[i++];
        }
        if (!temp.isEmpty())
        {
            s1.append(temp);
            temp.truncate(0);
        };

        while ( i<l1 && !str1[i].isLetter() )
        {
            s1.append(QString(str1[i++]));
        }
    }

    i=0;
    while ( i<l2 )
    {
        while ( i<l2 && str2[i].isLetter() )
        {
            temp += str2[i++];
        }
        if (!temp.isEmpty())
        {
            s2.append(temp);
            temp.truncate(0);
        };

        while ( i<l2 && !str2[i].isLetter() )
        {
            s2.append(QString(str2[i++]));
        }
    }
    
    uint mX = s1.count();
    uint nY = s2.count();

    uint mT = mX+1;
    uint nT = nY+1;

    QValueVector<LCSMarker> b(mT*nT, NOTHING);
    QValueVector<uint> c(mT*nT, 0);


// calculate the LCS
    b[0] = FINAL;
    uint index_cache;
    QStringList::iterator it1, it2;

    for (i=1, it1 = s1.begin(); i<mT; ++i, ++it1)
    {
        for (j=1, it2 = s2.begin(); j<nT; ++j, ++it2)
        {
            index_cache = i*nT+j;
            if ((*it1)==(*it2))
            {
                c[index_cache] = c[index_cache-nT-1] + 1;
                b[index_cache] = ARROW_UP_LEFT;
            }
            else if (c[index_cache-nT] >= c[index_cache-1])
            {
                c[index_cache] = c[index_cache-nT];
                b[index_cache] = ARROW_UP;
            }
            else
            {
                c[index_cache] = c[index_cache-1];
                b[index_cache] = ARROW_LEFT;
            }
        }
    }

    c.clear();

 
    LCSprinter printer(s1, s2, &b, nT, index_cache);

    /*//debug
  fprintf(stderr,"b:\n");
  for (i=0; i<=mT; i++) {
  for (j=0; j<nY; j++) {
//            fprintf(stderr,"[%3d]%1d.",i*nT+j,(uint) b[i*nT+j]);
  fprintf(stderr,"%1d.",(uint) b[i*nT+j]);
}
  fprintf(stderr,"%1d\n",(uint) b[i*nT+j]);
}
  fprintf(stderr,"\n");

	// print the c table
  fprintf(stderr,"c:\n");
  for (i=0; i<=mX; i++) {
  for (j=0; j<nY; j++) {
  fprintf(stderr,"%2d.",(uint) c[i*nY+j]);
}
  fprintf(stderr,"%2d\n",(uint) c[i*nY+j]);
}
  fprintf(stderr,"\n");
  */

    return printer.getString();
}




/*
// old algorithm by Wolfram Diestel <wolfram@steloj.de>
QString Diff::charDiff(QString s1, QString s2)
{
    int n = 3;
    s1+="xxx";
    s2+="xxx";
    
    int pos1=0;
    int pos2=0;
    int len1 = s1.length();
    int len2 = s2.length();
    QString resultstr;
    bool found = true;
    bool swap = false;
    while (found && pos1+n < len1 && pos2+n < len2) 
    {
        // while strings are equal, increase positions
        int len = 0;
        while (pos1+len < len1 && pos2+len < len2 &&
                (s1[pos1+len] == s2[pos2+len]))
        {
           len++;
        }
         
        // write first result
        resultstr += s1.mid(pos1,len);
        pos1 += len;
        pos2 += len;

        // If strings are not equal try to find equal substrings of
        // length n. For this increase positions alternating like
        // 0 1, 1 0, 0 2, 1 1, 2 0, 0 3, 1 2, 2 1, 3 0 etc.

        int x = 1;
        found = false;

        // If one position is already at the end stop processing
        if (pos1+n >= len1 || pos2+n >= len2) 
        {
            break;
        }


        // evtl. exchange
        if (len2 - pos2 < len1 - pos1) 
        {
            swap=true;
            
            int pos = pos1; 
            pos1 = pos2; 
            pos2 = pos;
            
            QString s = s1; 
            s1 = s2; 
            s2 = s;
            
            int len = len1; 
            len1 = len2; 
            len2 = len;
         }

         while( !found && (pos1+x < len1 || pos2+x <len2)) 
         {
            int p1;
            int p2;
            for(p1 = 0, p2 = x; p1 <= x; p1++, p2--) 
            {
                 if( pos1+p1+n >= len1 || pos2+p2+n >= len2)
                     break;

                 if(s1.mid(pos1+p1,n) == s2.mid(pos2+p2,n))
                 {
                    found = true;

                    // evtl. exchange back
                    if(swap) 
                    {
                         int pos = pos1; 
                         pos1 = pos2; 
                         pos2 = pos;
                         
                         QString s = s1;
                         s1 = s2;
                         s2 = s;
                         
                         int len = len1;
                         len1 = len2;
                         len2 = len;
                         
                         int p = p1;
                         p1 = p2;
                         p2 = p;
                         
                         swap=false;
                    }


                    // write current result 
                    if (p1 > 0) 
                    {
                         resultstr += "<KBABELDEL>";
                         resultstr += s1.mid(pos1,p1);
                         resultstr += "</KBABELDEL>";
                         pos1 += p1;
                    }

                    if (p2 > 0 ) 
                    {
                         resultstr += "<KBABELADD>";
                         resultstr += s2.mid(pos2,p2);
                         resultstr += "</KBABELADD>";
                         pos2 += p2;
                    }

                    break;
                 }
            }
            
            x++;
            
         }

    }

    bool removed=false;
 
    // evtl. exchange back
    if(swap) 
    {
         int pos = pos1; 
         pos1 = pos2; 
         pos2 = pos;
         
         QString s = s1;
         s1 = s2;
         s2 = s;
         
         int len = len1;
         len1 = len2;
         len2 = len;
         
         swap=false;
    }
   
    // if there is a rest, add it
    if(pos1+n < len1) 
    {
         resultstr += "<KBABELDEL>";
         resultstr += s1.mid(pos1,len1-pos1-n);
         resultstr += "</KBABELDEL>";
         removed=true;
    }

    if(pos2+n < len2) 
    {
         resultstr += "<KBABELADD>";
         resultstr += s2.mid(pos2,len2-pos2-n);
         resultstr += "</KBABELADD>";
         removed=true;
    }

    if( !removed ) 
    {
        resultstr.truncate( resultstr.length()-3 );
    }

    return resultstr;
}
*/
