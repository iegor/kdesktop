/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <krun.h>
#include <kdebug.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <ktempfile.h>
#include <ksimpleconfig.h>
#include <kaboutapplication.h>

#include "ksvg_widget.h"
#include "ksvg_factory.h"
#include "ksvg_plugin.moc"

#include "KSVGCanvas.h"
#include "KSVGLoader.h"
#include "CanvasFactory.h"
#include "DocumentFactory.h"

#include "SVGDocument.h"
#include "SVGWindowImpl.h"
#include "SVGZoomAndPan.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

#include <qimage.h>

using namespace KSVG;

#define DEFAULT_WIDTH 400
#define DEFAULT_HEIGHT 400
#define ZOOM_FACTOR 1.2

struct KSVGPlugin::Private
{
	KSVGWidget *window;
	KSVGPluginBrowserExtension *extension;

	KAction *zoomInAction;
	KAction *zoomOutAction;
	KAction *zoomResetAction;
	KAction *stopAnimationsAction;
	KAction *viewSourceAction;
	KAction *viewMemoryAction;
	KAction *aboutApp;
	KAction *saveToPNG;
	KToggleAction *fontKerningAction;
	KToggleAction *progressiveAction;
	KSelectAction *renderingBackendAction;

	QString description;

	QPoint panPoint;
	float zoomFactor;

	SVGDocumentImpl *doc;
	KSVGCanvas *canvas;
	QPixmap *backgroundPixmap;
	KAboutApplication *aboutKSVG;

	unsigned int width;
	unsigned int height;
};

KSVGPlugin::KSVGPlugin(QWidget *wparent, const char *, QObject *parent, const char *name, unsigned int width, unsigned int height) : KParts::ReadOnlyPart(parent, name)
{
	kdDebug(26003) << "KSVGPlugin::KSVGPlugin" << endl;
	setInstance(KSVGPluginFactory::instance());

	ksvgd = new KSVGPlugin::Private();

	ksvgd->width = width;
	ksvgd->height = height;

	ksvgd->zoomFactor = 1.0;

	ksvgd->doc = 0;

	ksvgd->window = new KSVGWidget(this, wparent, "Rendering Widget");
	connect(ksvgd->window, SIGNAL(browseURL(const QString &)), this, SLOT(browseURL(const QString &)));
	ksvgd->window->show();

	KParts::Part::setWidget(ksvgd->window);

	ksvgd->extension = new KSVGPluginBrowserExtension(this);

	ksvgd->backgroundPixmap = new QPixmap(width > 0 ? width : DEFAULT_WIDTH, height > 0 ? height : DEFAULT_HEIGHT);
	ksvgd->backgroundPixmap->fill();

	ksvgd->canvas = KSVG::CanvasFactory::self()->loadCanvas(width > 0 ? width : DEFAULT_WIDTH, height > 0 ? height : DEFAULT_HEIGHT);
	if(!ksvgd->canvas)
		return;

	ksvgd->canvas->setup(ksvgd->backgroundPixmap, ksvgd->window);

	ksvgd->zoomInAction = KStdAction::zoomIn(this, SLOT(slotZoomIn()), actionCollection());
	ksvgd->zoomOutAction = KStdAction::zoomOut(this, SLOT(slotZoomOut()), actionCollection());
	ksvgd->zoomResetAction =  new KAction(i18n("Zoom &Reset"), "viewmag", this, SLOT(slotZoomReset()), actionCollection(), "zoom_reset");
	ksvgd->stopAnimationsAction = new KAction(i18n("&Stop Animations"), "stop", Key_Escape, this, SLOT(slotStop()), actionCollection(), "stop_anims");
	ksvgd->viewSourceAction = new KAction(i18n("View &Source"), "document2", Key_F6, this, SLOT(slotViewSource()), actionCollection(), "view_source");
	ksvgd->viewMemoryAction = new KAction(i18n("View &Memory"), "document2", Key_F7, this, SLOT(slotViewMemory()), actionCollection(), "view_memory");
	ksvgd->saveToPNG = new KAction(i18n("Save to PNG..."), "save", 0, this, SLOT(slotSaveToPNG()), actionCollection(), "save_to_png");
//	ksvgd->aboutApp = KStdAction::aboutApp(this, SLOT(slotAboutKSVG()), actionCollection());//, "KSVG");
	ksvgd->aboutApp = new KAction(i18n("About KSVG"), "vectorgfx", 0, this, SLOT(slotAboutKSVG()), actionCollection(), "help_about_app");
	ksvgd->fontKerningAction = new KToggleAction(i18n("Use Font &Kerning"), "viewmagfit", Key_F8, this, SLOT(slotFontKerning()), actionCollection(), "font_kerning");
	ksvgd->progressiveAction = new KToggleAction(i18n("Use &Progressive Rendering"), "", Key_F9, this, SLOT(slotProgressiveRendering()), actionCollection(), "progressive");

	KSimpleConfig config("ksvgpluginrc", true);
	config.setGroup("Rendering");
	ksvgd->fontKerningAction->setChecked(config.readBoolEntry("FontKerning", true));
	ksvgd->progressiveAction->setChecked(config.readBoolEntry("ProgressiveRendering", true));

	ksvgd->renderingBackendAction = new KSelectAction(i18n("Rendering &Backend"), 0, this, SLOT(slotRenderingBackend()), actionCollection(), "rendering_backend");

	QStringList items;
	QPtrList<KSVG::CanvasInfo> canvasList = KSVG::CanvasFactory::self()->canvasList();
	QPtrListIterator<KSVG::CanvasInfo> it(canvasList);
	KSVG::CanvasInfo *canvasInfo = 0;
	while((canvasInfo = it.current()) != 0)
	{
		items << canvasInfo->name;
		++it;
	}

	ksvgd->renderingBackendAction->setItems(items);
	ksvgd->renderingBackendAction->setCurrentItem(KSVG::CanvasFactory::self()->itemInList(ksvgd->canvas));

	ksvgd->aboutKSVG = new KAboutApplication(KSVGPluginFactory::instance()->aboutData(), wparent);

	setXMLFile("ksvgplugin.rc");
}

