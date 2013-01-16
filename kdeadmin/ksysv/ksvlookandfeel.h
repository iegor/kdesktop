/**
 * Released under the GNU General Public License, version 2.
 * 
 * Copyright (c) 2000 Peter Putzer <putzer@kde.org>
 */

#ifndef KSVLOOKANDFEEL_H
#define KSVLOOKANDFEEL_H
#include "lookandfeelconfig.h"

#include <qfont.h>
class KSVLookAndFeel : public LookAndFeel
{ 
  Q_OBJECT
  
public:
  KSVLookAndFeel( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~KSVLookAndFeel();
  
  inline const QFont& serviceFont() const { return mServiceFont; }
  inline const QFont& numberFont() const { return mNumberFont; }
  
  void setServiceFont (const QFont& font);
  void setNumberFont (const QFont& font);

signals:
  void configChanged();

protected slots:
  void chooseServiceFont();
  void chooseNumberFont();
  void slotChanged();

private:
  QFont mServiceFont;
  QFont mNumberFont;
};

#endif // KSVLOOKANDFEEL_H
