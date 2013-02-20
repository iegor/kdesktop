// (c) 2000 Peter Putzer

#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <qstring.h>
#include <kdialogbase.h>

class QPushButton;

class KSVLookAndFeel;
class KSVMiscConfig;
class KSVPathConfig;
class KSVConfig;


class KSVPreferences : public KDialogBase
{
  Q_OBJECT

public:
  static KSVPreferences* self ();
  virtual ~KSVPreferences ();

  QColor newNormal () const;
  QColor newSelected () const;
  QColor changedNormal () const;
  QColor changedSelected () const;

  QString runlevelPath () const;
  QString scriptPath () const;

protected:
  virtual void showEvent (QShowEvent* e);

signals:
  void updateColors ();
  void updateServicesPath ();
  void updateRunlevelsPath ();
  void updateFonts ();

protected slots:
  virtual void slotApply ();
  virtual void slotCancel ();
  virtual void slotOk ();
  void slotChanged();

private slots:
  void reReadConfig();
  void showAllMessages ();

private:
  friend class KSVTopLevel;

  KSVPreferences (QWidget* parent);
  void setConfig ();
  bool checkPaths();
  void reReadMessages();
  KSVConfig* mConfig;

  KSVLookAndFeel* mLookAndFeel;
  KSVPathConfig* mPathConfig;
  KSVMiscConfig* mMiscConfig;
  bool configChanged;
};


#endif // PREFERENCES_DIALOG_H
