#include "field.h"
#include "field.moc"

#include <qwhatsthis.h>

#include <klocale.h>
#include <kgamelcd.h>

#include "common/commonprefs.h"
#include "board.h"


FEField::FEField(QWidget *parent)
    : Field(parent)
{
    Board *b = static_cast<Board *>(board);
    QWhatsThis::add(b->giftPool(), i18n("Display the amount of foul eggs sent by your opponent."));
}

void FEField::removedUpdated()
{
    Field::removedUpdated();
    const FEBoard *feb = static_cast<const FEBoard *>(board);
    KGameLCD *lcd = static_cast<KGameLCD *>(removedList->lcd(0));
    lcd->displayInt(feb->nbPuyos());
    if ( feb->nbPuyos() ) lcd->highlight();
    if ( CommonPrefs::showDetailedRemoved() )
      for (uint i=0; i<4; i++) {
        if ( !(feb->lastChained() & 2<<i) ) continue;
        lcd = static_cast<KGameLCD *>(removedList->lcd(i+1));
        lcd->displayInt(feb->nbChainedPuyos(i));
        if ( feb->nbChainedPuyos(i) ) lcd->highlight();
      }
}

void FEField::settingsChanged()
{
    Field::settingsChanged();

    removedList->clear();
    KGameLCD *lcd = new KGameLCD(6, removedList);
    removedList->append(i18n("Total:"), lcd);
    uint nb = static_cast<const FEBoard *>(board)->nbPuyos();
    lcd->displayInt(nb);
    lcd->show();
        
    if ( CommonPrefs::showDetailedRemoved() ) {
      QWhatsThis::add(removedList,
                      i18n("Display the number of removed groups (\"puyos\") classified by the number of chained removal."));
      for (uint i=0; i<4; i++) {
          KGameLCD *lcd = new KGameLCD(6, removedList);
          QString s = (i==3 ? ">3" : QString::number(i));
          removedList->append(s, lcd);
          uint nb = static_cast<const FEBoard *>(board)->nbChainedPuyos(i);
          lcd->displayInt(nb);
          lcd->show();
      }
    } else 
      QWhatsThis::add(removedList,
                    i18n("Display the number of removed groups (\"puyos\")."));
}
