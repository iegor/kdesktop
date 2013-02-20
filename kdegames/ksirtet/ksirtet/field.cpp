#include "field.h"
#include "field.moc"

#include <qwhatsthis.h>

#include <klocale.h>
#include <kgamelcd.h>

#include "board.h"
#include "prefs.h"
#include "piece.h"

//-----------------------------------------------------------------------------
KSField::KSField(QWidget *parent)
    : Field(parent)
{
    const Board *b = static_cast<Board *>(board);
    QWhatsThis::add(b->giftPool(), i18n("Indicate the number of garbage lines you received from your opponent."));
}

void KSField::removedUpdated()
{
    KGameLCD *lcd = static_cast<KGameLCD *>(removedList->lcd(0));
    lcd->displayInt(board->nbRemoved());
    if ( board->nbRemoved() ) lcd->highlight();
    if ( Prefs::showDetailedRemoved() ) {
      const KSBoard *ksb = static_cast<const KSBoard *>(board);
      for (uint i=0; i<4; i++) {
        if ( !(ksb->lastRemoved() & (2<<i)) ) continue;
        lcd = static_cast<KGameLCD *>(removedList->lcd(i+1));
        lcd->displayInt(ksb->nbRemovedLines(i));
        if ( ksb->nbRemovedLines(i) ) lcd->highlight();
      }
    }
}

void KSField::settingsChanged()
{
    Field::settingsChanged();
    bool b = Prefs::oldRotationStyle();
    static_cast<KSPieceInfo &>(Piece::info()).setOldRotationStyle(b);

    removedList->clear();
    QWhatsThis::remove(removedList);
    KGameLCD *lcd = new KGameLCD(5, removedList);
    QString s = (Prefs::showDetailedRemoved() ? i18n("Total:") : QString::null);
    removedList->append(s, lcd);
    lcd->displayInt( board->nbRemoved() );
    lcd->show();

    if ( Prefs::showDetailedRemoved() ) {
        for (uint i=0; i<4; i++) {
            KGameLCD *lcd = new KGameLCD(5, removedList);
            QString s = i18n("1 Line:", "%n Lines:", i+1);
            removedList->append(s, lcd);
            uint nb = static_cast<const KSBoard *>(board)->nbRemovedLines(i);
            lcd->displayInt(nb);
            lcd->show();
        }
    }
}
