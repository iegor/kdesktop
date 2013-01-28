#ifndef COMMON_HIGHSCORES_H
#define COMMON_HIGHSCORES_H

#include "base/highscores.h"

#include "lib/libksirtet_export.h"

class LIBKSIRTET_EXPORT CommonHighscores : public BaseHighscores
{
 public:
    CommonHighscores();

 private:
    void convertLegacy(uint level);
    bool isStrictlyLess(const KExtHighscore::Score &,
                        const KExtHighscore::Score &) const;
    void additionalQueryItems(KURL &, const KExtHighscore::Score &) const;
};

#endif
