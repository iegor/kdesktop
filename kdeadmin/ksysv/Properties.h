// (c) 2000 Peter Putzer

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <kpropertiesdialog.h>

class QHBox;
class QVBox;
class QTextView;
class QSpinBox;

class KLineEdit;

class KSVData;

class KSVServicePropertiesDialog : public KPropertiesDialog
{
  Q_OBJECT

public:
  KSVServicePropertiesDialog (KSVData& data, QWidget* parent);
  virtual ~KSVServicePropertiesDialog ();

signals:
  void startService (const QString&);
  void stopService (const QString&);
  void restartService (const QString&);
  void editService (const QString&);

private slots:
  void doEdit ();
  void doStart ();
  void doStop ();
  void doRestart ();

private:
  KSVData& mData;
};


class KSVEntryPropertiesDialog : public KPropertiesDialog
{
  Q_OBJECT

public:
  KSVEntryPropertiesDialog (KSVData& data, QWidget* parent);
  virtual ~KSVEntryPropertiesDialog ();

signals:
  void startService (const QString&);
  void stopService (const QString&);
  void restartService (const QString&);
  void editService (const QString&);

private slots:
  void doEdit ();
  void doStart ();
  void doStop ();
  void doRestart ();

private:
  KSVData& mData;
};

class KSVEntryPage : public KPropsDlgPlugin
{
  Q_OBJECT

public:
  KSVEntryPage (KSVData& data, KPropertiesDialog* props);
  virtual ~KSVEntryPage ();

  virtual void applyChanges ();

  inline int pageIndex () const { return mIndex; }

private slots:
  void emitChanged ();

private:
  KSVData& mData;
  QFrame* mPage;
  int mIndex;

  KLineEdit* mServiceEdit;
  KLineEdit* mLabelEdit;
  QSpinBox* mNumberEdit;
};

class KSVServicesPage : public KPropsDlgPlugin
{
  Q_OBJECT

public:
  KSVServicesPage (KSVData& data, KPropertiesDialog* props);
  virtual ~KSVServicesPage ();

  virtual void applyChanges ();

  inline int pageIndex () const { return mIndex; }

private:
  KSVData& mData;
  QVBox* mPage;
  QTextView* mDesc;
  int mIndex;
};

#endif // PROPERTIES_H
