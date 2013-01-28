 /***************************************************************************
    pluginKatexmltools.cpp
    copyright            : (C) 2001-2002 by Daniel Naber
    email                : daniel.naber@t-online.de
 ***************************************************************************/

/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or ( at your option ) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ***************************************************************************/

#ifndef _PLUGIN_KANT_XMLTOOLS_H
#define _PLUGIN_KANT_XMLTOOLS_H

#include "pseudo_dtd.h"

#include <qdict.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qprogressdialog.h>
#include <qintdict.h>

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>
#include <kate/viewmanager.h>

#include <ktexteditor/codecompletioninterface.h>

#include <kcombobox.h>
#include <kdialog.h>
#include <kdialogbase.h>

class PluginKateXMLTools : public Kate::Plugin, Kate::PluginViewInterface
{

  Q_OBJECT

  public:

    PluginKateXMLTools( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    virtual ~PluginKateXMLTools();
    void addView ( Kate::MainWindow *win );
    void removeView( Kate::MainWindow *win );


  public slots:

    void getDTD();

    void slotInsertElement();
    void slotCloseElement();
    void filterInsertString( KTextEditor::CompletionEntry *ce, QString *str );
    void completionDone( KTextEditor::CompletionEntry completionEntry );
    void completionAborted();

    void slotFinished( KIO::Job *job );
    void slotData( KIO::Job *, const QByteArray &data );

    void backspacePressed();
    void emptyKeyEvent();
    void keyEvent( int, int, const QString & );

    /// Connected to the document manager, to manage the dtd collection.
    void slotDocumentDeleted( uint n );

  protected:

    static QStringList sortQStringList( QStringList list );
    //bool eventFilter( QObject *object, QEvent *event );

    QString insideTag( Kate::View &kv );
    QString insideAttribute( Kate::View &kv );

    bool isOpeningTag( QString tag );
    bool isClosingTag( QString tag );
    bool isEmptyTag( QString tag );
    bool isQuote( QString ch );

    QString getParentElement( Kate::View &view, bool ignoreSingleBracket );

    enum Mode {none, entities, attributevalues, attributes, elements};
    enum PopupMode {noPopup, tagname, attributename, attributevalue, entityname};

    QValueList<KTextEditor::CompletionEntry> stringListToCompletionEntryList( QStringList list );

    /// Assign the PseudoDTD @p dtd to the Kate::Document @p doc
    void assignDTD( PseudoDTD *dtd, KTextEditor::Document *doc );

    /// temporary placeholder for the metaDTD file
    QString m_dtdString;
    /// temporary placeholder for the document to assign a DTD to while the file is loaded
    KTextEditor::Document *m_docToAssignTo;
    /// URL of the last loaded meta DTD
    QString m_urlString;

    uint m_lastLine, m_lastCol;
    QStringList m_lastAllowed;
    int m_popupOpenCol;

    Mode m_mode;
    int m_correctPos;

    // code completion stuff:
    KTextEditor::CodeCompletionInterface* m_codeInterface;

    /// maps KTE::Document::docNumber -> DTD
    QIntDict<PseudoDTD> m_docDtds;

    /// maps DTD filename -> DTD
    QDict<PseudoDTD> m_dtds;

    QPtrList<class PluginView> m_views;

    void connectSlots( Kate::View *kv );
    void disconnectSlots( Kate::View *kv );

    Kate::DocumentManager *m_documentManager;
};

class InsertElement : public KDialogBase
{

  Q_OBJECT

  public:
    InsertElement( QWidget *parent, const char *name );
    ~InsertElement();
    QString showDialog( QStringList &completions );
  private slots:
    void slotHistoryTextChanged( const QString& );

};

#endif // _PLUGIN_KANT_XMLTOOLS_H
// kate: space-indent on; indent-width 2; replace-tabs on; mixed-indent off;