KSVGPlugin::~KSVGPlugin()
{
	kdDebug(26003) << "KSVGPlugin::~KSVGPlugin" << endl;

	if(ksvgd->doc && ksvgd->doc->rootElement())
		ksvgd->doc->rootElement()->pauseAnimations();

	KSVG::CanvasFactory::self()->cleanup();
	delete ksvgd->extension;

	if(ksvgd->doc)
		ksvgd->doc->detach();

	delete ksvgd->canvas;
	delete ksvgd->backgroundPixmap;
	delete ksvgd;
}

SVGDocumentImpl *KSVGPlugin::docImpl()
{
	return ksvgd->doc;
}

SVGDocument KSVGPlugin::document()
{
	return SVGDocument(ksvgd->doc);
}

void KSVGPlugin::reset()
{
	if(ksvgd->canvas)
		ksvgd->canvas->reset();

	ksvgd->zoomFactor = 1;
	ksvgd->window->reset();
	ksvgd->panPoint = QPoint(0, 0);
}

bool KSVGPlugin::openURL(const KURL &url)
{
	m_url = url;

	if(!url.prettyURL().isEmpty())
	{
		reset();

		DocumentFactory *docFactory = DocumentFactory::self();

		// when embedded, stick to passed width and height
		ksvgd->doc = docFactory->requestDocumentImpl((ksvgd->width > 0 && ksvgd->height > 0));

		ksvgd->doc->attach(ksvgd->canvas);
		ksvgd->doc->addToDocumentDict(ksvgd->doc->handle(), ksvgd->doc);
		ksvgd->doc->setReferrer(ksvgd->extension->urlArgs().metaData()["referrer"]);

		connect(ksvgd->doc, SIGNAL(finishedParsing(bool, const QString &)), this, SLOT(slotParsingFinished(bool, const QString &)));
		connect(ksvgd->doc, SIGNAL(finishedRendering()), this, SLOT(slotRenderingFinished()));
		connect(ksvgd->doc, SIGNAL(gotDescription(const QString &)), this, SLOT(slotSetDescription(const QString &)));
		connect(ksvgd->doc, SIGNAL(gotTitle(const QString &)), this, SLOT(slotSetTitle(const QString &)));
		connect(ksvgd->doc, SIGNAL(gotURL(const QString &)), this, SLOT(slotGotURL(const QString &)));
		connect(ksvgd->window, SIGNAL(redraw(const QRect &)), this, SLOT(slotRedraw(const QRect &)));

		ksvgd->backgroundPixmap->fill();
		bitBlt(ksvgd->window, 0, 0, ksvgd->backgroundPixmap, 0, 0, ksvgd->backgroundPixmap->width(), ksvgd->backgroundPixmap->height());
		ksvgd->zoomFactor = 1;

		emit started(0);
		ksvgd->doc->open(url);
		emit completed();
	}
	else
		return false;

	return true;
}

