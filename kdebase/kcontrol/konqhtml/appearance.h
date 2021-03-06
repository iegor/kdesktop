// -*- c-basic-offset: 2 -*-
// (c) Martin R. Jones 1996
// (c) Bernd Wuebben 1998
// KControl port & modifications
// (c) Torben Weis 1998
// End of the KControl port, added 'kfmclient configure' call.
// (c) David Faure 1998
// Cleanup and modifications for KDE 2.1
// (c) Daniel Molkentin 2000

#ifndef __APPEARANCE_H__
#define __APPEARANCE_H__

#include <qwidget.h>
#include <qmap.h>

#include <kcmodule.h>

class QSpinBox;
class KFontCombo;

class KAppearanceOptions : public KCModule
{
  Q_OBJECT
public:
  KAppearanceOptions(KConfig *config, QString group, QWidget *parent=0, const char *name=0);
  ~KAppearanceOptions();

  virtual void load();
  virtual void load( bool useDefaults );
  virtual void save();
  virtual void defaults();

public slots:
  void slotFontSize( int );
  void slotMinimumFontSize( int );
  void slotStandardFont(const QString& n);
  void slotFixedFont(const QString& n);
  void slotSerifFont( const QString& n );
  void slotSansSerifFont( const QString& n );
  void slotCursiveFont( const QString& n );
  void slotFantasyFont( const QString& n );
  void slotEncoding( const QString& n);
  void slotFontSizeAdjust( int value );

private:
  void updateGUI();

private:

  KConfig *m_pConfig;
  QString m_groupname;
  QStringList m_families;

  KIntNumInput* m_minSize;
  KIntNumInput* m_MedSize;
  KIntNumInput* m_pageDPI;
  KFontCombo* m_pFonts[6];
  QComboBox* m_pEncoding;
  QSpinBox *m_pFontSizeAdjust;

  int fSize;
  int fMinSize;
  QStringList encodings;
  QStringList fonts;
  QStringList defaultFonts;
  QString encodingName;
};

#endif // __APPEARANCE_H__
