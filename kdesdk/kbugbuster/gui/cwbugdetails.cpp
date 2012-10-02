/*
    cwbugdetails.cpp  -  Details of a bug report

    copyright   : (c) 2001 by Martijn Klingens
    email       : klingens@kde.org

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include <qtextview.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>

#include "cwbugdetails.h"
#include "kbbprefs.h"
#include "bugsystem.h"
#include "bugserver.h"

#include <khtml_part.h>
#include <khtmlview.h>
#include <kdebug.h>
#include <kglobal.h>
#include <krun.h>

#include <qlayout.h>
#include <qpalette.h>

using namespace KBugBusterMainWindow;

CWBugDetails::CWBugDetails( QWidget *parent , const char * name )
  : QWidget( parent, name )
{
    QBoxLayout *topLayout = new QVBoxLayout( this );
    
    m_bugDesc = new KHTMLPart( this, "m_bugDesc" );
    connect( m_bugDesc->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ),
             this, SLOT( handleOpenURLRequest( const KURL &, const KParts::URLArgs & ) ) );

    topLayout->addWidget( m_bugDesc->view() );
}

CWBugDetails::~CWBugDetails()
{
}

void CWBugDetails::setBug( const Bug &bug, const BugDetails &details )
{
    QColorGroup cg = m_bugDesc->view()->palette().active();
    QString text =
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
        "<html><head><title></title></head>\n"
        "<style>";

    text.append(
        QString( "table.helpT { text-align: center; font-family: Verdana; font-weight: normal; font-size: 11px; color: #404040; width: 100%; background-color: #fafafa; border: 1px #6699CC solid; border-collapse: collapse; border-spacing: 0px; }\n"
                 "td.helpHed { border-bottom: 2px solid #000000; border-left: 1px solid #000000; background-color: %1; text-align: center; text-indent: 5px; font-family: Verdana; font-weight: bold; font-size: 11px; color: %2; }\n"
                 "td.helpBod { border-bottom: 1px solid #9CF; border-top: 0px; border-left: 1px solid #9CF; border-right: 0px; text-align: center; text-indent: 10px; font-family: Verdana, sans-serif, Arial; font-weight: normal; font-size: 11px; color: #404040; background-color: #000000; }\n"
                 "table.sofT { text-align: center; font-family: Verdana; font-weight: normal; font-size: 11px; color: #404040; width: 100%; background-color: #fafafa; border: 1px #000000 solid; border-collapse: collapse; border-spacing: 0px; }\n"
                 "</style>\n" )
        .arg( cg.highlight().name() )
        .arg( cg.highlightedText().name() ) );

    text.append( "<body style=\"margin: 0px\">\n" );

    QString highlightStyle = QString( "background: %1; color: %2; " )
                                      .arg( cg.highlight().name() )
                                      .arg( cg.highlightedText().name() );
    QString borderBottomStyle = QString( "border-bottom: solid %1 1px; " )
                                         .arg( cg.foreground().name() );
    QString borderTopStyle = QString( "border-top: solid %1 1px; " )
                                      .arg( cg.foreground().name() );

    QString submitter = bug.submitter().fullName( true );
    int age = details.age();
    text.append( "<div style=\"" + highlightStyle + "padding: 8px; float: left\">" );
    text.append( "<a href=\"" + BugSystem::self()->server()->bugLink( bug ).url()
                 + "\">" + i18n("Bug Report</a> from <b>%1</b> " )
                 .arg( submitter ) );
    int replies = details.parts().count() - 1;
    if ( replies >= 1 ) text += i18n( "(1 reply)", "(%n replies)", replies );
    text += "</div>";
    text += "<div style=\"" + highlightStyle + borderBottomStyle +
            " text-align: right; padding: 8px\">" +
            i18n( "1 day old", "%n days old", age ) + "</div>\n";

    text.append(
        QString( "<div style=\"background: %1; color: %2; " +
                 borderBottomStyle +
                 "border-bottom: solid %3 1px; "
                 "padding: 4px\">"
                 "<table cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">" )
                 .arg( cg.background().name() )
                 .arg( cg.foreground().name() ) );
    text.append( textBugDetailsAttribute( details.version(), i18n("Version") ) );
    text.append( textBugDetailsAttribute( details.source(), i18n("Source") ) );
    text.append( textBugDetailsAttribute( details.compiler(), i18n("Compiler") ) );
    text.append( textBugDetailsAttribute( details.os(), i18n("OS") ) );
    text.append( "</table></div>\n" );

    BugDetailsPart::List bdp = details.parts();
    BugDetailsPart::List::ConstIterator it;
    bool firstHeader = true;

    for ( it = bdp.begin(); it != bdp.end(); ++it ) {
        if ( bdp.count() > 1 ) {
            text.append( "<div style=\"" + highlightStyle + "padding: 8px; float: left; " );
            if ( !firstHeader ) text += borderTopStyle;

            text.append( borderBottomStyle + "\">" );
            QString sender = (*it).sender.fullName( true );
            QString date = KGlobal::locale()->formatDateTime( (*it).date, false );
            BugDetailsPart::List::ConstIterator it2 = it;
            if ( ++it2 == bdp.end() )
                text.append( "<a href=\"" + BugSystem::self()->server()->bugLink( bug ).url()
                             + "\">" + i18n("Bug Report</a> from <b>%1</b>")
                             .arg( sender ) );
            else {
                text.append( "<a href=\"" + BugSystem::self()->server()->bugLink( bug ).url() + QString("#c%1").arg( replies )
			     + "\">" + i18n("Reply #%1</a> from <b>%2</b>")
                             .arg( replies ).arg( sender ) );
                replies--;
            }
            text.append( "</div>\n" );
            text += "<div style=\"" + highlightStyle + borderBottomStyle;
            if ( !firstHeader ) text += borderTopStyle;
            text += "text-align: right; padding: 8px\">" + date + "</div>\n";
        
            firstHeader = false;
        }

        // Adding of <pre> tags is now done by BugDetailsJob::processNode. This
        // was breaking the display of attachments because they have <br/>\n
        // after each line -> two newlines with <pre>
        text.append( "<div style=\"padding: 8px\">" );
        text.append( (*it).text );
        text.append( "</div>\n" );
    }

    QValueList<BugDetailsImpl::AttachmentDetails> atts = details.attachmentDetails();
    if ( atts.count() > 0 ) {
        text.append( "<table summary=\"Attachment data table\" class=\"sofT\" cellspacing=\"0\">" );
        text.append( QString( "<tr> <td colspan=\"4\" class=\"helpHed\">%1</td> </tr>")
                     .arg( i18n( "Attachment List") ) );
        text.append( QString("<tr> <td class=\"helpHed\">%1</td> <td class=\"helpHed\">%2</td> <td class=\"helpHed\">%3</td> <td class=\"helpHed\">%4</td> </tr>")
                     .arg( i18n("Description") )
                     .arg( i18n("Date") )
                     .arg( i18n("View") )
                     .arg( i18n("Edit") ) );
        QValueList<BugDetailsImpl::AttachmentDetails>::iterator it;
        for ( it = atts.begin() ; it != atts.end() ; ++it ) {
            text.append( QString("<tr><td>%1</td>").arg( (*it).description ) ) ;
            text.append( QString("<td>%1</td>").arg( (*it).date ) );
            text.append( "<td><a href=\"" +
                         BugSystem::self()->server()->attachmentViewLink( (*it).id ).url() +
                         "\">" + i18n("View") + "</a></td>" );
            text.append( "<td><a href=\"" +
                         BugSystem::self()->server()->attachmentEditLink( (*it).id ).url() +
                         "\">" + i18n("Edit") + "</a></td>" );
            text.append( "</tr>" );
        }
    }

    text.append( "</body></html>" );

    //kdDebug() << "BEGIN OUTPUT" << text << "END OUTPUT" << endl;

    m_bugDesc->view()->setContentsPos(0,0);
    m_bugDesc->begin();
    m_bugDesc->write( text );
    m_bugDesc->end();

    if ( KBBPrefs::instance()->mDebugMode ) mSource = text;
}

void CWBugDetails::handleOpenURLRequest( const KURL &url, const KParts::URLArgs & )
{
    new KRun( url );
}

QString CWBugDetails::textBugDetailsAttribute( const QString &value,
                                               const QString &name )
{
    QString text = "";
    if ( !value.isEmpty() ) {
        text.append( "<tr><td style=\"width: 20%\"><b>" + name + "</b></td>"
                     "<td>" + value + "</td></tr>" );
    }
    return text;
}

QString CWBugDetails::source() const
{
    return mSource;
}

QString CWBugDetails::selectedText() const
{
    return m_bugDesc->selectedText();
}

#include "cwbugdetails.moc"

/* vim: set et ts=4 sw=4 softtabstop=4: */

