#include <DocumentFactory.h>
#include <SVGSVGElement.h>
#include <SVGDocument.h>
#include <SVGWindow.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "printnodetest.moc"

using namespace KSVG;

static KCmdLineOptions options[] =
{
	{ "+URL",   I18N_NOOP("SVG file to open"), 0 },
	KCmdLineLastOption
};

Worker::Worker(const KURL &url)
{
	m_doc = DocumentFactory::self()->requestDocument(this, SLOT(slotParsingFinished(bool, const QString &)));

	if(!m_doc)
		return;
	
	if(!DocumentFactory::self()->startParsing(m_doc, url))
		return;
}

Worker::~Worker()
{
	delete m_doc;
}

void Worker::slotParsingFinished(bool, const QString &)
{
	SVGWindow win = m_doc->window();	
	SVGSVGElement root = m_doc->rootElement();
	
	kdDebug() << win.printNode(root).string() << endl;
	exit(0);
}

int main(int argc, char **argv)
{
	KAboutData *about = new KAboutData("KSVG printNode test app", I18N_NOOP("KSVG test"), "0.1", I18N_NOOP("KSVG printNode test app"));
	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(options);
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() == 0)
	{
		args->usage();
		return -1;
	}
	
	KApplication a(argc, argv);
	(void) new Worker(args->url(0));
	return a.exec();
}

