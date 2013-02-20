/* This file is part of KolourPicker
   Copyright (c) 2001 Malte Starostik <malte@kde.org>

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

$Id: kolourpicker.cpp 845315 2008-08-11 13:19:13Z bminisini $
*/

#include <qfile.h>
#include <qtextstream.h>
#include <qlayout.h>
#include <qimage.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qcursor.h>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kaboutdata.h>
#include <kaboutapplication.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>

#include "kolourpicker.h"
#include "kolourpicker.moc"

#include <X11/Xlib.h>

// Applet initialization function
extern "C"
{
	KDE_EXPORT KPanelApplet* init(QWidget *parent, const QString& configFile)
	{
		KGlobal::locale()->insertCatalogue("kolourpicker");
		return new KolourPicker(configFile, KPanelApplet::Normal,
			KPanelApplet::About,
			parent, "kolourpicker");
	}
}

KolourPicker::KolourPicker(const QString& configFile, Type type,
	int actions, QWidget *parent, const char *name)
	: KPanelApplet(configFile, type, actions, parent, name),
	  m_picking(0)
{
	KAboutData *about = new KAboutData("kolourpicker",
									   I18N_NOOP("Color Picker"),
									   "v0.1",
									   I18N_NOOP("An applet to pick color values from anywhere on the screen"),
									   KAboutData::License_GPL_V2,
									   "(c) 2001 Malte Starostik");
	about->addAuthor("Malte Starostik", I18N_NOOP("Original Author"), "malte@kde.org");
	m_instance = new KInstance(about);

	KConfig *conf = config();
	conf->setGroup("General");
	QStringList history = conf->readListEntry("History");
	for (QStringList::ConstIterator it = history.begin(); it != history.end(); ++it)
		m_history.append(QColor(*it));

	setBackgroundOrigin(AncestorOrigin);

	m_colourButton = new SimpleButton(this);
	m_colourButton->setPixmap(SmallIcon("colorpicker"));
	m_colourButton->setFixedSize(20, 20);
	QToolTip::add(m_colourButton, i18n("Pick a color"));
	connect(m_colourButton, SIGNAL(clicked()), SLOT(slotPick()));

	m_historyButton = new SimpleButton(this);
	m_historyButton->setFixedSize(20, 20);
	if (m_history.count())
		m_historyButton->setPixmap(colorPixmap(m_history.last()));
	else
	{
		m_historyButton->setPixmap(colorPixmap(QColor()));
		m_historyButton->setEnabled(false);
	}
	QToolTip::add(m_historyButton, i18n("History"));
	connect(m_historyButton, SIGNAL(clicked()), SLOT(slotHistory()));
}

KolourPicker::~KolourPicker()
{
    KGlobal::locale()->removeCatalogue("kolourpicker");
}


int KolourPicker::heightForWidth(int width) const
{
	return (width > 40) ? 22 : 44;
}

int KolourPicker::widthForHeight(int height) const
{
	return (height > 40) ? 22 : 44;
}

void KolourPicker::about()
{
	KAboutApplication dlg(m_instance->aboutData());
	dlg.exec();
}

void KolourPicker::slotPick()
{
	m_picking = true;
	grabMouse(crossCursor);
	grabKeyboard();
}

void KolourPicker::slotHistory()
{
	KPopupMenu popup;
	popup.insertTitle(SmallIcon("colorize"), i18n("History"));
	QPtrList<QPopupMenu> subMenus;
	subMenus.setAutoDelete(true);
	for (QValueList<QColor>::ConstIterator it = m_history.fromLast();
		it != m_history.end();
		--it)
	{
		QPopupMenu *sub = copyPopup(*it, false);
		subMenus.append(sub);
		popup.insertItem(colorPixmap(*it),
			QString("%1, %2, %3").arg((*it).red()).arg((*it).green()).arg((*it).blue()),
			sub);
	}
	popup.insertSeparator();
	int clear = popup.insertItem(SmallIcon("history_clear"), i18n("&Clear History"));
	int id = popup.exec(QCursor::pos());
	if (id == clear)
	{
		m_history.clear();
		m_historyButton->setEnabled(false);
		arrangeButtons();
		KConfig *conf = config();
		conf->setGroup("General");
		conf->writeEntry("History", QStringList());
		conf->sync();
	}
	else if (id != -1)
		setClipboard(popup.findItem(id)->text());
}

void KolourPicker::mouseReleaseEvent(QMouseEvent *e)
{
	if (m_picking)
	{
		m_picking = false;
		releaseMouse();
		releaseKeyboard();
		QWidget *desktop = QApplication::desktop();
		QPixmap pm = QPixmap::grabWindow(desktop->winId(),
			e->globalPos().x(), e->globalPos().y(), 1, 1);
		QImage img = pm.convertToImage();
		QColor color(img.pixel(0, 0));

		// eventually remove a dupe
		QValueListIterator<QColor> dupe = m_history.find(color);
		if (dupe != m_history.end())
			m_history.remove(dupe);

		m_history.append(color);
		while (m_history.count() >= 10)
			m_history.remove(m_history.begin());
		m_historyButton->setEnabled(true);
		arrangeButtons();
		QStringList history;
		for (QValueList<QColor>::ConstIterator it = m_history.begin();
			it != m_history.end();
			++it)
		{
			history.append((*it).name());
		}
		KConfig *conf = config();
		conf->setGroup("General");
		conf->writeEntry("History", history);
		conf->sync();
		m_historyButton->setPixmap(colorPixmap(color));
		QPopupMenu *popup = copyPopup(color, true);
		int id = popup->exec(e->globalPos());
		if (id != -1)
			setClipboard( popup->findItem(id)->text() );
		delete popup;
	}
	else
		KPanelApplet::mouseReleaseEvent(e);
}

