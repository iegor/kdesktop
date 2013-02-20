/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

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

#include "algorithmdialog.h"
#include "algorithmcombo.h"
#include "collectioncombo.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <klocale.h>

using namespace KMrml;

class ScrollView : public QScrollView
{
public:
    ScrollView(QWidget* parent = 0, const char* name = 0)
        : QScrollView(parent, name)
    {
        setFrameStyle(QFrame::NoFrame);
        m_frame = new QFrame(viewport(), "ScrollView::m_frame");
        m_frame->setFrameStyle(QFrame::NoFrame);
        addChild(m_frame, 0, 0);
    };

    QFrame* frame() {return m_frame;};

protected:
    virtual void viewportResizeEvent(QResizeEvent* ev)
    {
      QScrollView::viewportResizeEvent(ev);
      m_frame->resize( kMax(m_frame->sizeHint().width(), ev->size().width()),
                       kMax(m_frame->sizeHint().height(), ev->size().height()));
    };

private:
      QFrame* m_frame;
};

AlgorithmDialog::AlgorithmDialog( const AlgorithmList& algorithms,
                                  const CollectionList& collections,
                                  const Collection& currentColl,
                                  QWidget *parent, const char *name )
    : KDialogBase( parent, name, false, i18n("Configure Query Algorithms"),
                   Ok | Cancel, Ok, false ),
      m_allAlgorithms( algorithms ),
      m_collections( collections )
{
    QWidget *box = makeMainWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout( box, 0, KDialog::spacingHint(),
                                                "mainLayout");

    QHBoxLayout *collectionLayout = new QHBoxLayout( 0L, 0, 0, "coll layout");
    collectionLayout->addWidget( new QLabel( i18n("Collection: "), box ));

    m_collectionCombo = new CollectionCombo( box, "collection combo" );
    m_collectionCombo->setCollections( &m_collections );
    collectionLayout->addWidget( m_collectionCombo );

    mainLayout->addLayout( collectionLayout );
    mainLayout->addSpacing( 14 );

    QHBox *algoHLayout = new QHBox( box );
    (void) new QLabel( i18n("Algorithm: "), algoHLayout);
    m_algoCombo = new AlgorithmCombo( algoHLayout, "algo combo" );

    QVGroupBox *groupBox = new QVGroupBox( box, "groupBox" );
    mainLayout->addWidget( groupBox );
    algoHLayout->raise();

    ScrollView *scrollView = new ScrollView( groupBox, "scroll view" );
    m_view = scrollView->frame();
    QVBoxLayout *viewLayout = new QVBoxLayout( scrollView );
    viewLayout->setSpacing( KDialog::spacingHint() );
    
    
    collectionChanged( currentColl );
    
    connect( m_algoCombo, SIGNAL( selected( const Algorithm& ) ),
             SLOT( initGUI( const Algorithm& ) ));
    connect( m_collectionCombo, SIGNAL( selected( const Collection& ) ),
             SLOT( collectionChanged( const Collection& ) ));

    algoHLayout->adjustSize();
    mainLayout->activate();
    algoHLayout->move( groupBox->x() + 10, groupBox->y() - 12 );
    
    box->setMinimumWidth( algoHLayout->sizeHint().width() + 
                          4 * KDialog::spacingHint() );
}

AlgorithmDialog::~AlgorithmDialog()
{
}

void AlgorithmDialog::collectionChanged( const Collection& coll )
{
    m_algosForCollection = m_allAlgorithms.algorithmsForCollection( coll );
    m_algoCombo->setAlgorithms( &m_algosForCollection );
    
    initGUI( m_algoCombo->current() );
}

void AlgorithmDialog::initGUI( const Algorithm& algo )
{
    m_algo = algo;
    
    
}

#include "algorithmdialog.moc"
