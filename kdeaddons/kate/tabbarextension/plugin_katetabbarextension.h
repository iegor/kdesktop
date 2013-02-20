/***************************************************************************
                           plugin_katetabbarextension.h
                           -------------------
    begin                : 2004-04-20
    copyright            : (C) 2004 by Dominik Haumann
    email                : dhdev@gmx.de
 ***************************************************************************/

/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ***************************************************************************/

#ifndef _PLUGIN_TABBAREXTENSION_H_
#define _PLUGIN_TABBAREXTENSION_H_

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>
#include <kate/pluginconfiginterface.h>
#include <kate/pluginconfiginterfaceextension.h>

#include <klibloader.h>
#include <klocale.h>
#include <ktoolbar.h>

#include <qwidget.h>
#include <qpushbutton.h>
#include <qptrlist.h>

class QBoxLayout;
class QCheckBox;
class KateTabBarButton;

/**
 * Same as QPtrList. Only difference is: overwrite comapreItems() for sorting reason.
 */
class MyPtrList : public QPtrList <KateTabBarButton>
{
  public:
  MyPtrList() { QPtrList<KateTabBarButton>::QPtrList(); }

  virtual int compareItems ( QPtrCollection::Item item1_, QPtrCollection::Item item2_ );
};

class KatePluginFactory : public KLibFactory
{
  Q_OBJECT

  public:
    KatePluginFactory();
    virtual ~KatePluginFactory();

    virtual QObject* createObject( QObject* parent = 0, const char* pname = 0,
        const char* name = "QObject", const QStringList &args = QStringList() );

  private:
    static KInstance* s_instance;
};

/**
 * This class implements a single tab. Every tab represents a document and shows
 * the current flag (modified: true or false).
 */
class KateTabBarButton: public QPushButton
{
  Q_OBJECT

  public:
    /**
     * constructor
     * @param pViewManager pointer to kate's the view manager
     * @param pDoc pointer to the document, that the tab button represents
     * @param parent parent widget
     * @param name name of the tab button
     */
    KateTabBarButton(Kate::ViewManager* pViewManager, Kate::Document *pDoc,
        QWidget* parent = 0, const char * name = 0);

    /**
     * standard destructor (emtpy)
     */
    ~KateTabBarButton();

    /**
     * @return the unique document ID
     */
    uint documentNumber();

    /**
     * get the document's full name (eg. main.cpp), used for comparison
     * @return the document's name
     */
    QString fullName() const;

    /**
     * negate the modified flag and change the button color
     * (usually red or black)
     */
    void triggerModified();

    /**
     * dirty flag indicates whether the file was modified on disk
     * @param d if true, a small warning icon appears
     */
    void setDirty(bool d);

    /**
     * set text for this tab
     * @param newText new text
     */
    virtual void setText( const QString& newText);

  signals:
    /**
     * signal that is only emitted when the toggle button toggles to state 'on'
     * @param tab pointer to the button that emitted the signal
     */
    void myToggled(KateTabBarButton* tab);

  public slots:
    /**
     * control the ToggleButton
     * @param on if true, the button is toggled on
     */
    virtual void setOn(bool on);

  private:
    bool modified;                     ///< if true, doc is modified
    uint myDocID;                      ///< unique document ID
    Kate::Document *doc;               ///< pointer to the doc
    Kate::ViewManager *viewManager;    ///< pointer to the view manager
};

/**
 * This class handels all tabs and implements a full-featured "tabbar".
 */
class KateTabBarExtension : public QWidget
{
  Q_OBJECT

