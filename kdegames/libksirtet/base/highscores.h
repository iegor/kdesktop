#ifndef BASE_HIGHSCORES_H
#define BASE_HIGHSCORES_H

#include <kexthighscore.h>
#include <kdemacros.h>

class KDE_EXPORT BaseHighscores : public KExtHighscore::Manager
{
 public:
    BaseHighscores();
};

#endif
