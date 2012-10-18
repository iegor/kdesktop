/*
  Model for winamp skins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#ifndef __WASKINMODEL_H
#define __WASKINMODEL_H

#include <qstring.h>
#include <qobject.h>
#include <qdir.h>

#include "waSkins.h"

enum skin_models { WA_MODEL_NORMAL, WA_MODEL_WINDOWSHADE };

class QChar;
class WaSkinModel;
extern WaSkinModel *_waskinmodel_instance;

class WaSkinModel : public QObject {
  Q_OBJECT 

  public:
     WaSkinModel();
    ~WaSkinModel();

    void getDigit(char number, QPaintDevice *dest, int x, int y);
    void getText(char text, QPaintDevice *dest, int x, int y);

    void bltTo(int id, QPaintDevice *dest, int x, int y);
    void bltTo(int id, QPaintDevice *dest, int x, int y, int argument);

    void paintBackgroundTo(int mapping, QPaintDevice *dest, int x, int y);

    QRect getGeometry(int id);
    QRect getMapGeometry(int id);

    bool load(QString skinDir);

    void setSkinModel(skin_models new_model);
    static WaSkinModel *instance() { return _waskinmodel_instance; }
 
  private:
    void resetSkinModel();

    QString findFile(const QDir &dir, const QString &filename);
    QChar deaccent(QChar input);

    int getPixmap(const QDir &dir, QString fname, QPixmap * target);
    void loadColors(const QDir &dir);
    void loadRegion(const QDir &dir);
 
  signals:
    void skinChanged(); 
};
#endif
