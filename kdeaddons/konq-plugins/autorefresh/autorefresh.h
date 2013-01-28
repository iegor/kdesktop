// -*- c++ -*-

/*
 * Copyright 2003 by Richard J. Moore, rich@kde.org
 */

#ifndef __plugin_autorefresh_h
#define __plugin_autorefresh_h
 
#include <kparts/plugin.h>
#include <klibloader.h>
 
class KURL;
class KInstance;
class QTimer;

/**
 * A plugin is the way to add actions to an existing @ref KParts application,
 * or to a @ref Part.
 *
 * The XML of those plugins looks exactly like of the shell or parts,
 * with one small difference: The document tag should have an additional
 * attribute, named "library", and contain the name of the library implementing
 * the plugin.
 *
 * If you want this plugin to be used by a part, you need to
 * install the rc file under the directory
 * "data" (KDEDIR/share/apps usually)+"/instancename/kpartplugins/"
 * where instancename is the name of the part's instance.
 **/
class AutoRefresh : public KParts::Plugin
{
    Q_OBJECT
public:

    /**
     * Construct a new KParts plugin.
     */
    AutoRefresh( QObject* parent = 0, const char* name = 0, const QStringList &args = QStringList() );

    /**
     * Destructor.
     */
    virtual ~AutoRefresh();

public slots:
    void slotRefresh();
    void slotIntervalChanged();
    
private:
   KSelectAction *refresher;
   QTimer *timer;
};

#endif
