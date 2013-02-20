/*
 *   Copyright (C) 2000 the KGhostView authors. See file AUTHORS.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qregexp.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstdguiitem.h>

#include "infodialog.h"

//
// Using KDialogBase in message box mode (gives centered action button)
//
InfoDialog::InfoDialog( QWidget *parent, const char *name, bool modal )
  :KDialogBase( i18n("Document Information"), Yes, Yes, Yes, parent,
		name, modal, true, KStdGuiItem::ok() )
{
  QFrame *page = makeMainWidget();
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  QGridLayout *glay = new QGridLayout( topLayout, 3, 2 );
  glay->setColStretch(1,1);

  QLabel *label = new QLabel( i18n("File name:" ), page );
  glay->addWidget( label, 0, 0, AlignRight|AlignVCenter );
  mFileLabel = new QLabel( page );
  glay->addWidget( mFileLabel, 0, 1 );

  label = new QLabel( i18n("Document title:" ), page );
  glay->addWidget( label, 1, 0, AlignRight|AlignVCenter );
  mTitleLabel = new QLabel( page );
  glay->addWidget( mTitleLabel, 1, 1 );

  label = new QLabel( i18n("Publication date:" ), page );
  glay->addWidget( label, 2, 0, AlignRight|AlignVCenter );
  mDateLabel = new QLabel( page );
  glay->addWidget( mDateLabel, 2, 1 );

  topLayout->addStretch(1);
}

namespace {
    /* For PDF files, the dates are in a standard format.
     *
     * According to the spec at http://partners.adobe.com/asn/tech/pdf/specifications.jsp
     * That format is "(D:YYYYMMDDHHmmSSOHH'mm')", where
     *      YYYY is year,
     *      MM month
     *      DD day
     *      HH hour
     *      mm minute
     *      SS second
     *      O is "+" or "-"
     *      HH is hour
     *      mm is minute
     *
     *      OHH'mm' form together the desviation to UCT time ( the timezone ).
     *      Everything after the YYYY is optional.
     *      The D: is "highly recommended", but legally optional
     *
     * For PS files, there is no such standard and dates appear
     * in any format they desire.
     */
    QString parseDate( const QString& dateStr ) {
	kdDebug( 4500 ) << "parseDate( \"" << dateStr << "\" )" << endl;
	QRegExp exp( "\\((?:D:)?"
		    "(\\d\\d\\d\\d)"
		    "(\\d\\d)?(\\d\\d)?(\\d\\d)?.*"
		    "(\\d\\d)?(\\d\\d)?.*"
		    "(?:(\\+|\\-)(\\d\\d)\'?(\\d\\d)\'?)?"
		    "\\)" );
	if ( exp.exactMatch( dateStr ) ) {
	    QStringList list = exp.capturedTexts();
	    QStringList::iterator iter = list.begin();
	    ++iter; // whole string!
#undef GET
#define GET( variable, def ) \
	    unsigned variable = def; \
		if ( iter != list.end() )  { \
		    variable = ( *iter ).toUInt();\
		    ++iter; \
		}
	    GET( year, 1 )
	    GET( month, 1 )
	    GET( day, 1 )
	    GET( hour, 0 )
	    GET( min, 0 )
	    GET( sec, 0 )
#undef GET
	    // FIXME: this ignores the timezone
	    QDate date( year, month, day );
	    QTime time( hour, min, sec );
	    KLocale locale( "kghostview" );
	    return locale.formatDateTime( QDateTime( date, time ) );
	}
	kdDebug( 4500 ) << "parseDate failed." << endl;
	return dateStr;
    }
}

void InfoDialog::setup( const QString &fileName, const QString &documentTitle,
			const QString &publicationDate )
{
  mFileLabel->setText( fileName );
  mTitleLabel->setText( documentTitle );
  mDateLabel->setText( parseDate( publicationDate ) );
}

#include "infodialog.moc"

// vim:sw=4:sts=4:ts=8:noet
