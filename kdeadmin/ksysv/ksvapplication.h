// (c) 2000 Peter Putzer

#ifndef KSVAPPLICATION_H
#define KSVAPPLICATION_H

#include <kuniqueapplication.h>

class KSVApplication : public KUniqueApplication
{
  Q_OBJECT

public:
  inline KSVApplication() {}
  virtual inline ~KSVApplication () {}

  virtual int newInstance ();
};

#endif // KSVAPPLICATION_H