void KSVGPlugin::browseURL(const QString &url)
{
	ksvgd->doc->rootElement()->pauseAnimations();
	KParts::URLArgs args;
	args.frameName = "_self";
	emit ksvgd->extension->openURLRequest(KURL(m_url, url), args);
}

void KSVGPlugin::slotRedraw(const QRect &r)
{
	if(ksvgd->window->width() != ksvgd->backgroundPixmap->width() ||
	   ksvgd->window->height() != ksvgd->backgroundPixmap->height())
	{
		ksvgd->backgroundPixmap->resize(ksvgd->window->width(), ksvgd->window->height());

		if(ksvgd->doc && ksvgd->doc->canvas())
		{
			ksvgd->doc->canvas()->resize(ksvgd->window->width(), ksvgd->window->height());
			ksvgd->doc->canvas()->blit();
		}
	}

	bitBlt(ksvgd->window, r.x(), r.y(), ksvgd->backgroundPixmap, r.x(), r.y(), r.width(), r.height());
}

void KSVGPlugin::slotViewSource()
{
	KTempFile temp;
	*temp.textStream() << KSVGLoader::getUrl(m_url, true) << endl;
	KRun::runURL(KURL(temp.name()), "text/plain", true);
}

void KSVGPlugin::slotViewMemory()
{
	KTempFile temp;
	*temp.textStream() << ksvgd->doc->window()->printNode(*ksvgd->doc).string() << endl;
	KRun::runURL(KURL(temp.name()), "text/plain", true);
}

void KSVGPlugin::slotFontKerning()
{
	KSimpleConfig config("ksvgpluginrc", false);
	config.setGroup("Rendering");
	config.writeEntry("FontKerning", ksvgd->fontKerningAction->isChecked());

	if(ksvgd->doc)
	{
		SVGSVGElementImpl *root = ksvgd->doc->rootElement();
		if(!root)
			return;

		ksvgd->doc->canvas()->fontContext()->setKerning(ksvgd->fontKerningAction->isChecked());
		update();
	}
}

void KSVGPlugin::slotProgressiveRendering()
{
	KSimpleConfig config("ksvgpluginrc", false);
	config.setGroup("Rendering");
	config.writeEntry("ProgressiveRendering", ksvgd->progressiveAction->isChecked());
}

