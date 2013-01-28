#ifndef _HISTORY_MANAGER_H
#define _HISTORY_MANAGER_H

#include <kurl.h>
#include <qvaluestack.h>
#include <qobject.h>

class HistoryManager : public QObject {
  Q_OBJECT
  public:
    HistoryManager(QObject *parent = NULL);
    ~HistoryManager();
    KURL back();
    KURL forward();
    void addURL(const KURL &);
    enum Buttons {
      Back,
      Forward
    };
  signals:
    void uiChanged(int, bool);
  protected:
    QValueStack<KURL> back_stack;
    QValueStack<KURL> forward_stack;
    KURL currentURL;
};
  
#endif
