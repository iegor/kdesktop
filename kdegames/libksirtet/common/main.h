#ifndef COMMON_MAIN_H
#define COMMON_MAIN_H

#include "base/main.h"

#include "lib/libksirtet_export.h"

class LIBKSIRTET_EXPORT MainWindow : public BaseMainWindow
{
    Q_OBJECT
public:
    MainWindow() {}

protected:
    void init();
    void addConfig(KConfigDialog *);
    void addKeys(KKeyDialog &);
    void saveKeys();
    virtual void focusInEvent(QFocusEvent *e);
};

#endif
