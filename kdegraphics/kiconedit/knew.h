/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#ifndef __KNEWICON_H__
#define __KNEWICON_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlistbox.h>

#include <kwizard.h>

#include "kresize.h"

class KIconListBox;
class KIconTemplateContainer;
class QButtonGroup;
class QRadioButton;
class QGroupBox;

struct KIconTemplate
{
  QString path;
  QString title;
};

class KIconListBoxItem : public QListBoxItem    
{
public:
  KIconListBoxItem( KIconTemplate ); 


  const QString path() { return icontemplate.path; }
  KIconTemplate& iconTemplate() { return icontemplate; }
  void reloadIcon();

protected:
  virtual void paint( QPainter * );      
  virtual int height( const QListBox * ) const;      
  virtual int width( const QListBox * ) const;      
private:
  QPixmap pm;
  KIconTemplate icontemplate;
};

class KIconListBox : public QListBox    
{
  Q_OBJECT
public:
  KIconListBox( QWidget *parent ) : QListBox(parent) {} ;
  const QString path(int idx) { return ((KIconListBoxItem*)item(idx))->path(); }
  KIconTemplate& iconTemplate(int idx) { return ((KIconListBoxItem*)item(idx))->iconTemplate(); }

};

class KIconTemplateContainer : public QValueList<KIconTemplate>
{
public:
   static KIconTemplateContainer* self()
   {
      if (!instance)
         instance = new KIconTemplateContainer;
      return instance;
   }

   void save();

private:
   static KIconTemplateContainer* instance;

  const KIconTemplateContainer operator = (const KIconTemplateContainer&);
  KIconTemplateContainer(const KIconTemplateContainer&);

  KIconTemplateContainer();  
  ~KIconTemplateContainer();
};

class NewSelect : public QWidget
{
  Q_OBJECT
public:
  NewSelect(QWidget *parent);
  ~NewSelect();

signals:
  void iconopenstyle(int);

protected slots:
  void buttonClicked(int);

protected:
  KWizard *wiz;
  QButtonGroup *grp;
  QRadioButton *rbscratch, *rbtempl;
};

class NewFromTemplate : public QWidget
{
  Q_OBJECT
public:
  NewFromTemplate(QWidget *parent);
  ~NewFromTemplate();

  const QString path() { return QString(templates->path(templates->currentItem())); }

public slots:
  void checkSelection(int);
 
protected:
  KIconListBox *templates;
  KWizard *wiz;
  QGroupBox *grp;
};

class KNewIcon : public KWizard
{
  Q_OBJECT
public:
  KNewIcon(QWidget *parent);
  ~KNewIcon();

  enum { Blank = 0, Template = 1};
  int openStyle() { return openstyle; }
  const QString templatePath() { return QString(templ->path()); }
  const QSize templateSize() { return scratch->getSize(); }

protected slots:
  void okClicked();
  void cancelClicked();
  void iconOpenStyle(int);
  void checkPage(bool);
  void checkPage(const QString &);

signals:
  void newicon(const QSize);
  void newicon(const QString);

protected:
  NewSelect       *select;
  KResizeWidget   *scratch;
  NewFromTemplate *templ;
  QWidget         *act;
  int              openstyle;
};

#endif // __KNEWICON_H__