  public:
    /**
     * constructor
     * @param pDocManager pointer to kate's document manager
     * @param win pinter to the main window
     * @param bHorizOrientation true, if orientation is Qt::Horizontal
     * @param bSort true, if orientation is Qt::Horizontal
     * @param parent parent widget
     * @param name name of widget
     * @param f widget flags
     */
    KateTabBarExtension( Kate::DocumentManager *pDocManager,
        Kate::MainWindow *win, bool bHorizOrientation, bool bSort,
        QWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    /** standard destructor */
    ~KateTabBarExtension();

    /**
     * @return the tabbar's orientation
     */
    Qt::Orientation orientation() const;

    /**
     * @return true, when sorting is alphabetically
     */
    bool sortByName() const;

    /**
     * set sorting type
     * @param sbn if true, sort by filename.lower()
     */
    void setSortByName(bool sbn);

    /**
     * sort all tabs
     */
    void updateSort();

  public slots:
    /**
     * called when a new document is created/loaded
     * @param doc the new document
     */
    void slotDocumentCreated (Kate::Document *doc);

    /**
     * called when an existing document is being deleted
     * @param documentNumber ID of the document
     */
    void slotDocumentDeleted (uint documentNumber);

    /**
     * called whenever the view changes
     * @param v pointer to the button that represents the active view
     */
    void slotActivateView(KateTabBarButton* v);

    /**
     * called when the document's modified flag changes
     * @param doc the document
     */
    void slotModChanged (Kate::Document *doc);

    /**
     * called when the document changed on disk [NOT SUPPORTED AT THE MOMENT]
     * @param doc the document
     * @param b modified flag
     * @param reason the reason why the file changed
     */
    void slotModifiedOnDisc (Kate::Document *doc, bool b, unsigned char reason);

    /**
     * called when the document's name changed
     * @param doc the document
     */
    void slotNameChanged (Kate::Document *doc);

    /**
     * called when the active view changes
     */
    void slotViewChanged ();

    /**
     * called whenever the toolbar's orientation changes
     * @param o new orientation [Qt::Horizontal or Qt::Vertical]
     */
    void slotMoved(Orientation o);

  private:
    KateTabBarButton* pCurrentTab;         ///< pointer to the current tab
    QBoxLayout* top;                       ///< layout that contains all tabs
    Kate::MainWindow* m_win;               ///< pointer to the main window
    Kate::DocumentManager* m_docManager;   ///< pointer to the document manager
//    QPtrList <KateTabBarButton> m_tabs;    ///< list containing all tabs
    MyPtrList m_tabs;                      ///< list containing all tabs
    Qt::Orientation m_orientation;         ///< save tabbar's orientation
    bool m_sort;                           ///< how to sort
};

/**
 * The tabbar's config page
 */
class KateTabBarExtensionConfigPage : public Kate::PluginConfigPage
{
  Q_OBJECT

  friend class KatePluginTabBarExtension;

  public:
    KateTabBarExtensionConfigPage (QObject* parent = 0L, QWidget *parentWidget = 0L);
    ~KateTabBarExtensionConfigPage ();

    /**
     * Reimplemented from Kate::PluginConfigPage
     * just emits configPageApplyRequest( this ).
     */
    virtual void apply();

    virtual void reset () { ; }
    virtual void defaults () { ; }

  signals:
    /**
     * Ask the plugin to set initial values
     */
    void configPageApplyRequest( KateTabBarExtensionConfigPage* );

    /**
     * Ask the plugin to apply changes
     */
    void configPageInitRequest( KateTabBarExtensionConfigPage* );

  private:
    QCheckBox* pSortAlpha;
};

class KatePluginTabBarExtension : public Kate::Plugin, Kate::PluginViewInterface, Kate::PluginConfigInterfaceExtension
{
  Q_OBJECT

  public:
    KatePluginTabBarExtension( QObject* parent = 0, const char* name = 0 );
    virtual ~KatePluginTabBarExtension();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);

    uint configPages () const { return 1; }
    Kate::PluginConfigPage *configPage (uint , QWidget *w, const char *name=0);
    QString configPageName(uint) const { return i18n("Tab Bar Extension"); }
    QString configPageFullName(uint) const { return i18n("Configure Tab Bar Extension"); }
    QPixmap configPagePixmap (uint number = 0, int size = KIcon::SizeSmall) const { return 0L; }

  public slots:
    void applyConfig( KateTabBarExtensionConfigPage* );

  private:
    void initConfigPage( KateTabBarExtensionConfigPage* );

  private:
    QPtrList<class PluginView> m_views;
    KConfig* pConfig;
};

#endif // _PLUGIN_TABBAREXTENSION_H_