// set both clipboard and selection
void KolourPicker::setClipboard(const QString& text)
{
	QClipboard *clip = QApplication::clipboard();
	bool oldMode = clip->selectionModeEnabled();
	clip->setSelectionMode(true);
	clip->setText(text);
	clip->setSelectionMode(false);
	clip->setText(text);
	clip->setSelectionMode( oldMode );
}

void KolourPicker::keyPressEvent(QKeyEvent *e)
{
	if (m_picking)
	{
		if (e->key() == Key_Escape)
		{
			m_picking = false;
			releaseMouse();
			releaseKeyboard();
		}
		e->accept();
		return;
	}
	KPanelApplet::keyPressEvent(e);
}

void KolourPicker::resizeEvent(QResizeEvent *)
{
	arrangeButtons();
}

void KolourPicker::arrangeButtons()
{
	int h, w, p;

	if (orientation() == Horizontal)
	{
		h = height();
		if (h > 40)
		{
			// vertical layout
			p = (h - 40)/3;
			m_colourButton->setGeometry(2, p, 20, 20);
			m_historyButton->setGeometry(2, 2*p+20, 20, 20);
		}
		else
		{
			// horizontal layout
			p = (h - 20)/2;
			m_colourButton->setGeometry(2, p, 20, 20);
			m_historyButton->setGeometry(24, p, 20, 20);
		}
	}
	else
	{
		w = width();
		if (w > 40)
		{
			// horizontal layout
			p = (w - 40)/3;
			m_colourButton->setGeometry(p, 2, 20, 20);
			m_historyButton->setGeometry(2*p+20, 2, 20, 20);
		}
		else
		{
			// vertical layout
			p = (w - 20)/2;
			m_colourButton->setGeometry(p, 2, 20, 20);
			m_historyButton->setGeometry(p, 24, 20, 20);
		}
	}

	updateGeometry();
}

QPopupMenu *KolourPicker::copyPopup(const QColor &c, bool title) const
{
	KPopupMenu *popup = new KPopupMenu;
	if (title)
		popup->insertTitle(colorPixmap(c), i18n("Copy Color Value"));
	QString value;
	// r, g, b
	value.sprintf("%u, %u, %u", c.red(), c.green(), c.blue());
	popup->insertItem(SmallIcon("text"), value);
	// HTML, lower case hex chars
	value.sprintf("#%.2x%.2x%.2x", c.red(), c.green(), c.blue());
	popup->insertItem(SmallIcon("html"), value);
	if (value.find(QRegExp("[a-f]")) >= 0)
	{
		// HTML, upper case hex chars
		value.sprintf("#%.2X%.2X%.2X", c.red(), c.green(), c.blue());
    popup->insertItem(SmallIcon("html"), value);
  }
  // lower case hex chars 
  value.sprintf( "%.2x%.2x%.2x", c.red(), c.green(), c.blue() ); 
  popup->insertItem( SmallIcon( "html" ), value ); 
  if ( value.find( QRegExp( "[a-f]" ) ) >= 0 ) 
  { 
    //  upper case hex chars 
    value.sprintf( "%.2X%.2X%.2X", c.red(), c.green(), c.blue() ); 
    popup->insertItem( SmallIcon( "html" ), value ); 
  } 
	// Color name
	QStringList names = colorNames(c.red(), c.green(), c.blue());
	for (QStringList::ConstIterator it = names.begin(); it != names.end(); ++it)
		popup->insertItem(SmallIcon("text"), *it);
	return popup;
}

#define AAFACTOR 4

QPixmap KolourPicker::colorPixmap(const QColor &c) const
{
    int x, y, dx, dy, d;
    
    QImage img(16 * AAFACTOR, 16 * AAFACTOR, 32);
    img.setAlphaBuffer(true);
    img.fill(0);
    
    for (x = 0; x < img.width(); x++)
        for (y = 0; y < img.height(); y++)
        {
            dx = 1 + x - 15 * AAFACTOR / 2;
            dy = 1 + y - 15 * AAFACTOR / 2;
            d = dx * dx + dy * dy;
            
            if (d < (36 * AAFACTOR * AAFACTOR))
                img.setPixel(x, y, c.pixel());
            else if (d < (56.25 * AAFACTOR * AAFACTOR))
                img.setPixel(x, y, qRgba(128, 128, 128, 255));
        }
    
    QBitmap mask(16, 16);
    mask.fill(Qt::color0);
    QPainter p(&mask);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::color1);
    p.drawEllipse(0, 0, 15, 15);
    p.end();
    
    QPixmap pm = QPixmap(img.smoothScale(16, 16));
    pm.setMask(mask);
    
    return pm;
}

const QStringList &KolourPicker::colorNames(int r, int g, int b) const
{
	static QStringList NullList;
	if (m_colorNames.isEmpty())
	{
		QFile f("/usr/lib/X11/rgb.txt");
		if (!f.open(IO_ReadOnly))
			return NullList;
		QTextStream str(&f);
		QString red, green, blue;
		while (!str.atEnd())
		{
			str >> red;
			if (red.simplifyWhiteSpace()[0].latin1() == '!')
			{
				str.readLine();
				continue;
			}
			str >> green >> blue;
			const_cast<KolourPicker *>(this)->m_colorNames[(red.toInt() << 16) + (green.toInt() << 8) + blue.toInt()]
					.append(str.readLine().simplifyWhiteSpace());
		}
	}
	return m_colorNames[(r << 16) + (g << 8) + b];
}

// vim: ts=4 sw=4 noet
