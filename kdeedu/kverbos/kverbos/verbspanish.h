/***************************************************************************
                          verbspanish.h  -  description
                             -------------------
    begin                : Sun Dec 23 2001
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

#ifndef VERBSPANISH_H
#define VERBSPANISH_H

// include qt-Header
#include <qvaluelist.h>

#define MAX_TIEMPOS 9
#define MAX_SUBTYPES 3
#define	PERSONEN 6
#define MAX_GRUPPEN 17
#define REGULAR 0
#define IRREGULAR 1
#define VERBMUSTER 3

enum endings {ar, er, ir, empty};
// dummy1 to dummy3 are standing for forms that aren't possible
enum timeTypes {presente, imperfecto, indefinido, futuro, condicional, subPresente,
                subPasado, subFuturo, imperativo,
                presentePro, imperfectoPro, indefinidoPro, futuroPro, condicionalPro,
                subPresentePro, subPasadoPro, subFuturoPro, dummy2,
                presentePer, imperfectoPer, dummy1, futuroPer, condicionalPer,
                subPresentePer, subPasadoPer, subFuturoPre, dummy3
               };
enum timeSubTypes {normal, progresivo, perfecto};
enum persons {yo, tu, el, nosotros, vosotros, ellos};

typedef QString formArray[MAX_TIEMPOS][PERSONEN];
typedef QString endingsArray[MAX_TIEMPOS][VERBMUSTER][PERSONEN];

/** This class contains all the information belonging to one verb:
  *
  *  - the foreign and the spanish meaning
  *  - the form of the gerund and the participle
  *  - the different time forms of the verb
  *  - if it is irregular or to which group the verb belongs
  *  - if the object already contains all the possible forms
  *
  *  Some information belongs to all verbs. It is stored in static variables.
  *  - a lists of the regular endings
  *  - an array with the different reflexiv pronouns
  *  - forms of the verbs estar and haber that are needed for the gerund and the perfect
  *    form of a time.
  *
  *  To the class belong some enums that are used in the class:
  *  endings are the differnet spanish verb endings, 0 = ar, 1 = er, 2 = ir.
  *  <pre>
  *  enum endings {ar, er, ir, empty};
  *  </pre>
  *
  *  'timeTypes' are the different spanish times. There are
  *  MAX_TIEMPOS (=9) * MAX_SUBTYPES (=3) = 27 different types. Types called 'dummyX'
  *  arent't existing but it is easier to use in the program.
  *  <pre>
  *  enum timeTypes {presente, imperfecto, indefinido, futuro, condicional, subPresente,
  *                subPasado, subFuturo, imperativo,
  *                presentePro, imperfectoPro, indefinidoPro, futuroPro, condicionalPro,
  *                subPresentePro, subPasadoPro, subFuturoPro, dummy2,
  *                presentePer, imperfectoPer, dummy1, futuroPer, condicionalPer,
  *                subPresentePer, subPasadoPer, subFuturoPre, dummy3
  *               };
  *  </pre>
  *
  *  timeSubTypes seperates between a normal form of the time or a progressiv, or perfekt
  *  form of the time. The verb object only stores the gerund or the participle for this
  *  forms the rest can be found in static variables of the class.
  *  <pre>
  *  enum timeSubTypes {normal, progresivo, perfecto};
  *  </pre>
  *
  *  Persons are the different grammatical persons including vosotros which is unusual
  *  in Latin America. (PERSONEN = 6)
  *  <pre>
  *  enum persons {yo, tu, el, nosotros, vosotros, ellos};
  *  </pre>
  *
  *	 The conjugated forms of the verb are stored in the array 'formArray'. The first index
  *  mentions the time and the second the person.
  *  <pre>
  *  typedef QString formArray[MAX_TIEMPOS][PERSONEN];
  *  </pre>
  *
  *  The type 'endingsArray' stores the different endings. The first index means the time,
  *  the second the type of verb (-ar, -er, -ir) and the lase one is the person.
  *  <pre>
  *  typedef QString endingsArray[MAX_TIEMPOS][MAX_SUBTYPES][PERSONEN];
  *  </pre>
  *
  *  The verbs are divided in several groups:
  *		0 -> regular
  *   1 -> irregular
  *   2 -> <e -> ie>
  *   3 -> <o -> ue>
  *   4 -> <u -> ue>
  *   5 -> <e -> i>
  *   6 -> <c -> qu>
  *   7 -> <g -> gu>
  *   8 -> <z -> c>
  *   9 -> <gu -> gü>
  *  10 -> <c -> z>
  *  11 -> <g -> j>
  *  12 -> <gu -> g>
  *  13 -> <qu -> c>
  *  14 -> <accent like 'enviar'>
  *  15 -> <accent like 'continuar'>
  *  16 -> \<missing i>
  *@author Arnold Kraschinski
  */

