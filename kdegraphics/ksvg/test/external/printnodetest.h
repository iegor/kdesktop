#ifndef PRINTNODETEST_H
#define PRINTNODETEST_H

#include <kurl.h>
#include <qobject.h>
	
namespace KSVG
{

class SVGDocument;
class Worker : public QObject
{
Q_OBJECT
public:
	Worker(const KURL &url);
	~Worker();

private slots:
	void slotParsingFinished(bool error, const QString &errorDesc);

private:
	// It's not allowed to store a m_doc object here,
	// because it's ctor is called before we can setup
	// our 'DocumentFactory', in that case it'll just crash (Niko)
	SVGDocument *m_doc;
};

}

#endif

// vim:ts=4:noet
