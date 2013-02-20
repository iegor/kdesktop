// Author: Eray Ozkural (exa) <erayo@cs.bilkent.edu.tr>, (c) 2002
//
// Copyright: GNU LGPL: http://www.gnu.org/licenses/lgpl.html

#include "kbuffer.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

KBuffer::KBuffer()
{
  bufPos = buf.end(); // will become 0 in the beginning
}

KBuffer::~KBuffer(){
}

/** open a memory buffer */
bool KBuffer::open(int ) {
   // ignore mode
   buf.erase(buf.begin(), buf.end()); // erase buffer
   buf.reserve(8); // prevent iterators from ever being 0 and start with a reasonable mem
   bufPos = buf.end();              // reset position
   return true;
}

/** Close buffer */
void KBuffer::close(){
}

/** No descriptions */
void KBuffer::flush(){
}

/** query buffer size */
Q_ULONG KBuffer::size() const {
  return buf.size();
}

/** read a block of memory from buffer, advances read/write position */
Q_LONG KBuffer::readBlock(char* data, long unsigned int maxLen) {
  int len;
  if ((long unsigned)(buf.end()-bufPos) > maxLen)
    len = maxLen;
  else
    len = buf.end()-bufPos;
  std::vector<char>::iterator bufPosNew = bufPos + len;
  for (std::vector<char>::iterator it=bufPos; it < bufPosNew; it++, data++)
    *data = *it;
  bufPos = bufPosNew;
  return len;
}

/** write a block of memory into buffer */
Q_LONG KBuffer::writeBlock(const char *data, long unsigned int len){
  int pos = bufPos-buf.begin();
  copy(data, data+len, inserter(buf,bufPos));
  bufPos = buf.begin() + pos + len;
  return len;
}

/** read a byte */
int KBuffer::getch() {
  if (bufPos!=buf.end())
    return *(bufPos++);
  else
    return -1;
}

/** write a byte */
int KBuffer::putch(int c) {
  int pos = bufPos-buf.begin();
  buf.insert(bufPos, c);
  bufPos = buf.begin() + pos + 1;
  return c;
}

/** undo last getch()
 */
int KBuffer::ungetch(int c) {
  if (bufPos!=buf.begin()) {
    bufPos--;
    return c;
  }
  else
    return -1;
}
