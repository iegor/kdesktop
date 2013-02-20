/***************************************************************************
                          kcmkuick.h 
                             -------------------
    copyright            : (C) 2001 by Holger Freyther
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef KCMKUICK_H
#define KCMKUICK_H

#include <kcmodule.h>
#include <kaboutdata.h>
#include "kcmkuickdialog.h"


class KCMKuick
	: public KCModule
{
	Q_OBJECT

public:
	KCMKuick (QWidget *parent, const char *name, const QStringList &);
        ~KCMKuick();	
	void load();
	void load(const QString &);
	void save();
	void defaults();
	QString quickHelp() const;
public slots:
	void configChanged();
private:
	KCMKuickDialog *dialog;
protected slots:
        void slotClearMoveCache();
	void slotClearCopyCache();
	void slotShowToggled();
};

#endif
