#ifndef KIMAGEVIEWERTEST_H
#define KIMAGEVIEWERTEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapplication.h>
#include <kparts/mainwindow.h>
 
namespace KImageViewer { class Canvas; };

class KImageViewerTest : public KParts::MainWindow
{
    Q_OBJECT
public:
    KImageViewerTest();
    virtual ~KImageViewerTest();
    void load(const KURL& url);

private:
    KImageViewer::Canvas *m_part;
};

#endif // KIMAGEVIEWERTEST_H
