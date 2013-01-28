#ifndef COMMON_SETTINGS_H
#define COMMON_SETTINGS_H

#include "base/settings.h"

#include "lib/libksirtet_export.h"


//-----------------------------------------------------------------------------
class LIBKSIRTET_EXPORT AppearanceConfig : public BaseAppearanceConfig
{
    Q_OBJECT
public:
    AppearanceConfig();
};

//-----------------------------------------------------------------------------
class LIBKSIRTET_EXPORT GameConfig : public QWidget
{
    Q_OBJECT
public:
    GameConfig();

protected:
    QGridLayout *_grid;
};

#endif
