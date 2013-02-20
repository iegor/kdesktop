/***************************************************************************
 *   Copyright (C) 2002, Anders Lund <anders@alweb.dk>                     *
 *   Copyright (C) 2003, 2004, Franck Quélain <shift@free.fr>              *
 *   Copyright (C) 2004, Kevin Krammer <kevin.krammer@gmx.at>              *
 *   Copyright (C) 2004, 2006, Oliviet Goffart <ogoffart @ kde.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/




// Qt includes
#include <qapplication.h>
#include <qtimer.h>

// KDE include
#include <dom/dom_doc.h>
#include <dom/dom_element.h>
#include <dom/dom_string.h>
#include <dom/html_document.h>
#include <kaction.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kshortcut.h>
#include <ktoolbar.h>
#include <kurl.h>

// local includes
#include "plugin_rellinks.h"


/** Rellinks factory */
typedef KGenericFactory<RelLinksPlugin> RelLinksFactory;
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,90)
#include <kaboutdata.h>
static const KAboutData aboutdata("rellinks", I18N_NOOP("Rellinks") , "1.0" );
K_EXPORT_COMPONENT_FACTORY( librellinksplugin, RelLinksFactory(&aboutdata) )
#else
K_EXPORT_COMPONENT_FACTORY( librellinksplugin, RelLinksFactory("rellinks") )
#endif

