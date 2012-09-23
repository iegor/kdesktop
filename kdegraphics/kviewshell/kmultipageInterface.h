// -*- C++ -*-
#ifndef kmultipageINTERFACE_H
#define kmultipageINTERFACE_H
 
#include <dcopobject.h>
 
class kmultipageInterface : virtual public DCOPObject
{
  K_DCOP
       
  k_dcop:
   virtual ASYNC   jumpToReference(const QString& reference) = 0;
   virtual QString name_of_current_file() = 0;
   virtual bool    is_file_loaded(const QString& filename) = 0;
};
 
#endif

