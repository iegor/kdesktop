// (c) 2000 Peter Putzer

#ifndef KSV_SPINBOX_H
#define KSV_SPINBOX_H

#include <qspinbox.h>

#include <kcompletion.h>

class KSVSpinBox : public QSpinBox, public KCompletionBase
{
  Q_OBJECT

public:
  KSVSpinBox (QWidget* parent, const char* name = 0L);
  virtual ~KSVSpinBox ();
  
  virtual bool eventFilter (QObject*, QEvent*);
  virtual void setCompletedText (const QString&);
  virtual void setCompletedItems (const QStringList&);

protected:
  virtual QString mapValueToText (int value);

private slots:
  void handleMatch (const QString&);

private:
  bool mClearedSelection;
};

#endif // KSV_SPINBOX_H
