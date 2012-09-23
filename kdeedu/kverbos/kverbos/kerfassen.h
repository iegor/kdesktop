/***************************************************************************
                          kerfassen.h  -  description
                             -------------------
    begin                : Sat Dec 15 2001
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

#ifndef KERFASSEN_H
#define KERFASSEN_H

#include <klocale.h>

#include "qerfassen.h"
#include "verbspanish.h"

class QLineEdit;


/** This class overloads the class QErfassen which was created with the Qt-designer.
  * The dialog is used to edit existing verbs and to add new verbs.
  *
  *@author Arnold Kraschinski
  */

class KErfassen : public QErfassen  {
public:
  /** An die Dialogklasse wird ein Zeiger auf eine Verbliste übergeben, damit
   *  Verben in die Liste eingefügt werden können. Es wird auch ein Iterator übergeben, der
   *  anzeigt, wo mit dem editieren begonnen werden soll.
   */
  KErfassen(spanishVerbList& l, int pos = -1, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
  /** der Destruktor */
  virtual ~KErfassen();
  /** Gibt zurück, ob durch den Ablauf des Dialoges eine Änderung an der
   *  bearbeiteten Verbliste entstanden ist (list = true).
   *  Wird als Parameter false übergeben, erfährt man, ob das Fenster des Dialoges
   *  geändert wurde.
   */
  bool wasChanged();
  /** The dialog works on his own copy of the verblist. So after the end of the dialog
    * the caling function has to fetch the list from the dialog with this function.
    */
  spanishVerbList getList();

private:
  /** Ein Verb befindet sich in den Eingabefeldern des Dialoges und
    * dieses soll nun in die Liste aufgenommen werden.
    */
  spanishVerbList::Iterator verbErfassen();
  /** The verb in the edit boxes is transfered to a verbSpanish object
    */
  verbSpanish verbFromWidget();
  /** Ein Verb aus der aktuellen Listenposition soll in dem Dialog dargestellt
   *  werden.
   */
  void verbToWidget();
  /** Deaktiviert die Eingabefelder für die Gerund-Form und für das Partizip
    * und die Karteikarten für die ganzen Formen. Außerdem wird der Radiobutton
    * wieder auf den Ausgangszustand zurückgesetzt.
    */
  void deaktivieren();
  /** Aktiviert die Eingabefelder für die Gerund-Form und für das Partizip
    * und die Karteikarten für die ganzen Formen. Außerdem wird der Radiobutton
    * in den gesetzten Zustand gebracht.
    */
  void aktivieren();
  /** In dem Dialog sollen ein Teil der Eingabefelder oder aber alle
    * Eingabefelder gelöscht werden.
    */
  void clearPage(int s);
  void clearAllPages();
  void clearAll();
  /** Es wird geprüft, ob der Inhalt des Eingabefensters verändert wurde.
   */
  bool widgetChanged();

public slots:
  /** Dieser Slot bearbeitet den Wechsel am RadioButtonIrregular
    * Wird der Knopf ausgewählt, dann müssen alle Felder für die
    * unregelmäßigen Formen aktiviert werden.
    * Wird der Button gelöscht, dann müssen die Felder deaktiviert
    * werden.
    */
  // virtual void slotToggleIrregular(bool b);
  virtual void slotTypChanged(int);
  /** Die folgenden Slots werden von den einzelnen Löschen-Buttons des
    * Dialoges aufgerufen.
    */
  virtual void slotClearPage1();
  virtual void slotClearPage2();
  virtual void slotClearPage3();
  virtual void slotClearPage4();
  virtual void slotClearPage5();
  virtual void slotClearPage6();
  virtual void slotClearPage7();
  virtual void slotClearPage8();
  virtual void slotClearAll();
  /** Wird aufgerufen, wenn der 'beenden'-Button gedrückt wurde.
   */
  virtual void slotDlgEnde();
  /** Die Eingabe eines Verbes wurde abgeschlossen und es wurde der
    * weiter Button gedrückt, um weitere Verben einzugeben.
    */
  virtual void slotWeiter();
  /** Es wurde an einer Position in der Liste der zurück-Button betätigt.
   *  Das gerade eingegebene Verb muss in die Liste übernommen werden und der
   *  Positionszeiger in der Liste muss eine Position nach hinten rücken, falls
   *  es ein Verb weiter vorne in der Liste gibt. Dieses Verb muss eventuell in
   *  den Dialog geladen werden.
   */
  virtual void slotZurueck();
  /** The dilog shows a verb but this verb shouldn't be changed. The user wants to
    * enter a new verb. So he can use this button to get an empty dialog window for his
    * new verb
    */
  virtual void slotNewVerb();
  /** Die folgenden Slots werden aufgerufen, wenn ein Button mit einem
   *  spanischen Sonderzeichen gedrückt wird.
   *  Das Zeichen wird dann dem Eingabefeld, das den Focus hat zugefügt.
   */
  virtual void slotN();
  virtual void slotA();
  virtual void slotE();
  virtual void slotI();
  virtual void slotO();
  virtual void slotU();

private:
  /** Der Zeiger auf das Dokumenten-Objekt des Programmes. */
  spanishVerbList liste;
  spanishVerbList::Iterator position;
  /** changed zeigt an, ob etwas geändert wurde. */
  bool         changed;
  /** Are pointers to the GUI elements for an easier use */
  QLineEdit* formEdit[6][MAX_TIEMPOS];
};

#endif
