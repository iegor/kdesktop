/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002	  by Stanislav Visnovsky <visnovsky@kde.org>

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

**************************************************************************** */
#include "catalog.h"
#include "catmanlistitem.h"
#include "multiroughtransdlg.h"

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kurl.h>

using namespace KBabel;

MultiRoughTransDlg::MultiRoughTransDlg(KBabelDictBox *dict, QPtrList<CatManListItem> files
                , QWidget *parent,const char *name)
        : RoughTransDlg(dict, new Catalog(), parent, name )
        ,_fileList(files)
{
    QWidget* bars = static_cast<QWidget*>(progressbar->parent());
    QLabel* label = new QLabel( i18n("Files:"), bars );
    filesProgressbar = new KProgress(bars,"files progressbar");
    filesProgressbar->setTextEnabled(true);
    filesProgressbar->setFormat("%v/%m (%p%)");
    filesProgressbar->setTotalSteps(files.count());
    QHBoxLayout* mylayout= new QHBoxLayout(bars->layout());
    mylayout->add(label);
    mylayout->add(filesProgressbar); 
    
    msgButtonClicked(0);
}

void MultiRoughTransDlg::msgButtonClicked(int id)
{
    RoughTransDlg::msgButtonClicked(id);
    
    enableButton(User1,true);
}

void MultiRoughTransDlg::translate()
{
    for ( CatManListItem* it = _fileList.first(); it ; it = _fileList.next() )
    {
	if( it->hasPo() )
	{
	    KURL url( it->poFile() );
	    if( catalog->openURL( url ) != OK )
	    {
		KMessageBox::error(this, i18n("Error while trying to read file:\n %1\n"
		    "Maybe it is not a valid PO file.").arg(url.prettyURL()));
		filesProgressbar->advance(1);
		continue;
	    }
	} else
	if( it->hasPot() )
	{
	    KURL url( it->poFile() );
	    KURL poturl( it->potFile() );
	    if( catalog->openURL( poturl, url ) != OK )
	    {
		KMessageBox::error(this, i18n("Error while trying to read file:\n %1\n"
		    "Maybe it is not a valid PO file.").arg(poturl.prettyURL()));
		filesProgressbar->advance(1);
		continue;
	    }
	}
	
	RoughTransDlg::translate();

	if( stop || cancel ) break;
	
	if( catalog->isModified() ) catalog->saveFile();
	
	it->forceUpdate();
	filesProgressbar->advance(1);
    }

    filesProgressbar->setValue(_fileList.count());
    
    showAllStatistics();
}

void MultiRoughTransDlg::showAllStatistics()
{
    int tt, ptc, etc;
    
    statistics( tt, ptc, etc);
    
    // sanity check
    if( tt == 0 ) tt = 1;

    int nothing=tt-ptc-etc;
    KLocale *locale = KGlobal::locale();
    QString statMsg = i18n("Result of the translation:\n"
            "Edited entries: %1\n"
            "Exact translations: %2 (%3%)\n"
            "Approximate translations: %4 (%5%)\n"
            "Nothing found: %6 (%7%)")
            .arg( locale->formatNumber(tt,0) )
            .arg( locale->formatNumber(etc,0) )
            .arg( locale->formatNumber( ((double)(10000*etc/tt))/100) )
            .arg( locale->formatNumber(ptc,0) )
            .arg( locale->formatNumber(((double)(10000*ptc/tt))/100) )
            .arg( locale->formatNumber(nothing,0) )
            .arg( locale->formatNumber(((double)(10000*nothing/tt)/100) ) );

    KMessageBox::information(this, statMsg
            , i18n("Rough Translation Statistics"));

    accept();
}

#include "multiroughtransdlg.moc"
