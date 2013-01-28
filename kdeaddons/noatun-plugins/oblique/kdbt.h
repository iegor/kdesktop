// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (c) 2002
//
// Copyright: GNU LGPL: http://www.gnu.org/licenses/lgpl.html


#ifndef KDbt_Interface
#define KDbt_Interface

#include <db_cxx.h>
#include <qdatastream.h>
#include <qbuffer.h>
#include "kbuffer.h"

/**A generic wrapper for "database thang" class that abstracts binary streaming operations.
  *@author Eray Ozkural (exa)
  */

template <typename T>
class KDbt : public Dbt {
public:
  /* assume streaming operators on QDataStream
  QDataStream & operator>> ( QDataStream& >>, T &);
  QDataStream & operator<< ( QDataStream& >>, T &);
  */
  KDbt() {
  }
  /** construct a Dbt from obj */
  KDbt(const T& obj) {
    set(obj);
  }
//  operator Dbt() {
//    return Dbt(thang.data(), thang.size());
//  }
  /** set "thang" to the contents of obj */
  void set(const T& obj) {
//    KBuffer buffer(thang);
    QDataStream ds(&thang);
    ds << obj;
//    std::cerr << "thang size " << thang.size() << endl;
//    buffer.close();
//    set_data(thang.data());
//    set_size(buffer.size());
    set_data(thang.data());
    set_size(thang.size());
  }
  void get(T& obj) {
    QByteArray buffer;
    buffer.setRawData((char*)get_data(),get_size());
    QDataStream ds(buffer,IO_ReadWrite);
    ds >> obj;
    buffer.resetRawData((char*)get_data(),get_size());
  }
private:
  /** Internal data */
//  QByteArray thang;
  KBuffer thang;
};

#endif
