// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (c) 2002
//
// Copyright: GNU LGPL: http://www.gnu.org/licenses/lgpl.html

#ifndef KBUFFER_H
#define KBUFFER_H

#include <qiodevice.h>
#include <vector>
#include <queue>

/**A buffer device for flexible and efficient buffers.

  *@author Eray Ozkural (exa)
  */

class KBuffer : public QIODevice  {
public:
  KBuffer();
  ~KBuffer();
  /** open a memory buffer */
  bool open(int mode);
  /** read in a block of memory */
  Q_LONG readBlock(char* data, long unsigned int maxLen);
  /** query buffer size */
  Q_ULONG size() const;
  /** No descriptions */
  void flush();
  /** Close buffer */
  void close();
  /** write a block of memory */
  Q_LONG writeBlock(const char *data, long unsigned int maxLen);
  /** read a byte */
  int getch();
  /** undo last getch()
 */
  int ungetch(int);
  /** write a byte */
  int putch(int);
  void* data() {
    return &buf[0];
  }
private:
  std::vector<char> buf;
  std::vector<char>::iterator bufPos;
};

#endif
