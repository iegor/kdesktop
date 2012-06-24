/*
  KPF - Public fileserver for KDE

  Copyright 2001 Rik Hemsley (rikkus) <rik@kde.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WEB_SERVER_SOCKET_H
#define WEB_SERVER_SOCKET_H

#include <qserversocket.h>

namespace KPF
{
  /**
   * Overridden to emit a signal on a new connection.
   */
  class WebServerSocket : public QServerSocket
  {
    Q_OBJECT

    public:

      WebServerSocket(Q_UINT16 port, uint maxconn);
      virtual void newConnection(int fd);

    signals:

      void connection(int);
  };

} // End namespace KPF

#endif // WEB_SERVER_SOCKET_H
// vim:ts=2:sw=2:tw=78:et
