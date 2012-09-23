/***************************************************************************
                          kverbosuser.h  -  description
                             -------------------
    begin                : Tue Dec 18 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVERBOSUSER_H
#define KVERBOSUSER_H

#include "verbspanish.h"

// Der maximale Wert für den falsch bzw. richtig Zähler
#define MAX_WERT 32767
// Die maximale Anzahl von Anfragen, die ein Verb in der Richtig-Liste verbleibt.
#define STAY_IN_R_LIST 25
#define FEHLERHAFT -1
#define UNGELERNT 0
#define RICHTIGGEMACHT 1
#define FALSCHGEWICHT 2
#define DEFAULTUSER "------"
#define MAX_RESULTS 10

/** Datenstruktur für ein Verb in den Listen
  * right - gibt an, wie oft das Verb schon richtig eingegeben wurde.
  * wrong - gibt an, wie oft das Verb schon falsch eingegeben wurde.
  * counter - ist der Zähler für die aufsummierte Gewichtung der Verben bei Verben in den
  *           ungelernten bzw. falschen Listen. In der Liste der richtig gelernten Verben
  *           ist das ein Zähler, der angibt, wie lange das Verb in der Liste bleiben kann.
  * used - zeigt an, ob das Verb in der Verbendatei des Hauptprogrammes vorkommt.
  * status - zeigt an, ob das Verb noch nicht geübt wurde (status = 0), ob es zuletzt falsch
  *          eingegeben wurde (status = -1) oder zuletzt richtig (status = 1)
  */
class eintrag
{
public:
  bool operator== (const eintrag& e) const;
  QString verb;
  int right;
  int wrong;
  int counter;
  int status;
  bool used;
};

typedef QValueList<eintrag> verbenListe;

/** This class holds information about an user of the program. It stores lists of
  * verbs. One list for the words that are in the opened verb file and aren't traines yet.
  * Another list for the words that have been correct. This verbs are stored in the
  * list to present them more rarely to the user. The third list are for verbs entered with
  * errors. This verbs are presented to the user with a greater probability.
  *
  *@author Arnold Kraschinski
  */
class KVerbosUser {
public: 
  KVerbosUser(spanishVerbList* pL, QString n=DEFAULTUSER);
  ~KVerbosUser();
  /** fills the lists with the verbs that are in the verb list of the main program */
  void fillList(spanishVerbList* pL);
  /** returns the user name */
  QString getName() { return name; }
  /** marks all verbs in the R(right) and the F(false) list as unused. That means they can't
    * be a suggestion. This are probably verbs that are stored user data but at the moment
    * they aren't in the used verb list.
    */
  void deaktivieren();
  /** calculates the Kumulus for the list with the verbs that aren't studied yet and
    * for the list with the verbs that have been wrong. */
  void updateKumulus();
  /** selects a verb out of the verbs that are in the U and F list. That means a
    * verb that hasn't been studied yet or one that was wrong. */
QString suggestVerb();
  /** The solution for the suggested verb was right. Now the verb should be deleted from
    * the list of unstudied verbs but added to the list with the right verbs.
    */
  void right();
  /** The opposite function to the above one. The solution was wrong and the verb has to
    * be moved to the list with the wrong verbs.
    */
  void wrong();
  /** Saves all the user information to the $KDEHOME/apps/kverbos/data/username.kverbos file */
  bool saveUser(const int& res, const int& num);
  /** Retrieve the old results and the number of sessions */
  bool getResults(int& s, QString d[], int r[][2]);
  /** returns a pointer to the users verb list */
  verbenListe* getListe() { return &liste; }

private:
  /** name of the user */
  QString name;
  /** the list of the verbs that the user has workes with */
  verbenListe liste;
  /** this is the latest selected verb of the list */
  QString auswahl;
  /** die aufaddierte Summe der gewichteten Verben */
  int kumulus;
  /** an Iterator for the verb list */
  verbenListe::Iterator it;
  /** the number of training sessions that the user has made */
  int sessions;
  /** The results of the last ten seccions */
  QString date[MAX_RESULTS];
  int result[MAX_RESULTS][2];
};

#endif
