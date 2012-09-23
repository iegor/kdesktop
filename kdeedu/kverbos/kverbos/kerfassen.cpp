/***************************************************************************
                          kerfassen.cpp  -  description
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

#include "kerfassen.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
//#include <qvaluelist.h>

#include <kmessagebox.h>

//#include "verbspanish.h"

// Die Konstruktoren sollen sicherstellen, dass am Anfang die Karteikarten und
// die Eingabefelder für Gerund und Partizip nicht aktiviert sind.
// Das Dialogfeld in den Zustand nicht geändert versetzen.
KErfassen::KErfassen(spanishVerbList& l, int pos, QWidget* parent, const char* name, bool modal, WFlags fl)
: QErfassen(parent, name, modal, fl), liste(l)
{
	formEdit[0][0] = EditYo2_1;
	formEdit[1][0] = EditTu2_1;
	formEdit[2][0] = EditEl2_1;
	formEdit[3][0] = EditNosotros2_1;
	formEdit[4][0] = EditVosotros2_1;
	formEdit[5][0] = EditEllos2_1;
	formEdit[0][1] = EditYo2_2;
	formEdit[1][1] = EditTu2_2;
	formEdit[2][1] = EditEl2_2;
	formEdit[3][1] = EditNosotros2_2;
	formEdit[4][1] = EditVosotros2_2;
	formEdit[5][1] = EditEllos2_2;
	formEdit[0][2] = EditYo2_3;
	formEdit[1][2] = EditTu2_3;
	formEdit[2][2] = EditEl2_3;
	formEdit[3][2] = EditNosotros2_3;
	formEdit[4][2] = EditVosotros2_3;
	formEdit[5][2] = EditEllos2_3;
	formEdit[0][3] = EditYo2_4;
	formEdit[1][3] = EditTu2_4;
	formEdit[2][3] = EditEl2_4;
	formEdit[3][3] = EditNosotros2_4;
	formEdit[4][3] = EditVosotros2_4;
	formEdit[5][3] = EditEllos2_4;
	formEdit[0][4] = EditYo2_5;
	formEdit[1][4] = EditTu2_5;
	formEdit[2][4] = EditEl2_5;
	formEdit[3][4] = EditNosotros2_5;
	formEdit[4][4] = EditVosotros2_5;
	formEdit[5][4] = EditEllos2_5;
	formEdit[0][5] = EditYo2_6;
	formEdit[1][5] = EditTu2_6;
	formEdit[2][5] = EditEl2_6;
	formEdit[3][5] = EditNosotros2_6;
	formEdit[4][5] = EditVosotros2_6;
	formEdit[5][5] = EditEllos2_6;
	formEdit[0][6] = EditYo2_7;
	formEdit[1][6] = EditTu2_7;
	formEdit[2][6] = EditEl2_7;
	formEdit[3][6] = EditNosotros2_7;
	formEdit[4][6] = EditVosotros2_7;
	formEdit[5][6] = EditEllos2_7;
	formEdit[0][7] = 0;
	formEdit[1][7] = 0;
	formEdit[2][7] = 0;
	formEdit[3][7] = 0;
	formEdit[4][7] = 0;
	formEdit[5][7] = 0;
	formEdit[0][8] = EditYo2_8;
	formEdit[1][8] = EditTu2_8;
	formEdit[2][8] = EditEl2_8;
	formEdit[3][8] = EditNosotros2_8;
	formEdit[4][8] = EditVosotros2_8;
	formEdit[5][8] = EditEllos2_8;
	if ((-1 == pos) || (pos >= (int)liste.count()))
		position = liste.end();
	else
		position = liste.at(pos);
	changed = false;
	verbToWidget();
	// einige Buttonbeschriftungen richtigstellen
	ButtonN->setText("ñ");
	ButtonA->setText("á");
	ButtonE->setText("é");
	ButtonI->setText("í");
	ButtonO->setText("ó");
	ButtonU->setText("ú");
	TextTu1->setText("tú");
	TextTu2->setText("tú");
	TextTu3->setText("tú");
	TextTu4->setText("tú");
	TextTu5->setText("tú");
	TextTu6->setText("tú");
	TextTu7->setText("tú");
	TextTu8->setText("tú");
	ComboBoxTypen->changeItem("gu > gü", 9);
	TabWidgetFormas->setCurrentPage(0);
	EditVerbo->setFocus();
	verbToWidget();
}

KErfassen::~KErfassen()
{
}

// Die Eingabefelder für das Gerund und das Partizip werden deaktiviert
// ebenso die Karteikarten, der Radiobutton wird zurückgesetzt
void KErfassen::deaktivieren()
{
	if (position == liste.end())
		ComboBoxTypen->setCurrentItem(REGULAR);
	else
		ComboBoxTypen->setCurrentItem((*position).getGroup());
	TabWidgetFormas->setEnabled(false);
	qWarning("desaktiviert");
	TabWidgetFormas->setCurrentPage(0);
	EditGerundio->setEnabled(false);
	EditParticipio->setEnabled(false);
	EditGerundio->setText("");
	EditParticipio->setText("");
	clearAllPages();
}

// Die Eingabefelder für das Gerund und das Partizip werden aktiviert
// ebenso die Karteikarten, der Radiobutton wird gesetzt
void KErfassen::aktivieren()
{
	ComboBoxTypen->setCurrentItem(IRREGULAR);
	TabWidgetFormas->setEnabled(true);
	TabWidgetFormas->setCurrentPage(0);
	EditGerundio->setEnabled(true);
	EditParticipio->setEnabled(true);
}

// löscht die Eingabefelder der ersten Karteikarte für die Formen
void KErfassen::clearPage(int s)
{
	for (int i=0; i<6; i++)
		formEdit[i][s]->setText("");
}

void KErfassen::clearAllPages()
{
	for (int i = presente; i<=subPasado; i++)
		clearPage(i);
	clearPage(imperativo);
}

// Löscht die Inhalte aller Datenfeldler des Dialoges.
void KErfassen::clearAll()
{
	EditVerbo->setText("");
	EditAleman->setText("");
	// Die Eingabefelder sollen wieder deaktiviert werden. Dabei werden sie auch gelöscht.
	deaktivieren();
	qWarning("clearAll deaktiviert");
}

bool KErfassen::wasChanged()
{
	return changed;
}

// prüft, ob der Fensterinhalt geändert wurde
bool KErfassen::widgetChanged()
{
	bool result = true;
	if (position != liste.end())
	{
		// Das Verb an der aktuellen Position ist nicht leer. Ist es gleich dem Verb
		// im Widget, dann gab es keine Änderung.
		if ((*position).allEqual(verbFromWidget()))
			result = false;
	}
	else
	{
		// Die aktuelle Position ist leer. Ist das Verb im Widget leer, dann erfolgte
		// keine Änderung.
		if (verbFromWidget().isEmpty())
			result = false;
	};
	return result;
}

// -----------------------------------------------
// Hier werden die Slots der Klasse implementiert.
// During the following passage the slots of this class are implemented.
// -----------------------------------------------

// the type of the verb was changed
void KErfassen::slotTypChanged(int i)
{
	if (IRREGULAR == i)
	{
		aktivieren();
	}
	else
	{
		deaktivieren();
qWarning("slotTypeChanged deaktiviert");
	}
}

// Die folgenden Slots löschen eine Seite der Karteikarten oder auch
// gleich alle Karten auf einmal.
void KErfassen::slotClearPage1()
{
	clearPage(presente);
}

void KErfassen::slotClearPage2()
{
	clearPage(imperfecto);
}

void KErfassen::slotClearPage3()
{
	clearPage(indefinido);
}

void KErfassen::slotClearPage4()
{
	clearPage(futuro);
}

void KErfassen::slotClearPage5()
{
	clearPage(condicional);
}

void KErfassen::slotClearPage6()
{
	clearPage(subPresente);
}

void KErfassen::slotClearPage7()
{
	clearPage(subPasado);
}

void KErfassen::slotClearPage8()
{
	clearPage(imperativo);
}

void KErfassen::slotClearAll()
{
	clearAll();
}

void KErfassen::slotDlgEnde()
{
	// Der Dialog wird beendet, nun muss geprüft werden, ob das Verb in den
	// Dialogfeldern ein neues oder geändertes Verb ist. Dazu wird das Verb aus den
	// Dialogfeldern geholt und mit dem Verb an der aktuellen Listenposition
	// verglichen.
	if (widgetChanged())
	{
		int willErfassen  = KMessageBox::warningYesNoCancel(this,
					i18n("The current verb is not in the list yet.\nDo you want to add it?"),
					i18n("Warning"),i18n("Add"),i18n("Do Not Add"));
		switch (willErfassen)
		{
		case KMessageBox::Yes : // YES
			position = verbErfassen();
		case KMessageBox::No : // NO
			done(0);
			break;
		default : // wirkt wie CANCEL
			break;
		};
	}
	else
		done(0);
}

// Ein Verb wurde eingegeben und die Eingabe mit dem
// weiter-Button beendet.
void KErfassen::slotWeiter()
{
	if ( widgetChanged() )
	{
		position = verbErfassen();
	};
	// es müssen alle Eingabefelder wieder gelöscht werden oder das Verb an der aktuellen
	// Position wird in den Dialog geladen und den Eingabefokus
	// erhält das Feld für das spanische Wort
	if (position == liste.end())
	{
		position  = liste.begin();
	}
	else
	{
		position++;
	};
	verbToWidget();
	TabWidgetFormas->setCurrentPage(0);
	EditVerbo->setFocus();
} // slotWeiter

// Die Eingabeposition in der Liste muss eine Position weiter nach vorne rutschen.
void KErfassen::slotZurueck()
{
	if ( widgetChanged() )
	{
		position = verbErfassen();
	};
	// es müssen alle Eingabefelder wieder gelöscht werden oder es muss das Verb an
	// der Position in den Dialog geladen werden und den Eingabefokus
	// erhält das Feld für das spanische Wort
	if (position == liste.begin())
	{
		position = liste.end();
	}
	else
	{
		position--;
	};
	verbToWidget();
	TabWidgetFormas->setCurrentPage(0);
	EditVerbo->setFocus();
} // slotZurueck

// The dilog shows a verb but this verb shouldn't be changed. The user wants to
// enter a new verb. So he can use this button to get an empty dialog window for his
// new verb
void KErfassen::slotNewVerb()
{
	if ( widgetChanged() )
		position = verbErfassen();
	position = liste.end();
	verbToWidget();
	TabWidgetFormas->setCurrentPage(0);
	EditVerbo->setFocus();
}

void KErfassen::slotN()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("ñ");
}

void KErfassen::slotA()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("á");
}

void KErfassen::slotE()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("é");
}

void KErfassen::slotI()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("í");
}

void KErfassen::slotO()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("ó");
}

void KErfassen::slotU()
{
	QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
	if (pl)
		pl->insert("ú");
}

// A verb that is in the dialog should be addded to the list.
// - if it is completely new verb then the verb should be added at its alphabetical position
// - if there is a verb which is equal to the new verb then a dialog asks whether the
//   verb should be added again or replaced or if the operation shoul be canceled.
spanishVerbList::Iterator KErfassen::verbErfassen()
{
	verbSpanish v = verbFromWidget();
	spanishVerbList::Iterator it = liste.begin();
	if (liste.isEmpty())
	{
		// The list was empty until now
		it = liste.append(v);
		changed = true;
	}
	else
	{
		while ((it != liste.end()) && (*it < v))
			it++;
		if ((it == liste.end()) || !(*it == v))
		{
			// Das Verb v wurde nicht in der Liste gefunden.
			it = liste.insert(it, v);
			changed = true;
		}
		else
		{
			// Das Verb wurde in der Liste gefunden.
			switch (KMessageBox::warningYesNoCancel(this,
					i18n("The current verb is already in the list.\nDo you want to replace it?\nIf you do not want to change the list press 'Cancel'."),
					i18n("Warning"), i18n("Replace"), i18n("Do Not Replace")))
			{
			case KMessageBox::Yes:	// the verb at the position is replaced by v.
				*it = v;
				changed = true;
				break;   	
			case KMessageBox::No: // the verb v is added as a new verb
				it = liste.insert(it, v);
				changed = true;
				break;
			default:
				// nothing is done the list remains unchanged.
				{};
			};
		};
	};
	return it;
} // VerbErfassen

// The information in the dialog is read and stored in an verb object and then returned
verbSpanish KErfassen::verbFromWidget()
{
	verbSpanish v;
	v.setSpanish(EditVerbo->text().simplifyWhiteSpace());
	v.setForeign(EditAleman->text().simplifyWhiteSpace());
	if (ComboBoxTypen->currentItem()!=IRREGULAR)
	{
		// regelmäßiges Verb - Endungstyp feststellen -> fertig
		v.setGroup(ComboBoxTypen->currentItem());
	}
	else
	{
		// unregelmäßiges Verb - alle Formen übertragen -> fertig
		v.setGroup(IRREGULAR);
		v.setGerund(EditGerundio->text().simplifyWhiteSpace());
		v.setPartizip(EditParticipio->text().simplifyWhiteSpace());
		// die anderen Verbformen für die Zeiten übertragen. 	
		for (int i=presente; i<=subPasado; i++)
			for (int j = yo; j<=ellos; j++)
				v.setForm(i, j, formEdit[j][i]->text().simplifyWhiteSpace());
		for (int j = yo; j<=ellos; j++)
			v.setForm(imperativo, j, formEdit[j][imperativo]->text().simplifyWhiteSpace());
	};
	return v;
}

// The verb at the position it should be displaied in the dialog
void KErfassen::verbToWidget()
{
	if (position == liste.end())
	{
		clearAll();
	}
	else
	{
		verbSpanish v = *position;
		EditVerbo->setText(v.getSpanish());
		EditAleman->setText(v.getForeign());
		ComboBoxTypen->setCurrentItem(v.getGroup());
		if (v.isGroupVerb())
		{
			deaktivieren();
qWarning("verbTowidget deaktiviert");
		}
		else
		{
			// this is an irregular verb: bring all the forms to the dialog widget
			aktivieren();
			EditGerundio->setText(v.getGerund());
			EditParticipio->setText(v.getPartizip());
			// die anderen Verbformen für die Zeiten übertragen. 	
			for (int i=presente; i<=subPasado; i++)
				for (int j = yo; j<=ellos; j++)
					formEdit[j][i]->setText(v.getForm(i, j));
			for (int j = yo; j<=ellos; j++)
				formEdit[j][imperativo]->setText(v.getForm(imperativo, j));
		};
	};
}

// The dialog works on his own copy of the verblist. So after the end of the dialog
// the caling function has to fetch the list from the dialog with this function.
spanishVerbList KErfassen::getList()
{
	return liste;
}



