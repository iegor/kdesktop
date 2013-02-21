#ifndef RUNLEVEL_AUTH_ICON_H
#define RUNLEVEL_AUTH_ICON_H

// (c) 2000 Peter Putzer <putzer@kde.org>
#include <kauthicon.h>

class QTimer;
class QFileInfo;

class RunlevelAuthIcon : public KAuthIcon
{
  Q_OBJECT
  Q_PROPERTY (int refreshInterval READ refreshInterval WRITE setRefreshInterval)
    
public:
  RunlevelAuthIcon (const QString& scriptPath, const QString& runlevelPath,
					QWidget* parent = 0L, const char* name = 0L);

  virtual ~RunlevelAuthIcon ();

  virtual bool status () const;

  inline int refreshInterval () const { return mInterval; }
  inline bool isCheckEnabled () const { return mCheckEnabled; }

public slots:
  virtual void updateStatus ();

  void setServicesPath (const QString& servicesPath);
  void setRunlevelPath (const QString& runlevelPath);

  void setRefreshInterval (int);

  void setCheckEnabled(bool);

private slots:
  void timerEvent ();

private:
  QTimer* mTimer;
  QFileInfo* mServicesInfo;
  QFileInfo** mRLInfo;
  bool mOld;
  int mInterval;
  
  bool mCheckEnabled;
};

#endif // RUNLEVEL_AUTH_ICON_H
