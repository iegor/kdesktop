/*
  operations with skinset.
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <config.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qbitmap.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include "waSkinMapping.h"
#include "waShadeMapping.h"

#include "waSkinModel.h"
#include "waColor.h"
#include "waRegion.h"
#include "waSkin.h"

WaSkinModel *_waskinmodel_instance = NULL;

// Our current skin map
// Can switch between normal and windowshade maps
const SkinMap *mapToGui;
const SkinDesc *mapFromFile;
int digit_width;
int digit_height;


struct WaPixmapEntry {
    const char *filename;
    QPixmap *pixmap;
};

WaPixmapEntry waPixmapEntries[11] = {
                                      {"main.bmp", NULL},
                                      {"cbuttons.bmp", NULL},
                                      {"monoster.bmp", NULL},
                                      {"numbers.bmp", NULL},
                                      {"shufrep.bmp", NULL},
                                      {"text.bmp", NULL},
                                      {"volume.bmp", NULL},
                                      {"balance.bmp", NULL},
                                      {"posbar.bmp", NULL},
                                      {"playpaus.bmp", NULL},
                                      {"titlebar.bmp", NULL}
                                  };

WaSkinModel::WaSkinModel()
{ 
    for (int x = 0;x < 11;x++) 
        waPixmapEntries[x].pixmap = new QPixmap;    

    resetSkinModel();
    _waskinmodel_instance = this;
}

WaSkinModel::~WaSkinModel()
{
    for (int x = 0;x < 11;x++) 
        delete waPixmapEntries[x].pixmap;    

    delete windowRegion;
    delete colorScheme;
}

bool WaSkinModel::load(QString skinDir)
{
    bool success = true;

    QDir dir(skinDir);

    if (findFile(dir, "main.bmp").isEmpty()) {
        // Ack, our skin dir doesn't exist, fall back to the default
        dir = QDir(KGlobal::dirs()->findDirs("data", "noatun/skins/winamp/" + WaSkin::defaultSkin())[0]);
        success = false;
    }

    for (int x = 0;x < 11;x++) {
      getPixmap(dir, waPixmapEntries[x].filename, 
		waPixmapEntries[x].pixmap);
    }

    resetSkinModel();

    loadColors(dir);
    loadRegion(dir);

    emit(skinChanged());

    return success;
}

// Does a case-insenstive file search (like DOS/Windows)
// Filename -must- be lowercase, which is an nice optimization, since
// this is a private API, and all our filenames are internally lowercase
// anyway
QString WaSkinModel::findFile(const QDir &dir, const QString &filename) {
    QFileInfo fileInfo;
    QString ret = "";

    QStringList strList = dir.entryList();

    for (QStringList::iterator file = strList.begin(); file != strList.end(); file++) {
	QFileInfo fileInfo(*file);

	if (fileInfo.isDir()) 
	    continue;

	if (fileInfo.filePath().lower() == filename) 
            return dir.absPath() + "/" + QString(fileInfo.filePath());
    }

    return "";
}

void WaSkinModel::loadColors(const QDir &dir) {
    QString colorFile = findFile(dir, "viscolor.txt");

    if (colorScheme) {
      delete colorScheme;
    }

    colorScheme = new WaColor(colorFile);
}

void WaSkinModel::loadRegion(const QDir &dir) {
    QString regionFile = findFile(dir, "region.txt");

    if (windowRegion) {
      delete windowRegion;
      windowRegion = 0;
    }

    windowRegion = new WaRegion(regionFile);
}

int WaSkinModel::getPixmap(const QDir &dir, QString fname,
			       QPixmap *target)
{
    QFileInfo fileInfo;
    QStringList strList = dir.entryList();
    QString abspath;

    abspath = findFile(dir, fname);

    if (!abspath.isEmpty()) {
        target->load(abspath);
        return true;
    }

    // now the filename mapping 1.8x -> 2.0
    if (fname == "volume.bmp") 
	return WaSkinModel::getPixmap(dir, QString("volbar.bmp"), target);

    if (fname == "numbers.bmp") 
	return WaSkinModel::getPixmap(dir, QString("nums_ex.bmp"), target);

    // Even 2.x themes can omit BALANCE, in which case we use VOLUME
    if (fname == "balance.bmp") 
	return WaSkinModel::getPixmap(dir, QString("volume.bmp"), target);

    return false;
}


QRect WaSkinModel::getGeometry(int id) {
  if ( (id < 0) || (id >= _WA_SKIN_ENTRIES) ) {
    kdDebug() << "Array index out of range. WaSkinModel::getGeometry"<<endl;
    exit(-1);
  }
  return QRect(mapFromFile[id].x, mapFromFile[id].y,
	       mapFromFile[id].width, mapFromFile[id].height);
}

QRect WaSkinModel::getMapGeometry(int id) {
  if ( (id < 0) || (id >= _WA_MAPPING_ENTRIES) ) {
    kdDebug() << "Array index out of range. WaSkinModel::getMapGeometry"<<endl;
    exit(-1);
  }  
  return QRect(mapToGui[id].x, mapToGui[id].y,
	       mapToGui[id].width, mapToGui[id].height);
}
 
void WaSkinModel::bltTo(int id, QPaintDevice *dest, int x, int y) {
    bitBlt(dest, x, y, waPixmapEntries[mapFromFile[id].fileId].pixmap,
           mapFromFile[id].x, mapFromFile[id].y,
           mapFromFile[id].width, mapFromFile[id].height);
}

void WaSkinModel::bltTo(int id, QPaintDevice *dest, int x, int y, int argument) {
  if (id == _WA_SKIN_VOLUME_BAR) {
    QPixmap *pix = waPixmapEntries[_WA_FILE_VOLUME].pixmap;

    int nBar = int((float)argument * 27.0 / 100.0);
    bitBlt(dest, x, y, pix, 0, 15 * nBar, 68, 13);

    return;
  }

  if (id == _WA_SKIN_BALANCE_BAR) {
    QPixmap *pix = waPixmapEntries[_WA_FILE_BALANCE].pixmap;

    argument = abs(argument);
 
    int nBar = int((float)argument * 27.0 / 100.0);
    bitBlt(dest, x, y, pix, 9, 15 * nBar, 38, 13);

    return;
  }

  bltTo(id, dest, x, y);
}

void WaSkinModel::getDigit(char number, QPaintDevice *dest, int x, int y) {
    if (number=='-') { 
      bltTo(_WA_SKIN_NUMBERS_MINUS, dest, x, y);
      return;
    }

    // empty number ?
    if (number == ' ') {
      bltTo(_WA_SKIN_NUMBERS_BLANK, dest, x, y);
      return;
    }

    // number
    QPixmap *pix = waPixmapEntries[mapFromFile[_WA_SKIN_NUMBERS].fileId].pixmap;

    // ordinary number:
    int index = number - '0';
    if ((index < 0) || (index > 9)) 
      return;

    bitBlt(dest, x, y, pix , (index * digit_width) + mapFromFile[_WA_SKIN_NUMBERS].x, mapFromFile[_WA_SKIN_NUMBERS].y, digit_width, digit_height);

    return;
}

void WaSkinModel::getText(char text, QPaintDevice * dest, int x, int y) {
  QPixmap *pix = waPixmapEntries[_WA_FILE_TEXT].pixmap;

  text = deaccent(text);

  if (('A' <= text) && (text <= 'Z')) {
    bitBlt(dest, x, y,pix,(text-'A')*5,0,5,6);
    return;
  }
  if (('a' <= text) && (text <= 'z')) {
    bitBlt(dest, x, y,pix,(text-'a')*5,0,5,6);
    return;
  }
  if (('0' <= text) && (text <= '9')) {
    bitBlt(dest, x, y,pix,(text-'0')*5,6,5,6);
    return;
  }
  if ('"' == text) {
    bitBlt(dest, x, y,pix,27*5,0,5,6);
    return;
  }
  if ('@' == text) {
    bitBlt(dest, x, y,pix,28*5,0,5,6);
    return;
  }


  if ('.' == text) {
    bitBlt(dest, x, y,pix,11*5,6,5,6);
    return;
  }
  if (':' == text) {
    bitBlt(dest, x, y,pix,12*5,6,5,6);
    return;
  }
  if (('(' == text) || ('<' == text) || ('{' == text)) {
    bitBlt(dest, x, y,pix,13*5,6,5,6);
    return;
  }
  if ((')' == text) || ('>' == text) || ('}' == text)) {
    bitBlt(dest, x, y,pix,14*5,6,5,6);
    return;
  }
  if ('-' == text) {
    bitBlt(dest, x, y,pix,15*5,6,5,6);
    return;
  }
  if (('`' == text) || ('\'' == text)) {
    bitBlt(dest, x, y,pix,16*5,6,5,6);
    return;
  }
  if ('!' == text) {
    bitBlt(dest, x, y,pix,17*5,6,5,6);
    return;
  }
  if ('_' == text) {
    bitBlt(dest, x, y,pix,18*5,6,5,6);
    return;
  }
  if ('+' == text) {
    bitBlt(dest, x, y,pix,19*5,6,5,6);
    return;
  }
  if ('\\' == text) {
    bitBlt(dest, x, y,pix,20*5,6,5,6);
    return;
  }
  if ('/' == text) {
    bitBlt(dest, x, y,pix,21*5,6,5,6);
    return;
  }
  if ('[' == text) {
    bitBlt(dest, x, y,pix,22*5,6,5,6);
    return;
  }
  if (']' == text) {
    bitBlt(dest, x, y,pix,23*5,6,5,6);
    return;
  }
  if ('^' == text) {
    bitBlt(dest, x, y,pix,24*5,6,5,6);
    return;
  }
  if ('&' == text) {
    bitBlt(dest, x, y,pix,25*5,6,5,6);
    return;
  }
  if ('%' == text) {
    bitBlt(dest, x, y,pix,26*5,6,5,6);
    return;
  }
  if (',' == text) {
    bitBlt(dest, x, y,pix,27*5,6,5,6);
    return;
  }
  if ('=' == text) {
    bitBlt(dest, x, y,pix,28*5,6,5,6);
    return;
  }
  if ('$' == text) {
    bitBlt(dest, x, y,pix,29*5,6,5,6);
    return;
  }
  if ('#' == text) {
    bitBlt(dest, x, y,pix,30*5,6,5,6);
    return;
  }
  if (('â' == text) || ('Â' == text)) {
    bitBlt(dest, x, y,pix,0*5,12,5,6);
    return;
  }
  if (('ö' == text) || ('Ö' == text)) {
    bitBlt(dest, x, y,pix,1*5,12,5,6);
    return;
  } 
  if (('ä' == text) || ('Ä' == text)) {
    bitBlt(dest, x, y,pix,2*5,12,5,6);
    return;
  } 
  if ('?' == text) {
    bitBlt(dest, x, y,pix,3*5,12,5,6);
    return;
  }
  if ('*' == text) {
    bitBlt(dest, x, y,pix,4*5,12,5,6);
    return;
  }
  // default back is space char
  bitBlt(dest, x, y,pix,(10*5),12,5,6);
}

void WaSkinModel::paintBackgroundTo(int mapping, QPaintDevice *dest, int x, int y) 
{
    QPixmap *pix = waPixmapEntries[mapFromFile[_WA_SKIN_MAIN].fileId].pixmap;
    QRect main_rect = getGeometry(_WA_SKIN_MAIN);
    QRect dest_rect = getMapGeometry(mapping);

    int source_x = main_rect.x() + dest_rect.x() + x;
    int source_y = main_rect.y() + dest_rect.y() + y;

    int width = dest_rect.width() - x;
    int height = dest_rect.height() - y;

    bitBlt(dest, x, y, pix, source_x, source_y, width, height);
}

void WaSkinModel::setSkinModel(skin_models new_model) {
    if (new_model == WA_MODEL_NORMAL) {
        mapToGui = normalMapToGui;
        mapFromFile = normalMapFromFile;
        digit_width = 9;
        digit_height = 13;
    }
    else if (new_model == WA_MODEL_WINDOWSHADE) {
        mapToGui = shadeMapToGui;
        mapFromFile = shadeMapFromFile;
        digit_width = 5;
        digit_height = 6;
    }

    emit(skinChanged());
}

void WaSkinModel::resetSkinModel() {
    mapToGui = normalMapToGui;
    mapFromFile = normalMapFromFile;
    digit_width = 9;
    digit_height = 13;
}


QChar WaSkinModel::deaccent(QChar input) {
    if (QString("Á").contains(input))
        return 'A';

    if (QString("ÉË").contains(input))
        return 'E';
 
    if (QString("ÍÎ").contains(input))
        return 'I';

    if (QString("ÔÓ").contains(input))
        return 'O';

    if (QString("ÚÜ").contains(input))
        return 'U';

    if (input == 'Ý')
        return 'Y';

    if (QString("áâ").contains(input))
        return 'a';

    if (QString("éë").contains(input))
        return 'e';
 
    if (QString("îí").contains(input))
        return 'i';

    if (QString("ôó").contains(input))
        return 'o';

    if (QString("úü").contains(input))
        return 'u';

   return input;
}

#include "waSkinModel.moc"
