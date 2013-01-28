// -*-c++-*-
#ifndef DUBCONFIGMODULE_H
#define DUBCONFIGMODULE_H


#include <noatun/pref.h>
#include "dubprefs.h"

/**
 * Noatun configuration module for dub

 * Eray Ozkural (exa)
 **/
class DubConfigModule : public CModule
{
Q_OBJECT
public:
  enum PlayOrder { normal, shuffle, repeat, single };
  enum PlayMode { allFiles, oneDir, recursiveDir };

  DubConfigModule(QObject *parent);

  ~DubConfigModule();

  virtual void save(void);
  virtual void reopen(void);

  void apply();

  /** The directory in which media files are stored */
  QString mediaDirectory;
  PlayMode playMode;
  PlayOrder playOrder;

  const DubPrefs* getPrefs() { return prefs; }
  //private:
  DubPrefs* prefs;
};

#endif
