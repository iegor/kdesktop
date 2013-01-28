/***************************************************************************
                          ksig.h  -  description
                             -------------------
    begin                : Tue Jul  9 23:14:22 EDT 2002
    copyright            : (C) 2002 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSIG_H
#define KSIG_H

#include "siglistview.h"

#include <kmainwindow.h>

class KTextEdit;
class QSplitter;

class KSig : public KMainWindow
{
    Q_OBJECT

public:
    KSig(QWidget *parent = 0, const char *name = 0);
    virtual ~KSig();

private:
    enum StatusBarColumns { LineNumber, ColumnNumber };

    void setupLayout();
    void setupActions();
    void setupSearchLine();
    void loadData();
    void readConfig();
    void writeConfig();

    virtual bool queryClose();

private slots:
    void add();
    void remove();
    void save();
    void quit();
    void setDataChanged(bool value = true);
    
    void editHeader();
    void editFooter();
    void setHeader(const QString &text) { header = text; writeConfig(); }
    void setFooter(const QString &text) { footer = text; writeConfig(); }

    void updateList();
    void updateEdit();
    void updateStatus(int line, int column);

private:
    QString header;
    QString footer;

    QSplitter *splitter;
    SigListView *sigList;
    KTextEdit *sigEdit;
    bool updateListLock;
    bool changed;
};

#endif