void KSVGPlugin::slotRenderingBackend()
{
	KSimpleConfig config("ksvgpluginrc", false);
	config.setGroup("Canvas");
	config.writeEntry("ActiveCanvas", KSVG::CanvasFactory::self()->internalNameFor(ksvgd->renderingBackendAction->currentText()));
	config.sync();
	KSVG::CanvasFactory::self()->deleteCanvas(ksvgd->canvas);
	ksvgd->canvas = KSVG::CanvasFactory::self()->loadCanvas(ksvgd->width > 0 ? ksvgd->width : DEFAULT_WIDTH, ksvgd->height > 0 ? ksvgd->height : DEFAULT_HEIGHT);
	if(!ksvgd->canvas)
		return;

	ksvgd->canvas->setup(ksvgd->backgroundPixmap, ksvgd->window);
	openURL(m_url);
}

void KSVGPlugin::slotAboutKSVG()
{
	ksvgd->aboutKSVG->show();
}

void KSVGPlugin::slotStop()
{
	if(ksvgd->doc->rootElement()->animationsPaused())
		ksvgd->doc->rootElement()->unpauseAnimations();
	else
		ksvgd->doc->rootElement()->pauseAnimations();
}

void KSVGPlugin::slotParsingFinished(bool error, const QString &errorDesc)
{
	emit completed();

	if(error)
	{
		kdDebug(26003) << "Finished with error : " << errorDesc << endl;
		emit setStatusBarText(errorDesc);
	}
	else
		kdDebug(26003) << "Finished without errors!" << endl;
}

void KSVGPlugin::slotRenderingFinished()
{
	bitBlt(ksvgd->window, 0, 0, ksvgd->backgroundPixmap, 0, 0, ksvgd->canvas->width(), ksvgd->canvas->height());
}

void KSVGPlugin::slotZoomIn()
{
	ksvgd->zoomFactor *= ZOOM_FACTOR;
	update();
}

void KSVGPlugin::slotZoomOut()
{
	ksvgd->zoomFactor *= (1.0 / ZOOM_FACTOR);
	update();
}

void KSVGPlugin::slotZoomReset()
{
	ksvgd->zoomFactor = 1.0;
	update();
}

void KSVGPlugin::slotSaveToPNG()
{
	if(ksvgd && ksvgd->backgroundPixmap)
	{
		QImage img = ksvgd->backgroundPixmap->convertToImage();
		QString filename = KFileDialog::getSaveFileName();
		if(!filename.isEmpty())
			img.save(filename, "PNG");
	}
}

void KSVGPlugin::setPanPoint(const QPoint &translate)
{
	ksvgd->panPoint = translate;
	update();
}

void KSVGPlugin::update()
{
	if(ksvgd->doc)
	{
		SVGSVGElementImpl *root = ksvgd->doc->rootElement();
		if(!root || root->zoomAndPan() != SVG_ZOOMANDPAN_MAGNIFY)
			return;

		ksvgd->backgroundPixmap->fill();

		bool zoomChanged = ksvgd->zoomFactor != root->currentScale();
		root->setCurrentScale(ksvgd->zoomFactor);
		root->setCurrentTranslate(ksvgd->panPoint);

		ksvgd->doc->syncCachedMatrices();

		if(zoomChanged)
			ksvgd->doc->canvas()->update(ksvgd->zoomFactor);
		else
			ksvgd->doc->canvas()->update(ksvgd->panPoint);

		// Fixes drawing glitches
		slotRedraw(QRect(0, 0, ksvgd->backgroundPixmap->width(), ksvgd->backgroundPixmap->height()));
	}
}

void KSVGPlugin::slotSetDescription(const QString &desc)
{
	ksvgd->description = desc;
	emit setStatusBarText(i18n("Description: %1").arg(desc));
}

void KSVGPlugin::slotSetTitle(const QString &title)
{
	emit setWindowCaption(title);
}

void KSVGPlugin::slotGotURL(const QString &text)
{
	if(text.isNull() && !ksvgd->description.isEmpty())
		emit setStatusBarText(i18n("Description: %1").arg(ksvgd->description));
	else
		emit setStatusBarText(text);
}

// vim:ts=4:noet
