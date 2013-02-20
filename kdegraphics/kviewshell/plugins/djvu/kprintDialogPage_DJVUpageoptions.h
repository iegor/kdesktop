// KPrintDialogPage_PageOptions.h
//
// Part of KVIEWSHELL - A framework for multipage text/gfx viewers
//
// (C) 2005 Stefan Kebekus
// Distributed under the GPL

// Add header files alphabetically

#ifndef KPRINTDIALOGPAGE_DJVUPAGEOPTIONS_H
#define KPRINTDIALOGPAGE_DJVUPAGEOPTIONS_H


#include <kdeprint/kprintdialogpage.h>


class QVBoxLayout;
class QCheckBox;


// This is a fairly standard KPrintDialogPage that allows the user to
// chose page size & placement options: shrink oversized pages, and
// expand small pages

class KPrintDialogPage_DJVUPageOptions : public KPrintDialogPage
{
 public:
  KPrintDialogPage_DJVUPageOptions( QWidget *parent = 0, const char *name = 0 );
  
  void getOptions( QMap<QString,QString>& opts, bool incldef = false );
  void setOptions( const QMap<QString,QString>& opts );
  bool isValid( QString& msg );

  QCheckBox* checkBox_rotate;
  QCheckBox* checkBox_fitpage;

 private:
  QVBoxLayout* kprintDialogPage_pageoptions_baseLayout;
};


#endif // KPRINTDIALOGPAGE_PAGEOPTIONS_H
