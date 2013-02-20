/* This file is part of the KDE project
   Copyright (C) 2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MRMLPART_H
#define MRMLPART_H

#include <qcstring.h>
#include <qstringlist.h>

#include <kurl.h>
#include <kparts/factory.h>
#include <kparts/part.h>

#include <kmrml_config.h>

#include "mrml_elements.h"

class QCheckBox;
class QHGroupBox;
class QPushButton;

class KAboutData;
class KComboBox;
class KIntNumInput;

namespace KIO {
    class FileCopyJob;
    class TransferJob;
}

namespace KMrml
{

class AlgorithmDialog;
class Browser;
class CollectionCombo;
class MrmlView;

class MrmlPart : public KParts::ReadOnlyPart
{
    Q_OBJECT

public:
    enum Status { NeedCollection, CanSearch, InProgress };

    MrmlPart( QWidget *parentWidget, const char *widgetName,
              QObject *parent, const char *name, const QStringList& args );
    ~MrmlPart();

    QString sessionId() const { return m_sessionId; }
    QString transactionId() const { return QString::null; } // ###

    void saveState( QDataStream& stream );
    void restoreState( QDataStream& stream );

    static KAboutData *createAboutData();

public slots:
    virtual bool openURL( const KURL& );
    virtual bool closeURL();
	
    void slotActivated( const KURL& url, ButtonState );

protected:
    virtual bool openFile();
    Algorithm firstAlgorithmForCollection( const Collection& coll ) const;
    Collection currentCollection() const;

signals:
    /**
     * allow plugins to extend the query
     */
    void aboutToStartQuery( QDomDocument& );

private slots:
    void slotStartClicked();
    void slotSetStatusBar( const QString& );
    void slotSetStatusBar( const KURL& url ) { slotSetStatusBar( url.prettyURL() ); }
    void slotHostComboActivated( const QString& );

    void slotResult( KIO::Job * );
    void slotData( KIO::Job *, const QByteArray& );

    void slotDownloadResult( KIO::Job * );

    void slotConfigureAlgorithm();
    void slotApplyAlgoConfig();
    void slotAlgoConfigFinished();

private:
    void createQuery( const KURL::List * relevantItems = 0L );
    void initCollections( const QDomElement& );
    void initAlgorithms( const QDomElement& );
    void performQuery( QDomDocument& doc );
    void parseMrml( QDomDocument& doc );
    void parseQueryResult( QDomElement& );
    void enableExtensionActions( const KURL& url, bool enable );
    KIO::TransferJob * transferJob( const KURL& url );

    void initHostCombo();
    void enableServerDependentWidgets( bool enable );

    void setStatus( Status status );

    void contactServer( const KURL& url );
    void downloadReferenceFiles( const KURL::List& downloadList );

    KIO::TransferJob *m_job;
    MrmlView *m_view;
    Config m_config;
    KIntNumInput * m_resultSizeInput;
    CollectionCombo * m_collectionCombo;
    QPushButton *m_algoButton;
    QHGroupBox *m_panel;
    QPushButton *m_startButton;
    QCheckBox *m_random;
    Browser *m_browser;
    AlgorithmDialog *m_algoConfig;
    KComboBox *m_hostCombo;

    QPtrList<KIO::FileCopyJob> m_downloadJobs;
    QStringList m_tempFiles;

    QString m_sessionId;
    KURL::List m_queryList; // a list of valid LOCAL (!) urls to query for

    CollectionList m_collections;
    AlgorithmList m_algorithms;

    Status m_status;
    static uint s_sessionId;

};

class PartFactory : public KParts::Factory
{
    Q_OBJECT

public:
    PartFactory();
    ~PartFactory();

    static KInstance * instance();

protected:
    virtual KParts::Part * createPartObject( QWidget *parentWidget = 0,
                                        const char *widgetName = 0,
                                        QObject *parent = 0,
                                        const char *name = 0,
                                        const char *classname = "KParts::Part",
                                        const QStringList& args = QStringList() );

private:
    static KInstance * s_instance;

};

}

#endif // MRMLPART_H
