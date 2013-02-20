/* This file is part of the KDE project
   Copyright (C) 2004 Arend van Beelen jr. <arend@auton.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <unistd.h>

#include <dcopclient.h>
#include <kapplication.h>
#include <kaction.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <khtml_part.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kprocess.h>
#include <kprotocolmanager.h>
#include <kstandarddirs.h>
#include <kurifilter.h>

#include <kparts/mainwindow.h>
#include <kparts/partmanager.h>

#include <qpainter.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qstyle.h>
#include <qwhatsthis.h>
#include "searchbar.h"

typedef KGenericFactory<SearchBarPlugin> SearchBarPluginFactory;
K_EXPORT_COMPONENT_FACTORY(libsearchbarplugin,
                           SearchBarPluginFactory("searchbarplugin"))


SearchBarPlugin::SearchBarPlugin(QObject *parent, const char *name,
                                 const QStringList &) :
  KParts::Plugin(parent, name),
  m_searchCombo(0),
  m_searchMode(UseSearchProvider),
  m_urlEnterLock(false)
{
	m_searchCombo = new SearchBarCombo(0L, "search combo");
	m_searchCombo->setDuplicatesEnabled(false);
	m_searchCombo->setMaxCount(5);
	m_searchCombo->setFixedWidth(180);
	m_searchCombo->setLineEdit(new KLineEdit(m_searchCombo));
	m_searchCombo->lineEdit()->installEventFilter(this);

	m_popupMenu = 0;

	m_searchComboAction = new KWidgetAction(m_searchCombo, i18n("Search Bar"), 0,
	                                                     0, 0, actionCollection(), "toolbar_search_bar");
	m_searchComboAction->setShortcutConfigurable(false);

	connect(m_searchCombo, SIGNAL(activated(const QString &)),
	                       SLOT(startSearch(const QString &)));
	connect(m_searchCombo, SIGNAL(iconClicked()), SLOT(showSelectionMenu()));

	QWhatsThis::add(m_searchCombo, i18n("Search Bar<p>"
	                                    "Enter a search term. Click on the icon to change search mode or provider."));

	new KAction( i18n( "Focus Searchbar" ), CTRL+Key_S,
			       this, SLOT(focusSearchbar()),
			       actionCollection(), "focus_search_bar");

	configurationChanged();

	KParts::MainWindow *mainWin = static_cast<KParts::MainWindow*>(parent);

	//Grab the part manager. Don't know of any other way, and neither does Tronical, so..
	KParts::PartManager *partMan = static_cast<KParts::PartManager*>(mainWin->child(0, "KParts::PartManager"));
	if (partMan)
	{
		connect(partMan, SIGNAL(activePartChanged(KParts::Part*)),
		                 SLOT  (partChanged      (KParts::Part*)));
		partChanged(partMan->activePart());
	}
}

SearchBarPlugin::~SearchBarPlugin()
{
	KConfig *config = kapp->config();
	config->setGroup("SearchBar");
	config->writeEntry("Mode", (int) m_searchMode);
	config->writeEntry("CurrentEngine", m_currentEngine);

	delete m_searchCombo;
	m_searchCombo = 0L;
}

QChar delimiter()
{
        KConfig config( "kuriikwsfilterrc", true, false );
        config.setGroup( "General" );
        return config.readNumEntry( "KeywordDelimiter", ':' );
}

bool SearchBarPlugin::eventFilter(QObject *o, QEvent *e)
{
	if( o==m_searchCombo->lineEdit() && e->type() == QEvent::KeyPress ) 
	{
		QKeyEvent *k = (QKeyEvent *)e;
		if(k->state() & ControlButton)
		{
			if(k->key()==Key_Down)
			{
				nextSearchEntry();
				return true;
			}
			if(k->key()==Key_Up)
			{
				previousSearchEntry();
				return true;
			}
		}
	}
	return false;
}

void SearchBarPlugin::nextSearchEntry()
{
	if(m_searchMode == FindInThisPage)
	{
		m_searchMode = UseSearchProvider;
		if(m_searchEngines.count())
		{
			m_currentEngine = *m_searchEngines.at(0);
		}
		else
		{
			m_currentEngine = "google";
		}
	}
	else
	{
		QStringList::ConstIterator it = m_searchEngines.find(m_currentEngine);
		it++;
		if(it==m_searchEngines.end())
		{
			m_searchMode = FindInThisPage;
		}
		else
		{
			m_currentEngine = *it;
		}
	}
	setIcon();
}

void SearchBarPlugin::previousSearchEntry()
{
	if(m_searchMode == FindInThisPage)
	{
		m_searchMode = UseSearchProvider;
		if(m_searchEngines.count())
		{
			m_currentEngine = *m_searchEngines.fromLast();
		}
		else
		{
			m_currentEngine = "google";
		}
	}
	else
	{
		QStringList::ConstIterator it = m_searchEngines.find(m_currentEngine);
		if(it==m_searchEngines.begin())
		{
			m_searchMode = FindInThisPage;
		}
		else
		{
			it--;
			m_currentEngine = *it;
		}
	}
	setIcon();
}

void SearchBarPlugin::startSearch(const QString &search)
{
	if(m_urlEnterLock || search.isEmpty() || !m_part)
		return;

	if(m_searchMode == FindInThisPage)
	{
		m_part->findText(search, 0);
		m_part->findTextNext();
	}
	else if(m_searchMode == UseSearchProvider)
	{
		m_urlEnterLock = true;
	        KService::Ptr service;
		KURIFilterData data;
		QStringList list;
		list << "kurisearchfilter" << "kuriikwsfilter";

		service = KService::serviceByDesktopPath(QString("searchproviders/%1.desktop").arg(m_currentEngine));
                if (service) {
		    const QString searchProviderPrefix = *(service->property("Keys").toStringList().begin()) + delimiter();
		    data.setData( searchProviderPrefix + search );
                }

		if(!service || !KURIFilter::self()->filterURI(data, list))
		{
			data.setData( QString::fromLatin1( "google" ) + delimiter() + search );
			KURIFilter::self()->filterURI( data, list );
		}

		if(KApplication::keyboardMouseState() & Qt::ControlButton) 
		{
			KParts::URLArgs args;
			args.setNewTab(true);
			emit m_part->browserExtension()->createNewWindow( data.uri(), args );
		}
		else
		{
		        emit m_part->browserExtension()->openURLRequest(data.uri());
		}
	}

	if(m_searchCombo->text(0).isEmpty())
	{
		m_searchCombo->changeItem(m_searchIcon, search, 0);
	}
	else
	{
		if(m_searchCombo->findHistoryItem(search) == -1)
		{
			m_searchCombo->insertItem(m_searchIcon, search, 0);
		}
	}

	m_searchCombo->setCurrentText("");
	m_urlEnterLock = false;
}

void SearchBarPlugin::setIcon()
{
	QString hinttext;
	if(m_searchMode == FindInThisPage)
	{
		m_searchIcon = SmallIcon("find");
		hinttext = i18n("Find in This Page");
	}
	else
	{
		QString providername;
		KService::Ptr service;
		KURIFilterData data;
		QStringList list;
		list << "kurisearchfilter" << "kuriikwsfilter";

		service = KService::serviceByDesktopPath(QString("searchproviders/%1.desktop").arg(m_currentEngine));
                if (service) {
		    const QString searchProviderPrefix = *(service->property("Keys").toStringList().begin()) + delimiter();
		    data.setData( searchProviderPrefix + "some keyword" );
                }

		if (service && KURIFilter::self()->filterURI(data, list))
		{
			QString iconPath = locate("cache", KMimeType::favIconForURL(data.uri()) + ".png");
			if(iconPath.isEmpty())
			{
				m_searchIcon = SmallIcon("enhanced_browsing");
			}
			else
			{
				m_searchIcon = QPixmap(iconPath);
			}
			providername = service->name();
		}
		else
		{
			m_searchIcon = SmallIcon("google");
			providername = "Google";
		}
		hinttext = i18n("%1 Search").arg(providername);;
	}
	static_cast<KLineEdit*>(m_searchCombo->lineEdit())->setClickMessage(hinttext);

        // Create a bit wider icon with arrow
	QPixmap arrowmap = QPixmap(m_searchIcon.width()+5,m_searchIcon.height()+5);
	arrowmap.fill(m_searchCombo->lineEdit()->backgroundColor());
	QPainter p( &arrowmap );
	p.drawPixmap(0, 2, m_searchIcon);
	QStyle::SFlags arrowFlags = QStyle::Style_Default;
	m_searchCombo->style().drawPrimitive(QStyle::PE_ArrowDown, &p, QRect(arrowmap.width()-6, 
	    arrowmap.height()-6, 6, 5), m_searchCombo->colorGroup(), arrowFlags, QStyleOption() );
	p.end();
	m_searchIcon = arrowmap;

	m_searchCombo->setIcon(m_searchIcon);
}

void SearchBarPlugin::showSelectionMenu()
{
	if(!m_popupMenu)
	{
		KService::Ptr service;
		QPixmap icon;
		KURIFilterData data;
		QStringList list;
		list << "kurisearchfilter" << "kuriikwsfilter";

		m_popupMenu = new QPopupMenu(m_searchCombo, "search selection menu");
		m_popupMenu->insertItem(SmallIcon("find"), i18n("Find in This Page"), this, SLOT(useFindInThisPage()),  0, 999);
		m_popupMenu->insertSeparator();

		int i=-1;
		for (QStringList::ConstIterator it = m_searchEngines.begin(); it != m_searchEngines.end(); ++it )
		{
			i++;
			service = KService::serviceByDesktopPath(QString("searchproviders/%1.desktop").arg(*it));
			if(!service)
			{
				continue;
			}
			const QString searchProviderPrefix = *(service->property("Keys").toStringList().begin()) + delimiter();
			data.setData( searchProviderPrefix + "some keyword" );

			if(KURIFilter::self()->filterURI(data, list))
			{
				QString iconPath = locate("cache", KMimeType::favIconForURL(data.uri()) + ".png");
				if(iconPath.isEmpty())
				{
					icon = SmallIcon("enhanced_browsing");
				}
				else
				{
					icon = QPixmap( iconPath );
				}
			m_popupMenu->insertItem(icon, service->name(), i);
			}
		}

		m_popupMenu->insertSeparator();
		m_popupMenu->insertItem(SmallIcon("enhanced_browsing"), i18n("Select Search Engines..."),
			this, SLOT(selectSearchEngines()), 0, 1000);
		connect(m_popupMenu, SIGNAL(activated(int)), SLOT(useSearchProvider(int)));
	}
	m_popupMenu->popup(m_searchCombo->mapToGlobal(QPoint(0, m_searchCombo->height() + 1)), 0);
}

void SearchBarPlugin::useFindInThisPage()
{
	m_searchMode = FindInThisPage;
	setIcon();
}

void SearchBarPlugin::useSearchProvider(int id)
{
	if(id>900)
	{
		// Not a search engine entry selected
		return;
	}
	m_searchMode = UseSearchProvider;
	m_currentEngine = *m_searchEngines.at(id);
	setIcon();
}

void SearchBarPlugin::selectSearchEngines()
{
	KProcess *process = new KProcess;

	*process << "kcmshell" << "ebrowsing";

	connect(process, SIGNAL(processExited(KProcess *)), SLOT(searchEnginesSelected(KProcess *)));

	if(!process->start())
	{
		kdDebug(1202) << "Couldn't invoke kcmshell." << endl;
		delete process;
	}
}

void SearchBarPlugin::searchEnginesSelected(KProcess *process)
{
	if(!process || process->exitStatus() == 0)
	{
		KConfig *config = kapp->config();
		config->setGroup("SearchBar");
		config->writeEntry("CurrentEngine", m_currentEngine);
		config->sync();
		configurationChanged();
	}
	delete process;
}

void SearchBarPlugin::configurationChanged()
{
	KConfig *config = new KConfig("kuriikwsfilterrc");
	config->setGroup("General");
	QString engine = config->readEntry("DefaultSearchEngine", "google");

	QStringList favoriteEngines;
	favoriteEngines << "google" << "google_groups" << "google_news" << "webster" << "dmoz" << "wikipedia";
	favoriteEngines = config->readListEntry("FavoriteSearchEngines", favoriteEngines);

	delete m_popupMenu;
	m_popupMenu = 0;
	m_searchEngines.clear();
	m_searchEngines << engine;
	for (QStringList::ConstIterator it = favoriteEngines.begin(); it != favoriteEngines.end(); ++it )
		if(*it!=engine)
			m_searchEngines << *it;

	delete config;
	if(engine.isEmpty())
	{
		m_providerName = "Google";
	}
	else
	{
		KDesktopFile file("searchproviders/" + engine + ".desktop", true, "services");
		m_providerName = file.readName();
	}

	config = kapp->config();
	config->setGroup("SearchBar");
	m_searchMode = (SearchModes) config->readNumEntry("Mode", (int) UseSearchProvider);
	m_currentEngine = config->readEntry("CurrentEngine", engine);

	if ( m_currentEngine.isEmpty() )
	    m_currentEngine = "google";

	setIcon();
}

void SearchBarPlugin::partChanged(KParts::Part *newPart)
{
	m_part = ::qt_cast<KHTMLPart*>(newPart);

	//Delay since when destroying tabs part 0 gets activated for a bit, before the proper part
	QTimer::singleShot(0, this, SLOT(updateComboVisibility()));
}

void SearchBarPlugin::updateComboVisibility()
{
	if (m_part.isNull() || !m_searchComboAction->isPlugged())
	{
		m_searchCombo->setPluginActive(false);
		m_searchCombo->hide();
	}
	else
	{
		m_searchCombo->setPluginActive(true);
		m_searchCombo->show();
	}
}

void SearchBarPlugin::focusSearchbar()
{
	QFocusEvent::setReason( QFocusEvent::Shortcut );
	m_searchCombo->setFocus();
	QFocusEvent::resetReason();
}

SearchBarCombo::SearchBarCombo(QWidget *parent, const char *name) :
  KHistoryCombo(parent, name),
  m_pluginActive(true)
{
	connect(this, SIGNAL(cleared()), SLOT(historyCleared()));
}

const QPixmap &SearchBarCombo::icon() const
{
	return m_icon;
}

void SearchBarCombo::setIcon(const QPixmap &icon)
{
	m_icon = icon;

	if(count() == 0)
	{
		insertItem(m_icon, 0);
	}
	else
	{
		for(int i = 0; i < count(); i++)
		{
			changeItem(m_icon, text(i), i);
		}
	}
}

int SearchBarCombo::findHistoryItem(const QString &searchText)
{
	for(int i = 0; i < count(); i++)
	{
		if(text(i) == searchText)
		{
			return i;
		}
	}

	return -1;
}

void SearchBarCombo::mousePressEvent(QMouseEvent *e)
{
	int x0 = QStyle::visualRect( style().querySubControlMetrics( QStyle::CC_ComboBox, this, QStyle::SC_ComboBoxEditField ), this ).x();

	if(e->x() > x0 + 2 && e->x() < lineEdit()->x())
	{
		emit iconClicked();

		e->accept();
	}
	else
	{
		KHistoryCombo::mousePressEvent(e);
	}
}

void SearchBarCombo::historyCleared()
{
	setIcon(m_icon);
}

void SearchBarCombo::setPluginActive(bool pluginActive)
{
	m_pluginActive = pluginActive;
}

void SearchBarCombo::show()
{
	if(m_pluginActive)
	{
		KHistoryCombo::show();
	}
}

#include "searchbar.moc"