class verbSpanish
{
public:
   verbSpanish();
   verbSpanish(const verbSpanish& v);
   virtual ~verbSpanish();
   verbSpanish& operator= (const verbSpanish& v);
   /** If two verbs are compared only the foreign form and the spanish form are compared */
   bool operator== (const verbSpanish& v) const;
   /** This overloaded == operator compares the verb to a QString.
    *  Only the spanish meaning is compared.
    */
   bool operator== (const QString& s) const;
   /** The spa/nisch forms of the verb are compared */
   bool operator< (const verbSpanish& v) const;
   /** Compares the verb objects completely, all forms and fields.
    */
   bool allEqual(verbSpanish v);
   /** tests if the fields of the object contain only empty strings
    */
   bool isEmpty() const;
   /** returns the foreign meaning of the verb
    */
   QString getForeign() const;
   /** sets the foreign meaning of the verb
    */
   void setForeign(const QString &s);
   /** returns the spanish meaning of the verb
    */
   QString getSpanish() const;
   /** sets the spanish meaning of the verb
    */
   void setSpanish(const QString &s);
   /** returns the gerund of the verb
    */
   QString getGerund();
   /** sets the gerund form to the string given in s
    */
   void setGerund(const QString &s);
   /** returns the particip of the verb
    */
   QString getPartizip();
   /** sets the participe to the string given in s
    */
   void setPartizip(const QString &s);
   /** returns if the verb belongs to one of the groups
    */
   bool isGroupVerb();
   /** returns the group number to which the verb belongs
    */
   int getGroup() { return group; }
   /** sets the field regular corresponding to b
    */
   void setGroup(const int g);
   /** returns the desired verb form indicated by time, person and type. This means
    *  with this function complex forms like a progressise or a perfect form can be
    *  retrieved.
    *  The parts of the form a stored like this:
    *  parts[0] the reflexive pronoun
    *  parts[1} the auxiliary verb
    *  parts[2] the verb form
    *  The return value 'bool' indicates if the form could be formed.
    */
   bool getForm(int t, int p, int typ, QString parts[]);
   /** returns the desired verb form indicated by time and person as a string
    */
   QString getForm(int t, int p);
   /** sets the verb form that is indicated by time and person
    */
   void setForm(int t, int p, QString s);
   /** The following function generates the regular verb forms. The forms can be
    *  generated time by time or all times in one step by using conjugateAll()
    */
   bool conjugateAll();
   /** returns if the verb is a reflexive one
    */
   bool isReflexiv() const;
   /** returns the reflexive pronoun that belongs to the person p
    */
   QString getRefPron(int p);
   /** tests if the string is an automatic generated Form, that means the last char is '#' */
   bool isAutoForm(QString& s) const;
private:
   bool conjugatePresente();
   bool conjugateImperfecto();
   bool conjugateIndefinido();
   bool conjugateFuturo();
   bool conjugateCondicional();
   bool conjugateSubPresente();
   bool conjugateSubPasado();
   bool conjugateImperativo();
   /** returns the verb stem
    */
   QString getStem() const;
   /** returns the ending of the verb
    */
   endings getEnding(bool& good);
   /** bildet die Formen, d.h. hängt die Endungen an den unveränderlichen
    *  Teil an.
    *  Die Form muss bisher ein leerer String sein. Anderenfalls wird angenommen, dass
    *  bereits eine gültige Form vorliegt und diese Form eine unregelmäßige Form ist, die
    *  nicht überschrieben werden soll.
    */
   bool conjugateTime(const int t, const QString &s);
   /** Es wird verglichen, ob die beiden Strings gemeinsame Bestandteile haben. Also
    *  "gehen, laufen, springen" wird etwa mit "gehen, laufen" verglichen. Die Funktion
    *  gibt in so einem Fall 'true' zurück, auch wenn im zweiten String ein Teil
    *  fehlt. Das ist nötig, um zwei Verben als gleich zu erkennen, auch wenn bei einem
    *  mehr oder weniger deutsche Bedeutungen oder die Bedeutungen in anderer Reihenfolge
    *	 angegeben sind.
    */
   bool vergleich(const QString& s1, const QString& s2) const;
   /** Deletes the '#' character from autogenerated forms. */
   QString delAutoForm(QString s) const;
   /** does the changes for group verbs
    */
   bool groupVerb(QString &s, int time=-1, int person=-1);
   /** the verb in the foreign language
    */
   QString foreign;
   /** the spanish verb
    */
   QString spanish;
   /** the gerund form of the verb
    */
   QString gerund;
   /** the partizip form of the verb
    */
   QString partizip;
   /** the array for all the different forms of the verb
    */
   formArray forms;
   /** tells to which group the verb belongs
    */
   int group;
   /** tells if all verbforms are present or if some are missing.
    */
   bool conjugated;
   /** set to true if the class has found an error in the verb structure. */
   bool error;

   /** In this array all the different endings for all the different times
    *  are stored.
    */
   static endingsArray verbEndings;
   /** the reflexive pronouns
    */
   static QString refPron[PERSONEN];
public:
   /** The forms of the verb estar. They are needed for the gerund form.
    */
   static QString estar[MAX_TIEMPOS][PERSONEN];
   /** The forms of the verb haber. They are needed for perfect forms
    */
   static QString haber[MAX_TIEMPOS][PERSONEN];
   /** This array contains the names of the time forms as strings
    */
   static QString timeNames[MAX_TIEMPOS*MAX_SUBTYPES];
   /** This array contains the names of the different verb groups
    */
   static QString groupNames[MAX_GRUPPEN];
   static int maxTiempos;
   static int maxSubtypes;
   static int persons;
   static int maxForms;
   /** The language that is selected at the moment. This selection affects all the instances
     * of the class. */
   static QString language;
};

/** Very often a list of verbs is needed. This is the type definition of a
  * QValueList template with the verbSpanish type
  */
typedef QValueList<verbSpanish> spanishVerbList;

#endif