/** Constructor of the plugin. */
RelLinksPlugin::RelLinksPlugin(QObject *parent, const char *name, const QStringList &)
    : KParts::Plugin( parent, name ),
      m_part(0),
	  m_viewVisible(false)
{

    setInstance(RelLinksFactory::instance());

    // ------------- Navigation links --------------
    kaction_map["home"] =  new KAction( i18n("&Top"), "2uparrow", KShortcut("Ctrl+Alt+T"), this, SLOT(goHome()), actionCollection(), "rellinks_top" );
    kaction_map["home"]->setWhatsThis( i18n("<p>This link references a home page or the top of some hierarchy.</p>") );

    kaction_map["up"] =  new KAction( i18n("&Up"), "1uparrow", KShortcut("Ctrl+Alt+U"), this, SLOT(goUp()), actionCollection(), "rellinks_up" );
    kaction_map["up"]->setWhatsThis( i18n("<p>This link references the immediate parent of the current document.</p>") );

    bool isRTL = QApplication::reverseLayout();

    kaction_map["begin"] =  new KAction( i18n("&First"), isRTL ? "2rightarrow" : "2leftarrow", KShortcut("Ctrl+Alt+F"), this,  SLOT(goFirst()), actionCollection(), "rellinks_first" );
    kaction_map["begin"]->setWhatsThis( i18n("<p>This link type tells search engines which document is considered by the author to be the starting point of the collection.</p>") );

    kaction_map["prev"] =  new KAction( i18n("&Previous"), isRTL ? "1rightarrow" : "1leftarrow", KShortcut("Ctrl+Alt+P"), this,  SLOT(goPrevious()), actionCollection(), "rellinks_previous" );
    kaction_map["prev"]->setWhatsThis( i18n("<p>This link references the previous document in an ordered series of documents.</p>") );

    kaction_map["next"] =  new KAction( i18n("&Next"), isRTL ? "1leftarrow" : "1rightarrow", KShortcut("Ctrl+Alt+N"), this,  SLOT(goNext()), actionCollection(), "rellinks_next" );
    kaction_map["next"]->setWhatsThis( i18n("<p>This link references the next document in an ordered series of documents.</p>") );

    kaction_map["last"] =  new KAction( i18n("&Last"), isRTL ? "2leftarrow" : "2rightarrow", KShortcut("Ctrl+Alt+L"), this,  SLOT(goLast()), actionCollection(), "rellinks_last" );
    kaction_map["last"]->setWhatsThis( i18n("<p>This link references the end of a sequence of documents.</p>") );

    // ------------ special items --------------------------
    kaction_map["search"]  = new KAction( i18n("&Search"), "filefind", KShortcut("Ctrl+Alt+S"), this, SLOT(goSearch()), actionCollection(), "rellinks_search" );
    kaction_map["search"]->setWhatsThis( i18n("<p>This link references the search.</p>") );

    // ------------ Document structure links ---------------
    m_document = new KActionMenu( i18n("Document"),  "contents", actionCollection(), "rellinks_document" );
    m_document->setWhatsThis( i18n("<p>This menu contains the links referring the document information.</p>") );
    m_document->setDelayed(false);

    kaction_map["contents"] = new KAction( i18n("Table of &Contents"), "contents", KShortcut("Ctrl+Alt+C"),  this,  SLOT(goContents()), actionCollection(), "rellinks_toc" );
    m_document->insert(kaction_map["contents"]);
    kaction_map["contents"]->setWhatsThis( i18n("<p>This link references the table of contents.</p>") );

    kactionmenu_map["chapter"] = new KActionMenu( i18n("Chapters"), "fileopen", actionCollection(), "rellinks_chapters" );
    m_document->insert(kactionmenu_map["chapter"]);
    connect( kactionmenu_map["chapter"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT(goChapter(int)));
    kactionmenu_map["chapter"]->setWhatsThis( i18n("<p>This menu references the chapters of the document.</p>") );
    kactionmenu_map["chapter"]->setDelayed(false);

    kactionmenu_map["section"] = new KActionMenu( i18n("Sections"), "fileopen", actionCollection(), "rellinks_sections" );
    m_document->insert(kactionmenu_map["section"]);
    connect( kactionmenu_map["section"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goSection( int ) ) );
    kactionmenu_map["section"]->setWhatsThis( i18n("<p>This menu references the sections of the document.</p>") );
    kactionmenu_map["section"]->setDelayed(false);

    kactionmenu_map["subsection"] = new KActionMenu( i18n("Subsections"), "fileopen", actionCollection(), "rellinks_subsections" );
    m_document->insert(kactionmenu_map["subsection"]);
    connect( kactionmenu_map["subsection"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goSubsection( int ) ) );
    kactionmenu_map["subsection"]->setWhatsThis( i18n("<p>This menu references the subsections of the document.</p>") );
    kactionmenu_map["subsection"]->setDelayed(false);

    kactionmenu_map["appendix"] = new KActionMenu( i18n("Appendix"), "edit", actionCollection(), "rellinks_appendix" );
    m_document->insert(kactionmenu_map["appendix"]);
    connect( kactionmenu_map["appendix"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goAppendix( int ) ) );
    kactionmenu_map["appendix"]->setWhatsThis( i18n("<p>This link references the appendix.</p>") );
    kactionmenu_map["appendix"]->setDelayed(false);

    kaction_map["glossary"] = new KAction( i18n("&Glossary"), "flag", KShortcut("Ctrl+Alt+G"), this, SLOT(goGlossary()), actionCollection(), "rellinks_glossary" );
    m_document->insert(kaction_map["glossary"]);
    kaction_map["glossary"]->setWhatsThis( i18n("<p>This link references the glossary.</p>") );

    kaction_map["index"] = new KAction( i18n("&Index"), "info", KShortcut("Ctrl+Alt+I"), this, SLOT(goIndex()), actionCollection(), "rellinks_index" );
    m_document->insert(kaction_map["index"]);
    kaction_map["index"]->setWhatsThis( i18n("<p>This link references the index.</p>") );

    // Other links
    m_more  = new KActionMenu( i18n("More"), "misc", actionCollection(), "rellinks_more" );
    m_more->setWhatsThis( i18n("<p>This menu contains other important links.</p>") );
    m_more->setDelayed(false);

    kaction_map["help"] = new KAction( i18n("&Help"), "help", KShortcut("Ctrl+Alt+H"), this, SLOT(goHelp()), actionCollection(), "rellinks_help" );
    m_more->insert(kaction_map["help"]);
    kaction_map["help"]->setWhatsThis( i18n("<p>This link references the help.</p>") );

    kaction_map["author"]  = new KAction( i18n("&Authors"), "mail_new", KShortcut("Ctrl+Alt+A"), this, SLOT(goAuthor()), actionCollection(), "rellinks_authors" );
    m_more->insert(kaction_map["author"]);
    kaction_map["author"]->setWhatsThis( i18n("<p>This link references the author.</p>") );

    kaction_map["copyright"]   = new KAction( i18n("Copy&right"), "signature", KShortcut("Ctrl+Alt+R"), this, SLOT(goCopyright()), actionCollection(), "rellinks_copyright" );
    m_more->insert(kaction_map["copyright"]);
    kaction_map["copyright"]->setWhatsThis( i18n("<p>This link references the copyright.</p>") );

    kactionmenu_map["bookmark"] = new KActionMenu( i18n("Bookmarks"), "bookmark_folder", actionCollection(), "rellinks_bookmarks" );
    m_more->insert(kactionmenu_map["bookmark"]);
    kactionmenu_map["bookmark"]->setWhatsThis( i18n("<p>This menu references the bookmarks.</p>") );
    connect( kactionmenu_map["bookmark"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goBookmark( int ) ) );
    kactionmenu_map["bookmark"]->setDelayed(false);

    kactionmenu_map["alternate"] = new KActionMenu( i18n("Other Versions"), "attach", actionCollection(), "rellinks_other_versions" );
    m_more->insert(kactionmenu_map["alternate"]);
    kactionmenu_map["alternate"]->setWhatsThis( i18n("<p>This link references the alternate versions of this document.</p>") );
    connect( kactionmenu_map["alternate"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goAlternate( int ) ) );
    kactionmenu_map["alternate"]->setDelayed(false);

    // Unclassified menu
    m_links = new KActionMenu( i18n("Miscellaneous"), "rellinks", actionCollection(), "rellinks_links" );
    kactionmenu_map["unclassified"] = m_links;
    kactionmenu_map["unclassified"]->setWhatsThis( i18n("<p>Miscellaneous links.</p>") );
    connect( kactionmenu_map["unclassified"]->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( goAllElements( int ) ) );
    kactionmenu_map["unclassified"]->setDelayed(false);

    // We unactivate all the possible actions
    disableAll();

    // When the rendering of the HTML is done, we update the site navigation bar
    m_part = dynamic_cast<KHTMLPart *>(parent);
    if (!m_part)
        return;

    connect( m_part, SIGNAL( docCreated() ), this, SLOT( newDocument() ) );
    connect( m_part, SIGNAL( completed() ), this, SLOT( loadingFinished() ) );

    // create polling timer and connect it
    m_pollTimer = new QTimer(this, "polling timer");
    connect( m_pollTimer, SIGNAL( timeout() ), this, SLOT( updateToolbar() ) );

    // delay access to our part's members until it has finished its initialisation
    QTimer::singleShot(0, this, SLOT(delayedSetup()));
}

/** Destructor */
RelLinksPlugin::~RelLinksPlugin() {
}

bool RelLinksPlugin::eventFilter(QObject *watched, QEvent* event) {
    if (m_part == 0) return false;

    if (watched == 0 || event == 0) return false;

    if (watched == m_view)
    {
        switch (event->type())
        {
            case QEvent::Show:
                m_viewVisible = true;
                updateToolbar();
                break;

            case QEvent::Hide:
                m_viewVisible = false;
                updateToolbar();
                break;

            case QEvent::Close:
                m_pollTimer->stop();
                m_view->removeEventFilter(this);
                break;

            default:
                break;
        }
    }

    // we never filter an event, we just want to know about it
    return false;
}

void RelLinksPlugin::delayedSetup()
{
    if (m_part == 0) return;

    m_view = m_part->view();
    m_view->installEventFilter(this);
    m_viewVisible = m_view->isVisible();
}

void RelLinksPlugin::newDocument() {
    // start calling upateToolbar periodically to get the new links as soon as possible

    m_pollTimer->start(500);
    //kdDebug(90210) << "newDocument()" << endl;

    updateToolbar();
}

void RelLinksPlugin::loadingFinished() {
    m_pollTimer->stop();
    //kdDebug(90210) << "loadingFinished()" << endl;
    updateToolbar();
	guessRelations();
}

/* Update the site navigation bar */
void RelLinksPlugin::updateToolbar() {

    // If we have a part
    if (!m_part)
        return;

    // We disable all
    disableAll();

    // get a list of LINK nodes in document
    DOM::NodeList linkNodes = m_part->document().getElementsByTagName( "link" );

    //kdDebug(90210) << "Rellinks: Link nodes =" << linkNodes.length() << endl;

    bool showBar = false;
    unsigned long nodeLength = linkNodes.length();

    for ( unsigned int i=0; i < nodeLength; i++ ) {
        // create a entry for each one
        DOM::Element e( linkNodes.item( i ) );


        // --- Retrieve of the relation type --

        QString rel = e.getAttribute( "rel" ).string();
        rel = rel.simplifyWhiteSpace();
        if (rel.isEmpty()) {
            // If the "rel" attribut is null then use the "rev" attribute...
            QString rev = e.getAttribute( "rev" ).string();
            rev = rev.simplifyWhiteSpace();
            if (rev.isEmpty()) {
                // if "rev" attribut is also empty => ignore
                continue;
            }
            // Determine the "rel" equivalent of "rev" type
            rel =  transformRevToRel(rev);
        }
        // Determin the name used internally
        QString lrel = getLinkType(rel.lower());
        // relation to ignore
        if (lrel.isEmpty()) continue;
//	kdDebug() << "lrel=" << lrel << endl;

        // -- Retrieve of other usefull informations --

        QString href = e.getAttribute( "href" ).string();
        // if nowhere to go, ignore the link
        if (href.isEmpty()) continue;
        QString title = e.getAttribute( "title" ).string();
        QString hreflang = e.getAttribute( "hreflang" ).string();

        KURL ref( m_part->url(), href );
        if ( title.isEmpty() )
            title = ref.prettyURL();

        // escape ampersand before settings as action title, otherwise the menu entry will interpret it as an
        // accelerator
        title.replace('&', "&&");

        // -- Menus activation --

        // Activation of "Document" menu ?
        if (lrel == "contents" || lrel == "glossary" || lrel == "index" || lrel == "appendix") {
            m_document->setEnabled(true);
        }
        // Activation of "More" menu ?
        if (lrel == "help" || lrel == "author" || lrel == "copyright" ) {
            m_more->setEnabled(true);
        }

        // -- Buttons or menu items activation / creation --
        if (lrel == "bookmark" || lrel == "alternate") {
            int id = kactionmenu_map[lrel]->popupMenu()->insertItem( title );
            m_more->setEnabled(true);
            kactionmenu_map[lrel]->setEnabled(true);
            element_map[lrel][id] = e;

        } else if (lrel == "appendix" || lrel == "chapter" || lrel == "section" || lrel == "subsection") {
            int id = kactionmenu_map[lrel]->popupMenu()->insertItem( title );
            m_document->setEnabled(true);
            kactionmenu_map[lrel]->setEnabled(true);
            element_map[lrel][id] = e;

        } else {
            // It is a unique action
            element_map[lrel][0] = e;
            if (kaction_map[lrel]) {
                kaction_map[lrel]->setEnabled(true);
                // Tooltip
                if (hreflang.isEmpty()) {
                    kaction_map[lrel]->setToolTip( title );
                } else {
                    kaction_map[lrel]->setToolTip( title + " [" + hreflang + "]");
                }
            } else {
                // For the moment all the elements are reference in a separated menu
                // TODO : reference the unknown ?
                int id = kactionmenu_map["unclassified"]->popupMenu()->insertItem( lrel + " : " + title );
                kactionmenu_map["unclassified"]->setEnabled(true);
                element_map["unclassified"][id] = e;
            }

        }

        showBar = true;
    }
}


void RelLinksPlugin::guessRelations()
{
	m_part = dynamic_cast<KHTMLPart *>(parent());
	if (!m_part || m_part->document().isNull() )
		return;
	
	//If the page already contains some link, that mean the webmaster is aware
	//of the meaning of <link> so we can consider that if prev/next was possible
	//they are already there.
	if(!element_map.isEmpty())
		return;

	// - The number of didgit may not be more of 3, or this is certenly an id.
	// - We make sure that the number is followed by a dot, a &, or the end, we
	//   don't want to match stuff like that:   page.html?id=A14E12FD
	// - We make also sure the number is not preceded dirrectly by others number
	QRegExp rx("^(.*[=/?&][^=/?&.\\-0-9]*)([\\d]{1,3})([.&][^/0-9]{0,15})?$");
	
	
	const QString zeros("0000");
	QString url=m_part->url().url();
	if(rx.search(url)!=-1)
	{
		uint val=rx.cap(2).toUInt();
		uint lenval=rx.cap(2).length();
		QString nval_str=QString::number(val+1);
		//prepend by zeros if the original also contains zeros.
		if(nval_str.length() < lenval && rx.cap(2)[0]=='0')
			nval_str.prepend(zeros.left(lenval-nval_str.length()));

		QString href=rx.cap(1)+ nval_str + rx.cap(3);
		KURL ref( m_part->url(), href );
		QString title = i18n("[Autodetected] %1").arg(ref.prettyURL());
		DOM::Element e= m_part->document().createElement("link");
		e.setAttribute("href",href);
		element_map["next"][0] = e;
		kaction_map["next"]->setEnabled(true);
		kaction_map["next"]->setToolTip( title );

		if(val>1)
		{
			nval_str=QString::number(val-1);
			if(nval_str.length() < lenval && rx.cap(2)[0]=='0')
				nval_str.prepend(zeros.left(lenval-nval_str.length()));
			QString href=rx.cap(1)+ nval_str + rx.cap(3);
			KURL ref( m_part->url(), href );
			QString title = i18n("[Autodetected] %1").arg(ref.prettyURL());
			e= m_part->document().createElement("link");
			e.setAttribute("href",href);
			element_map["prev"][0] = e;
			kaction_map["prev"]->setEnabled(true);
			kaction_map["prev"]->setToolTip( title );
		}
	}
}


/** Menu links */
void RelLinksPlugin::goToLink(const QString & rel, int id) {
    // have the KHTML part open it
    KHTMLPart *part = dynamic_cast<KHTMLPart *>(parent());
    if (!part)
        return;

    DOM::Element e = element_map[rel][id];
    QString href = e.getAttribute("href").string();
    KURL url( part->url(), href );
    QString target = e.getAttribute("target").string();

    // URL arguments
    KParts::URLArgs args;
    args.frameName = target;

    // Add base url if not valid
    if (url.isValid()) {
        part->browserExtension()->openURLRequest(url, args);
    } else {
        KURL baseURL = part->baseURL();
        QString endURL = url.prettyURL();
        KURL realURL = KURL(baseURL, endURL);
        part->browserExtension()->openURLRequest(realURL, args);
    }

}

void RelLinksPlugin::goHome() {
    goToLink("home");
}

void RelLinksPlugin::goUp() {
    goToLink("up");
}

void RelLinksPlugin::goFirst() {
    goToLink("begin");
}

void RelLinksPlugin::goPrevious() {
    goToLink("prev");
}

void RelLinksPlugin::goNext() {
    goToLink("next");
}

void RelLinksPlugin::goLast() {
    goToLink("last");
}

void RelLinksPlugin::goContents() {
    goToLink("contents");
}

void RelLinksPlugin::goIndex() {
    goToLink("index");
}

void RelLinksPlugin::goGlossary() {
    goToLink("glossary");
}

void RelLinksPlugin::goHelp() {
    goToLink("help");
}

void RelLinksPlugin::goSearch() {
    goToLink("search");
}

void RelLinksPlugin::goAuthor() {
    goToLink("author");
}


void RelLinksPlugin::goCopyright() {
    goToLink("copyright");
}

void RelLinksPlugin::goBookmark(int id) {
    goToLink("bookmark", id);
}

void RelLinksPlugin::goChapter(int id) {
    goToLink("chapter", id);
}

void RelLinksPlugin::goSection(int id) {
    goToLink("section", id);
}

void RelLinksPlugin::goSubsection(int id) {
    goToLink("subsection", id);
}

void RelLinksPlugin::goAppendix(int id) {
    goToLink("appendix", id);
}

void RelLinksPlugin::goAlternate(int id) {
    goToLink("alternate", id);
}

void RelLinksPlugin::goAllElements(int id) {
    goToLink("unclassified", id);
}

void RelLinksPlugin::disableAll() {
    element_map.clear();

    // Clear actions
    KActionMap::Iterator it;
    for ( it = kaction_map.begin(); it != kaction_map.end(); ++it ) {
        // If I don't test it crash :(
        if (it.data()) {
            it.data()->setEnabled(false);
            it.data()->setToolTip(it.data()->text().remove('&'));
        }
    }

    // Clear actions
    KActionMenuMap::Iterator itmenu;
    for ( itmenu = kactionmenu_map.begin(); itmenu != kactionmenu_map.end(); ++itmenu ) {
        // If I don't test it crash :(
        if (itmenu.data()) {
            itmenu.data()->popupMenu()->clear();
            itmenu.data()->setEnabled(false);
            itmenu.data()->setToolTip(itmenu.data()->text().remove('&'));
        }
    }

    // Unactivate menus
    m_more->setEnabled(false);
    m_document->setEnabled(false);

}


QString RelLinksPlugin::getLinkType(const QString &lrel) {
    // Relations to ignore...
    if (lrel.contains("stylesheet")
          || lrel == "script"
          || lrel == "icon"
          || lrel == "shortcut icon"
          || lrel == "prefetch" )
        return QString::null;

    // ...known relations...
    if (lrel == "top" || lrel == "origin" || lrel == "start")
        return "home";
    if (lrel == "parent")
        return "up";
    if (lrel == "first")
        return "begin";
    if (lrel == "previous")
        return "prev";
    if (lrel == "child")
        return "next";
    if (lrel == "end")
        return "last";
    if (lrel == "toc")
        return "contents";
    if (lrel == "find")
        return "search";
    if (lrel == "alternative stylesheet")
        return "alternate stylesheet";
    if (lrel == "authors")
        return "author";
    if (lrel == "toc")
        return "contents";

    //...unknown relations or name that don't need to change
    return lrel;
}

QString RelLinksPlugin::transformRevToRel(const QString &rev) {
    QString altRev = getLinkType(rev);

    // Known relations
    if (altRev == "prev")
        return getLinkType("next");
    if (altRev == "next")
        return getLinkType("prev");
    if (altRev == "made")
        return getLinkType("author");
    if (altRev == "up")
        return getLinkType("child");
    if (altRev == "sibling")
        return getLinkType("sibling");

    //...unknown inverse relation => ignore for the moment
    return QString::null;
}

#include "plugin_rellinks.moc"
